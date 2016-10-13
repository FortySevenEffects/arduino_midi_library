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
build="$root/build/dist/MIDI"

echo "root:  $root"
echo "build: $build"

# Create a destination directory structure
mkdir -p "$build/examples"
mkdir -p "$build/src"

# Copy sources
cp -rf "$root/src" "$build"

# Copy resources
cp -f "$root/keywords.txt"          "$build/"
cp -f "$root/library.properties"    "$build/"
cp -f "$root/library.json"          "$build/"
cp -f "$root/LICENSE"               "$build/"

# Copy examples
cp -rf "$root/examples" "$build"

# Generate package
cd "$build/.."
zip -r Arduino_MIDI_Library.zip MIDI

# Generate doc
cd "$root/doc"
doxygen
