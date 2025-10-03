# Iida devkit

PCB design exercise

## Hardware

Hardware design related files can be found in the `hardware` directory.
To get started with hardware development, take a look at the _README_ file therein.

The harware was designed with [KiCad EDA](https://www.kicad.org/).

## Firmware

### Setup

1. Copy the `.devcontainer` and `.vscode` directories to your workspace.
2. Use the Dockerfile inside the `.devcontainer/Dockerfile` to build and image and launch a container using that image. Alternatively, download the image with `docker pull ghcr.io/sakuglumoff/iida-devkit:latest`.
   - If you're using Visual Studio Code, you can open the workspace in a dev container.
3. Install the `west` tool with `pip install west`.
4. Initialize the west projects with `west init -m git@github.com:SakuGlumoff/iida-devkit.git --mr trunk`.
5. Update the west projects with `west update`.
6. Install required Python packages with `pip install -r zephyr/scripts/requirements.txt`.
7. Export the Zephyr library for CMake with `west zephyr-export`.

### Building the application

Build the application with `west build -b iida_devkit application -- -DBOARD_ROOT=..`.

### Running the application

Run the application with `west flash`.
