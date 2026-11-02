#!/usr/bin/env bash
# ============================================================
# test.sh
#
# ECE 309 Project 1 - Phase 4: AI-Assisted Testing
#
# This script deterministically tests harness.c by piping fixed,
# predefined input into the compiled "harness" program and
# checking that its output contains exactly the text we expect.
# Because the input is hardcoded (not typed by a human), running
# this script always produces the same result -- no interaction
# required, which is exactly what's needed to "prove the harness
# works without human interaction."
#
# Usage:
#   bash test.sh
#
# Assumes harness.c is in the same directory as this script.
# ============================================================

# "set -u" tells bash to treat using an undefined variable as an
# error and stop. This helps catch typos in this script itself.
# We deliberately do NOT use "set -e" (exit on any error), because
# we want ALL test cases to run and be reported, rather than the
# script quitting after the first failure.
set -u

# Counters so we can print a final summary and use a meaningful
# exit code (0 = everything passed, 1 = at least one test failed).
PASS_COUNT=0
FAIL_COUNT=0

# --------------------------------------------------------------
# Step 1: (Re)compile the harness, using the exact command from
# the assignment. Compiling here -- rather than assuming a
# pre-built binary exists -- means this script always tests
# whatever the current harness.c actually contains.
# --------------------------------------------------------------
echo "Compiling harness.c..."
gcc harness.c -o harness
if [ $? -ne 0 ]; then
    echo "FAIL: harness.c did not compile. Aborting tests."
    exit 1
fi
echo "Compiled successfully."
echo ""

# --------------------------------------------------------------
# run_test: a small helper so we don't repeat the same
# "send input -> capture output -> check it -> report" logic for
# every test case.
#
# Arguments:
#   $1 - a human-readable description of what's being tested
#   $2 - the input to send to the harness (use \n between lines,
#        since printf "%b" below will expand it into real newlines,
#        simulating multiple turns typed by a user)
#   $3 - a snippet of text we expect to find somewhere in the
#        harness's output if it behaved correctly
# --------------------------------------------------------------
run_test() {
    local description="$1"
    local input_text="$2"
    local expected_text="$3"

    # printf "%b" expands backslash sequences (like \n) in
    # input_text into real newline characters, then that text is
    # piped in as if a user were typing each line and pressing
    # Enter. Standard output from the harness is captured into
    # actual_output.
    actual_output=$(printf "%b" "$input_text" | ./harness)

    # grep -q runs quietly and only sets an exit status: 0 if the
    # fixed string was found in actual_output, 1 if it was not.
    # -F treats expected_text as a literal string, not a regex
    # pattern, which avoids surprises if it contains characters
    # like "?" or "!".
    if echo "$actual_output" | grep -qF "$expected_text"; then
        echo "PASS: $description"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "FAIL: $description"
        echo "  Expected output to contain: $expected_text"
        echo "  --- Actual output was ---"
        echo "$actual_output" | sed 's/^/  /'
        echo "  --------------------------"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi
}

# --------------------------------------------------------------
# Step 2: Test cases. Each one sends a small, fixed "conversation"
# into the harness and checks for the exact behavior the spec
# requires.
# --------------------------------------------------------------

# The word "hello" by itself should trigger the hardcoded greeting.
run_test \
    "'hello' triggers the greeting" \
    "hello\nexit\n" \
    "Model: Hello there! How can I help you today?"

# "hello" appearing inside a longer sentence should ALSO trigger
# the greeting, since the harness does a substring search rather
# than requiring an exact match.
run_test \
    "'hello' inside a sentence still triggers the greeting" \
    "oh hello there friend\nexit\n" \
    "Model: Hello there! How can I help you today?"

# Any other input should just be echoed straight back, prefixed
# with "Model: ".
run_test \
    "unrecognized input is echoed back verbatim" \
    "some random text\nexit\n" \
    "Model: some random text"

# Typing "exit" on its own should print a goodbye message and end
# the program cleanly (rather than hanging or crashing).
run_test \
    "'exit' shuts the program down cleanly" \
    "exit\n" \
    "Goodbye!"

# A line that merely contains "exit" as part of a longer sentence
# should NOT quit the program (the harness checks for an exact
# match on "exit", not a substring) -- it should be echoed back
# like any other input, and the program should still be running
# to accept the real "exit" afterward.
run_test \
    "'exit' inside a longer sentence does NOT quit early" \
    "please exit now\nexit\n" \
    "Model: please exit now"

# --------------------------------------------------------------
# Step 3: Print a summary and exit with a status that reflects
# whether every test passed (0) or not (1) -- useful if this is
# ever run automatically (e.g., by a grading script).
# --------------------------------------------------------------
echo ""
echo "================================"
echo "Results: $PASS_COUNT passed, $FAIL_COUNT failed"
echo "================================"

if [ "$FAIL_COUNT" -eq 0 ]; then
    exit 0
else
    exit 1
fi