# SysEx Encoding & Decoding

There are various ways of encoding & decoding arbitrary 8-bit wide data into
SysEx, which is 7-bit wide.

The [official documentation](http://www.somascape.org/midi/tech/spec.html#nusx_fd)
for FileDump data exchanges states the following:

  > The 8-bit file data needs to be converted to 7-bit form,
  > with the result that every 7 bytes of file data translates
  > to 8 bytes in the MIDI stream.
  >
  > For each group of 7 bytes (of file data) the top bit from each
  > is used to construct an eigth byte, which is sent first.
  > So:
  > ```
  >   AAAAaaaa BBBBbbbb CCCCcccc DDDDdddd EEEEeeee FFFFffff GGGGgggg
  > ```
  > becomes:
  > ```
  >   0ABCDEFG 0AAAaaaa 0BBBbbbb 0CCCcccc 0DDDdddd 0EEEeeee 0FFFffff 0GGGgggg
  > ```
  >
  > The final group may have less than 7 bytes, and is coded as follows
  > (e.g. with 3 bytes in the final group):
  > ```
  >   0ABC0000 0AAAaaaa 0BBBbbbb 0CCCcccc
  > ```

## SysEx encoding / decoding functions

The MIDI library supplies two functions to do this, `encodeSysEx` and `decodeSysEx`.

Example usage:
```c++
#include <MIDI.h>

static const byte myData[12] = {
    // Hex dump: CAFEBABE BAADF00D FACADE42
    0xca, 0xfe, 0xba, 0xbe, 0xba, 0xad, 0xf0, 0x0d,
    0xfa, 0xca, 0xde, 0x42
};

byte encoded[16];
const unsigned encodedSize = midi::encodeSysEx(myData, encoded, 12);
// Encoded hex dump: 07 4a 7e 3a 3e 3a 2d 70 07 0d 7a 4a 5e 42

byte decoded[12];
const unsigned decoded = midi::decodeSysEx(encoded, decoded, encodedSize);
```

## Special case for Korg devices

Korg apparently uses another convention for their SysEx encoding / decoding,
where:
```
AAAAaaaa BBBBbbbb CCCCcccc DDDDdddd EEEEeeee FFFFffff GGGGgggg
```
becomes:
```
0GFEDCBA 0AAAaaaa 0BBBbbbb 0CCCcccc 0DDDdddd 0EEEeeee 0FFFffff 0GGGgggg
```

The order of the bits in the "header" byte is reversed.
To follow this beheaviour, set the inFlipHeaderBits argument to true.

Example:
```c++
void handleSysEx(byte* inData, unsigned inSize)
{
    // SysEx body data starts at 3rd byte: F0 42 aa bb cc dd F7
    // 42 being the hex value of the Korg SysEx ID.
    const unsigned dataStartOffset   = 2;
    const unsigned encodedDataLength = inSize - 3; // Remove F0 42 & F7

    // Create a large enough buffer where to decode the message
    byte decodedData[64];

    const unsigned decodedSize = decodeSysEx(inData + dataStartOffset,
                                             decodedData,
                                             encodedDataLength,
                                             true); // flip header bits
    // Do stuff with your message
}
```

See original discussion in issue [#92](FortySevenEffects/arduino_midi_library#92).
