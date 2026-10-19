---
name: serial-usb
description: Discover and communicate with the Iida devkit over USB serial using VID 0x0483, PID 0x0001, and a product string containing "Iida devkit".
---

# Serial USB Device

Use `scripts/serial_usb.py` to interface with the Iida devkit.

## Safety and behavior

- Run `discover` before opening the device.
- Only use ports matching:
  - Vendor ID: `0x0483`
  - Product ID: `0x0001`
  - Product string containing `Iida devkit`, case-insensitively
- If multiple matching devices are present, ask the user which port to use.
- Do not invent commands for the device protocol.
- Confirm potentially destructive commands with the user before sending them.
- Opening a serial port may reset some embedded devices.

## Setup

Install the Python dependency if it is unavailable:

```shell
python -m pip install -r .agents/skills/serial-usb/requirements.txt
```

## Discover the device

```shell
python .agents/skills/serial-usb/scripts/serial_usb.py discover
```

## Read serial output

The default baud rate is 921600:

```shell
python .agents/skills/serial-usb/scripts/serial_usb.py \
  --baud 921600 read --duration 5
```

Select a port explicitly when multiple devices match:

```shell
python .agents/skills/serial-usb/scripts/serial_usb.py \
  --port /dev/ttyACM0 read --duration 5
```

## Send text

Send text without a line ending:

```shell
python .agents/skills/serial-usb/scripts/serial_usb.py \
  send --text "status"
```

Send text followed by LF or CRLF:

```shell
python .agents/skills/serial-usb/scripts/serial_usb.py \
  send --text "status" --line-ending lf --read-duration 2
```

## Send bytes

Hexadecimal input may contain spaces:

```shell
python .agents/skills/serial-usb/scripts/serial_usb.py \
  send --hex "01 02 ff" --read-duration 2
```

Commands emit JSON so results can be interpreted reliably. Received data includes both decoded text and hexadecimal bytes.
