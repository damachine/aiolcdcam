---
applyTo: "**"
---

**Sprachregeln**
- Beantworte Chat-Fragen auf Deutsch.
- Nutze Englisch für README.md, AUR-README.md, CHANGELOG.md und Linux-Manpages.
- Schreibe Code-Kommentare grundsätzlich auf Englisch.

**Kommentar- und Dokumentationsstil**
- Verwende Doxygen-Stil für Funktionskommentare: Beschreibung, @brief, @param, @return, ggf. Beispiele.
- Öffnende geschweifte Klammern stehen bei Funktionen und Kontrollstrukturen in derselben Zeile (K&R-Stil).
- Kommentiere alle nicht sofort verständlichen Codeabschnitte.

**Code-Richtlinien (C99)**
- Halte dich an ISO/IEC 9899:1999 (C99).
- Überprüfe Rückgabewerte von malloc(), calloc(), realloc().
- Gib dynamisch reservierten Speicher frei und setze Zeiger danach auf NULL.
- Verwende Include Guards: `#ifndef HEADER_H` / `#define HEADER_H` / `#endif`.
- Binde nur notwendige Header ein; trenne System- und lokale Header.

**Namenskonventionen**
- Funktionsnamen sind Verben im snake_case, z.B. `calculate_sum()`, `parse_input()`.
- Variablen im snake_case, z.B. `user_count`.
- Konstanten und Makros in UPPER_CASE, z.B. `MAX_SIZE`, `PI`.
- Strukturen per typedef im PascalCase, z.B. `MyStruct`.
- Nutze möglichst Forward Declarations.
- Wähle beschreibende Namen, vermeide Abkürzungen.

**Weitere Hinweise**
- Dokumentiere komplexe Algorithmen und Datenstrukturen ausführlich.
