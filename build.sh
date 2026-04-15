#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT_DIR}/build"

if [[ -z "${VCPKG_ROOT:-}" ]]; then
  VCPKG_ROOT="${ROOT_DIR}/vcpkg"
fi

if [[ ! -d "${VCPKG_ROOT}" ]]; then
  git clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}"
fi

if [[ ! -x "${VCPKG_ROOT}/vcpkg" ]]; then
  "${VCPKG_ROOT}/bootstrap-vcpkg.sh"
fi

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" \
  -DCMAKE_BUILD_TYPE=Release

cmake --build "${BUILD_DIR}" --config Release

