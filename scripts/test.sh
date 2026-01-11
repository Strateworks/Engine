# Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
# All rights reserved.

#!/usr/bin/env bash
set -euo pipefail

BUILD_TYPE=${1:-debug}
LINK_TYPE=${2:-static}

mkdir -p build
cd build

ctest --output-on-failure --verbose

cd ..

if [[ "$BUILD_TYPE" == "debug" ]]; then
  gcovr --filter='src/' --print-summary --sort-percentage --gcov-ignore-parse-errors=negative_hits.warn
  gcovr --filter='src/' --gcov-ignore-parse-errors=negative_hits.warn --sonarqube > coverage.xml
fi