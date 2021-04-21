FROM valian/docker-python-opencv-ffmpeg:py3

RUN apt-get update -y && \
    apt-get install -y \
    libeigen3-dev \
    software-properties-common

# Install NodeJS
RUN apt-get install -y curl && \
    curl -sL https://deb.nodesource.com/setup_14.x | bash - && \
    apt -y install nodejs && \
    npm set unsafe-perm true

# Install cmake
RUN apt-get install -y apt-transport-https ca-certificates && \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add - && \
    apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
    apt-get update && \
    apt-get install -y cmake

RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*