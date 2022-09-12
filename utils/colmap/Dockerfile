ARG UBUNTU_VERSION=22.04

FROM ubuntu:${UBUNTU_VERSION} as builder
ARG MAX_BUILD_JOBS=16

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    git \
    cmake \
    build-essential \
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
    libgmp-dev \
    libmpfr-dev \
    libmetis-dev \
    libhdf5-dev \
    libatlas-base-dev

RUN useradd -m colmap
WORKDIR /opt
RUN chown -R colmap:colmap .
USER colmap

# libeigen
RUN git clone --depth 1 --branch 3.4.0 https://gitlab.com/libeigen/eigen.git
WORKDIR /opt/eigen
RUN mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE="Release"
USER root
RUN cd build && make install
USER colmap

# ceres
WORKDIR /opt
RUN git clone --depth 1 --branch 2.1.0 https://ceres-solver.googlesource.com/ceres-solver
WORKDIR /opt/ceres-solver
RUN mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE="Release" \
    -DBUILD_TESTING=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_BENCHMARKS=OFF && \
    make -j ${MAX_BUILD_JOBS}
USER root
RUN cd build && make install
USER colmap

# cgal
WORKDIR /opt
RUN git clone --depth 1 --branch v5.5 https://github.com/CGAL/cgal.git
WORKDIR /opt/cgal
RUN mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE="Release" \
    -DWITH_examples=OFF \
    -DWITH_demos=OFF
USER root
RUN cd build && make install
USER colmap

# colmap
WORKDIR /opt
RUN git clone --branch dev https://github.com/colmap/colmap.git
WORKDIR /opt/colmap
COPY colmap.patch .
RUN git checkout 7acff8178e315979a129fbd1a8026a8e9ef19f6e
RUN git apply colmap.patch
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DBOOST_STATIC=OFF \
    -DGUI_ENABLED=OFF && \
    make -j ${MAX_BUILD_JOBS}
USER root
RUN cd build && make install
USER colmap

FROM ubuntu:${UBUNTU_VERSION} as runtime
LABEL maintainer="vitalijus.dobrovolskis@alumni.fh-aachen.de"

RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    libglew2.2 \
    libglib2.0-0 \
    libgoogle-glog0v5 \
    libfreeimage3 \
    libmetis5 \
    libboost-filesystem1.74.0 \
    libboost-program-options1.74.0 \
    libspqr2 \
    libcxsparse3 \
    libcublas11 \
    libcusolver11 \
    libatlas-base-dev \
    libhdf5-cpp-103-1 \
    libpython3.10 \
    libgmp-dev \
    libmpfr-dev

WORKDIR /usr/local
COPY --from=builder /usr/local/bin/colmap        bin/colmap

COPY --from=builder /usr/local/include/colmap    include/colmap
COPY --from=builder /usr/local/include/CGAL      include/CGAL
COPY --from=builder /usr/local/include/ceres     include/ceres
COPY --from=builder /usr/local/include/eigen3    include/eigen3

COPY --from=builder /usr/local/lib/libceres.a    lib/libceres.a
COPY --from=builder /usr/local/lib/cmake/Ceres   lib/cmake/Ceres
COPY --from=builder /usr/local/lib/cmake/CGAL    lib/cmake/CGAL
COPY --from=builder /usr/local/lib/colmap        lib/colmap

COPY --from=builder /usr/local/share/colmap               share/colmap
COPY --from=builder /usr/local/share/eigen3               share/eigen3
COPY --from=builder /usr/local/share/pkgconfig/eigen3.pc  share/pkgconfig/eigen3.pc

RUN useradd -m colmap
USER colmap
