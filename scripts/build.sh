# Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
# All rights reserved.

#!/usr/bin/env bash
set -euo pipefail

BUILD_TYPE=${1:-debug}
LINK_TYPE=${2:-static}
BUILD_THREADS=${3:-1}
BUILD_TESTS=${4:-tests_on}

mkdir -p build
cd build

if [[ "$LINK_TYPE" == "static" ]]; then
ENABLE_STATIC="ON"
else
ENABLE_STATIC="OFF"
fi

if [[ "$BUILD_TYPE" == "debug" ]]; then
ENABLE_DEBUG="ON"
else
ENABLE_DEBUG="OFF"
fi

if [[ "$BUILD_TESTS" == "tests_on" ]]; then
ENABLED_TESTS="ON"
else
ENABLED_TESTS="OFF"
fi

cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DENABLE_TESTS=$ENABLED_TESTS -DENABLE_CI=ON -DENABLE_DEBUG=$ENABLE_DEBUG -DENABLE_STATIC_LINKING=$ENABLE_STATIC

make -j$BUILD_THREADS