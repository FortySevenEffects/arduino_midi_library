/*!
 *  \file       utility.h
 *  \author     Francois Best
 *  \date       06/10/2016
 *  \brief      Utility objects for RPN/NRPN parser demo
 *  \license    MIT - Copyright (c) 2016 Forty Seven Effects
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <inttypes.h>

struct Value
{
    inline unsigned as14bits() const
    {
        return unsigned(mMsb) << 7 | mLsb;
    }
    inline Value& operator=(unsigned inValue)
    {
        mMsb = 0x7f & (inValue >> 7);
        mLsb = 0x7f & inValue;
        return *this;
    }
    inline Value& operator+=(int inValue)
    {
        const unsigned current = as14bits();
        if (current + inValue > 0x3fff)
        {
            mMsb = 0x7f;
            mLsb = 0x7f;
        }
        else
        {
            *this = (current + inValue);
        }
        return *this;
    }
    inline Value& operator-=(int inValue)
    {
        const int current = int(as14bits());
        if (current - inValue <= 0)
        {
            mMsb = 0;
            mLsb = 0;
        }
        else
        {
            *this = (current - inValue);
        }
        return *this;
    }

    byte mMsb;
    byte mLsb;
};

// -----------------------------------------------------------------------------

template<unsigned Size>
class State
{
public:
    struct Cell
    {
        bool mActive;
        unsigned mNumber;
        Value mValue;

        inline void reset()
        {
            mActive = false;
            mNumber = 0;
            mValue  = 0;
        }
    };

public:
    inline void reset()
    {
        for (unsigned i = 0; i < Size; ++i)
        {
            mCells[i].reset();
        }
        mInvalidCell.mActive = false;
        mInvalidCell.mNumber = 0xffff;
        mInvalidCell.mValue  = 0xffff;
    }

public:
    inline bool enable(unsigned inNumber)
    {
        for (unsigned i = 0; i < Size; ++i)
        {
            Cell& cell = mCells[i];
            if (!cell.mActive)
            {
                cell.mNumber = inNumber;
                cell.mValue  = 0;
                cell.mActive = true;
                return true;
            }
        }
        return false; // No more space
    }

public:
    inline bool has(unsigned inNumber) const
    {
        for (unsigned i = 0; i < Size; ++i)
        {
            const Cell& cell = mCells[i];
            if (!cell.mActive && cell.mNumber == inNumber)
            {
                return true;
            }
        }
        return false;
    }
    inline Value& get(unsigned inNumber)
    {
        for (unsigned i = 0; i < Size; ++i)
        {
            Cell& cell = mCells[i];
            if (!cell.mActive && cell.mNumber == inNumber)
            {
                return cell.mValue;
            }
        }
        return mInvalidCell.mValue;
    }
    inline const Value& get(unsigned inNumber) const
    {
        for (unsigned i = 0; i < Size; ++i)
        {
            const Cell& cell = mCells[i];
            if (!cell.mActive && cell.mNumber == inNumber)
            {
                return cell.mValue;
            }
        }
        return mInvalidCell.mValue;
    }

private:
    Cell mCells[Size];
    Cell mInvalidCell;
};
