# VDR Plugin 'menuorgedit' - Menu Organization Editor

## Description

This plugin provides an OSD-based editor for VDR's menuorg.xml configuration file.
The menuorg plugin allows reorganization of VDR's main menu through an XML file,
but lacks a graphical interface for editing. This plugin fills that gap by providing
a user-friendly OSD interface accessible via remote control.

## Features

- **Visual Menu Editor**: Edit menu structure directly from VDR's OSD
- **Item Management**: Add, edit, delete, and reorder menu items
- **Multiple Item Types**: Support for system menus, plugins, commands, submenus, and separators
- **Live Preview**: See changes in the menu structure immediately
- **Safe Editing**: Automatic backup creation before saving changes
- **Full menuorg.xml Support**: Complete support for all menuorg.xml features

## Requirements

- VDR >= 2.7.8
- libxml2-dev
- menuorg plugin (for XML format compatibility)

## Installation

### Compilation

```bash
# Install dependencies (Debian/Ubuntu)
sudo apt-get install vdr-dev libxml2-dev

# Compile the plugin
cd vdr-plugin-menuorgedit
make

# Install
sudo make install
```

### VDR Configuration

Add the plugin to your VDR startup:

```bash
vdr -P "menuorgedit"
```

Or with custom config directory:

```bash
vdr -P "menuorgedit -c /path/to/config"
```

Default config directory: `/etc/vdr/plugins/menuorg`

## Usage

### Starting the Editor

1. Open VDR main menu
2. Select "Menu Editor"
3. The current menuorg.xml structure will be displayed

### Key Bindings

| Key    | Function                |
|--------|-------------------------|
| Red    | Add new item            |
| Green  | Edit selected item      |
| Yellow | Move item up            |
| Blue   | Delete item             |
| 0      | Move item down          |
| OK     | Save changes            |
| Back   | Exit (confirms if unsaved) |

### Adding Items

1. Press **Red** button
2. Select item type:
   - **System Menu**: VDR built-in menus (Schedule, Channels, etc.)
   - **Plugin**: Entry for installed VDR plugins
   - **Command**: External shell command
   - **Submenu**: Container for other items
   - **Separator**: Visual separator line
3. Fill in required fields:
   - **Name**: Internal identifier
   - **Title**: Display name (optional for plugins)
   - **Execute**: Command path (for Command type)
   - **Confirm**: Ask before executing (for Command type)
4. Press **OK** to confirm

### Editing Items

1. Navigate to desired item
2. Press **Green** button
3. Modify fields as needed
4. Press **OK** to save changes

### Reordering Items

- Press **Yellow** to move item up
- Press **0** to move item down
- Changes are shown immediately in the menu

### Deleting Items

1. Navigate to item
2. Press **Blue** button
3. Confirm deletion

### Saving Changes

Press **OK** at any time to save the current structure to menuorg.xml.
A backup file (menuorg.xml.backup) is automatically created before saving.

## Menu Item Types

### System Menu

VDR built-in menus:
- Schedule
- Channels
- Timers
- Recordings
- Setup
- Commands

### Plugin

Reference to installed VDR plugins. The plugin must be loaded for the menu entry to work.

### Command

Execute external shell commands. Options:
- **Execute**: Full path to command/script
- **Confirm**: Show confirmation dialog before execution

### Submenu

Container for organizing other menu items hierarchically.

### Separator

Visual separator line in menu. Optional title can be provided.

## XML Format

The plugin reads and writes standard menuorg.xml format:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<menus>
    <system name="Schedule"/>
    <system name="Channels"/>
    
    <separator/>
    
    <menu name="Multimedia">
        <plugin name="mp3" title="Music Player"/>
        <plugin name="mplayer"/>
    </menu>
    
    <command name="Disk Space" 
             execute="/usr/bin/df -h" 
             confirm="yes"/>
</menus>
```

### File Location

- Default: `/etc/vdr/plugins/menuorg/menuorg.xml`
- Configurable via `-c` parameter
- Backup: `/etc/vdr/plugins/menuorg/menuorg.xml.backup`

## Troubleshooting

### Plugin doesn't load

Check VDR log for errors:
```bash
journalctl -u vdr | grep menuorgedit
```

### Cannot save configuration

Verify permissions:
```bash
ls -la /etc/vdr/plugins/menuorg/
```

VDR must have write access to the menuorg directory.

### Changes not visible in main menu

After saving:
1. Restart VDR, or
2. Reload plugins: `svdrpsend PLUG menuorg RELO`

Note: The menuorg plugin must be loaded for changes to take effect.

### Item types not available

Ensure the corresponding VDR features are enabled:
- System menus: Built into VDR
- Plugins: Must be installed and loaded
- Commands: VDR's commands.conf must allow custom commands

## Development

### Project Structure

```
vdr-plugin-menuorgedit/
├── menuorgedit.h          # Main plugin header
├── menuorgedit.c          # Plugin implementation
├── menuorgstructure.h     # Data model header
├── menuorgstructure.c     # Data model implementation
├── menuorgxml.h           # XML parser header
├── menuorgxml.c           # XML parser implementation
├── menuorgmenu.h          # OSD menu header
├── menuorgmenu.c          # OSD menu implementation
├── Makefile               # Build configuration
└── README.md              # This file
```

### Building Debug Version

```bash
make CXXFLAGS="-g -O0"
```

### Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Known Issues

- Nested submenu editing is simplified (flat view)
- No drag-and-drop support (use up/down buttons)
- Plugin detection requires VDR restart to see new plugins

## Future Enhancements

- [ ] Enhanced submenu navigation
- [ ] Import/export of menu templates
- [ ] Visual menu preview
- [ ] Plugin auto-detection without restart
- [ ] Undo/redo functionality
- [ ] Menu validation and warnings

## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

See the file COPYING for more information.

## Author

Created for VDR 2.7.8

## See Also

- VDR: https://www.tvdr.de/
- menuorg plugin: Part of VDR standard plugins
- VDR Plugin Development: https://www.vdr-wiki.de/

## Changelog

### Version 0.1.0 (Initial Release)
- Basic OSD menu editor
- Support for all menuorg.xml element types
- Add, edit, delete, reorder functionality
- Automatic backup creation
- XML import/export
