# Changelog

Alle bemerkenswerten Änderungen an diesem Projekt werden in dieser Datei dokumentiert.

Das Format basiert auf [Keep a Changelog](https://keepachangelog.com/de/1.0.0/),
und dieses Projekt folgt der [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.1.2] - 2025-07-08

### Added
- **CoolerControl Setup-Anweisungen**: Detaillierte Anleitung zur korrekten LCD-Konfiguration
- **LCD Image-Modus Requirement**: Expliziter Hinweis, dass LCD auf "Image"-Modus gestellt werden muss
- **Erweiterte Dokumentation**: Vollständige Setup-Schritte in README und Manpage
- **Manpage-Verbesserungen**: SETUP-Sektion mit Schritt-für-Schritt-Anleitung

### Changed
- **Binary-Name**: Von `nzxt` zu `aiolcdcam` für bessere Eindeutigkeit
- **Service-Name**: Konsistente Benennung als `aiolcdcam.service`
- **PID-Datei**: Von `/var/run/nzxt_cam.pid` zu `/var/run/aiolcdcam.pid`
- **Dokumentation**: Alle Beispiele und Referenzen auf `aiolcdcam` aktualisiert
- **Manpage**: Korrigierte Service-Namen und Binary-Referenzen

### Fixed
- **Konsistente Benennung**: Einheitliche Verwendung von `aiolcdcam` in allen Dateien
- **Setup-Dokumentation**: Klarstellung der CoolerControl-Konfigurationsschritte
- **Service-Referenzen**: Korrekte systemd-Service-Namen in Dokumentation

---

## [0.1.1] - 2025-07-06

### Added
- **Single-Instance-Enforcement**: Nur eine aiolcdcam Daemon-Instanz kann gleichzeitig laufen
- **Robuste PID-Datei-Verwaltung**: `/var/run/aiolcdcam.pid` für systemd-kompatible Prozessverwaltung
- **Intelligente Instanz-Koordination**: Service vs. manuelle Starts werden korrekt gehandhabt
- **Shutdown-Image in C**: Direktes Senden des face.png Bildes beim Daemon-Stopp
- **Makefile-Prozessmanagement**: Automatisches Beenden aller laufenden `aiolcdcam` Prozesse vor Installation
- **Vollständige Englische Ausgaben**: Alle benutzerorientierten Meldungen auf Englisch
- **Erweiterte Fehlerbehandlung**: EPERM-Behandlung für root-owned Prozesse
- **🆕 Automatische Dependency-Installation**: `make install` erkennt automatisch Linux-Distribution und installiert fehlende Abhängigkeiten
- **🆕 Distribution-Auto-Detection**: Unterstützung für Arch, Debian/Ubuntu, Fedora, RHEL/CentOS, openSUSE
- **🆕 Ein-Befehl-Installation**: Komplette Installation mit `sudo make install` (Dependencies + Build + Service-Setup)
- **🆕 Universelle Kompatibilität**: Funktioniert out-of-the-box auf allen großen Linux-Distributionen
- **🆕 Robuste Paket-Installation**: Automatische Fallback-Informationen bei unbekannten Distributionen

### Changed
- **PID-Datei-Speicherort**: Von `/tmp/aiolcdcam.pid` zu `/var/run/aiolcdcam.pid` (systemd-kompatibel)
- **Signal-Handler**: Integriertes Shutdown-Image-Versenden direkt in C
- **Makefile**: Erweiterte Service- und Prozess-Erkennung mit `killall`
- **Daemon-Ausgaben**: Vollständige Übersetzung deutscher Ausgaben ins Englische
- **Systemd-Service**: Vereinfachte Service-Datei ohne ExecStop (Shutdown in C gehandhabt)
- **🆕 Makefile-Install-Target**: Jetzt mit automatischer Dependency-Prüfung und -Installation
- **🆕 Help-Ausgabe**: Bereinigte `make help` ohne manuelle Dependency-Targets (intern gehandhabt)
- **🆕 Benutzerfreundlichkeit**: Ein Befehl für komplette Installation statt mehrere Schritte
- **🆕 Distribution-Agnostisch**: Keine manuellen Abhängigkeits-Installationsschritte mehr nötig
- **🆕 Makefile-Struktur**: Interne Targets für saubere Benutzer-API

### Fixed
- **Doppelte Instanzen**: Verhindert mehrfache gleichzeitige Daemon-Ausführung
- **Service-Konflikte**: Manueller Start schlägt fehl wenn systemd Service läuft
- **Zombie-Prozesse**: Robuste Beendigung von hängenden Instanzen
- **Shutdown-Race-Conditions**: `shutdown_sent` Flag verhindert doppelte Image-Übertragung
- **Permissions-Fehler**: Korrekte Behandlung von EPERM bei Prozess-Checks

### Security
- **Prozess-Isolation**: Einzelne Daemon-Instanz verhindert Resource-Konflikte
- **PID-Datei-Sicherheit**: Sichere PID-Verwaltung in `/var/run/`

---

## [0.1.0] - 2025-07-05

### Added
- **Modulare C-Architektur**: Vollständige Aufspaltung in separate Module
  - `cpu_monitor.c/.h` - CPU-Temperatur und Auslastung
  - `gpu_monitor.c/.h` - GPU-Temperatur und Auslastung (NVIDIA)
  - `coolant_monitor.c/.h` - Coolant-Temperatur-Überwachung
  - `display.c/.h` - Rendering-Engine und Modi-Logik
  - `coolercontrol.c/.h` - REST-API-Kommunikation
- **Zentrale Konfiguration**: Alle Einstellungen in `include/config.h`
- **4 Display-Modi**:
  - `def` - Nur Temperaturen (ressourcenschonend)
  - `1` - Temperaturen + vertikale Auslastungsbalken
  - `2` - Temperaturen + Kreisdiagramme
  - `3` - Temperaturen + horizontale Auslastungsbalken
- **I/O-Optimierung**: Modi-spezifische Sensordaten-Abfrage
- **Änderungserkennung**: Display-Updates nur bei signifikanten Änderungen
- **Professionelle systemd-Integration**: Vollständige Service-Unterstützung
- **Umfassendes Build-System**: Modulare Makefile mit Abhängigkeitserkennung

### Changed
- **Projektstruktur**: Von Monolith zu modularer Standard-C-Architektur
- **Performance**: Intelligentes Caching und selektive Sensor-Abfrage
- **Wartbarkeit**: Saubere Trennung von Logik und Konfiguration

### Fixed
- **Memory-Leaks**: Vollständige Cairo-Ressourcen-Verwaltung
- **Performance-Bottlenecks**: Optimierte Rendering-Pipeline
- **Code-Duplikation**: Eliminierung redundanter Funktionen

---

## [0.0.1] - 2025-07-04

### Added
- **Erste funktionsfähige Version**: Monolithisches C-Programm `aiolcdcam.c`
- **NZXT Kraken LCD-Unterstützung**: Anzeige von CPU/GPU-Temperaturen
- **CoolerControl-Integration**: REST-API-Kommunikation über curl
- **Cairo-basiertes Rendering**: PNG-Generierung für LCD-Display
- **Grundlegende Modi**: Temperatur-only und Auslastungsanzeigen
- **hwmon-Integration**: Automatische CPU/Coolant-Sensor-Erkennung
- **nvidia-smi-Integration**: GPU-Temperatur und Auslastungsdaten

### Technical Details
- **Sprache**: C99 mit GCC-Optimierungen
- **Abhängigkeiten**: libcairo, libcurl, nvidia-smi
- **Target**: Linux-Systeme mit NZXT Kraken AIO-Kühlern
- **Display**: 240x240 Pixel LCD mit PNG-Upload

---

## Geplante Features (Roadmap)

### [0.2.0] - Geplant
- **Unabhängigkeit von CoolerControl**: Direkte USB/HID-Kommunikation mit NZXT-Geräten
- **Native NZXT-Protokoll**: Implementierung des nativen NZXT Kraken-Protokolls
- **Standalone-Betrieb**: Komplette Elimination der CoolerControl-Abhängigkeit
- **Multi-Device-Support**: Unterstützung mehrerer NZXT-Geräte
- **Konfigurationsdateien**: YAML/JSON-basierte Konfiguration
- **Erweiterte Modi**: Mehr Visualisierungsoptionen
- **Logging-System**: Strukturiertes Logging mit Log-Levels

### [0.3.0] - Geplant
- **Plugin-Architektur**: Erweiterbare Sensor-Module
- **libusb-Integration**: Direkte USB-Kommunikation ohne Daemon-Abhängigkeiten
- **Reverse-Engineering**: Vollständige NZXT-Protokoll-Dokumentation
- **Web-Interface**: Browser-basierte Konfiguration
- **Alerts und Schwellwerte**: Temperatur-Benachrichtigungen
- **Historische Daten**: Temperatur-Logging und Trends
- **Performance-Profiling**: Detaillierte Leistungsmetriken

### [0.4.0] - Langfristig
- **Universal NZXT Support**: Kraken, Smart Device V2, RGB & Fan Controller
- **Community Protocol**: Open-Source NZXT-Protokoll-Bibliothek
- **Cross-Platform**: Windows und macOS Unterstützung
- **Hardware-Detection**: Automatische NZXT-Geräteerkennung

---

## Entwicklungsrichtlinien

- **Semantic Versioning**: MAJOR.MINOR.PATCH Format
- **Keep a Changelog**: Strukturierte Änderungsdokumentation
- **C99 Standard**: Moderne C-Entwicklung mit GCC-Optimierungen
- **Modulare Architektur**: Saubere Trennung von Funktionalitäten
- **Performance First**: Ressourcenschonende Implementierung
- **Production Ready**: Robuste Fehlerbehandlung und systemd-Integration

---

**Autor**: DAMACHINE (christkue79@gmail.com)  
**Lizenz**: MIT License  
**Repository**: LCD AIO CAM - Modularer C-Daemon
