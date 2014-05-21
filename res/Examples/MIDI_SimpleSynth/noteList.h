/*!
 *  \file       synth-core_NoteList.h
 *  \author     Francois Best
 *  \date       24/05/2013
 *  \license    GPL v3.0 - Copyright Forty Seven Effects 2013
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

#include <inttypes.h>

typedef uint8_t byte;

// -----------------------------------------------------------------------------

struct MidiNote
{
    inline MidiNote();
    inline MidiNote(byte inPitch, byte inVelocity);
    inline MidiNote(const MidiNote& inOther);
    inline MidiNote& operator= (const MidiNote& inOther);

    byte pitch;
    byte velocity;
};

// -----------------------------------------------------------------------------

template<byte Size>
class MidiNoteList
{
private:
    struct Cell
    {
        inline Cell();
        inline Cell(const Cell& inOther);
        inline Cell& operator= (const Cell& inOther);

        MidiNote note;
        bool active;
        Cell* next;
        Cell* prev;
    };

public:
    inline  MidiNoteList();
    inline ~MidiNoteList();

public:
    inline void add(const MidiNote& inNote);
    inline void remove(byte inPitch);

public:
    inline bool get(byte inIndex, byte& outPitch) const;
    inline bool getTail(byte& outPitch) const;

public:
    inline bool empty() const;
    inline byte size() const;

private:
    inline Cell* getFirstEmptyCell();
    inline void print() const;

private:
    Cell mArray[Size];
    Cell* mHead;
    Cell* mTail;
    byte mSize;
};

// ########################################################################## //
// Inline implementation

inline MidiNote::MidiNote()
    : pitch(0)
    , velocity(0)
{
}

inline MidiNote::MidiNote(byte inPitch, byte inVelocity)
    : pitch(inPitch)
    , velocity(inVelocity)
{
}

inline MidiNote::MidiNote(const MidiNote& inOther)
    : pitch(inOther.pitch)
    , velocity(inOther.velocity)
{
}

inline MidiNote& MidiNote::operator= (const MidiNote& inOther)
{
    pitch = inOther.pitch;
    velocity = inOther.velocity;
    return *this;
}

// ########################################################################## //

template<byte Size>
inline MidiNoteList<Size>::Cell::Cell()
    : note()
    , active(false)
    , next(0)
    , prev(0)
{
}

template<byte Size>
inline MidiNoteList<Size>::Cell::Cell(const Cell& inOther)
    : note(inOther.note)
    , active(inOther.active)
    , next(inOther.next)
    , prev(inOther.prev)
{
}

template<byte Size>
inline typename MidiNoteList<Size>::Cell& MidiNoteList<Size>::Cell::operator= (const Cell& inOther)
{
    note = inOther.note;
    active = inOther.active;
    next = inOther.next;
    prev = inOther.prev;
    return *this;
}

// ########################################################################## //

template<byte Size>
inline MidiNoteList<Size>::MidiNoteList()
{
}

template<byte Size>
inline MidiNoteList<Size>::~MidiNoteList()
{
}

// -----------------------------------------------------------------------------

template<byte Size>
inline void MidiNoteList<Size>::add(const MidiNote& inNote)
{
    if (mHead == 0)
    {
        mArray[0].note   = inNote;
        mArray[0].active = true;
        mArray[0].next   = 0;
        mArray[0].prev   = 0;
        mHead = mArray;
        mTail = mArray;
    }
    else
    {
        // Find the first inactive cell, and use it as tail.
        Cell* const oldTail = mTail;
        Cell* const newTail = getFirstEmptyCell();

        newTail->active = true;
        newTail->note = inNote;

        oldTail->next = newTail;
        newTail->prev = oldTail;
        newTail->next = 0;
        mTail = newTail;
    }
    mSize++;
    print();
}

template<byte Size>
inline void MidiNoteList<Size>::remove(byte inPitch)
{
    if (mHead != 0)
    {
        for (Cell* it = mHead; it != 0; it = it->next)
        {
            if (it->note.pitch == inPitch)
            {
                Cell* const prev = it->prev;
                Cell* const next = it->next;

                it->active = false;
                it->next = 0;
                it->prev = 0;

                // Reconnect both ends
                if (it == mHead)
                {
                    //AVR_ASSERT(prev == 0);
                    mHead = next;
                }
                else
                {
                    //AVR_ASSERT(prev != 0);
                    prev->next = next;
                }

                if (it == mTail)
                {
                    //AVR_ASSERT(next == 0);
                    mTail = prev;
                }
                else
                {
                    //AVR_ASSERT(next != 0);
                    next->prev = prev;
                }

                mSize--;
                break;
            }
        }
    }
    print();
}

// -----------------------------------------------------------------------------

template<byte Size>
inline bool MidiNoteList<Size>::get(byte inIndex, byte& outPitch) const
{
    if (mTail)
    {
        const Cell* it = mTail;
        for (byte i = 0; i < inIndex; ++i)
        {
            if (it->prev)
            {
                it = it->prev;
            }
        }

        print();
        //AVR_LOG("Index " << inIndex << ": " << it->note.pitch);

        outPitch = it->note.pitch;
        return true;
    }
    return false;
}

template<byte Size>
inline bool MidiNoteList<Size>::getTail(byte& outPitch) const
{
    if (mTail)
    {
        outPitch = mTail->note.pitch;
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------

template<byte Size>
inline bool MidiNoteList<Size>::empty() const
{
    return mSize == 0;
}

template<byte Size>
inline byte MidiNoteList<Size>::size() const
{
    return mSize;
}

// -----------------------------------------------------------------------------

template<byte Size>
inline typename MidiNoteList<Size>::Cell* MidiNoteList<Size>::getFirstEmptyCell()
{
    for (byte i = 0; i < Size; ++i)
    {
        if (mArray[i].active == false)
        {
            return mArray + i;
        }
    }
    return 0;
}

template<byte Size>
inline void MidiNoteList<Size>::print() const
{
//#ifndef NDEBUG
//    AVR_DBG("Note List: [ ");
//    if (mHead)
//    {
//        for (const Cell* it = mHead; it != 0; it = it->next)
//        {
//            AVR_DBG(it->note.pitch);
//            if (it->next)
//                AVR_DBG(" -> ");
//        }
//    }
//    AVR_LOG(" ]");
//#endif
}
