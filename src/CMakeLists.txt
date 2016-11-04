project(midi)

add_library(midi STATIC
    midi_Namespace.h
    midi_Defs.h
    midi_Message.h
    midi_Settings.h
    midi_RingBuffer.h
    midi_RingBuffer.hpp
    midi_UsbTransport.h
    midi_UsbTransport.hpp
    MIDI.cpp
    MIDI.hpp
    MIDI.h
)
