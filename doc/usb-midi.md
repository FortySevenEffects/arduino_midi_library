# Using the library for MIDI through USB

Some boards have "native" USB functionality, which means they can directly
connect to a computer and appear as a USB MIDI device.

Here are the officially supported boards:

-   Arduino Leonardo
-   Arduino Due

## Pre-requisites

To use USB MIDI to send messages to the computer, you must disable Running
Status in the settings. This is because the USB transport is stateless and has
no knowledge of the last message's running status.
The setting is off by default, so if you use custom settings, keep that in mind.
