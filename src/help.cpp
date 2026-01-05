// help.cpp
/*
 * foreach — a small, cross-platform Unix-style utility
 * Help and versioning helpers
 *
 * Author: David Joffe
 * URL:    https://github.com/djsoftware1/foreach
 *
 * Copyright (c) 2026 David Joffe
 * License: MIT License (see LICENSE file)
 *
 * File created 5 Jan 2026 - David Joffe
 */

#include <iostream>
#include "help.h"

static const char* FOREACH_VERSION = "1.0.0";

void print_version() {
    std::cout << "for-each " << FOREACH_VERSION << "\n";
}

void print_help() {
    std::cout <<
R"(for-each — execute a command once per input line

Usage:
  for-each [options] command [args...]

Options:
  -h, --help
        Show this help and exit.

  --version
        Show version information and exit.

  -s, --space-delim
        Split input fields on whitespace instead of tabs.

  --delim CHAR
        Split input fields on the specified character.

  --include-empty
        Process empty or whitespace-only input lines.
        By default, such lines are skipped.

Variable expansion:
  $1, $2, ...
        Positional fields from the input line.

  $*
        The entire input line.

  $#
        Input line number (1-based).

Behavior:
  If the command template contains no $ placeholders,
  the input line is automatically appended as the final argument.

  If any $ placeholders are present, no implicit argument
  is added.

Examples:
  printf "a\nb\n" | for-each echo
  printf "x\ty\n" | for-each echo "$1 -> $2"
  ls *.md | for-each pandoc '$1' -o '$*.html'

Project:
  https://github.com/djsoftware1/foreach
)";
}
