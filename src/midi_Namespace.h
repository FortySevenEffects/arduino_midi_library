/*!
 *  @file       midi_Namespace.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Namespace declaration
 *  @version    4.2
 *  @author     Francois Best
 *  @date       24/02/11
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

#define MIDI_NAMESPACE                  midi
#define BEGIN_MIDI_NAMESPACE            namespace MIDI_NAMESPACE {
#define END_MIDI_NAMESPACE              }

#define USING_NAMESPACE_MIDI            using namespace MIDI_NAMESPACE;

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE
