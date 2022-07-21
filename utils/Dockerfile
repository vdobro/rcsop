FROM registry.git.fh-aachen.de/ba-vd/sfm-pipeline/colmap:generic

ENV DEBIAN_FRONTEND=noninteractive
USER root
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    curl xz-utils cmake git build-essential clang \
    libboost-program-options-dev \
    libboost-filesystem-dev \
    libboost-graph-dev \
    libboost-system-dev \
    libboost-test-dev \
    libsuitesparse-dev \
    libfreeimage-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    libglew-dev \
    qtbase5-dev \
    libqt5opengl5-dev \
    libomp-dev \
    libgmp-dev \
    libmpfr-dev \
    libmetis-dev \
    libhdf5-dev \
    libatlas-base-dev \
    libsfml-dev \
    libcairomm-1.0-dev

USER colmap
WORKDIR /app
