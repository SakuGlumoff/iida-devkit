# Iida devkit

PCB design exercise

## Hardware

Hardware design related files can be found in the `hardware` directory.
To get started with hardware development, take a look at the _README_ file therein.

The harware was designed with [KiCad EDA](https://www.kicad.org/) and manufactured with [Aisler](https://aisler.net/).

## Firmware

### Setup

1. Start a new virtual environment for Python with `python3 -m venv .venv && source .venv/bin/activate`.
2. Install west with `pip install west`.
3. Initialize the west projects with `west init .`.
4. Update the west projects with `west update`.
5. Install required Python packages with `pip install -r zephyr/scripts/requirements.txt`.
6. Export the Zephyr library for CMake with `west zephyr-export`.
7. Install the toolchain with `west sdk install -t arm-zephyr-eabi`.

### Building the application

Build the application with `west build -b iida_devkit@4.2.1 . -- -DBOARD_ROOT=.`.

### Running the application

Run the application with `west flash`.
