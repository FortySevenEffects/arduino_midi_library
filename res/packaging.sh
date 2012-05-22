#!/bin/bash

# Create a temporary destination folder
mkdir -p temp/doc
mkdir -p temp/examples

# Copy sources
cp ../src/* temp

# Copy resources
cp keywords.txt temp

# Copy examples
cp -r examples/* temp/examples

# Generate & copy doc
cd ../doc
/Applications/Doxygen.app/Contents/Resources/doxygen Doxyfile
rm -rf latex
cd ../res

cp -r ../doc/* temp/doc

# Generate package
mv temp MIDI
zip -r MIDI.zip MIDI
mv MIDI.zip Arduino_MIDI_Library_v.zip
