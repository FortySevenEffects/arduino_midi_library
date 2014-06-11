/*!
 *  @file       midi_Message.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Message struct definition
 *  @version    4.2
 *  @author     Francois Best
 *  @date       11/06/14
 *  @license    GPL v3.0 - Copyright Forty Seven Effects 2014
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "midi_Namespace.h"
#include "midi_Defs.h"

BEGIN_MIDI_NAMESPACE

/*! The Message structure contains decoded data of a MIDI message
    read from the serial port with read()
 */
template<unsigned SysExMaxSize>
struct Message
{
    /*! The maximum size for the System Exclusive array.
    */
    static const unsigned sSysExMaxSize = SysExMaxSize;

    /*! The MIDI channel on which the message was recieved.
     \n Value goes from 1 to 16.
     */
    Channel channel;

    /*! The type of the message
     (see the MidiType enum for types reference)
     */
    MidiType type;

    /*! The first data byte.
     \n Value goes from 0 to 127.
     */
    DataByte data1;

    /*! The second data byte.
     If the message is only 2 bytes long, this one is null.
     \n Value goes from 0 to 127.
     */
    DataByte data2;

    /*! System Exclusive dedicated byte array.
     \n Array length is stocked on 16 bits,
     in data1 (LSB) and data2 (MSB)
     */
    DataByte sysexArray[sSysExMaxSize];

    /*! This boolean indicates if the message is valid or not.
     There is no channel consideration here,
     validity means the message respects the MIDI norm.
     */
    bool valid;

    inline unsigned getSysExSize() const
    {
        const unsigned size = unsigned(data2) << 8 | data1;
        return size > sSysExMaxSize ? sSysExMaxSize : size;
    }
};

END_MIDI_NAMESPACE
