Answer all questions in german.
Write all code comments, README, AUR-README, CHANGELOG, MAN and GitHub commit messages in english.
This project follows a custom versioning scheme: "0.year.month.day.hourminute", (e.g., 0.2025.07.08.1759 = Year 2025, July 8th, 17:59).
This project uses C99 standard. All code should be compliant with ISO/IEC 9899:1999 (C99).
Always check return values of malloc(), calloc(), realloc().
Free all dynamically allocated memory.
Set pointers to NULL after freeing.
Use include guards: #ifndef HEADER_H / #define HEADER_H / #endif.
Include only necessary headers.
Separate system headers from local headers.
Function names should be verbs: calculate_sum(), parse_input().
Use snake_case for function names and variables: my_function(), user_count.
Use UPPER_CASE for constants and macros: MAX_SIZE, PI.
Use PascalCase for typedef structs: MyStruct.
Use forward declarations when possible.
Use descriptive names, avoid abbreviations.
Opening braces on the same line for functions and control structures (K&R style).
Use Doxygen-style comments for functions. Include a brief description, parameters, return values, and examples if useful.
Include brief description, parameters, return values, and examples.
Document complex algorithms and data structures.
Add comments for non-obvious code sections.
Handle all dependencies clearly in the build system (e.g. Makefile, PKGBUILD) and document them in README and related documentation.
Each entry in CHANGELOG.md must be less than or equal to 1000 characters.
Before every GitHub commit, always check and update the "VERSION" in Makefile and "pkgver" in PKGBUILD so that both match the current release.
Always commit with an English message less than 100 characters.