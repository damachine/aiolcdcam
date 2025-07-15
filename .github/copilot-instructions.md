Answer all questions in chat in German language.

Answer all questions in less than 1000 characters, and words of no more than 12 characters.

Use @c for inline code snippets and @code for code blocks in documentation.

All code comments, README files, CHANGELOG entries, and documentation should be written in English standard README format and Keep a Changelog format (https://keepachangelog.com/en/1.0.0/).

All code examples should be written in C, as this is the primary language for our embedded device project.

When providing code examples, ensure they are concise and relevant to the question asked. Avoid overly complex examples that may confuse the reader.

This is an embedded device project with resource constraints, so consider memory usage, stack size, and avoid dynamic allocation where possible.

We use C99 as our standard for embedded device compatibility, so always generate code that follows C99 conventions and use `-std=c99` in compiler examples.

We follow K&R brace style with 4 spaces for indentation (no tabs) and use snake_case for functions and variables.

We require strict error handling - always check return values from functions like malloc(), fopen(), and system calls.

Memory safety is critical - always pair malloc() with free(), use strncpy() instead of strcpy(), and include bounds checking for array access.

We organize code with src/ for source files and include/ for headers, following typical C project structure.

When suggesting security practices, focus on C-specific vulnerabilities like buffer overflows, format string attacks, and integer overflows.

We prefer POSIX-compliant functions for portability and avoid platform-specific code when possible.
