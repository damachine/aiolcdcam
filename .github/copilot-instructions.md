---
applyTo: "**"
---

**Sprachregeln**
- Beantworte Chat-Fragen auf Deutsch.
- Nutze Englisch für README.md, AUR-README.md, CHANGELOG.md und Linux-Manpages.
- Schreibe Code-Kommentare grundsätzlich auf Englisch.

**Kommentar- und Dokumentationsstil**
- Schreibe Code-Kommentare grundsätzlich auf Englisch.
- Verwende Doxygen-Stil für Funktionskommentare.
- Öffnende geschweifte Klammern stehen bei Funktionen und Kontrollstrukturen in derselben Zeile (K&R-Stil).
- Kommentiere alle nicht sofort verständlichen Codeabschnitte.
- Nutze Inline-Kommentare sparsam, nur wenn nötig.
- Vermeide redundante Kommentare, die den Code wiederholen.
- Dokumentiere komplexe Algorithmen und Datenstrukturen ausführlich.
- Nutze `@brief` für eine kurze Zusammenfassung der Funktion.
- Nutze `@details` für eine ausführliche Beschreibung der Funktion.
- Nutze `@todo` für unvollständige Funktionen oder Features.
- Nutze `@warning` für potenziell gefährliche oder kritische Codeabschnitte.
- Nutze `@see` für Verweise auf verwandte Funktionen oder Dokumentation.
- Nutze `@example` für Codebeispiele, die die Nutzung einer Funktion demonstrieren.
- Nutze `@pre` für Vorbedingungen, die erfüllt sein müssen, bevor die Funktion aufgerufen wird.
- Nutze `@post` für Nachbedingungen, die nach dem Aufruf der Funktion erfüllt sein müssen.
- Nutze `@param[in]` und `@param[out]` für Eingabe- und Ausgabeparameter.
- Nutze `@return` für die Rückgabewerte von Funktionen.
- Nutze `@param` für die Beschreibung von Funktionsparametern.

**EXTRA Kommentar- und Dokumentationsstil immer am Anfang einer Datei `.c` `.h`**
```c
/*
 * @note Must be initialized before calling read_cpu_temp().
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
- Nutze den EXTRA Kommentar- und Dokumentationsstil immer am anfang einer Datei `.c` `.h` `@note` `@author` `@copyright` `@license` `@version`.
- Entferne den EXTRA Kommentar- und Dokumentationsstil, wenn er nicht am anfang der Datei steht.

**Code-Richtlinien (C99)**
- Halte dich an ISO/IEC 9899:1999 (C99).
- Binde nur notwendige Header ein; trenne System- und lokale Header
- Überprüfe Rückgabewerte von malloc(), calloc(), realloc().
- Gib dynamisch reservierten Speicher frei und setze Zeiger danach auf NULL.
- Halte den Code strukturiert: Trenne Deklarationen, Definitionen und Implementierungen.
- Halte den Code sauber und lesbar: Einrückung mit 4 Leerzeichen, keine Tabulatoren.
- Vermeide unnötigen Code und redundante Kommentare.
- Verwende Include Guards: `#ifndef HEADER_H` / `#define HEADER_H` / `#endif`.
- Nutze `const` für unveränderliche Variablen und Funktionsparameter.
- Nutze `static` für Funktionen und Variablen, die nur in der Datei sichtbar sein sollen.
- Nutze `inline` für kleine, häufig genutzte Funktionen.

**Namenskonventionen**
- Halte die Namenskonventionen konsistent im gesamten Projekt ein.
- Wähle beschreibende Namen, vermeide Abkürzungen.
- Nutze sprechende Namen für Variablen, Funktionen und Strukturen.
- Nutze möglichst Forward Declarations.
- Funktionsnamen sind Verben im snake_case, z.B. `calculate_sum()`, `parse_input()`.
- Variablen im snake_case, z.B. `user_count`.
- Konstanten und Makros in UPPER_CASE, z.B. `MAX_SIZE`, `PI`.
- Strukturen per typedef im PascalCase, z.B. `MyStruct`.
- Vermeide kryptische Abkürzungen, außer sie sind allgemein bekannt.
- Nutze `enum` für Status- und Fehlercodes, z.B. `enum Status { SUCCESS, ERROR }`.
- Nutze `typedef` für komplexe Datentypen, z.B. `typedef struct { int x; int y; } Point;`.

**Weitere Hinweise**
- Dokumentiere komplexe Algorithmen und Datenstrukturen ausführlich.
