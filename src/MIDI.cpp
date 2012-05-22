/*!
 *  @file		MIDI.cpp
 *  Project		MIDI Library
 *	@brief		MIDI Library for the Arduino
 *	@version	3.2
 *  @author		Francois Best 
 *	@date		24/02/11
 *  license		GPL Forty Seven Effects - 2011
 */

#include "MIDI.h"
#include <stdlib.h>
#include "Arduino.h"			// If using an old (pre-1.0) version of Arduino, use WConstants.h instead of Arduino.h
#include "HardwareSerial.h"


#if TEENSY_SUPPORT && defined(CORE_TEENSY)
/* By default, no Serial instance is loaded.
 Create one for the library and bind it.
 */
HardwareSerial UARTSerial;

// Now make sure we use the right one:
#ifdef USE_SERIAL_PORT
#undef USE_SERIAL_PORT
#define USE_SERIAL_PORT UARTSerial
#endif

#endif // TEENSY_SUPPORT

/*! Main instance (the class comes pre-instantiated). */
MIDI_Class MIDI;


/*! \brief Default constructor for MIDI_Class. */
MIDI_Class::MIDI_Class()
{ 
	
#if USE_CALLBACKS
	
	// Initialise callbacks to NULL pointer
	mNoteOffCallback				= NULL;
	mNoteOnCallback					= NULL;
	mAfterTouchPolyCallback			= NULL;
	mControlChangeCallback			= NULL;
	mProgramChangeCallback			= NULL;
	mAfterTouchChannelCallback		= NULL;
	mPitchBendCallback				= NULL;
	mSystemExclusiveCallback		= NULL;
	mTimeCodeQuarterFrameCallback	= NULL;
	mSongPositionCallback			= NULL;
	mSongSelectCallback				= NULL;
	mTuneRequestCallback			= NULL;
	mClockCallback					= NULL;
	mStartCallback					= NULL;
	mContinueCallback				= NULL;
	mStopCallback					= NULL;
	mActiveSensingCallback			= NULL;
	mSystemResetCallback			= NULL;
	
#endif
	
}


/*! \brief Default destructor for MIDI_Class.
 
 This is not really useful for the Arduino, as it is never called...
 */
MIDI_Class::~MIDI_Class()
{

}


/*! \brief Call the begin method in the setup() function of the Arduino.
 
 All parameters are set to their default values:
 - Input channel set to 1 if no value is specified
 - Full thru mirroring
 */
void MIDI_Class::begin(const byte inChannel)
{
	
	// Initialise the Serial port
	USE_SERIAL_PORT.begin(MIDI_BAUDRATE);
	
	
#if COMPILE_MIDI_OUT
	
#if USE_RUNNING_STATUS
	
	mRunningStatus_TX = InvalidType;
	
#endif // USE_RUNNING_STATUS
	
#endif // COMPILE_MIDI_OUT
	
	
#if COMPILE_MIDI_IN
	
	mInputChannel = inChannel;
	mRunningStatus_RX = InvalidType;
	mPendingMessageIndex = 0;
	mPendingMessageExpectedLenght = 0;
	
	mMessage.valid = false;
	mMessage.type = InvalidType;
	mMessage.channel = 0;
	mMessage.data1 = 0;
	mMessage.data2 = 0;
	
#endif // COMPILE_MIDI_IN
	
	
#if (COMPILE_MIDI_IN && COMPILE_MIDI_OUT && COMPILE_MIDI_THRU) // Thru
	
	mThruFilterMode = Full;
	mThruActivated = true;
	
#endif // Thru
	
}


#if COMPILE_MIDI_OUT

// Private method for generating a status byte from channel and type
const byte MIDI_Class::genstatus(const kMIDIType inType,
								 const byte inChannel) const
{
	
	return ((byte)inType | ((inChannel-1) & 0x0F));
	
}


/*! \brief Generate and send a MIDI message from the values given.
 \param type	The message type (see type defines for reference)
 \param data1	The first data byte.
 \param data2	The second data byte (if the message contains only 1 data byte, set this one to 0).
 \param channel	The output channel on which the message will be sent (values from 1 to 16). Note: you cannot send to OMNI.
 
 This is an internal method, use it only if you need to send raw data from your code, at your own risks.
 */
void MIDI_Class::send(kMIDIType type,
					  byte data1,
					  byte data2,
					  byte channel)
{
	
	// Then test if channel is valid
	if (channel >= MIDI_CHANNEL_OFF || channel == MIDI_CHANNEL_OMNI || type < NoteOff) {
		
#if USE_RUNNING_STATUS	
		mRunningStatus_TX = InvalidType;
#endif 
		
		return; // Don't send anything
	}
	
	if (type <= PitchBend) {
		// Channel messages
		
		// Protection: remove MSBs on data
		data1 &= 0x7F;
		data2 &= 0x7F;
		
		byte statusbyte = genstatus(type,channel);
		
#if USE_RUNNING_STATUS
		// Check Running Status
		if (mRunningStatus_TX != statusbyte) {
			// New message, memorise and send header
			mRunningStatus_TX = statusbyte;
			USE_SERIAL_PORT.write(mRunningStatus_TX);
		}
#else
		// Don't care about running status, send the Control byte.
		USE_SERIAL_PORT.write(statusbyte);
#endif
		
		// Then send data
		USE_SERIAL_PORT.write(data1);
		if (type != ProgramChange && type != AfterTouchChannel) {
			USE_SERIAL_PORT.write(data2);
		}
		return;
	}
	if (type >= TuneRequest && type <= SystemReset) {
		// System Real-time and 1 byte.
		sendRealTime(type);
	}
	
}


/*! \brief Send a Note On message 
 \param NoteNumber	Pitch value in the MIDI format (0 to 127). Take a look at the values, names and frequencies of notes here: http://www.phys.unsw.edu.au/jw/notes.html\n
 \param Velocity	Note attack velocity (0 to 127). A NoteOn with 0 velocity is considered as a NoteOff.
 \param Channel		The channel on which the message will be sent (1 to 16). 
 */
void MIDI_Class::sendNoteOn(byte NoteNumber,
							byte Velocity,
							byte Channel)
{ 
	
	send(NoteOn,NoteNumber,Velocity,Channel);

}


/*! \brief Send a Note Off message (a real Note Off, not a Note On with null velocity)
 \param NoteNumber	Pitch value in the MIDI format (0 to 127). Take a look at the values, names and frequencies of notes here: http://www.phys.unsw.edu.au/jw/notes.html\n
 \param Velocity	Release velocity (0 to 127).
 \param Channel		The channel on which the message will be sent (1 to 16).
 */
void MIDI_Class::sendNoteOff(byte NoteNumber,
							 byte Velocity,
							 byte Channel)
{
	
	send(NoteOff,NoteNumber,Velocity,Channel);

}


/*! \brief Send a Program Change message 
 \param ProgramNumber	The Program to select (0 to 127).
 \param Channel			The channel on which the message will be sent (1 to 16).
 */
void MIDI_Class::sendProgramChange(byte ProgramNumber,
								   byte Channel)
{
	
	send(ProgramChange,ProgramNumber,0,Channel);

}


/*! \brief Send a Control Change message 
 \param ControlNumber	The controller number (0 to 127). See the detailed description here: http://www.somascape.org/midi/tech/spec.html#ctrlnums
 \param ControlValue	The value for the specified controller (0 to 127).
 \param Channel			The channel on which the message will be sent (1 to 16). 
 */
void MIDI_Class::sendControlChange(byte ControlNumber,
								   byte ControlValue,
								   byte Channel)
{
	
	send(ControlChange,ControlNumber,ControlValue,Channel);

}


/*! \brief Send a Polyphonic AfterTouch message (applies to only one specified note)
 \param NoteNumber		The note to apply AfterTouch to (0 to 127).
 \param Pressure		The amount of AfterTouch to apply (0 to 127).
 \param Channel			The channel on which the message will be sent (1 to 16). 
 */
void MIDI_Class::sendPolyPressure(byte NoteNumber,
								  byte Pressure,
								  byte Channel)
{
	
	send(AfterTouchPoly,NoteNumber,Pressure,Channel);

}


/*! \brief Send a MonoPhonic AfterTouch message (applies to all notes)
 \param Pressure		The amount of AfterTouch to apply to all notes.
 \param Channel			The channel on which the message will be sent (1 to 16). 
 */
void MIDI_Class::sendAfterTouch(byte Pressure,
								byte Channel)
{
	
	send(AfterTouchChannel,Pressure,0,Channel);

}


/*! \brief Send a Pitch Bend message using a signed integer value.
 \param PitchValue	The amount of bend to send (in a signed integer format), between -8192 (maximum downwards bend) and 8191 (max upwards bend), center value is 0.
 \param Channel		The channel on which the message will be sent (1 to 16).
 */
void MIDI_Class::sendPitchBend(int PitchValue,
							   byte Channel)
{
	
	unsigned int bend = PitchValue + 8192;
	sendPitchBend(bend,Channel);
	
}


/*! \brief Send a Pitch Bend message using an unsigned integer value.
 \param PitchValue	The amount of bend to send (in a signed integer format), between 0 (maximum downwards bend) and 16383 (max upwards bend), center value is 8192.
 \param Channel		The channel on which the message will be sent (1 to 16).
 */
void MIDI_Class::sendPitchBend(unsigned int PitchValue,
							   byte Channel)
{
	
	send(PitchBend,(PitchValue & 0x7F),(PitchValue >> 7) & 0x7F,Channel);
	
}


/*! \brief Send a Pitch Bend message using a floating point value.
 \param PitchValue	The amount of bend to send (in a floating point format), between -1.0f (maximum downwards bend) and +1.0f (max upwards bend), center value is 0.0f.
 \param Channel		The channel on which the message will be sent (1 to 16).
 */
void MIDI_Class::sendPitchBend(double PitchValue,
							   byte Channel)
{
	
	unsigned int pitchval = (PitchValue+1.f)*8192;
	if (pitchval > 16383) pitchval = 16383;		// overflow protection
	sendPitchBend(pitchval,Channel);
	
}


/*! \brief Generate and send a System Exclusive frame.
 \param length	The size of the array to send
 \param array	The byte array containing the data to send
 \param ArrayContainsBoundaries  When set to 'true', 0xF0 & 0xF7 bytes (start & stop SysEx) will NOT be sent (and therefore must be included in the array).
 default value is set to 'false' for compatibility with previous versions of the library.
 */
void MIDI_Class::sendSysEx(int length,
						   const byte *const array,
						   bool ArrayContainsBoundaries)
{
	
	if (ArrayContainsBoundaries == false) {
		
		USE_SERIAL_PORT.write(0xF0);
		
		for (int i=0;i<length;++i) {
			
			USE_SERIAL_PORT.write(array[i]);
			
		}
		
		USE_SERIAL_PORT.write(0xF7);
		
	}
	else {
		
		for (int i=0;i<length;++i) {
			
			USE_SERIAL_PORT.write(array[i]);
			
		}
		
	}
	
#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif
	
}


/*! \brief Send a Tune Request message. 
 
 When a MIDI unit receives this message, it should tune its oscillators (if equipped with any) 
 */
void MIDI_Class::sendTuneRequest()
{
	
	sendRealTime(TuneRequest);

}


/*! \brief Send a MIDI Time Code Quarter Frame. 
 
 See MIDI Specification for more information.
 \param TypeNibble	MTC type
 \param ValuesNibble	MTC data
 */
void MIDI_Class::sendTimeCodeQuarterFrame(byte TypeNibble, byte ValuesNibble)
{
	
	byte data = ( ((TypeNibble & 0x07) << 4) | (ValuesNibble & 0x0F) );
	sendTimeCodeQuarterFrame(data);
	
}


/*! \brief Send a MIDI Time Code Quarter Frame. 
 
 See MIDI Specification for more information.
 \param data	 if you want to encode directly the nibbles in your program, you can send the byte here.
 */
void MIDI_Class::sendTimeCodeQuarterFrame(byte data)
{
	
	USE_SERIAL_PORT.write((byte)TimeCodeQuarterFrame);
	USE_SERIAL_PORT.write(data);

#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif
	
}


/*! \brief Send a Song Position Pointer message.
 \param Beats	The number of beats since the start of the song.
 */
void MIDI_Class::sendSongPosition(unsigned int Beats)
{
	
	USE_SERIAL_PORT.write((byte)SongPosition);
	USE_SERIAL_PORT.write(Beats & 0x7F);
	USE_SERIAL_PORT.write((Beats >> 7) & 0x7F);

#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif
	
}


/*! \brief Send a Song Select message */
void MIDI_Class::sendSongSelect(byte SongNumber)
{
	
	USE_SERIAL_PORT.write((byte)SongSelect);
	USE_SERIAL_PORT.write(SongNumber & 0x7F);

#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif
	
}


/*! \brief Send a Real Time (one byte) message. 
 
 \param Type The available Real Time types are: Start, Stop, Continue, Clock, ActiveSensing and SystemReset.
 You can also send a Tune Request with this method.
 @see kMIDIType
 */
void MIDI_Class::sendRealTime(kMIDIType Type)
{
	switch (Type) {
		case TuneRequest: // Not really real-time, but one byte anyway.
		case Clock:
		case Start:
		case Stop:	
		case Continue:
		case ActiveSensing:
		case SystemReset:
			USE_SERIAL_PORT.write((byte)Type);
			break;
		default:
			// Invalid Real Time marker
			break;
	}
	
	// Do not cancel Running Status for real-time messages as they can be interleaved within any message.
	// Though, TuneRequest can be sent here, and as it is a System Common message, it must reset Running Status.
#if USE_RUNNING_STATUS
	if (Type == TuneRequest) mRunningStatus_TX = InvalidType;
#endif
	
}

#endif // COMPILE_MIDI_OUT



#if COMPILE_MIDI_IN

/*! \brief Read a MIDI message from the serial port using the main input channel (see setInputChannel() for reference).
 
 Returned value: true if any valid message has been stored in the structure, false if not.
 A valid message is a message that matches the input channel. \n\n
 If the Thru is enabled and the messages matches the filter, it is sent back on the MIDI output.
 */
bool MIDI_Class::read()
{
	
	return read(mInputChannel);
	
}


/*! \brief Reading/thru-ing method, the same as read() with a given input channel to read on. */
bool MIDI_Class::read(const byte inChannel)
{
	
	if (inChannel >= MIDI_CHANNEL_OFF) return false; // MIDI Input disabled.
	
	if (parse(inChannel)) {
		
		if (input_filter(inChannel)) {
			
#if (COMPILE_MIDI_OUT && COMPILE_MIDI_THRU)
			thru_filter(inChannel);
#endif
			
#if USE_CALLBACKS
			launchCallback();
#endif
			
			return true;
		}
		
	}
	
	return false;
	
}


// Private method: MIDI parser
bool MIDI_Class::parse(byte inChannel)
{ 
	
	const int bytes_available = USE_SERIAL_PORT.available();
	
	if (bytes_available <= 0) {
		// No data available.
		return false;
	}
	
	// If the buffer is full -> Don't Panic! Call the Vogons to destroy it.
	if (bytes_available == 128) {
		USE_SERIAL_PORT.flush();
	}	
	else {
		
		/* Parsing algorithm:
		 Get a byte from the serial buffer.
		 * If there is no pending message to be recomposed, start a new one.
		 - Find type and channel (if pertinent)
		 - Look for other bytes in buffer, call parser recursively, until the message is assembled or the buffer is empty.
		 * Else, add the extracted byte to the pending message, and check validity. When the message is done, store it.
		 */
		
		
		const byte extracted = USE_SERIAL_PORT.read();
		
		if (mPendingMessageIndex == 0) { // Start a new pending message
			mPendingMessage[0] = extracted;
			
			// Check for running status first
			switch (getTypeFromStatusByte(mRunningStatus_RX)) {
					// Only these types allow Running Status:
				case NoteOff:
				case NoteOn:
				case AfterTouchPoly:
				case ControlChange:
				case ProgramChange:
				case AfterTouchChannel:
				case PitchBend:	
					
					// If the status byte is not received, prepend it to the pending message
					if (extracted < 0x80) {
						mPendingMessage[0] = mRunningStatus_RX;
						mPendingMessage[1] = extracted;
						mPendingMessageIndex = 1;
					}
					// Else: well, we received another status byte, so the running status does not apply here.
					// It will be updated upon completion of this message.
					
					break;
					
				default:
					// No running status
					break;
			}
			
			
			switch (getTypeFromStatusByte(mPendingMessage[0])) {
					
					// 1 byte messages
				case Start:
				case Continue:
				case Stop:
				case Clock:
				case ActiveSensing:
				case SystemReset:
				case TuneRequest:
					// Handle the message type directly here.
					mMessage.type = getTypeFromStatusByte(mPendingMessage[0]);
					mMessage.channel = 0;
					mMessage.data1 = 0;
					mMessage.data2 = 0;
					mMessage.valid = true;
					
					// \fix Running Status broken when receiving Clock messages.
					// Do not reset all input attributes, Running Status must remain unchanged.
					//reset_input_attributes(); 
					
					// We still need to reset these
					mPendingMessageIndex = 0;
					mPendingMessageExpectedLenght = 0;
					
					return true;
					break;
					
					// 2 bytes messages
				case ProgramChange:
				case AfterTouchChannel:
				case TimeCodeQuarterFrame:
				case SongSelect:
					mPendingMessageExpectedLenght = 2;
					break;
					
					// 3 bytes messages
				case NoteOn:
				case NoteOff:
				case ControlChange:
				case PitchBend:
				case AfterTouchPoly:
				case SongPosition:
					mPendingMessageExpectedLenght = 3;
					break;
					
				case SystemExclusive:
					mPendingMessageExpectedLenght = MIDI_SYSEX_ARRAY_SIZE; // As the message can be any lenght between 3 and MIDI_SYSEX_ARRAY_SIZE bytes
					mRunningStatus_RX = InvalidType;
					break;
					
				case InvalidType:
				default:
					// This is obviously wrong. Let's get the hell out'a here.
					reset_input_attributes();
					return false;
					break;
			}
			
			// Then update the index of the pending message.
			mPendingMessageIndex++;
			
#if USE_1BYTE_PARSING
			// Message is not complete.
			return false;
#else
			// Call the parser recursively
			// to parse the rest of the message.
			return parse(inChannel);
#endif
			
		}
		else { 
			
			// First, test if this is a status byte
			if (extracted >= 0x80) {
				
				// Reception of status bytes in the middle of an uncompleted message
				// are allowed only for interleaved Real Time message or EOX
				switch (extracted) {
					case Clock:
					case Start:
					case Continue:
					case Stop:
					case ActiveSensing:
					case SystemReset:
						
						/*
						 This is tricky. Here we will have to extract the one-byte message,
						 pass it to the structure for being read outside the MIDI class,
						 and recompose the message it was interleaved into.
						 
						 Oh, and without killing the running status.. 
						 
						 This is done by leaving the pending message as is, it will be completed on next calls.
						 */
						
						mMessage.type = (kMIDIType)extracted;
						mMessage.data1 = 0;
						mMessage.data2 = 0;
						mMessage.channel = 0;
						mMessage.valid = true;
						return true;
						
						break;
						
						// End of Exclusive
					case 0xF7:
						if (getTypeFromStatusByte(mPendingMessage[0]) == SystemExclusive) {
							
							// Store System Exclusive array in midimsg structure
							for (byte i=0;i<MIDI_SYSEX_ARRAY_SIZE;i++) {
								mMessage.sysex_array[i] = mPendingMessage[i];
							}
							
							mMessage.type = SystemExclusive;

							// Get length
							mMessage.data1 = (mPendingMessageIndex+1) & 0xFF;	
							mMessage.data2 = (mPendingMessageIndex+1) >> 8;
							
							mMessage.channel = 0;
							mMessage.valid = true;
							
							reset_input_attributes();
							
							return true;
						}
						else {
							// Well well well.. error.
							reset_input_attributes();
							return false;
						}
						
						break;
					default:
						break;
				}
				
				
				
			}
			
			
			// Add extracted data byte to pending message
			mPendingMessage[mPendingMessageIndex] = extracted;
			
			
			// Now we are going to check if we have reached the end of the message
			if (mPendingMessageIndex >= (mPendingMessageExpectedLenght-1)) {
				
				// "FML" case: fall down here with an overflown SysEx..
				// This means we received the last possible data byte that can fit the buffer.
				// If this happens, try increasing MIDI_SYSEX_ARRAY_SIZE.
				if (getTypeFromStatusByte(mPendingMessage[0]) == SystemExclusive) {
					reset_input_attributes();
					return false;
				}
				
				
				mMessage.type = getTypeFromStatusByte(mPendingMessage[0]);
				mMessage.channel = (mPendingMessage[0] & 0x0F)+1; // Don't check if it is a Channel Message
				
				mMessage.data1 = mPendingMessage[1];
				
				// Save data2 only if applicable
				if (mPendingMessageExpectedLenght == 3)	mMessage.data2 = mPendingMessage[2];
				else mMessage.data2 = 0;
				
				// Reset local variables
				mPendingMessageIndex = 0;
				mPendingMessageExpectedLenght = 0;
				
				mMessage.valid = true;
				
				// Activate running status (if enabled for the received type)
				switch (mMessage.type) {
					case NoteOff:
					case NoteOn:
					case AfterTouchPoly:
					case ControlChange:
					case ProgramChange:
					case AfterTouchChannel:
					case PitchBend:	
						// Running status enabled: store it from received message
						mRunningStatus_RX = mPendingMessage[0];
						break;
						
					default:
						// No running status
						mRunningStatus_RX = InvalidType;
						break;
				}
				return true;
			}
			else {
				// Then update the index of the pending message.
				mPendingMessageIndex++;
				
#if USE_1BYTE_PARSING
				// Message is not complete.
				return false;
#else
				// Call the parser recursively
				// to parse the rest of the message.
				return parse(inChannel);
#endif
				
			}
			
		}
		
	}
	
	// What are our chances to fall here?
	return false;
}


// Private method: check if the received message is on the listened channel
bool MIDI_Class::input_filter(byte inChannel)
{
	
	
	// This method handles recognition of channel (to know if the message is destinated to the Arduino)
	
	
	if (mMessage.type == InvalidType) return false;
	
	
	// First, check if the received message is Channel
	if (mMessage.type >= NoteOff && mMessage.type <= PitchBend) {
		
		// Then we need to know if we listen to it
		if ((mMessage.channel == mInputChannel) || (mInputChannel == MIDI_CHANNEL_OMNI)) {
			return true;
			
		}
		else {
			// We don't listen to this channel
			return false;
		}
		
	}
	else {
		
		// System messages are always received
		return true;
	}
	
}


// Private method: reset input attributes
void MIDI_Class::reset_input_attributes()
{
	
	mPendingMessageIndex = 0;
	mPendingMessageExpectedLenght = 0;
	mRunningStatus_RX = InvalidType;
	
}


// Getters
/*! \brief Get the last received message's type
 
 Returns an enumerated type. @see kMIDIType
 */
kMIDIType MIDI_Class::getType() const
{
	
	return mMessage.type;

}


/*! \brief Get the channel of the message stored in the structure.
 
 Channel range is 1 to 16. For non-channel messages, this will return 0.
 */
byte MIDI_Class::getChannel() const
{
	
	return mMessage.channel;

}


/*! \brief Get the first data byte of the last received message. */
byte MIDI_Class::getData1() const
{
	
	return mMessage.data1;

}


/*! \brief Get the second data byte of the last received message. */
byte MIDI_Class::getData2() const
{ 
	
	return mMessage.data2;

}


/*! \brief Get the System Exclusive byte array. 
 
 @see getSysExArrayLength to get the array's length in bytes.
 */
const byte * MIDI_Class::getSysExArray() const
{ 
	
	return mMessage.sysex_array;

}

/*! \brief Get the lenght of the System Exclusive array.
 
 It is coded using data1 as LSB and data2 as MSB.
 \return The array's length, in bytes.
 */
unsigned int MIDI_Class::getSysExArrayLength() const
{
	
	unsigned int coded_size = ((unsigned int)(mMessage.data2) << 8) | mMessage.data1;
	
	return (coded_size > MIDI_SYSEX_ARRAY_SIZE) ? MIDI_SYSEX_ARRAY_SIZE : coded_size;
	
}


/*! \brief Check if a valid message is stored in the structure. */
bool MIDI_Class::check() const
{ 
	
	return mMessage.valid;

}


// Setters
/*! \brief Set the value for the input MIDI channel 
 \param Channel the channel value. Valid values are 1 to 16, 
 MIDI_CHANNEL_OMNI if you want to listen to all channels, and MIDI_CHANNEL_OFF to disable MIDI input.
 */
void MIDI_Class::setInputChannel(const byte Channel)
{ 
	
	mInputChannel = Channel;
	
}


#if USE_CALLBACKS

void MIDI_Class::setHandleNoteOff(void (*fptr)(byte channel, byte note, byte velocity))			{ mNoteOffCallback = fptr; }
void MIDI_Class::setHandleNoteOn(void (*fptr)(byte channel, byte note, byte velocity))			{ mNoteOnCallback = fptr; }
void MIDI_Class::setHandleAfterTouchPoly(void (*fptr)(byte channel, byte note, byte pressure))	{ mAfterTouchPolyCallback = fptr; }
void MIDI_Class::setHandleControlChange(void (*fptr)(byte channel, byte number, byte value))	{ mControlChangeCallback = fptr; }
void MIDI_Class::setHandleProgramChange(void (*fptr)(byte channel, byte number))				{ mProgramChangeCallback = fptr; }
void MIDI_Class::setHandleAfterTouchChannel(void (*fptr)(byte channel, byte pressure))			{ mAfterTouchChannelCallback = fptr; }
void MIDI_Class::setHandlePitchBend(void (*fptr)(byte channel, int bend))						{ mPitchBendCallback = fptr; }
void MIDI_Class::setHandleSystemExclusive(void (*fptr)(byte * array, byte size))				{ mSystemExclusiveCallback = fptr; }
void MIDI_Class::setHandleTimeCodeQuarterFrame(void (*fptr)(byte data))							{ mTimeCodeQuarterFrameCallback = fptr; }
void MIDI_Class::setHandleSongPosition(void (*fptr)(unsigned int beats))						{ mSongPositionCallback = fptr; }
void MIDI_Class::setHandleSongSelect(void (*fptr)(byte songnumber))								{ mSongSelectCallback = fptr; }
void MIDI_Class::setHandleTuneRequest(void (*fptr)(void))										{ mTuneRequestCallback = fptr; }
void MIDI_Class::setHandleClock(void (*fptr)(void))												{ mClockCallback = fptr; }
void MIDI_Class::setHandleStart(void (*fptr)(void))												{ mStartCallback = fptr; }
void MIDI_Class::setHandleContinue(void (*fptr)(void))											{ mContinueCallback = fptr; }
void MIDI_Class::setHandleStop(void (*fptr)(void))												{ mStopCallback = fptr; }
void MIDI_Class::setHandleActiveSensing(void (*fptr)(void))										{ mActiveSensingCallback = fptr; }
void MIDI_Class::setHandleSystemReset(void (*fptr)(void))										{ mSystemResetCallback = fptr; }


/*! \brief Detach an external function from the given type.
 
 Use this method to cancel the effects of setHandle********.
 \param Type		The type of message to unbind. When a message of this type is received, no function will be called.
 */
void MIDI_Class::disconnectCallbackFromType(kMIDIType Type)
{
	
	switch (Type) {
		case NoteOff:               mNoteOffCallback = NULL;                break;
		case NoteOn:                mNoteOnCallback = NULL;                 break;
		case AfterTouchPoly:        mAfterTouchPolyCallback = NULL;         break;
		case ControlChange:         mControlChangeCallback = NULL;          break;
		case ProgramChange:         mProgramChangeCallback = NULL;          break;
		case AfterTouchChannel:     mAfterTouchChannelCallback = NULL;      break;
		case PitchBend:             mPitchBendCallback = NULL;              break;
		case SystemExclusive:       mSystemExclusiveCallback = NULL;        break;
		case TimeCodeQuarterFrame:  mTimeCodeQuarterFrameCallback = NULL;   break;
		case SongPosition:          mSongPositionCallback = NULL;           break;
		case SongSelect:            mSongSelectCallback = NULL;             break;
		case TuneRequest:           mTuneRequestCallback = NULL;            break;
		case Clock:                 mClockCallback = NULL;                  break;
		case Start:                 mStartCallback = NULL;                  break;
		case Continue:              mContinueCallback = NULL;               break;
		case Stop:                  mStopCallback = NULL;                   break;
		case ActiveSensing:         mActiveSensingCallback = NULL;          break;
		case SystemReset:           mSystemResetCallback = NULL;            break;
		default:
			break;
	}
	
}


// Private - launch callback function based on received type.
void MIDI_Class::launchCallback()
{
	
	// The order is mixed to allow frequent messages to trigger their callback faster.
	
	switch (mMessage.type) {
			// Notes
		case NoteOff:				if (mNoteOffCallback != NULL)				mNoteOffCallback(mMessage.channel,mMessage.data1,mMessage.data2);	break;
		case NoteOn:				if (mNoteOnCallback != NULL)				mNoteOnCallback(mMessage.channel,mMessage.data1,mMessage.data2);	break;
			
			// Real-time messages
		case Clock:					if (mClockCallback != NULL)					mClockCallback();			break;			
		case Start:					if (mStartCallback != NULL)					mStartCallback();			break;
		case Continue:				if (mContinueCallback != NULL)				mContinueCallback();		break;
		case Stop:					if (mStopCallback != NULL)					mStopCallback();			break;
		case ActiveSensing:			if (mActiveSensingCallback != NULL)			mActiveSensingCallback();	break;
			
			// Continuous controllers
		case ControlChange:			if (mControlChangeCallback != NULL)			mControlChangeCallback(mMessage.channel,mMessage.data1,mMessage.data2);	break;
		case PitchBend:				if (mPitchBendCallback != NULL)				mPitchBendCallback(mMessage.channel,(int)((mMessage.data1 & 0x7F) | ((mMessage.data2 & 0x7F)<< 7)) - 8192);	break; // TODO: check this
		case AfterTouchPoly:		if (mAfterTouchPolyCallback != NULL)		mAfterTouchPolyCallback(mMessage.channel,mMessage.data1,mMessage.data2);	break;
		case AfterTouchChannel:		if (mAfterTouchChannelCallback != NULL)		mAfterTouchChannelCallback(mMessage.channel,mMessage.data1);	break;
			
		case ProgramChange:			if (mProgramChangeCallback != NULL)			mProgramChangeCallback(mMessage.channel,mMessage.data1);	break;
		case SystemExclusive:		if (mSystemExclusiveCallback != NULL)		mSystemExclusiveCallback(mMessage.sysex_array,mMessage.data1);	break;
			
			// Occasional messages
		case TimeCodeQuarterFrame:	if (mTimeCodeQuarterFrameCallback != NULL)	mTimeCodeQuarterFrameCallback(mMessage.data1);	break;
		case SongPosition:			if (mSongPositionCallback != NULL)			mSongPositionCallback((mMessage.data1 & 0x7F) | ((mMessage.data2 & 0x7F)<< 7));	break;
		case SongSelect:			if (mSongSelectCallback != NULL)			mSongSelectCallback(mMessage.data1);	break;
		case TuneRequest:			if (mTuneRequestCallback != NULL)			mTuneRequestCallback();	break;
			
		case SystemReset:			if (mSystemResetCallback != NULL)			mSystemResetCallback();	break;
		case InvalidType:
		default:
			break;
	}
	
}


#endif // USE_CALLBACKS


#endif // COMPILE_MIDI_IN




#if (COMPILE_MIDI_IN && COMPILE_MIDI_OUT && COMPILE_MIDI_THRU) // Thru

/*! \brief Set the filter for thru mirroring
 \param inThruFilterMode a filter mode
 
 @see kThruFilterMode
 */
void MIDI_Class::setThruFilterMode(kThruFilterMode inThruFilterMode)
{ 
	
	mThruFilterMode = inThruFilterMode;
	if (mThruFilterMode != Off) mThruActivated = true;
	else mThruActivated = false;
	
}


/*! \brief Setter method: turn message mirroring on. */
void MIDI_Class::turnThruOn(kThruFilterMode inThruFilterMode)
{ 
	
	mThruActivated = true;
	mThruFilterMode = inThruFilterMode;
	
}


/*! \brief Setter method: turn message mirroring off. */
void MIDI_Class::turnThruOff()
{
	
	mThruActivated = false; 
	mThruFilterMode = Off;
	
}


// This method is called upon reception of a message and takes care of Thru filtering and sending.
void MIDI_Class::thru_filter(byte inChannel)
{
	
	/*
	 This method handles Soft-Thru filtering.
	 
	 Soft-Thru filtering:
	 - All system messages (System Exclusive, Common and Real Time) are passed to output unless filter is set to Off
	 - Channel messages are passed to the output whether their channel is matching the input channel and the filter setting
	 
	 */
	
#if TEENSY_SUPPORT && TEENSY_MIDI_TO_USB
	// Pass the message to the USB side if enabled
	
#endif
	
	// If the feature is disabled, don't do anything.
	if (!mThruActivated || (mThruFilterMode == Off)) return;
	
	
	// First, check if the received message is Channel
	if (mMessage.type >= NoteOff && mMessage.type <= PitchBend) {
		
		const bool filter_condition = ((mMessage.channel == mInputChannel) || (mInputChannel == MIDI_CHANNEL_OMNI));
		
		// Now let's pass it to the output
		switch (mThruFilterMode) {
			case Full:
				send(mMessage.type,mMessage.data1,mMessage.data2,mMessage.channel);
				return;
				break;
			case SameChannel:
				if (filter_condition) {
					send(mMessage.type,mMessage.data1,mMessage.data2,mMessage.channel);
					return;
				}
				break;
			case DifferentChannel:
				if (!filter_condition) {
					send(mMessage.type,mMessage.data1,mMessage.data2,mMessage.channel);
					return;
				}
				break;
			case Off:
				// Do nothing. 
				// Technically it's impossible to get there because the case was already tested earlier.
				break;
			default:
				break;
		}
		
	}
	else {
		
		// Send the message to the output
		switch (mMessage.type) {
				// Real Time and 1 byte
			case Clock:
			case Start:
			case Stop:
			case Continue:
			case ActiveSensing:
			case SystemReset:
			case TuneRequest:	
				sendRealTime(mMessage.type);
				return;
				break;
				
			case SystemExclusive:
				// Send SysEx (0xF0 and 0xF7 are included in the buffer)
				sendSysEx(mMessage.data1,mMessage.sysex_array,true); 
				return;
				break;
				
			case SongSelect:
				sendSongSelect(mMessage.data1);
				return;
				break;
				
			case SongPosition:
				sendSongPosition(mMessage.data1 | ((unsigned)mMessage.data2<<7));
				return;
				break;
				
			case TimeCodeQuarterFrame:
				sendTimeCodeQuarterFrame(mMessage.data1,mMessage.data2);
				return;
				break;
			default:
				break;
				
		}
		
	}
	
}


#endif // Thru


