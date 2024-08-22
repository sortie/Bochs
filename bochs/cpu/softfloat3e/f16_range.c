/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016 The Regents of the University of
California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float16 f16_range(float16 a, float16 b, bool is_max, bool is_abs, int sign_ctrl, softfloat_status_t *status)
{
    bool signA;
    int8_t expA;
    uint16_t sigA;
    bool signB;
    int8_t expB;
    uint16_t sigB;
    bool aIsNaN, bIsNaN;
    uint16_t z;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    signA = signF16UI(a);
    expA  = expF16UI(a);
    sigA  = fracF16UI(a);
    signB = signF16UI(b);
    expB  = expF16UI(b);
    sigB  = fracF16UI(b);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (softfloat_isSigNaNF16UI(a)) {
        return softfloat_propagateNaNF16UI(a, 0, status);
    }
    if (softfloat_isSigNaNF16UI(b)) {
        return softfloat_propagateNaNF16UI(b, 0, status);
    }

    aIsNaN = isNaNF16UI(a);
    bIsNaN = isNaNF16UI(b);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (! expA && sigA) {
        if (softfloat_denormalsAreZeros(status)) {
            a = packToF16UI(signA, 0, 0);
        }
        else if (! bIsNaN) {
            softfloat_raiseFlags(status, softfloat_flag_denormal);
        }
    }

    if (! expB && sigB) {
        if (softfloat_denormalsAreZeros(status)) {
            b = packToF16UI(signB, 0, 0);
        }
        else if (! aIsNaN) {
            softfloat_raiseFlags(status, softfloat_flag_denormal);
        }
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (bIsNaN) {
        z = a;
    }
    else if (aIsNaN) {
        z = b;
    }
    else if (signA != signB && ! is_abs) {
        if (! is_max) {
            z = signA ? a : b;
        } else {
            z = signA ? b : a;
        }
    } else {
        float16 tmp_a = a, tmp_b = b;
        if (is_abs) {
            tmp_a = tmp_a & ~0x8000; // clear the sign bit
            tmp_b = tmp_b & ~0x8000;
            signA = 0;
        }

        if (! is_max) {
            z = (signA ^ (tmp_a < tmp_b)) ? a : b;
        } else {
            z = (signA ^ (tmp_a < tmp_b)) ? b : a;
        }
    }

    switch(sign_ctrl) {
    case 0:
        z = (z & ~0x8000) | (a & 0x8000); // keep sign of a
        break;
    case 1:
        break; // preserve sign of compare result
    case 2:
        z = z & ~0x8000; // zero out the sign bit
        break;
    case 3:
        z = z | 0x8000;  // set the sign bit
        break;
    }

    return z;
}       
