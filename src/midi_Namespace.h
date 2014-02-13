/*!
 *  @file       midi_Namespace.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Namespace declaration
 *  @version    4.0
 *  @author     Francois Best
 *  @date       24/02/11
 *  license     GPL Forty Seven Effects - 2011
 */

#pragma once

#define MIDI_NAMESPACE                  midi
#define BEGIN_MIDI_NAMESPACE            namespace MIDI_NAMESPACE {
#define END_MIDI_NAMESPACE              }

#define USING_NAMESPACE_MIDI            using namespace MIDI_NAMESPACE;

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE
