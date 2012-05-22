#!/bin/bash

if [[ -d /Applications/Arduino.app ]]
then
    
    # Define locations
    
    lib_path=/Applications/Arduino.app/Contents/Resources/Java/libraries/MIDI
    
    if [[ -d $lib_path ]]
    then
        # Remove old lib
        rm -rf $lib_path
    fi
    
    # Create folder
    mkdir $lib_path
    
    # Install contents
    cp -r * $lib_path
    
    # Cleanup
    rm $lib_path/install_mac.sh
    
else
    echo "Arduino application not found."
fi
