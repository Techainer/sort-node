FROM valian/docker-python-opencv-ffmpeg:py3

RUN apt-get update -y && \
    apt-get install -y \
    libvtk-java \
    python-vtk \
    tcl-vtk \
    libvtk5-dev \
    libvtk5-qt4-dev \
    libusb-1.0-0-dev \ 
    libeigen3-dev \
    libboost-all-dev \
    software-properties-common

# Install NodeJS
RUN apt-get install -y curl && \
    curl -sL https://deb.nodesource.com/setup_14.x | bash - && \
    apt -y install nodejs

# Install cmake
RUN apt-get install -y apt-transport-https ca-certificates && \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add - && \
    apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
    apt-get update && \
    apt-get install -y cmake

RUN npm set unsafe-perm true && npm install -g yarn

RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#Add new sudo user
ARG USERNAME=linus
ARG UID=1000
ARG GID=1000

RUN useradd -m $USERNAME
RUN usermod -aG sudo $USERNAME
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
# Replace 1000 with your user/group id
RUN usermod --uid $UID $USERNAME && groupmod --gid $GID $USERNAME

# Change user
USER $USERNAME
WORKDIR /home/$USERNAME

