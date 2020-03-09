#pragma once

#include "midi_Namespace.h"

BEGIN_MIDI_NAMESPACE

template <class SerialPort>
class serialMIDI
{
public:
	serialMIDI(SerialPort& inSerial)
        : mSerial(inSerial)
	{
	};

public:
	void begin(MIDI_NAMESPACE::Channel inChannel = 1)
	{
	}

	bool beginTransmission(MidiType)
	{
		return true;
	};

	void write(byte byte)
	{
		mSerial.write(byte);
	};

	void endTransmission()
	{
	};

	byte read()
	{
		return mSerial.read();
	};

	unsigned available()
	{
        return mSerial.available();
	};

private:
    SerialPort& mSerial;
};

/*! \brief Create an instance of the library attached to a serial port.
 You can use HardwareSerial or SoftwareSerial for the serial port.
 Example: MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi2);
 Then call midi2.begin(), midi2.read() etc..
 */
#define MIDI_CREATE_INSTANCE(Type, SerialPort, Name)  \
    typedef MIDI_NAMESPACE::serialMIDI<Type> __amt;\
    __amt serialMidi(SerialPort);\
    MIDI_NAMESPACE::MidiInterface<__amt> Name((__amt&)serialMidi);

#if defined(ARDUINO_SAM_DUE) || defined(USBCON) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
    // Leonardo, Due and other USB boards use Serial1 by default.
    #define MIDI_CREATE_DEFAULT_INSTANCE()                                      \
        MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#else
    /*! \brief Create an instance of the library with default name, serial port
    and settings, for compatibility with sketches written with pre-v4.2 MIDI Lib,
    or if you don't bother using custom names, serial port or settings.
    */
    #define MIDI_CREATE_DEFAULT_INSTANCE()                                      \
        MIDI_CREATE_INSTANCE(HardwareSerial, Serial,  MIDI);
#endif


END_MIDI_NAMESPACE
