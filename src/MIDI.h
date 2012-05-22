/*!
 *  @file		MIDI.h
 *  Project		MIDI Library
 *	@brief		MIDI Library for the Arduino - With Teensy support
 *	Version		3.2
 *  @author		Francois Best 
 *	@date		24/02/11
 *  License		GPL Forty Seven Effects - 2011
 */

#ifndef LIB_MIDI_H_
#define LIB_MIDI_H_

#include <inttypes.h> 


/*  
    ###############################################################
    #                                                             #
    #    CONFIGURATION AREA                                       #
    #                                                             #
    #    Here are a few settings you can change to customize      #
    #    the library for your own project. You can for example    #
    #    choose to compile only parts of it so you gain flash     #
    #    space and optimise the speed of your sketch.             #
    #                                                             #
    ###############################################################
 */

#define TEENSY_SUPPORT          1           // Set this to 1 to enable Teensyduino support.
#define TEENSY_USB_TO_MIDI      1           // Set this to 1 to forward incoming messages on the USB MIDI to the UART.
#define TEENSY_MIDI_TO_USB      1           // Set this to 1 to forward incoming messages on the UART to the USB MIDI.

#define COMPILE_MIDI_IN         1           // Set this setting to 1 to use the MIDI input.
#define COMPILE_MIDI_OUT        1           // Set this setting to 1 to use the MIDI output. 
#define COMPILE_MIDI_THRU       1           // Set this setting to 1 to use the MIDI Soft Thru feature
                                            // Please note that the Thru will work only when both COMPILE_MIDI_IN and COMPILE_MIDI_OUT set to 1.


#define USE_SERIAL_PORT         Serial      // Change the number (to Serial1 for example) if you want
                                            // to use a different serial port for MIDI I/O.


#define USE_RUNNING_STATUS		1			// Running status enables short messages when sending multiple values
                                            // of the same type and channel.
                                            // Set to 0 if you have troubles with controlling you hardware.


#define USE_CALLBACKS           1           // Set this to 1 if you want to use callback handlers (to bind your functions to the library).
                                            // To use the callbacks, you need to have COMPILE_MIDI_IN set to 1

#define USE_1BYTE_PARSING       1           // Each call to MIDI.read will only parse one byte (might be faster).


// END OF CONFIGURATION AREA 
// (do not modify anything under this line unless you know what you are doing)


#define MIDI_BAUDRATE			31250

#define MIDI_CHANNEL_OMNI		0
#define MIDI_CHANNEL_OFF		17			// and over

#define MIDI_SYSEX_ARRAY_SIZE	255			// Maximum size is 65535 bytes.

/*! Type definition for practical use (because "unsigned char" is a bit long to write.. )*/
typedef uint8_t byte;
typedef uint16_t word;

/*! Enumeration of MIDI types */
enum kMIDIType {
	NoteOff	              = 0x80,	///< Note Off
	NoteOn                = 0x90,	///< Note On
	AfterTouchPoly        = 0xA0,	///< Polyphonic AfterTouch
	ControlChange         = 0xB0,	///< Control Change / Channel Mode
	ProgramChange         = 0xC0,	///< Program Change
	AfterTouchChannel     = 0xD0,	///< Channel (monophonic) AfterTouch
	PitchBend             = 0xE0,	///< Pitch Bend
	SystemExclusive       = 0xF0,	///< System Exclusive
	TimeCodeQuarterFrame  = 0xF1,	///< System Common - MIDI Time Code Quarter Frame
	SongPosition          = 0xF2,	///< System Common - Song Position Pointer
	SongSelect            = 0xF3,	///< System Common - Song Select
	TuneRequest           = 0xF6,	///< System Common - Tune Request
	Clock                 = 0xF8,	///< System Real Time - Timing Clock
	Start                 = 0xFA,	///< System Real Time - Start
	Continue              = 0xFB,	///< System Real Time - Continue
	Stop                  = 0xFC,	///< System Real Time - Stop
	ActiveSensing         = 0xFE,	///< System Real Time - Active Sensing
	SystemReset           = 0xFF,	///< System Real Time - System Reset
	InvalidType           = 0x00    ///< For notifying errors
};

/*! Enumeration of Thru filter modes */
enum kThruFilterMode {
	Off                   = 0,  ///< Thru disabled (nothing passes through).
	Full                  = 1,  ///< Fully enabled Thru (every incoming message is sent back).
	SameChannel           = 2,  ///< Only the messages on the Input Channel will be sent back.
	DifferentChannel      = 3   ///< All the messages but the ones on the Input Channel will be sent back.
};


/*! The midimsg structure contains decoded data of a MIDI message read from the serial port with read() or thru(). \n */
struct midimsg {
	/*! The MIDI channel on which the message was recieved. \n Value goes from 1 to 16. */
	byte channel; 
	/*! The type of the message (see the define section for types reference) */
	kMIDIType type;
	/*! The first data byte.\n Value goes from 0 to 127.\n */
	byte data1;
	/*! The second data byte. If the message is only 2 bytes long, this one is null.\n Value goes from 0 to 127. */
	byte data2;
	/*! System Exclusive dedicated byte array. \n Array length is stocked on 16 bits, in data1 (LSB) and data2 (MSB) */
	byte sysex_array[MIDI_SYSEX_ARRAY_SIZE];
	/*! This boolean indicates if the message is valid or not. There is no channel consideration here, validity means the message respects the MIDI norm. */
	bool valid;
};




/*! \brief The main class for MIDI handling.\n
	See member descriptions to know how to use it,
	or check out the examples supplied with the library.
 */
class MIDI_Class {
	
	
public:
	// Constructor and Destructor
	MIDI_Class();
	~MIDI_Class();
	
	
	void begin(const byte inChannel = 1);
	
	
	
	
/* ####### OUTPUT COMPILATION BLOCK ####### */	
#if COMPILE_MIDI_OUT

public:	
	
	void sendNoteOn(byte NoteNumber,byte Velocity,byte Channel);
	void sendNoteOff(byte NoteNumber,byte Velocity,byte Channel);
	void sendProgramChange(byte ProgramNumber,byte Channel);
	void sendControlChange(byte ControlNumber, byte ControlValue,byte Channel);
	void sendPitchBend(int PitchValue,byte Channel);
	void sendPitchBend(unsigned int PitchValue,byte Channel);
	void sendPitchBend(double PitchValue,byte Channel);
	void sendPolyPressure(byte NoteNumber,byte Pressure,byte Channel);
	void sendAfterTouch(byte Pressure,byte Channel);
	void sendSysEx(int length, const byte *const array,bool ArrayContainsBoundaries = false);	
	void sendTimeCodeQuarterFrame(byte TypeNibble, byte ValuesNibble);
	void sendTimeCodeQuarterFrame(byte data);
	void sendSongPosition(unsigned int Beats);
	void sendSongSelect(byte SongNumber);
	void sendTuneRequest();
	void sendRealTime(kMIDIType Type);
	
	void send(kMIDIType type, byte param1, byte param2, byte channel);
	
private:
	
	const byte genstatus(const kMIDIType inType,const byte inChannel) const;
	
	
	// Attributes
#if USE_RUNNING_STATUS
	byte			mRunningStatus_TX;
#endif // USE_RUNNING_STATUS

#endif	// COMPILE_MIDI_OUT
	

	
/* ####### INPUT COMPILATION BLOCK ####### */
#if COMPILE_MIDI_IN	
	
public:
	
	bool read();
	bool read(const byte Channel);
	
	// Getters
	kMIDIType getType() const;
	byte getChannel() const;
	byte getData1() const;
	byte getData2() const;
	const byte * getSysExArray() const;
	unsigned int getSysExArrayLength() const;
	bool check() const;
	
	byte getInputChannel() const 
    {
        return mInputChannel;
    }
	
	// Setters
	void setInputChannel(const byte Channel);
	
	/*! \brief Extract an enumerated MIDI type from a status byte.
	 
	 This is a utility static method, used internally, made public so you can handle kMIDITypes more easily.
	 */
	static inline const kMIDIType getTypeFromStatusByte(const byte inStatus) 
    {
		if ((inStatus < 0x80) 
			|| (inStatus == 0xF4) 
			|| (inStatus == 0xF5) 
			|| (inStatus == 0xF9) 
			|| (inStatus == 0xFD)) return InvalidType; // data bytes and undefined.
		if (inStatus < 0xF0) return (kMIDIType)(inStatus & 0xF0);	// Channel message, remove channel nibble.
		else return (kMIDIType)inStatus;
	}
	
	
#if USE_CALLBACKS
	
	void setHandleNoteOff(void (*fptr)(byte channel, byte note, byte velocity));
	void setHandleNoteOn(void (*fptr)(byte channel, byte note, byte velocity));
	void setHandleAfterTouchPoly(void (*fptr)(byte channel, byte note, byte pressure));
	void setHandleControlChange(void (*fptr)(byte channel, byte number, byte value));
	void setHandleProgramChange(void (*fptr)(byte channel, byte number));
	void setHandleAfterTouchChannel(void (*fptr)(byte channel, byte pressure));
	void setHandlePitchBend(void (*fptr)(byte channel, int bend));
	void setHandleSystemExclusive(void (*fptr)(byte * array, byte size));
	void setHandleTimeCodeQuarterFrame(void (*fptr)(byte data));
	void setHandleSongPosition(void (*fptr)(unsigned int beats));
	void setHandleSongSelect(void (*fptr)(byte songnumber));
	void setHandleTuneRequest(void (*fptr)(void));
	void setHandleClock(void (*fptr)(void));
	void setHandleStart(void (*fptr)(void));
	void setHandleContinue(void (*fptr)(void));
	void setHandleStop(void (*fptr)(void));
	void setHandleActiveSensing(void (*fptr)(void));
	void setHandleSystemReset(void (*fptr)(void));
	
	void disconnectCallbackFromType(kMIDIType Type);
	
#endif // USE_CALLBACKS
	
	
private:
	
	bool input_filter(byte inChannel);
	bool parse(byte inChannel);
	void reset_input_attributes();
	
	// Attributes
	byte			mRunningStatus_RX;
	byte			mInputChannel;
	
	byte			mPendingMessage[MIDI_SYSEX_ARRAY_SIZE];
	unsigned int	mPendingMessageExpectedLenght;
	unsigned int	mPendingMessageIndex;					// Extended to unsigned int for larger sysex payloads.
	
	midimsg			mMessage;
	
#if USE_CALLBACKS
	
	void launchCallback();
	
	void (*mNoteOffCallback)(byte channel, byte note, byte velocity);
	void (*mNoteOnCallback)(byte channel, byte note, byte velocity);
	void (*mAfterTouchPolyCallback)(byte channel, byte note, byte velocity);
	void (*mControlChangeCallback)(byte channel, byte, byte);
	void (*mProgramChangeCallback)(byte channel, byte);
	void (*mAfterTouchChannelCallback)(byte channel, byte);
	void (*mPitchBendCallback)(byte channel, int);
	void (*mSystemExclusiveCallback)(byte * array, byte size);
	void (*mTimeCodeQuarterFrameCallback)(byte data);
	void (*mSongPositionCallback)(unsigned int beats);
	void (*mSongSelectCallback)(byte songnumber);
	void (*mTuneRequestCallback)(void);
	void (*mClockCallback)(void);
	void (*mStartCallback)(void);
	void (*mContinueCallback)(void);
	void (*mStopCallback)(void);
	void (*mActiveSensingCallback)(void);
	void (*mSystemResetCallback)(void);
	
#endif // USE_CALLBACKS
	
	
#endif // COMPILE_MIDI_IN
	

/* ####### THRU COMPILATION BLOCK ####### */
#if (COMPILE_MIDI_IN && COMPILE_MIDI_OUT && COMPILE_MIDI_THRU) // Thru
	
public:
	
	// Getters
	kThruFilterMode getFilterMode() const { return mThruFilterMode; }
	bool getThruState() const { return mThruActivated; }
	
	
	// Setters
	void turnThruOn(kThruFilterMode inThruFilterMode = Full);
	void turnThruOff();
	
	void setThruFilterMode(const kThruFilterMode inThruFilterMode);
	
	
private:
	
	void thru_filter(byte inChannel);
	
	bool				mThruActivated;
	kThruFilterMode		mThruFilterMode;
	
#endif // Thru
	
};

extern MIDI_Class MIDI;

#endif // LIB_MIDI_H_
