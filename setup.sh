#!/bin/bash

set -e  # Exit on error

# ANSI color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install packages on Debian/Ubuntu
install_apt() {
    echo -e "${GREEN}[INFO] Installing packages via apt...${NC}"
    sudo apt update
    sudo apt install -y "$@"
}

# Function to install packages on Arch Linux
install_pacman() {
    echo -e "${GREEN}[INFO] Installing packages via pacman...${NC}"
    sudo pacman -Sy --noconfirm "$@"
}

# Function to install development tools
install_dev_packages() {
    if [[ "$DISTRO" == "ubuntu" ]]; then
        install_apt build-essential libavcodec-dev libavformat-dev libswscale-dev libncurses5-dev
    elif [[ "$DISTRO" == "arch" ]]; then
        install_pacman base-devel ffmpeg ncurses
    fi
}

# Function to check if required packages are installed
check_dependencies() {
    local missing=()

    for cmd in gcc make ffmpeg ncurses; do
        if ! command_exists "$cmd"; then
            missing+=("$cmd")
        fi
    done

    if [ ${#missing[@]} -gt 0 ]; then
        echo -e "${RED}[ERROR] Missing dependencies: ${missing[@]}${NC}"
        return 1
    else
        echo -e "${GREEN}[INFO] All dependencies are installed.${NC}"
        return 0
    fi
}

# Detect distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        case "$ID" in
            ubuntu|debian) DISTRO="ubuntu" ;;
            arch) DISTRO="arch" ;;
            *) DISTRO="unknown" ;;
        esac
    else
        DISTRO="unknown"
    fi
}

# Main logic
main() {
    echo -e "${GREEN}[INFO] Starting dependency check and installation...${NC}"

    detect_distro

    if [[ "$DISTRO" == "unknown" ]]; then
        echo -e "${RED}[ERROR] Unsupported Linux distribution.${NC}"
        exit 1
    fi

    echo -e "${GREEN}[INFO] Detected distribution: $DISTRO${NC}"

    if check_dependencies; then
        echo -e "${GREEN}[SUCCESS] All dependencies are already installed.${NC}"
        exit 0
    else
        echo -e "${GREEN}[INFO] Installing missing dependencies...${NC}"
        install_dev_packages
        if check_dependencies; then
            echo -e "${GREEN}[SUCCESS] All dependencies are now installed.${NC}"
        else
            echo -e "${RED}[ERROR] Some dependencies are still missing after installation.${NC}"
            exit 1
        fi
    fi
}

# Run main
main