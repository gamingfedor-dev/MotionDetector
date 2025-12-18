#!/bin/bash
set -e


PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
BUILD_TYPE="${1:-Release}"


echo "=== Building Motion Detector ==="
echo "Build type: ${BUILD_TYPE}"


mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"


cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
make -j$(nproc)


echo "=== Build complete ==="
echo "Binary: ${BUILD_DIR}/motion_detector"
