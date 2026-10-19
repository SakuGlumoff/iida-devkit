#!/usr/bin/env bash

set -euo pipefail

readonly JLINK_EXE=/opt/jlink/JLinkExe
readonly DEVICE=STM32L552CE
readonly INTERFACE=SWD
readonly SPEED_KHZ=4000
readonly FLASH_BASE=0x08000000

usage() {
    printf 'Usage: %s <firmware-file>\n' "${0##*/}" >&2
}

if [[ $# -ne 1 ]]; then
    usage
    exit 2
fi

if [[ ! -x "$JLINK_EXE" ]]; then
    printf 'Error: J-Link Commander is not executable: %s\n' "$JLINK_EXE" >&2
    exit 1
fi

if [[ ! -f "$1" ]]; then
    printf 'Error: firmware is not a regular file: %s\n' "$1" >&2
    exit 1
fi

firmware=$(realpath -- "$1")
if [[ "$firmware" == *'"'* || "$firmware" == *$'\n'* || "$firmware" == *$'\r'* ]]; then
    printf 'Error: firmware path contains unsupported characters\n' >&2
    exit 1
fi

if [[ "${firmware,,}" == *.bin ]]; then
    load_command="LoadFile \"$firmware\" $FLASH_BASE"
else
    load_command="LoadFile \"$firmware\""
fi

printf '%s\n' "$load_command" Reset Go Exit | \
    "$JLINK_EXE" \
        -Device "$DEVICE" \
        -If "$INTERFACE" \
        -Speed "$SPEED_KHZ" \
        -AutoConnect 1
