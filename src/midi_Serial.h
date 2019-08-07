#pragma once

BEGIN_MIDI_NAMESPACE

struct DefaultSettings : AbstractDefaultSettings
{
	/*! Override the default MIDI baudrate to transmit over USB serial, to
	a decoding program such as Hairless MIDI (set baudrate to 115200)\n
	http://projectgus.github.io/hairless-midiserial/
	*/
	static const long BaudRate = 31250;
};

END_MIDI_NAMESPACE

BEGIN_MIDI_NAMESPACE

template<class SerialPort, class _Settings = DefaultSettings>
class MidiInterface : public AbstractMidiInterface<DefaultSettings>
{
public:
	typedef _Settings Settings; 

public:
	MidiInterface(SerialPort& inSerial)
		: mSerial(inSerial) 
	{
	};

	void begin(Channel inChannel = 1)
	{
		// Initialise the Serial port
#if defined(AVR_CAKE)
		mSerial. template open<Settings::BaudRate>();
#else
		mSerial.begin(Settings::BaudRate);
#endif

		AbstractMidiInterface<Settings>::begin(inChannel);
	}

	inline void __write(byte byte)
	{
		mSerial.write(byte);
	};

	inline byte __read()
	{
		return mSerial.read();
	};

	inline int __available()
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
#define MIDI_CREATE_INSTANCE(Type, SerialPort, Name)                            \
	midi::MidiInterface<Type> Name((Type&)SerialPort);

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
	MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);
#endif

 /*! \brief Create an instance of the library attached to a serial port with
  custom settings.
  @see DefaultSettings
  @see MIDI_CREATE_INSTANCE
  */
#define MIDI_CREATE_CUSTOM_INSTANCE(Type, SerialPort, Name, Settings)           \
	midi::MidiInterface<Type, Settings> Name((Type&)SerialPort);

END_MIDI_NAMESPACE