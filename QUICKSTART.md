# VDR menuorgedit Plugin - Quick Start Guide

## 🚀 Schnellstart

### 1. Voraussetzungen prüfen

```bash
# VDR Development Headers
dpkg -l | grep vdr-dev

# libxml2 Development
dpkg -l | grep libxml2-dev

# Falls fehlend, installieren:
sudo apt-get install vdr-dev libxml2-dev build-essential
```

### 2. Kompilieren

```bash
cd vdr-plugin-menuorgedit
make
```

Bei Erfolg wird `libvdr-menuorgedit.so` erstellt.

### 3. Installieren

```bash
# Automatische Installation mit Script
sudo ./install.sh install

# ODER manuell
sudo make install
sudo mkdir -p /etc/vdr/plugins/menuorg
sudo cp examples/menuorg.xml /etc/vdr/plugins/menuorg/
```

### 4. VDR starten

```bash
# Plugin zur VDR-Konfiguration hinzufügen
# In /etc/vdr/plugins.conf oder beim Start:
vdr -P menuorgedit
```

### 5. Plugin verwenden

1. VDR Hauptmenü öffnen
2. "Menü-Editor" auswählen
3. Mit Fernbedienung Menü bearbeiten:
   - **Rot**: Neuen Eintrag hinzufügen
   - **Grün**: Eintrag bearbeiten
   - **Gelb**: Nach oben verschieben
   - **Blau**: Löschen
   - **0**: Nach unten verschieben
   - **OK**: Speichern

## 📁 Projektstruktur

```
vdr-plugin-menuorgedit/
│
├── menuorgedit.h/.c        # Haupt-Plugin (cPlugin Interface)
├── menuorgstructure.h/.c   # Datenmodell (Menüstruktur)
├── menuorgxml.h/.c         # XML Parser (libxml2)
├── menuorgmenu.h/.c        # OSD Menüs (cOsdMenu)
│
├── Makefile                # Build-System
├── README.md               # Vollständige Dokumentation
├── HISTORY                 # Versionshistorie
├── COPYING                 # GPL-2 Lizenz
├── install.sh              # Installations-Script
│
├── po/                     # Übersetzungen
│   └── de_DE.po           # Deutsche Übersetzung
│
└── examples/               # Beispiel-Konfigurationen
    └── menuorg.xml        # Beispiel menuorg.xml
```

## 🔧 Kompilierungs-Optionen

```bash
# Standard-Build
make

# Debug-Build
make CXXFLAGS="-g -O0 -DDEBUG"

# Nur kompilieren (nicht installieren)
make all

# Aufräumen
make clean

# Distribution-Paket erstellen
make dist
```

## 🎯 Verwendungsbeispiele

### Beispiel 1: System-Menü hinzufügen

1. **Rot** drücken (Neu)
2. Typ: "System Menu"
3. System Menu: "Schedule" auswählen
4. **OK** drücken

### Beispiel 2: Plugin-Eintrag erstellen

1. **Rot** drücken
2. Typ: "Plugin"
3. Name: "mp3"
4. Titel: "Musikplayer"
5. **OK** drücken

### Beispiel 3: Untermenü anlegen

1. **Rot** drücken
2. Typ: "Submenu"
3. Name: "Multimedia"
4. **OK** drücken
5. Weitere Einträge als Kinder hinzufügen

### Beispiel 4: Befehl definieren

1. **Rot** drücken
2. Typ: "Command"
3. Name: "Speicherplatz"
4. Ausführen: "/usr/bin/df -h"
5. Bestätigen: "Ja"
6. **OK** drücken

## 🐛 Fehlersuche

### Plugin lädt nicht

```bash
# VDR Log prüfen
journalctl -u vdr -f | grep menuorgedit

# oder
tail -f /var/log/syslog | grep vdr
```

### Kompilierungsfehler

```bash
# VDR Version prüfen
vdr --version

# Sollte >= 2.7.8 sein
# Bei älteren Versionen API-Änderungen möglich
```

### XML-Parser Fehler

```bash
# XML-Datei validieren
xmllint --noout /etc/vdr/plugins/menuorg/menuorg.xml

# Berechtigungen prüfen
ls -la /etc/vdr/plugins/menuorg/
# VDR-User muss Schreibrechte haben
```

### Änderungen nicht sichtbar

```bash
# menuorg-Plugin muss geladen sein!
# VDR neu starten oder Plugins neu laden:
svdrpsend PLUG menuorg RELO
```

## 📝 Wichtige Hinweise

1. **Backup**: Das Plugin erstellt automatisch `menuorg.xml.backup` vor dem Speichern

2. **menuorg Plugin**: Das menuorg-Plugin muss installiert sein, damit die XML-Datei 
   beim VDR-Start verarbeitet wird

3. **Berechtigungen**: VDR benötigt Schreibrechte im Config-Verzeichnis:
   ```bash
   sudo chown -R vdr:vdr /etc/vdr/plugins/menuorg
   ```

4. **Neustart**: Nach Änderungen an menuorg.xml muss VDR neu gestartet werden 
   (oder SVDRP RELO-Befehl)

## 🔗 Weiterführende Links

- VDR Homepage: https://www.tvdr.de/
- VDR Wiki: https://www.vdr-wiki.de/
- Plugin Development: https://www.vdr-wiki.de/wiki/index.php/Plugin_development

## ✨ Features in Planung

- [ ] Verbesserte Untermenü-Navigation
- [ ] Drag & Drop Unterstützung
- [ ] Import/Export von Menü-Templates
- [ ] Plugin-Auto-Erkennung
- [ ] Visuelle Vorschau
- [ ] Undo/Redo Funktion

## 📧 Support

Bei Problemen oder Fragen:
1. README.md vollständig lesen
2. Log-Dateien prüfen
3. VDR-Portal.de Forum nutzen
4. Issue im Repository öffnen (falls vorhanden)
