#!/usr/bin/env python3

import argparse
import json
import sys
import time
from typing import Any

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    print(
        json.dumps(
            {
                "error": "pyserial is not installed",
                "install": (
                    "python -m pip install -r "
                    ".agents/skills/iida-serial/requirements.txt"
                ),
            }
        ),
        file=sys.stderr,
    )
    raise SystemExit(2)


VENDOR_ID = 0x0483
PRODUCT_ID = 0x0001
PRODUCT_SUBSTRING = "Iida devkit"
DEFAULT_BAUD = 115200


def port_details(port: Any) -> dict[str, Any]:
    return {
        "port": port.device,
        "vid": f"0x{port.vid:04x}" if port.vid is not None else None,
        "pid": f"0x{port.pid:04x}" if port.pid is not None else None,
        "product": port.product,
        "manufacturer": port.manufacturer,
        "serial_number": port.serial_number,
    }


def is_iida_devkit(port: Any) -> bool:
    product = port.product or ""
    return (
        port.vid == VENDOR_ID
        and port.pid == PRODUCT_ID
        and PRODUCT_SUBSTRING.casefold() in product.casefold()
    )


def matching_ports() -> list[Any]:
    return [port for port in list_ports.comports() if is_iida_devkit(port)]


def select_port(requested_port: str | None) -> Any:
    matches = matching_ports()

    if requested_port:
        selected = next(
            (port for port in matches if port.device == requested_port),
            None,
        )
        if selected is None:
            raise RuntimeError(
                f"{requested_port!r} does not match the Iida devkit USB identity"
            )
        return selected

    if not matches:
        raise RuntimeError("No matching Iida devkit was found")

    if len(matches) > 1:
        devices = ", ".join(port.device for port in matches)
        raise RuntimeError(
            f"Multiple Iida devkits found ({devices}); select one with --port"
        )

    return matches[0]


def read_bytes(connection: serial.Serial, duration: float) -> bytes:
    deadline = time.monotonic() + duration
    received = bytearray()

    while time.monotonic() < deadline:
        available = connection.in_waiting
        chunk = connection.read(available or 1)
        if chunk:
            received.extend(chunk)

    return bytes(received)


def response_payload(port: Any, data: bytes, encoding: str) -> dict[str, Any]:
    return {
        "device": port_details(port),
        "received": {
            "length": len(data),
            "text": data.decode(encoding, errors="replace"),
            "hex": data.hex(" "),
        },
    }


def discover(_: argparse.Namespace) -> int:
    matches = matching_ports()
    print(
        json.dumps(
            {
                "count": len(matches),
                "devices": [port_details(port) for port in matches],
            },
            indent=2,
        )
    )
    return 0 if matches else 1


def read(args: argparse.Namespace) -> int:
    port = select_port(args.port)

    with serial.Serial(
        port=port.device,
        baudrate=args.baud,
        timeout=0.1,
        write_timeout=args.write_timeout,
    ) as connection:
        data = read_bytes(connection, args.duration)

    print(json.dumps(response_payload(port, data, args.encoding), indent=2))
    return 0


def send(args: argparse.Namespace) -> int:
    port = select_port(args.port)

    if args.text is not None:
        endings = {"none": "", "lf": "\n", "crlf": "\r\n"}
        payload = (args.text + endings[args.line_ending]).encode(args.encoding)
    else:
        try:
            payload = bytes.fromhex(args.hex)
        except ValueError as error:
            raise RuntimeError(f"Invalid hexadecimal payload: {error}") from error

    with serial.Serial(
        port=port.device,
        baudrate=args.baud,
        timeout=0.1,
        write_timeout=args.write_timeout,
    ) as connection:
        connection.write(payload)
        connection.flush()
        data = read_bytes(connection, args.read_duration)

    result = response_payload(port, data, args.encoding)
    result["sent"] = {
        "length": len(payload),
        "hex": payload.hex(" "),
    }
    print(json.dumps(result, indent=2))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Discover and communicate with the Iida devkit."
    )
    parser.add_argument("--port", help="Explicit matching serial port")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD)
    parser.add_argument("--encoding", default="utf-8")
    parser.add_argument("--write-timeout", type=float, default=2.0)

    commands = parser.add_subparsers(dest="command", required=True)

    discover_parser = commands.add_parser("discover")
    discover_parser.set_defaults(handler=discover)

    read_parser = commands.add_parser("read")
    read_parser.add_argument("--duration", type=float, default=5.0)
    read_parser.set_defaults(handler=read)

    send_parser = commands.add_parser("send")
    payload = send_parser.add_mutually_exclusive_group(required=True)
    payload.add_argument("--text")
    payload.add_argument("--hex")
    send_parser.add_argument(
        "--line-ending",
        choices=("none", "lf", "crlf"),
        default="none",
    )
    send_parser.add_argument("--read-duration", type=float, default=1.0)
    send_parser.set_defaults(handler=send)

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    try:
        return args.handler(args)
    except (RuntimeError, serial.SerialException) as error:
        print(json.dumps({"error": str(error)}), file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
