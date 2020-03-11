#pragma once

#include "midi_Namespace.h"

BEGIN_MIDI_NAMESPACE

struct DefaultSerialSettings
{
    /*! Override the default MIDI baudrate to transmit over USB serial, to
    a decoding program such as Hairless MIDI (set baudrate to 115200)\n
    http://projectgus.github.io/hairless-midiserial/
    */
    static const long BaudRate = 31250;
};

template <class SerialPort, class _Settings = DefaultSerialSettings>
class SerialMIDI
{
    typedef _Settings Settings;

public:
	SerialMIDI(SerialPort& inSerial)
        : mSerial(inSerial)
	{
	};

public:
	void begin(MIDI_NAMESPACE::Channel inChannel = 1)
	{
        // Initialise the Serial port
        #if defined(AVR_CAKE)
            mSerial. template open<Settings::BaudRate>();
        #else
            mSerial.begin(Settings::BaudRate);
        #endif
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
    typedef MIDI_NAMESPACE::SerialMIDI<Type> __smt;\
    typedef MIDI_NAMESPACE::MidiInterface<__smt> TypedMidiInterface;\
    __smt serialMidi(SerialPort);\
    TypedMidiInterface Name((__smt&)serialMidi);

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
