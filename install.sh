#!/bin/bash
#
# Installation script for VDR menuorgedit plugin
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}VDR menuorgedit Plugin Installation${NC}"
echo "========================================"
echo

# Check if running as root for installation
if [ "$EUID" -ne 0 ] && [ "$1" = "install" ]; then 
    echo -e "${RED}Error: Installation requires root privileges${NC}"
    echo "Please run: sudo $0 install"
    exit 1
fi

# Check dependencies
echo "Checking dependencies..."
MISSING_DEPS=""

if ! pkg-config --exists libxml-2.0; then
    MISSING_DEPS="${MISSING_DEPS} libxml2-dev"
fi

if ! pkg-config --exists vdr; then
    MISSING_DEPS="${MISSING_DEPS} vdr-dev"
fi

if [ -n "$MISSING_DEPS" ]; then
    echo -e "${RED}Missing dependencies:${MISSING_DEPS}${NC}"
    echo
    echo "Install with:"
    echo -e "${YELLOW}sudo apt-get install${MISSING_DEPS}${NC}"
    exit 1
fi

echo -e "${GREEN}✓ All dependencies found${NC}"
echo

# Compile
if [ "$1" != "install-only" ]; then
    echo "Compiling plugin..."
    if make clean && make; then
        echo -e "${GREEN}✓ Compilation successful${NC}"
    else
        echo -e "${RED}✗ Compilation failed${NC}"
        exit 1
    fi
    echo
fi

# Install
if [ "$1" = "install" ] || [ "$1" = "install-only" ]; then
    echo "Installing plugin..."
    if make install; then
        echo -e "${GREEN}✓ Plugin installed${NC}"
    else
        echo -e "${RED}✗ Installation failed${NC}"
        exit 1
    fi
    echo
    
    # Create config directory if it doesn't exist
    CONFIG_DIR="/etc/vdr/plugins/menuorg"
    if [ ! -d "$CONFIG_DIR" ]; then
        echo "Creating config directory..."
        mkdir -p "$CONFIG_DIR"
        chmod 755 "$CONFIG_DIR"
        echo -e "${GREEN}✓ Config directory created: $CONFIG_DIR${NC}"
    fi
    
    # Copy example if menuorg.xml doesn't exist
    if [ ! -f "$CONFIG_DIR/menuorg.xml" ] && [ -f "examples/menuorg.xml" ]; then
        echo "Installing example configuration..."
        cp examples/menuorg.xml "$CONFIG_DIR/"
        chmod 644 "$CONFIG_DIR/menuorg.xml"
        echo -e "${GREEN}✓ Example configuration installed${NC}"
    fi
    
    echo
    echo -e "${GREEN}Installation complete!${NC}"
    echo
    echo "To use the plugin, add to your VDR startup:"
    echo -e "${YELLOW}vdr -P menuorgedit${NC}"
    echo
    echo "Or with custom config directory:"
    echo -e "${YELLOW}vdr -P 'menuorgedit -c /path/to/config'${NC}"
    echo
    echo "Configuration file location:"
    echo -e "${YELLOW}$CONFIG_DIR/menuorg.xml${NC}"
    echo
else
    echo "Compilation complete!"
    echo
    echo "To install, run:"
    echo -e "${YELLOW}sudo $0 install${NC}"
    echo
fi
