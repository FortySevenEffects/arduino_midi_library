#!/bin/sh

cd "`dirname "${0}"`"

root="`pwd`/.."
logsDir="$root/logs"
libDir="$HOME/Documents/Arduino/libraries/MIDI"
arduino="/Applications/Arduino.app/Contents/MacOS/JavaApplicationStub"

# ------------------------------------------------------------------------------

function cleanup
{
    # Parameter:
    #   $1: working directory
    mkdir -p "$1"
    rm -rf   "$/*"
}

function installTo
{
    # Parameter:
    #   $1: install directory
    cp -rf "$root/src/*"            "$1/"
    cp -rf "$root/res/examples"     "$1/"
    cp -rf "$root/res/keywords.txt" "$1/"
}

function verify
{
    # Parameter:
    #   $1: path to .ino file
    #   $2: board option
    "$arduino"                          \
        --verify "$1"                   \
        --verbose-build                 \
        #--board arduino:avr:leonardo
}

function checkExamples
{
    cwd=`pwd`
    cd "$libDir/examples"
    for d in *;
    do
        verify "$libDir/examples/$d/$d.ino"     \
            >  "$logsDir/$d.build.log"          \
            2> "$logsDir/$d.build.err.log"
    done
    cd "$cwd"
}

# ------------------------------------------------------------------------------
# Main

#cleanup     "$libDir"
#installTo   "$libDir"

checkExamples
