FROM	debian:bookworm

LABEL	org.opencontainers.image.authors="saku.glumoff@gmail.com"

WORKDIR	/tmp

# Install prerequisites
RUN	echo "deb http://deb.debian.org/debian bookworm-backports main" >> /etc/apt/sources.list.d/backports.list && \
	apt-get update -y && \
	apt-get install -y \
	git git-lfs cmake ninja-build gperf \
	ccache dfu-util device-tree-compiler wget curl \
	xz-utils file unzip gzip \
	make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1 \
	bash-completion \
	libncurses5 libncursesw5

# Install a Zephyr compatible Python version
RUN	apt-get update -y && \
	apt-get install -y \
	pkg-config zlib1g-dev openssl \
	libssl-dev libreadline-dev libbz2-dev \
	libsqlite3-dev libgdbm-dev libncurses5-dev \
	libncursesw5-dev tk-dev uuid-dev \
	liblzma-dev libgdbm-compat-dev libffi-dev
RUN	wget https://www.python.org/ftp/python/3.13.5/Python-3.13.5.tar.xz && \
	tar -xf Python-3.13.5.tar.xz && \
	rm Python-3.13.5.tar.xz && \
	cd Python-3.13.5 && \
	./configure --prefix=/opt/python/3.13.5 --enable-shared --enable-optimizations && \
	make -j$(nproc) && \
	make install
RUN	apt-get update -y && \
	apt-get install -y \
	python3-venv python3-dev python3-pip python3-setuptools python3-tk python3-wheel
ENV	PATH="$PATH:/opt/python/3.13.5/bin"
ENV	LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/opt/python/3.13.5/lib"

# Install J-Link tools
RUN	curl \
	-d "accept_license_agreement=accepted&submit=Download+software" \
	-X POST \
	-O "https://www.segger.com/downloads/jlink/JLink_Linux_V798f_x86_64.tgz" && \
	tar -xvf JLink_Linux_V798f_x86_64.tgz && \
	rm JLink_Linux_V798f_x86_64.tgz && \
	mv JLink_Linux_V798f_x86_64 /opt/jlink

# Install LLVM tools
RUN	wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc && \
	echo "deb http://apt.llvm.org/bookworm/ llvm-toolchain-bookworm-19 main" > /etc/apt/sources.list.d/llvm.list && \
	echo "deb-src http://apt.llvm.org/bookworm/ llvm-toolchain-bookworm-19 main" >> /etc/apt/sources.list.d/llvm.list && \
	apt update -y && \
	apt install -y clang-format-19 && \
	ln -sf /usr/bin/clang-format-19 /usr/bin/clang-format

# Install Zephyr dependencies
RUN	apt-get update -y && \
	apt-get install -y \
	git cmake ninja-build gperf \
	ccache dfu-util device-tree-compiler wget \
	python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
	make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1

# Install KiCad
RUN	apt update -y && \
	apt install -y kicad/bookworm-backports

# Install documentation tools
RUN	apt-get update -y && \
	apt-get install -y \
	graphviz

# Clean up
RUN	rm -rf /tmp/*
RUN	mv /usr/lib/python3.11/EXTERNALLY-MANAGED /usr/lib/python3.11/EXTERNALLY-MANAGED.old

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
USER 	$USERNAME

WORKDIR	/home/$USERNAME

ENV	PATH=$PATH:/home/$USERNAME/.local/bin

# Install Zephyr toolchain
RUN	wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.17.4/zephyr-sdk-0.17.4_linux-x86_64_minimal.tar.xz && \
	tar -xvf zephyr-sdk-0.17.4_linux-x86_64_minimal.tar.xz && \
	rm zephyr-sdk-0.17.4_linux-x86_64_minimal.tar.xz && \
	./zephyr-sdk-0.17.4/setup.sh -t arm-zephyr-eabi -h -c
ENV	ZEPHYR_SDK_INSTALL_DIR=/home/$USERNAME/zephyr-sdk-0.17.4

SHELL	["/bin/bash"]
