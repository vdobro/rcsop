#!/bin/bash
set -euo pipefail

git submodule update --init --recursive -q

CPU_CORES=$(nproc)

pwd=$(pwd)
INSTALL_DIR="$pwd/build"
mkdir -p "${INSTALL_DIR}"

BUILD_DIR="$pwd/cmake_build"
mkdir -p "$BUILD_DIR"

export CMAKE_BUILD_TYPE="Release"

# eigen
EIGEN_BUILD_DIR="$BUILD_DIR/eigen"
mkdir -p "$EIGEN_BUILD_DIR"
cd "$EIGEN_BUILD_DIR"
cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" "$pwd/eigen"
make install

# flann
FLANN_SOURCE_DIR="$pwd/flann"
cd "$FLANN_SOURCE_DIR"
touch src/cpp/empty.cpp
sed -e '/add_library(flann_cpp SHARED/ s/""/empty.cpp/' \
    -e '/add_library(flann SHARED/ s/""/empty.cpp/' \
    -e '/cuda_add_library(flann_cuda SHARED/ s/""/empty.cpp/' \
    -i src/cpp/CMakeLists.txt

FLANN_BUILD_DIR="$BUILD_DIR/flann"
mkdir -p "$FLANN_BUILD_DIR"
cd "$FLANN_BUILD_DIR"
cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    -DCMAKE_BUILD_TYPE="Release" \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_DOC=OFF \
    -DBUILD_C_BINDINGS=OFF \
    -DBUILD_PYTHON_BINDINGS=OFF \
    -DBUILD_MATLAB_BINDINGS=OFF \
    -DBUILD_CUDA_LIB=OFF \
    "$FLANN_SOURCE_DIR"

make -j "$CPU_CORES"
make install
cd "$FLANN_SOURCE_DIR"
git reset --hard
rm -f src/cpp/empty.cpp

# ceres
CERES_BUILD_DIR="$BUILD_DIR/ceres"
mkdir -p "$CERES_BUILD_DIR"
cd "$CERES_BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DBUILD_TESTING=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_BENCHMARKS=OFF \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    "$pwd/ceres"
make -j "$CPU_CORES"
make install

# cgal
CGAL_BUILD_DIR="$BUILD_DIR/cgal"
CGAL_SOURCE_DIR="$pwd/cgal"
mkdir -p "$CGAL_BUILD_DIR"
cd "$CGAL_BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    -DCGAL_DIR="$CGAL_SOURCE_DIR" \
    -DWITH_examples=OFF \
    -DWITH_demos=OFF \
    "$CGAL_SOURCE_DIR"
make install

# colmap
COLMAP_SRC_DIR="$pwd/colmap"
cd "$COLMAP_SRC_DIR"
rm -f "cmake/FindEigen3.cmake"

COLMAP_BUILD_DIR="$BUILD_DIR/colmap"
mkdir -p "$COLMAP_BUILD_DIR"
cd "$COLMAP_BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DCUDA_NVCC_FLAGS="--std c++14" \
    -DCGAL_DIR="$CGAL_SOURCE_DIR" \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    -DGUI_ENABLED=ON \
    "$COLMAP_SRC_DIR"
make -j "$CPU_CORES"
make install

cd "$COLMAP_SRC_DIR"
git reset --hard

# pcl
#PCL_BUILD_DIR="$BUILD_DIR/pcl"
#mkdir -p "$PCL_BUILD_DIR"
#cd "$PCL_BUILD_DIR"
#cmake -DCMAKE_BUILD_TYPE="Release" \
#    -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
#    "$pwd/pcl"
#make -j 4
#make install

cd "$pwd"
