#!/bin/bash

# Create a temporary destination folder
mkdir -p temp/doc
mkdir -p temp/examples

# Copy sources
cp ../src/* temp

# Copy resources
cp keywords.txt temp
cp install_* temp
rm temp/install_local_*

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


# Remove temp folder
rm -rf MIDI

# Archive generated packaged

if [[ !( -d ../bin ) ]]
then
    mkdir ../bin    # Create archives location
fi

mv MIDI.zip ../bin/Arduino_MIDI_Library.zip
