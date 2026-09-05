/* ============================================================
 * harness.c
 *
 * ECE 309 Project 1 - LLM Mini-Harness (Phase 1: Core Loop)
 *
 * This is a minimal, single-file command-line program that
 * stands in for the "core loop" of an LLM agent harness. On
 * each pass through the loop it:
 *
 *   1. Prints a prompt and reads one line of text from the
 *      user (standard input) using fgets.
 *   2. Decides how to respond using three simple rules:
 *        - the exact word "exit"      -> shut the program down
 *        - text containing "hello"    -> print a fixed greeting
 *          (this stands in for the "mock model")
 *        - anything else              -> echo the input back
 *
 * Only standard C library headers are used (<stdio.h> and
 * <string.h>), and only standard C library functions, so this
 * compiles on any POSIX-compliant system with the exact command
 * given in the assignment:
 *
 *     gcc harness.c -o harness
 *
 * This file intentionally does NOT implement conversation
 * history, tool calls, or anything beyond the core loop -- those
 * are later phases of the project.
 * ============================================================ */

#include <stdio.h>   /* printf, fgets, fflush, stdin, stdout       */
#include <string.h>  /* strcmp, strstr, strcspn                    */

/* Maximum number of characters (including the terminator) we will
 * accept on one line of input. Using a fixed-size stack buffer
 * (rather than dynamic allocation) keeps this first phase of the
 * harness simple; memory management for longer-lived state (like
 * conversation history) is left for a later phase of the project. */
#define MAX_INPUT_LEN 1024

int main(void) {
    /* Buffer that holds one line of user input at a time. It is
     * declared once, outside the loop, and overwritten on every
     * iteration -- we don't need a new buffer for each turn since
     * we only ever look at "the current line" in this phase. */
    char input[MAX_INPUT_LEN];

    printf("=== LLM Mini-Harness (Phase 1: Core Loop) ===\n");
    printf("Type a message and press Enter. Type 'exit' to quit.\n\n");

    /* ------------------------------------------------------
     * Core Loop
     * ------------------------------------------------------
     * "while (1)" creates an infinite loop. The ONLY ways out
     * are the explicit "break" statements below (when the user
     * types "exit", or if the input stream unexpectedly closes).
     * ------------------------------------------------------ */
    while (1) {

        /* Print the prompt without a trailing newline, so the
         * user's typed text appears right after "You: " on the
         * same line, like a chat interface. */
        printf("You: ");

        /* Force the "You: " text out to the terminal right away.
         * Standard output is normally line-buffered when it's a
         * terminal, but flushing explicitly guarantees the prompt
         * is visible before we block waiting for input -- this
         * also matters if the program's output is ever redirected
         * to a file or pipe, where buffering behaves differently. */
        fflush(stdout);

        /* fgets(buffer, size, stream) reads a single line of text
         * from 'stream' (here, stdin -- the keyboard/terminal) into
         * 'buffer'. It reads at most (size - 1) characters and
         * always null-terminates the result, so it can never
         * overflow 'input'. It keeps the newline character ('\n')
         * at the end of the string if there was room for it.
         *
         * fgets returns NULL if no input could be read at all --
         * for example, if the input stream has been closed (this
         * happens when a test script pipes in a fixed set of lines
         * and then runs out, or the user presses Ctrl+D). We treat
         * that the same as an exit request instead of crashing or
         * spinning forever on empty reads. */
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nInput stream closed. Shutting down.\n");
            break;
        }

        /* fgets left a trailing '\n' in 'input' (if the line fit in
         * our buffer). We don't want that newline when we compare
         * the string or print it back, so we trim it:
         *
         *   strcspn(input, "\n") scans 'input' and returns the
         *   index of the first '\n' character it finds (or the
         *   index of the string's own null terminator, if there is
         *   no '\n' -- which happens if the typed line was longer
         *   than our buffer).
         *
         * Writing '\0' at that index shortens the string right
         * there, effectively deleting the newline (and anything
         * after it) from what we treat as the input. */
        input[strcspn(input, "\n")] = '\0';

        /* --------------------------------------------------
         * Rule 1: exact command "exit" shuts the harness down.
         * --------------------------------------------------
         * strcmp(a, b) returns 0 only when the two strings are
         * IDENTICAL. Using strcmp (rather than a substring check)
         * on purpose here, so only a line that is exactly "exit"
         * quits the program -- a line like "please exit now" will
         * fall through to the default echo rule instead. */
        if (strcmp(input, "exit") == 0) {
            printf("Goodbye!\n");
            break; /* leave the while(1) loop -> program ends */
        }

        /* --------------------------------------------------
         * Rule 2: any input containing "hello" gets a greeting.
         * --------------------------------------------------
         * strstr(haystack, needle) searches for 'needle' inside
         * 'haystack' and returns a pointer to the first place it
         * occurs, or NULL if 'needle' never appears. Because this
         * is a substring search (not an exact match), inputs like
         * "hello there" or "oh, hello!" both trigger the greeting.
         * This check is case-sensitive, matching the assignment's
         * example spec. */
        if (strstr(input, "hello") != NULL) {
            printf("Model: Hello there! How can I help you today?\n");
            continue; /* skip the echo rule below; loop again */
        }

        /* --------------------------------------------------
         * Rule 3 (default): echo whatever the user typed.
         * --------------------------------------------------
         * If we reach this line, the input was neither "exit" nor
         * did it contain "hello", so the mock model's "response"
         * is simply the user's own text reflected back to them. */
        printf("Model: %s\n", input);
    }

    /* Returning 0 signals to the shell/OS that the program exited
     * normally, with no errors. */
    return 0;
}