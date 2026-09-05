# Project 1 Vibe Coding Log

Claude Sonnet 5 was used for this project.

---

Me:
> I am creating an AI harness as part of a course at NC State. This project is only the first part of the harness (which will be completed over the course of the semester); there's no need for it to be too complicated. The instructions given by my professor are complex - for now, only complete the main program and nothing else. Create a simple command line program in C. Do not use any external libraries. Do not use any features that will not compile in a POSIX environment. The program should run an infinite while loop that asks for user input (using fgets). If the user types "exit", the program should end immediately. If the user types any text containing the word "hello", the program should print a hard-coded greeting. If the user types anything else, their input should be echoed. The code should be heavily commented, with plenty of explanation as to what it is doing.
> 
> [PDF of project spec]

Model:
> Compiles clean with `-Wall -Wextra -pedantic` and behaves correctly against a piped test (`hello there` → greeting, `some random text` → echoed, `exit` → clean shutdown). Copying it to your outputs now.
> 
> I compiled and tested it before handing it off — no external libraries, just `<stdio.h>` and `<string.h>`, and `gcc harness.c -o harness` builds it exactly as the assignment specifies.
> 
> Quick rundown of the logic:
> 
> - **Loop**: `while (1)` prints `You: `, reads a line with `fgets` into a fixed 1024-byte buffer, and trims the trailing newline with `strcspn`.
> - **Exit**: `strcmp(input, "exit") == 0` checks for an exact match, so it only quits on a bare "exit" — not "please exit."
> - **Hello**: `strstr(input, "hello") != NULL` does a substring search, so "hello there" or "oh hello!" both trigger the hardcoded greeting.
> - **Echo**: anything else falls through to `printf("Model: %s\n", input)`.
> - It also treats a closed input stream (`fgets` returning `NULL`) as a graceful shutdown rather than crashing — handy since your Phase 4 test script will pipe input in and then run dry.
> 
> Since this is Phase 1 only, there's no conversation history or tool execution yet — that's intentionally left for the next parts of the project.
> 
> [C code in harness.c]

Me:
> Create a simple Bash test script (test.sh) to deterministically verify the output of harness.c

Model:
> All 5 checks pass. Let's also confirm it correctly catches a failure (so you know it's not just always printing PASS) before handing it off.
> 
> I built and ran it against your actual `harness.c` before sending it over — 5/5 checks pass, and I also verified it correctly reports FAIL (with a diff of expected vs. actual output) when something doesn't match, so it's a real check rather than a script that just prints PASS.
> 
> What it does:
> 
> - Compiles `harness.c` fresh each run, so it always tests your current code.
> - A `run_test` helper pipes a fixed "conversation" (e.g. `"hello\nexit\n"`) into `./harness` via `printf "%b"`, captures the output, and greps for the expected snippet.
> - Five deterministic cases: plain `"hello"`, `"hello"` embedded in a sentence, an unrecognized line getting echoed, a clean `"exit"`, and a sentence merely containing "exit" (which should not quit early, since your harness uses `strcmp` for an exact match there).
> - Prints a pass/fail count and exits `0` only if everything passed — handy if a grader or CI script runs it non-interactively.
> 
> To run it yourself: put `test.sh` next to `harness.c` and run `bash test.sh`.
> 
> [Bash code in test\.sh]