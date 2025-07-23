---
applyTo: "**"
---

**Sprachregeln**
- Beantworte Chat-Fragen auf Deutsch.
- Nutze Englisch für README.md, AUR-README.md, CHANGELOG.md und Linux-Manpages.
- Schreibe Code-Kommentare grundsätzlich auf Englisch.

**Kommentar- und Dokumentationsstil**
- Schreibe Code-Kommentare grundsätzlich auf Englisch.
- Verwende Doxygen-Stil für Funktionskommentare
- Nutze `@todo` für unvollständige Funktionen oder Features.
- Nutze `@bug` für bekannte Fehler oder Probleme.
- Nutze `@note` für wichtige Hinweise oder Besonderheiten.
- Nutze `@warning` für potenziell gefährliche oder kritische Codeabschnitte.
- Nutze `@see` für Verweise auf verwandte Funktionen oder Dokumentation.
- Nutze `@example` für Codebeispiele, die die Nutzung einer Funktion demonstrieren.
- Nutze `@deprecated` für veraltete Funktionen oder Features.
- Nutze `@since` für die Angabe, seit welcher Version eine Funktion verfügbar ist.
- Nutze `@version` für die Angabe der aktuellen Version der Funktion.
- Nutze `@author` für die Angabe des Autors der Funktion.
- Nutze `@copyright` für die Angabe von Urheberrechten.
- Nutze `@license` für die Angabe der Lizenzbedingungen.
- Nutze `@param[in]` und `@param[out]` für Eingabe- und Ausgabeparameter.
- Nutze `@return` für die Rückgabewerte von Funktionen.
- Nutze `@throws` für die Angabe von Ausnahmen, die eine Funktion werfen kann.
- Nutze `@pre` für Vorbedingungen, die erfüllt sein müssen, bevor die Funktion aufgerufen wird.
- Nutze `@post` für Nachbedingungen, die nach dem Aufruf der Funktion erfüllt sein müssen.
- Nutze `@brief` für eine kurze Zusammenfassung der Funktion.
- Nutze `@details` für eine ausführliche Beschreibung der Funktion.
- Nutze `@param` für die Beschreibung von Funktionsparametern.
- Öffnende geschweifte Klammern stehen bei Funktionen und Kontrollstrukturen in derselben Zeile (K&R-Stil).
- Kommentiere alle nicht sofort verständlichen Codeabschnitte.
- Nutze Inline-Kommentare sparsam, nur wenn nötig.
- Vermeide redundante Kommentare, die den Code wiederholen.
- Dokumentiere komplexe Algorithmen und Datenstrukturen ausführlich.

**Code-Richtlinien (C99)**
- Halte dich an ISO/IEC 9899:1999 (C99).
- Überprüfe Rückgabewerte von malloc(), calloc(), realloc().
- Gib dynamisch reservierten Speicher frei und setze Zeiger danach auf NULL.
- Verwende Include Guards: `#ifndef HEADER_H` / `#define HEADER_H` / `#endif`.
- Binde nur notwendige Header ein; trenne System- und lokale Header.
- Halte den Code sauber und lesbar: Einrückung mit 4 Leerzeichen, keine Tabulatoren.
- Vermeide unnötigen Code und redundante Kommentare.
- Nutze `const` für unveränderliche Variablen und Funktionsparameter.
- Nutze `static` für Funktionen und Variablen, die nur in der Datei sichtbar sein sollen.
- Nutze `inline` für kleine, häufig genutzte Funktionen.
- Halte den Code strukturiert: Trenne Deklarationen, Definitionen und Implementierungen.

**Namenskonventionen**
- Funktionsnamen sind Verben im snake_case, z.B. `calculate_sum()`, `parse_input()`.
- Variablen im snake_case, z.B. `user_count`.
- Konstanten und Makros in UPPER_CASE, z.B. `MAX_SIZE`, `PI`.
- Strukturen per typedef im PascalCase, z.B. `MyStruct`.
- Nutze möglichst Forward Declarations.
- Wähle beschreibende Namen, vermeide Abkürzungen.
- Nutze sprechende Namen für Variablen, Funktionen und Strukturen.
- Vermeide kryptische Abkürzungen, außer sie sind allgemein bekannt.
- Nutze `enum` für Status- und Fehlercodes, z.B. `enum Status { SUCCESS, ERROR }`.
- Nutze `typedef` für komplexe Datentypen, z.B. `typedef struct { int x; int y; } Point;`.
- Halte die Namenskonventionen konsistent im gesamten Projekt ein.

**Weitere Hinweise**
- Dokumentiere komplexe Algorithmen und Datenstrukturen ausführlich.
