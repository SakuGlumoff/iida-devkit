---
name: builder
description: Build the project inside its development container using docker exec and west build.
---

# Builder

Build the project by invoking `west build` through Docker from the host.

## Initialization

If the docker image is not already running, start it with:

```bash
docker build \
   --file .devcontainer/Dockerfile \
   --build-arg USER_ID=$(id -u) \
   --build-arg GROUP_ID=$(id -g) \
   --tag iida-devkit:latest \
   .

docker run --detach \
   --name iida-devkit \
   --init \
   --volume "$PWD:/workspace" \
   --workdir /workspace \
   iida-devkit:latest \
   sleep infinity
```

## Procedure

1. Build using:
   `docker exec iida-devkit bash -lc 'west build -p -b iida_devkit application/app'`
   If the build fails to a workspace error, try removing the `build` directory.

2. Pass any build arguments requested by the user after `west build`. For example:
   `docker exec iida-devkit bash -lc 'west build -p -b iida_devkit application/app -- -DEXTRA_CONF_FILE=debug.conf'`

3. Report the captured build output and clearly summarize any compiler or linker errors.

## Safety

- Do not use an interactive Docker session.
- Quote container and workspace values safely.