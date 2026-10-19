# Flash

Flash a firmware file to the Iida devkit with SEGGER J-Link, then reset and start the device.

## Usage

Run the provided script with the user-requested firmware path:

```shell
.agents/skills/flash/scripts/flash.sh <firmware-file>
```

ELF, Intel HEX, and Motorola S-record files carry their load addresses. A raw `.bin` file is loaded at the STM32L552CE main flash base address, `0x08000000`.

The script sends these commands directly to J-Link Commander without creating a temporary command file:

1. `LoadFile`
2. `Reset`
3. `Go`
4. `Exit`

It invokes `/opt/jlink/JLinkExe` with device `STM32L552CE`, interface `SWD`, and speed `4000` kHz.

## Procedure

1. Use only the firmware file explicitly requested by the user.
2. Run `flash.sh` non-interactively and capture its complete output and exit status.
3. Report connection, programming, and verification failures clearly. State that flashing succeeded only when J-Link completes without errors.

## Safety

- Do not mass-erase, unlock, recover, alter option bytes, or disable readout protection unless explicitly requested.
- Do not reset or start the target before programming completes.
- Do not substitute another flashing tool when `/opt/jlink/JLinkExe` is unavailable.

## Supporting files

Skill directory:
`file:///.agents/skills/flash/scripts/flash.sh`
