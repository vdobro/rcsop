FROM registry.git.fh-aachen.de/ba-vd/sfm-pipeline/colmap:generic

ENV DEBIAN_FRONTEND=noninteractive
USER root
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    curl \
    xz-utils \
    cmake \
    build-essential \
    libboost-all-dev

WORKDIR /home/colmap
USER colmap
