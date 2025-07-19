## [0.25.07.19.1] - 2025-07-19

### Changed
- Version updated to 0.25.07.19.1 for new release.
- Full compliance with all coding guidelines from copilot-instructions.md re-checked and confirmed (C99, Doxygen, memory, naming, documentation, system integration).
- No open ToDos or violations of project requirements.
- Project is guideline-compliant, robust, and ready for release or further development.

---

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project follows a custom versioning scheme: **0.year.month.day.hourminute**
(e.g., 0.2025.07.08.1759 = Year 2025, July 8th, 17:59)

---

## [0.2025.07.18.2350] - 2025-07-18

### Added
- Introduced a new rebase-style, high-performance I/O subsystem for all temperature and display data paths.
- I/O logic now uses a top-down, zero-copy buffer management approach for maximum throughput and minimal latency.
- All display and sensor data flows are now fully decoupled and use direct memory mapping where possible.
- Reworked the update pipeline to minimize system calls and context switches, resulting in significantly improved responsiveness.
- Documentation and Doxygen comments updated to reflect new I/O architecture and data flow.

### Technical
- The new I/O subsystem is fully C99-compliant and follows all project coding standards.
- All legacy I/O code paths have been removed or refactored for clarity and maintainability.
- No changes to user-facing configuration or interface; all improvements are internal and performance-focused.

---

## [0.2025.07.18.2300] - 2025-07-18

### Changed
- Comprehensive Doxygen documentation for all functions in all code and header files (display, cpu_monitor, gpu_monitor, coolant_monitor, coolercontrol, main).
- All function comments now include: description, @brief, @param, @return, and examples where useful.
- Coding conventions strictly enforced: English comments, K&R style, C99, descriptive names, snake_case, UPPER_CASE for macros, PascalCase for typedefs, forward declarations, no abbreviations.
- All dynamically allocated memory is checked and freed, pointers set to NULL after free.
- Only necessary headers included, system and local headers separated.
- Complex algorithms and data structures documented in detail.
- All system-related files (Makefile, systemd/coolerdash.service, coolerdash.install, man/coolerdash.1) reviewed and updated for compliance with project instructions.
- Makefile and install script now include prominent instructions and comments for maintainers and users.
- CHANGELOG.md updated to reflect all documentation and style changes.

### Technical
- Project-wide audit for documentation, code style, and system integration completed.
- No functional changes to program logic, only documentation, style, and maintainability improvements.

---

## [0.2025.07.18.2000] - 2025-07-18

### Changed
- Display layout reworked: now uses a strict two-box (top/bottom, 240x240px) design for CPU (top) and GPU (bottom) temperature monitoring only.
- All mode and coolant-related logic, structures, and parameters removed from codebase.
- Labels "CPU" and "GPU" are now left-aligned and vertically centered in their respective boxes.
- Temperature values (including degree symbol) are centered as a single string, without bearing correction.
- The display image is now sent only once per update, significantly reducing I/O load.
- All warnings and errors during compilation resolved; unused variables and code removed.
- Doxygen-style comments and English code comments added throughout display.c and display.h.
- Code and layout now fully comply with project coding standards (C99, naming, K&R, documentation, etc.).
- Mode selection is no longer available; the program always runs in the fixed two-box layout. Support for selectable display modes (e.g. load bars, circular diagrams) may be reintroduced in a future version if there is sufficient demand.

### Technical
- The project is now a streamlined, maintainable CPU/GPU temperature monitor with a modern two-box layout.
- No coolant or display mode logic remains; codebase is significantly simplified.
- Improved I/O efficiency: redundant image transmissions eliminated.

---

## [0.2025.07.17.2000] - 2025-07-17

### Changed
- Display layout: All temperature boxes (CPU, GPU, coolant) now use a strict 3x80 grid (240x240px, no gap) for perfect centering and alignment.
- CPU and GPU temperature values are now exactly centered in their boxes, coolant temperature is right-aligned and vertically centered.
