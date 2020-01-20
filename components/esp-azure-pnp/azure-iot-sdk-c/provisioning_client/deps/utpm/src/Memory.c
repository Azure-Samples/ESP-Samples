// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//** Description
// This file contains a set of miscellaneous memory manipulation routines. Many
// of the functions have the same semantics as functions defined in string.h.
// Those functions are not used directly in the TPM because they are not 'safe'
//
// This version uses string.h after adding guards.  This is because the math
// libraries invariably use those functions so it is not practical to prevent
// those library functions from being pulled into the build.

//** Includes and Data Definitions
#include <stdio.h>
#include <string.h>
#include "azure_utpm_c/Tpm.h"
#include "azure_utpm_c/Memory_fp.h"

//** Functions

//*** MemoryCopy()
// This is an alias for memmove. This is used in place of memcpy because
// some of the moves may overlap and rather than try to make sure that
// memmove is used when necessary, it is always used.
// The #if 0 is used to prevent instantiation of the MemoryCopy function so that
// the #define is always used
void MemoryCopy(void* dest, const void* src, int sSize)
{
    if (dest != NULL && src != NULL)
    {
        memmove(dest, src, sSize);
    }
}

//*** MemoryEqual()
// This function indicates if two buffers have the same values in the indicated
// number of bytes.
// return type: BOOL
//      TRUE    all octets are the same
//      FALSE   all octets are not the same
BOOL MemoryEqual(
    const void      *buffer1,       // IN: compare buffer1
    const void      *buffer2,       // IN: compare buffer2
    unsigned int     size           // IN: size of bytes being compared
    )
{
    BYTE         equal = 0;
    const BYTE  *b1 = (BYTE *)buffer1;
    const BYTE  *b2 = (BYTE *)buffer2;
//
    // Compare all bytes so that there is no leakage of information
    // due to timing differences.
    for(; size > 0; size--)
        equal |= (*b1++ ^ *b2++);
    return (equal == 0);
}

//*** MemoryCopy2B()
// This function copies a TPM2B. This can be used when the TPM2B types are
// the same or different.
//
// This function returns the number of octets in the data buffer of the TPM2B.
INT16 MemoryCopy2B(TPM2B* dest, const TPM2B* source, unsigned int dSize)
{
    INT16 result;
    if (dest == NULL)
    {
        result = 0;
    }
    else
    {
        if (source == NULL)
        {
            dest->size = 0;
        }
        else if (source->size > dSize)
        {
            dest->size = 0;
        }
        else
        {
            //pAssert(source->size <= dSize);
            MemoryCopy(dest->buffer, source->buffer, source->size);
            dest->size = source->size;
        }
        result = dest->size;
    }
    return result;
}

//*** MemoryConcat2B()
// This function will concatenate the buffer contents of a TPM2B to an
// the buffer contents of another TPM2B and adjust the size accordingly
//      ('a' := ('a' | 'b')).
void MemoryConcat2B(
    TPM2B* aInOut,        // IN/OUT: destination 2B
    TPM2B* bIn,           // IN: second 2B
    unsigned int aMaxSize       // IN: The size of aInOut.buffer (max values for
                                    //     aInOut.size)
    )
{
    if (bIn->size <= aMaxSize - aInOut->size)
    {
        MemoryCopy(&aInOut->buffer[aInOut->size], &bIn->buffer, bIn->size);
        aInOut->size = aInOut->size + bIn->size;
    }
    return;
}

//*** MemoryEqual2B()
// This function will compare two TPM2B structures. To be equal, they
// need to be the same size and the buffer contexts need to be the same
// in all octets.
// return type: BOOL
//      TRUE    size and buffer contents are the same
//      FALSE   size or buffer contents are not the same
BOOL
MemoryEqual2B(
    const TPM2B     *aIn,           // IN: compare value
    const TPM2B     *bIn            // IN: compare value
    )
{
    if(aIn->size != bIn->size)
        return FALSE;
    return MemoryEqual(aIn->buffer, bIn->buffer, aIn->size);
}

//*** MemorySet()
// This function will set all the octets in the specified memory range to
// the specified octet value.
// Note: A previous version had an additional parameter (dSize) that was
// intended to make sure that the destination would not be overrun. The
// problem is that, in use, all that was happening was that the value of
// size was used for dSize so there was no benefit in the extra parameter.
void
MemorySet(
    void            *dest,
    int              value,
    size_t           size
    )
{
    memset(dest, value, size);
}

//*** MemoryPad2B()
// Function to pad a TPM2B with zeros and adjust the size.
void
MemoryPad2B(
    TPM2B           *b,
    UINT16           newSize
    )
{
    MemorySet(&b->buffer[b->size], 0, newSize - b->size);
    b->size = newSize;
}


//*** Uint16ToByteArray()
// Function to write an integer to a byte array
#ifndef INLINE_FUNCTIONS
void
Uint16ToByteArray(
    UINT16              i,
    BYTE                *a
    )
{
    a[1] = (BYTE)(i); i >>= 8;
    a[0] = (BYTE)(i);
}
#endif // INLINE_FUNCTIONS


//*** Uint32ToByteArray()
// Function to write an integer to a byte array
#ifndef INLINE_FUNCTIONS
void
Uint32ToByteArray(
    UINT32              i,
    BYTE                *a
    )
{
    a[3] = (BYTE)(i); i >>= 8;
    a[2] = (BYTE)(i); i >>= 8;
    a[1] = (BYTE)(i); i >>= 8;
    a[0] = (BYTE)(i);
}
#endif // INLINE_FUNCTIONS

//*** Uint64ToByteArray()
// Function to write an integer to a byte array
#ifndef INLINE_FUNCTIONS
void
Uint64ToByteArray(
    UINT64               i,
    BYTE                *a
    )
{
    a[7] = (BYTE)(i); i >>= 8;
    a[6] = (BYTE)(i); i >>= 8;
    a[5] = (BYTE)(i); i >>= 8;
    a[4] = (BYTE)(i); i >>= 8;
    a[3] = (BYTE)(i); i >>= 8;
    a[2] = (BYTE)(i); i >>= 8;
    a[1] = (BYTE)(i); i >>= 8;
    a[0] = (BYTE)(i);
}
#endif // INLINE_FUNCTIONS


//*** ByteArrayToUint16()
// Function to write an integer to a byte array
#ifndef INLINE_FUNCTIONS
UINT16 ByteArrayToUint16(BYTE* a)
{
    UINT16      retVal;
    retVal  = a[0]; retVal <<= 8;
    retVal += a[1];
    return retVal;
}
#endif // INLINE_FUNCTIONS


//*** ByteArrayToUint32()
// Function to write an integer to a byte array
#ifndef INLINE_FUNCTIONS
UINT32 ByteArrayToUint32(BYTE* a)
{
    UINT32      retVal;
    retVal  = a[0]; retVal <<= 8;
    retVal += a[1]; retVal <<= 8;
    retVal += a[2]; retVal <<= 8;
    retVal += a[3];
    return retVal;
}
#endif // INLINE_FUNCTIONS

//*** ByteArrayToUint64()
// Function to write an integer to a byte array
#ifndef INLINE_FUNCTIONS
UINT64 ByteArrayToUint64(BYTE* a)
{
    UINT64      retVal;
    retVal  = a[0]; retVal <<= 8;
    retVal += a[1]; retVal <<= 8;
    retVal += a[2]; retVal <<= 8;
    retVal += a[3]; retVal <<= 8;
    retVal += a[4]; retVal <<= 8;
    retVal += a[5]; retVal <<= 8;
    retVal += a[6]; retVal <<= 8;
    retVal += a[7];
    return retVal;
}
#endif // INLINE_FUNCTIONS
