FROM	debian:trixie

LABEL	org.opencontainers.image.authors="saku.glumoff@gmail.com"

WORKDIR	/tmp

# Install required packages
RUN	apt-get update -y && \
	apt-get install -y \
	git git-lfs \
	cmake ninja-build gperf ccache dfu-util device-tree-compiler \
	make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1 \
	wget curl \
	python3 python3-dev python3-venv python3-tk python3-pip python3-setuptools python3-wheel \
	xz-utils file unzip gzip \
	bash-completion \
	libncurses6 libncursesw6 \
	clang-format-19 \
	graphviz

# Install J-Link tools
RUN	curl \
	-d "accept_license_agreement=accepted&submit=Download+software" \
	-X POST \
	-O "https://www.segger.com/downloads/jlink/JLink_Linux_V954_x86_64.tgz" && \
	tar -xvf JLink_Linux_V954_x86_64.tgz && \
	rm JLink_Linux_V954_x86_64.tgz && \
	mv JLink_Linux_V954_x86_64 /opt/jlink

# Clean up
RUN	rm -rf /tmp/*
RUN	mv /usr/lib/python3.13/EXTERNALLY-MANAGED /usr/lib/python3.13/EXTERNALLY-MANAGED.old

WORKDIR	/

# Create and set a default user
ARG	USERNAME=user
ARG	USER_UID=1000
ARG	USER_GID=$USER_UID
RUN	groupadd --gid $USER_GID $USERNAME \
	&& useradd --uid $USER_UID --gid $USER_GID -s /bin/bash -m $USERNAME \
	#
	# Add sudo support.
	&& apt-get update \
	&& apt-get install -y sudo \
	&& echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
	&& chmod 0440 /etc/sudoers.d/$USERNAME
USER	$USERNAME

WORKDIR	/home/$USERNAME

ENV	PATH=$PATH:/home/$USERNAME/.local/bin

# Install Zephyr toolchain
RUN	wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v1.0.1/zephyr-sdk-1.0.1_linux-x86_64_gnu.tar.xz && \
	tar -xvf zephyr-sdk-1.0.1_linux-x86_64_gnu.tar.xz && \
	rm zephyr-sdk-1.0.1_linux-x86_64_gnu.tar.xz && \
	./zephyr-sdk-1.0.1/setup.sh -t arm-zephyr-eabi -h -c
ENV	ZEPHYR_SDK_INSTALL_DIR=/home/$USERNAME/zephyr-sdk-1.0.1

SHELL	["/bin/bash"]
