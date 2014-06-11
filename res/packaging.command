#!/bin/sh
#
# Generate an archive with packaged content for easier delivery.
# The generated archive contains:
#   - Source files (MIDI.cpp / MIDI.h)
#   - Resources (keywords.txt)
#   - Examples for Arduino IDE
#   - Installation scripts

cd "`dirname "${0}"`"

root="${PWD}/.."
build="$root/build/MIDI"

echo "root:  $root"
echo "build: $build"

# Create a temporary destination folder
mkdir -p "$build"
mkdir -p "$build/examples"

# Copy sources
cd "$root/src/"
cp * "$build/"

# Copy resources
cd "$root/res/"
cp keywords.txt "$build/"

# Copy examples
cd "$root/res/examples/"
cp -r * "$build/examples"

# Generate package
cd "$build/.."
zip -r Arduino_MIDI_Library.zip MIDI

# Generate doc
cd "$root/doc"
doxygen
