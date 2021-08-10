#!/usr/bin/env bash

export LCOV_ROOT="$HOME/lcov"
mkdir -p "$LCOV_ROOT"
wget http://ftp.de.debian.org/debian/pool/main/l/lcov/lcov_1.14.orig.tar.gz --output-document="$LCOV_ROOT/lcov.tar.gz"
tar xf "$LCOV_ROOT/lcov.tar.gz" --strip-components=1 -C $LCOV_ROOT
export PATH="$LCOV_ROOT/bin:$PATH"
which lcov
