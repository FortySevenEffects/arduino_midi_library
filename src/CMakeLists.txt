increase_warning_level()

project(midi)

add_library(midi STATIC
    midi_Namespace.h
    midi_Defs.h
    midi_Message.h
    midi_Platform.h
    midi_Settings.h
    MIDI.cpp
    MIDI.hpp
    MIDI.h
    serialMIDI.h
)
