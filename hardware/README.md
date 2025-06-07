# Hardware

Hardware for Iida devkit.

## Requirements

| Tool  | Version |
|-------|---------|
| KiCad | >= 8    |
| Git   | >= 2.34 |

## How to get started

Install the required version of _KiCad_ from https://www.kicad.org/download/.
Install the required version of _Git_ from https://git-scm.com/downloads.
Download the symbol and part libraries used in this project by running `git submodule update --init --recursive`.
You can now open the project file `iida_devkit.kicad_pro` with _KiCad_.

## PCB antenna design

### Target and environment

The antenna was designed to work with LTE band 20 and to prefer the uplink frequencies more than the downlink frequencies. The lowest downlink frequency is 791 MHz and the highest uplink frequency is 862 MHz. The center frequency is thus 826.5 MHz.

The antenna is located on a 1.6 mm thick board with an assumed dielectric constant of 4.3. There should be a copper keep-out area on the same layer as the antenna and on all layers below it.

As a success criteria, the S11 parameter (50 ohm normalized) value should be below -6 dB for the bandwidth edges and -20 dB for the center frequency. The VSWR should be thus below 3 for the bandwidth edges and near 1 for the center frequency.

There is no enclosure specified so assume that there isn't one.

### Simulation and design

The antenna was simulated and designed using Ansys Electronics Desktop Student 2024 R2 HFSS.

#### Antenna

![Antenna design in HFSS][pcb_antenna_hfss]

#### Dimensions

![Dimensions of the PCB antenna][pcb_antenna_dimensions]

The keepout area is marked in green.

#### S parameter

![S11 parameter of the PCB antenna][pcb_antenna_s_param]

#### VSWR

![VSWR of the PCB antenna][pcb_antenna_vswr]

#### Z parameter

![Z(p) parameter of the PCB antenna][pcb_antenna_z_param]

[pcb_antenna_hfss]: img/hfss_design.png
[pcb_antenna_dimensions]: img/dimensions.png
[pcb_antenna_s_param]: img/s_param.png
[pcb_antenna_vswr]: img/vswr.png
[pcb_antenna_z_param]: img/z_param.png
