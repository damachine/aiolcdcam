---
applyTo: "**"
---

Use german language to answer chat questions.
Use english language in the README.md, AUR-README.md, CHANGELOG.md and the Linux man files. 
Use english language for code comments.
Use Doxygen-style comments for functions. Include a description, @brief, @param, @return values and examples if useful.
Opening braces on the same line for functions and control structures (K&R style).
Add comments for non-obvious code sections.

This project uses C99 standard.
All code should be compliant with ISO/IEC 9899:1999 (C99).
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
Document complex algorithms and data structures.
