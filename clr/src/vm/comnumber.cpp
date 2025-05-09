// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
#include "common.h"
#include "excep.h"
#include "comnumber.h"
#include "comstring.h"
#include "comdecimal.h"
#include <stdlib.h>

typedef wchar_t wchar;

#define INT32_PRECISION 10
#define UINT32_PRECISION INT32_PRECISION
#define INT64_PRECISION 19
#define UINT64_PRECISION 20
#define FLOAT_PRECISION 7
#define DOUBLE_PRECISION 15
#define LARGE_BUFFER_SIZE 600
#define MIN_BUFFER_SIZE 105

#define SCALE_NAN 0x80000000
#define SCALE_INF 0x7FFFFFFF

struct FPSINGLE {
#if BIGENDIAN
    unsigned int sign: 1;
    unsigned int exp: 8;
    unsigned int mant: 23;
#else
    unsigned int mant: 23;
    unsigned int exp: 8;
    unsigned int sign: 1;
#endif
};

struct FPDOUBLE {
#if BIGENDIAN
    unsigned int sign: 1;
    unsigned int exp: 11;
    unsigned int mantHi: 20;
    unsigned int mantLo;
#else
    unsigned int mantLo;
    unsigned int mantHi: 20;
    unsigned int exp: 11;
    unsigned int sign: 1;
#endif
};

static const char* const posCurrencyFormats[] = {
    "$#", "#$", "$ #", "# $"};

static const char* const negCurrencyFormats[] = {
        "($#)", "-$#", "$-#", "$#-",
            "(#$)", "-#$", "#-$", "#$-",
            "-# $", "-$ #", "# $-", "$ #-",
            "$ -#", "#- $", "($ #)", "(# $)"};

static const char* const posPercentFormats[] = {
            "# %", "#%", "%#", "% #"
        };

static const char* const negPercentFormats[] = {
    "-# %", "-#%", "-%#",
        "%-#", "%#-",
        "#-%", "#%-",
        "-% #", "# %-", "% #-",
        "% -#", "#- %"
        };

static const char* const negNumberFormats[] = {
            "(#)", "-#", "- #", "#-", "# -",
        };

static const char posNumberFormat[] = "#";



void DoubleToNumber(double value, int precision, NUMBER* number)
{
    WRAPPER_CONTRACT
    _ASSERTE(number != NULL);

    number->precision = precision;
    if (((FPDOUBLE*)&value)->exp == 0x7FF) {
        number->scale = (((FPDOUBLE*)&value)->mantLo || ((FPDOUBLE*)&value)->mantHi) ? SCALE_NAN: SCALE_INF;
        number->sign = ((FPDOUBLE*)&value)->sign;
        number->digits[0] = 0;
    }
    else {
        char* src = _ecvt(value, precision, &number->scale, &number->sign);
        wchar* dst = number->digits;
        if (*src != '0') {
            while (*src) *dst++ = *src++;
        }
        *dst = 0;
    }
}

/*===========================================================
    Portable NumberToDouble implementation
    --------------------------------------

    - does the conversion with the best possible precision.
    - does not use any float arithmetic so it is not sensitive
    to differences in precision of floating point calculations
    across platforms.

    The internal integer representation of the float number is
    UINT64 mantisa + INT exponent. The mantisa is kept normalized
    ie with the most significant one being 63-th bit of UINT64.
===========================================================*/

//
// get 32-bit integer from at most 9 digits
//
static unsigned DigitsToInt(__in_ecount(count) wchar* p, int count)
{
    LEAF_CONTRACT

    _ASSERTE(1 <= count && count <= 9);
    wchar* end = p + count;
    unsigned res = *p - '0';
    for ( p = p + 1; p < end; p++) {
        res = 10 * res + *p - '0';
    }
    return res;
}

//
// helper macro to multiply two 32-bit uints
//
#define Mul32x32To64(a, b) ((UINT64)((UINT32)(a)) * (UINT64)((UINT32)(b)))

//
// multiply two numbers in the internal integer representation
//
static UINT64 Mul64Lossy(UINT64 a, UINT64 b, INT* pexp)
{
    LEAF_CONTRACT

    // it's ok to losse some precision here - Mul64 will be called
    // at most twice during the conversion, so the error won't propagate
    // to any of the 53 significant bits of the result
    UINT64 val = Mul32x32To64(a >> 32, b >> 32) +
        (Mul32x32To64(a >> 32, b) >> 32) +
        (Mul32x32To64(a, b >> 32) >> 32);

    // normalize
    if ((val & I64(0x8000000000000000)) == 0) { val <<= 1; *pexp -= 1; }

    return val;
}

//
// precomputed tables with powers of 10. These allows us to do at most
// two Mul64 during the conversion. This is important not only
// for speed, but also for precision because of Mul64 computes with 1 bit error.
//

static const UINT64 rgval64Power10[] = {
// powers of 10
/*1*/ UI64(0xa000000000000000),
/*2*/ UI64(0xc800000000000000),
/*3*/ UI64(0xfa00000000000000),
/*4*/ UI64(0x9c40000000000000),
/*5*/ UI64(0xc350000000000000),
/*6*/ UI64(0xf424000000000000),
/*7*/ UI64(0x9896800000000000),
/*8*/ UI64(0xbebc200000000000),
/*9*/ UI64(0xee6b280000000000),
/*10*/ UI64(0x9502f90000000000),
/*11*/ UI64(0xba43b74000000000),
/*12*/ UI64(0xe8d4a51000000000),
/*13*/ UI64(0x9184e72a00000000),
/*14*/ UI64(0xb5e620f480000000),
/*15*/ UI64(0xe35fa931a0000000),

// powers of 0.1
/*1*/ UI64(0xcccccccccccccccd),
/*2*/ UI64(0xa3d70a3d70a3d70b),
/*3*/ UI64(0x83126e978d4fdf3c),
/*4*/ UI64(0xd1b71758e219652e),
/*5*/ UI64(0xa7c5ac471b478425),
/*6*/ UI64(0x8637bd05af6c69b7),
/*7*/ UI64(0xd6bf94d5e57a42be),
/*8*/ UI64(0xabcc77118461ceff),
/*9*/ UI64(0x89705f4136b4a599),
/*10*/ UI64(0xdbe6fecebdedd5c2),
/*11*/ UI64(0xafebff0bcb24ab02),
/*12*/ UI64(0x8cbccc096f5088cf),
/*13*/ UI64(0xe12e13424bb40e18),
/*14*/ UI64(0xb424dc35095cd813),
/*15*/ UI64(0x901d7cf73ab0acdc),
};

static const INT8 rgexp64Power10[] = {
// exponents for both powers of 10 and 0.1
/*1*/ 4,
/*2*/ 7,
/*3*/ 10,
/*4*/ 14,
/*5*/ 17,
/*6*/ 20,
/*7*/ 24,
/*8*/ 27,
/*9*/ 30,
/*10*/ 34,
/*11*/ 37,
/*12*/ 40,
/*13*/ 44,
/*14*/ 47,
/*15*/ 50,
};

static const UINT64 rgval64Power10By16[] = {
// powers of 10^16
/*1*/ UI64(0x8e1bc9bf04000000),
/*2*/ UI64(0x9dc5ada82b70b59e),
/*3*/ UI64(0xaf298d050e4395d6),
/*4*/ UI64(0xc2781f49ffcfa6d4),
/*5*/ UI64(0xd7e77a8f87daf7fa),
/*6*/ UI64(0xefb3ab16c59b14a0),
/*7*/ UI64(0x850fadc09923329c),
/*8*/ UI64(0x93ba47c980e98cde),
/*9*/ UI64(0xa402b9c5a8d3a6e6),
/*10*/ UI64(0xb616a12b7fe617a8),
/*11*/ UI64(0xca28a291859bbf90),
/*12*/ UI64(0xe070f78d39275566),
/*13*/ UI64(0xf92e0c3537826140),
/*14*/ UI64(0x8a5296ffe33cc92c),
/*15*/ UI64(0x9991a6f3d6bf1762),
/*16*/ UI64(0xaa7eebfb9df9de8a),
/*17*/ UI64(0xbd49d14aa79dbc7e),
/*18*/ UI64(0xd226fc195c6a2f88),
/*19*/ UI64(0xe950df20247c83f8),
/*20*/ UI64(0x81842f29f2cce373),
/*21*/ UI64(0x8fcac257558ee4e2),

// powers of 0.1^16
/*1*/ UI64(0xe69594bec44de160),
/*2*/ UI64(0xcfb11ead453994c3),
/*3*/ UI64(0xbb127c53b17ec165),
/*4*/ UI64(0xa87fea27a539e9b3),
/*5*/ UI64(0x97c560ba6b0919b5),
/*6*/ UI64(0x88b402f7fd7553ab),
/*7*/ UI64(0xf64335bcf065d3a0),
/*8*/ UI64(0xddd0467c64bce4c4),
/*9*/ UI64(0xc7caba6e7c5382ed),
/*10*/ UI64(0xb3f4e093db73a0b7),
/*11*/ UI64(0xa21727db38cb0053),
/*12*/ UI64(0x91ff83775423cc29),
/*13*/ UI64(0x8380dea93da4bc82),
/*14*/ UI64(0xece53cec4a314f00),
/*15*/ UI64(0xd5605fcdcf32e217),
/*16*/ UI64(0xc0314325637a1978),
/*17*/ UI64(0xad1c8eab5ee43ba2),
/*18*/ UI64(0x9becce62836ac5b0),
/*19*/ UI64(0x8c71dcd9ba0b495c),
/*20*/ UI64(0xfd00b89747823938),
/*21*/ UI64(0xe3e27a444d8d991a),
};

static const INT16 rgexp64Power10By16[] = {
// exponents for both powers of 10^16 and 0.1^16
/*1*/ 54,
/*2*/ 107,
/*3*/ 160,
/*4*/ 213,
/*5*/ 266,
/*6*/ 319,
/*7*/ 373,
/*8*/ 426,
/*9*/ 479,
/*10*/ 532,
/*11*/ 585,
/*12*/ 638,
/*13*/ 691,
/*14*/ 745,
/*15*/ 798,
/*16*/ 851,
/*17*/ 904,
/*18*/ 957,
/*19*/ 1010,
/*20*/ 1064,
/*21*/ 1117,
};

#ifdef _DEBUG
//
// slower high precision version of Mul64 for computation of the tables
//
static UINT64 Mul64Precise(UINT64 a, UINT64 b, INT* pexp)
{
    LEAF_CONTRACT

    UINT64 hilo =
        ((Mul32x32To64(a >> 32, b) >> 1) +
        (Mul32x32To64(a, b >> 32) >> 1) +
        (Mul32x32To64(a, b) >> 33)) >> 30;

    UINT64 val = Mul32x32To64(a >> 32, b >> 32) + (hilo >> 1) + (hilo & 1);

    // normalize
    if ((val & I64(0x8000000000000000)) == 0) { val <<= 1; *pexp -= 1; }

    return val;
}

//
// debug-only verification of the precomputed tables
//
static void CheckTable(UINT64 val, INT exp, LPCVOID table, int size, LPCSTR name, int tabletype)
{
    WRAPPER_CONTRACT

    UINT64 multval = val;
    INT mulexp = exp;
    bool fBad = false;
    for (int i = 0; i < size; i++) {
        switch (tabletype) {
        case 1:
            if (((UINT64*)table)[i] != val) {
                if (!fBad) {
                    fprintf(stderr, "%s:\n", name);
                    fBad = true;
                }
                fprintf(stderr, "/*%d*/ I64(0x%I64x),\n", i+1, val);
            }
            break;
        case 2:
            if (((INT8*)table)[i] != exp) {
                if (!fBad) {
                    fprintf(stderr, "%s:\n", name);
                    fBad = true;
                }
                fprintf(stderr, "/*%d*/ %d,\n", i+1, exp);
            }
            break;
        case 3:
            if (((INT16*)table)[i] != exp) {
                if (!fBad) {
                    fprintf(stderr, "%s:\n", name);
                    fBad = true;
                }
                fprintf(stderr, "/*%d*/ %d,\n", i+1, exp);
            }
            break;
        default:
            _ASSERTE(false);
            break;
        }

        exp += mulexp;
        val = Mul64Precise(val, multval, &exp);
    }
    _ASSERTE(!fBad || !"NumberToDouble table not correct. Correct version dumped to stderr.");
}

void CheckTables()
{
    WRAPPER_CONTRACT

    UINT64 val; INT exp;

    val = I64(0xa000000000000000); exp = 4; // 10
    CheckTable(val, exp, rgval64Power10, 15, "rgval64Power10", 1);
    CheckTable(val, exp, rgexp64Power10, 15, "rgexp64Power10", 2);

    val = I64(0x8e1bc9bf04000000); exp = 54; //10^16
    CheckTable(val, exp, rgval64Power10By16, 21, "rgval64Power10By16", 1);
    CheckTable(val, exp, rgexp64Power10By16, 21, "rgexp64Power10By16", 3);

    val = I64(0xCCCCCCCCCCCCCCCD); exp = -3; // 0.1
    CheckTable(val, exp, rgval64Power10+15, 15, "rgval64Power10 - inv", 1);

    val = I64(0xe69594bec44de160); exp = -53; // 0.1^16
    CheckTable(val, exp, rgval64Power10By16+21, 21, "rgval64Power10By16 - inv", 1);
}
#endif // _DEBUG

void NumberToDouble(NUMBER* number, double* value)
{
    WRAPPER_CONTRACT

    UINT64 val;
    INT exp;
    wchar* src = number->digits;
    int remaining;
    int total;
    int count;
    int scale;
    int absscale;
    int index;

#ifdef _DEBUG
    static bool fCheckedTables = false;
    if (!fCheckedTables) {
        CheckTables();
        fCheckedTables = true;
    }
#endif // _DEBUG

    total = (int)wcslen(src);
    remaining = total;

    // skip the leading zeros
    while (*src == '0') {
        remaining--;
        src++;
    }

    if (remaining == 0) {
        *value = 0;
        goto done;
    }

    count = min(remaining, 9);
    remaining -= count;
    val = DigitsToInt(src, count);

    if (remaining > 0) {
        count = min(remaining, 9);
        remaining -= count;

        // get the denormalized power of 10
        UINT32 mult = (UINT32)(rgval64Power10[count-1] >> (64 - rgexp64Power10[count-1]));
        val = Mul32x32To64(val, mult) + DigitsToInt(src+9, count);
    }

    scale = number->scale - (total - remaining);
    absscale = abs(scale);
    if (absscale >= 22 * 16) {
        // overflow / underflow
        *(UINT64*)value = (scale > 0) ? I64(0x7FF0000000000000) : 0;
        goto done;
    }

    exp = 64;

    // normalize the mantisa
    if ((val & I64(0xFFFFFFFF00000000)) == 0) { val <<= 32; exp -= 32; }
    if ((val & I64(0xFFFF000000000000)) == 0) { val <<= 16; exp -= 16; }
    if ((val & I64(0xFF00000000000000)) == 0) { val <<= 8; exp -= 8; }
    if ((val & I64(0xF000000000000000)) == 0) { val <<= 4; exp -= 4; }
    if ((val & I64(0xC000000000000000)) == 0) { val <<= 2; exp -= 2; }
    if ((val & I64(0x8000000000000000)) == 0) { val <<= 1; exp -= 1; }

    index = absscale & 15;
    if (index) {
        INT multexp = rgexp64Power10[index-1];
        // the exponents are shared between the inverted and regular table
        exp += (scale < 0) ? (-multexp + 1) : multexp;

        UINT64 multval = rgval64Power10[index + ((scale < 0) ? 15 : 0) - 1];
        val = Mul64Lossy(val, multval, &exp);
    }

    index = absscale >> 4;
    if (index) {
        INT multexp = rgexp64Power10By16[index-1];
        // the exponents are shared between the inverted and regular table
        exp += (scale < 0) ? (-multexp + 1) : multexp;

        UINT64 multval = rgval64Power10By16[index + ((scale < 0) ? 21 : 0) - 1];
        val = Mul64Lossy(val, multval, &exp);
    }

    // round & scale down
    if ((UINT32)val & (1 << 10))
    {
        // IEEE round to even
        UINT64 tmp = val + ((1 << 10) - 1) + (((UINT32)val >> 11) & 1);
        if (tmp < val) {
            // overflow
            tmp = (tmp >> 1) | I64(0x8000000000000000);
            exp += 1;
        }
        val = tmp;
    }
    val >>= 11;

    exp += 0x3FE;

    if (exp <= 0) {
        if (exp <= -52) {
            // underflow
            val = 0;
        }
        else {
            // denormalized
            val >>= (-exp+1);
        }
    }
    else
    if (exp >= 0x7FF) {
        // overflow
        val = I64(0x7FF0000000000000);
    }
    else {
        val = ((UINT64)exp << 52) + (val & I64(0x000FFFFFFFFFFFFF));
    }

    *(UINT64*)value = val;

done:
    if (number->sign) *(UINT64*)value |= I64(0x8000000000000000);
}

wchar_t* COMNumber::Int32ToDecChars(wchar_t* p, unsigned int value, int digits)
{
    LEAF_CONTRACT
    _ASSERTE(p != NULL);

    while (--digits >= 0 || value != 0) {
        *--p = value % 10 + '0';
        value /= 10;
    }
    return p;
}

unsigned int Int64DivMod1E9(unsigned __int64* value)
{
    LEAF_CONTRACT
    _ASSERTE(value != NULL);

    unsigned int rem = (unsigned int)(*value % 1000000000);
    *value /= 1000000000;
    return rem;
}





inline void AddStringRef(wchar** ppBuffer, STRINGREF strRef)
{
    WRAPPER_CONTRACT
    _ASSERTE(ppBuffer != NULL && strRef != NULL);

    wchar* buffer = strRef->GetBuffer();
    _ASSERTE(buffer != NULL);
    DWORD length = strRef->GetStringLength();
    for (wchar* str = buffer; str < buffer + length; (*ppBuffer)++, str++)
    {
        **ppBuffer = *str;
    }
}

LPCWSTR MatchChars(LPCWSTR p, LPCWSTR str)
{
    LEAF_CONTRACT
    _ASSERTE(p != NULL && str != NULL);

    if (!*str) return 0;
    for (; *str; p++, str++)
    {
        if (*p != *str) //We only hurt the failure case
        {
            if ((*str == 0xA0) && (*p == 0x20)) // This fix is for French or Kazakh cultures. Since a user cannot type 0xA0 as a
                // space character we use 0x20 space character instead to mean the same.
                continue;
            return 0;
        }
    }
    return p;
}

wchar* Int32ToHexChars(wchar* p, unsigned int value, int hexBase, int digits)
{
    LEAF_CONTRACT
    _ASSERTE(p != NULL);

    while (--digits >= 0 || value != 0) {
        int digit = value & 0xF;
        *--p = digit + (digit < 10? '0': hexBase);
        value >>= 4;
    }
    return p;
}

STRINGREF Int32ToDecStr(int value, int digits, STRINGREF sNegative)
{
    CONTRACTL {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    CQuickBytes buf;

    if (digits < 1) digits = 1;

    UINT maxDigitsLength = (digits > 15) ? digits : 15; // Since an int32 can have maximum of 10 chars as a String
    UINT bufferLength = (maxDigitsLength > 100) ? maxDigitsLength : 100;
    int negLength = 0;
    wchar* src = NULL;

    if (value < 0) {
        _ASSERTE(sNegative != NULL);
        src = sNegative->GetBuffer();
        _ASSERTE(src != NULL);
        negLength = sNegative->GetStringLength();
        if ((UINT) negLength > bufferLength - maxDigitsLength) {
            bufferLength = (UINT) negLength + maxDigitsLength;
        }
    }

    wchar *buffer = (wchar*)buf.AllocThrows(bufferLength * sizeof(WCHAR));
    wchar* p = COMNumber::Int32ToDecChars(buffer + bufferLength, value >= 0? value: -value, digits);
    _ASSERTE(p != NULL);
    if (value < 0) {
        for (int i =negLength - 1; i >= 0; i--)
        {
            *(--p) = *(src+i);
        }
    }

    _ASSERTE(buffer + bufferLength - p >=0 && buffer <= p);
    return COMString::NewString(p, (int)(buffer + bufferLength - p));
}

STRINGREF UInt32ToDecStr(unsigned int value, int digits)
{
    WRAPPER_CONTRACT

    wchar buffer[100];
    if (digits < 1) digits = 1;
    wchar* p = COMNumber::Int32ToDecChars(buffer + 100, value, digits);
    _ASSERTE(p != NULL && p >= buffer && p < (buffer + 100));
    return COMString::NewString(p, (int) (buffer + 100 - p));
}

STRINGREF Int32ToHexStr(unsigned int value, int hexBase, int digits)
{
    WRAPPER_CONTRACT

    wchar buffer[100];
    if (digits < 1) digits = 1;
    wchar* p = Int32ToHexChars(buffer + 100, value, hexBase, digits);
    return COMString::NewString(p, (int) (buffer + 100 - p));
}

void Int32ToNumber(int value, NUMBER* number)
{
    WRAPPER_CONTRACT
    _ASSERTE(number != NULL);

    wchar buffer[INT32_PRECISION+1];
    number->precision = INT32_PRECISION;
    if (value >= 0) {
        number->sign = 0;
    }
    else {
        number->sign = 1;
        value = -value;
    }
    wchar* p = COMNumber::Int32ToDecChars(buffer + INT32_PRECISION, value, 0);
    _ASSERTE(p != NULL);
    int i = (int) (buffer + INT32_PRECISION - p);
    number->scale = i;
    wchar* dst = number->digits;
    _ASSERTE(dst != NULL);
    while (--i >= 0) *dst++ = *p++;
    *dst = 0;
}

void UInt32ToNumber(unsigned int value, NUMBER* number)
{
    WRAPPER_CONTRACT
    _ASSERTE(number != NULL);

    wchar buffer[UINT32_PRECISION+1];
    number->precision = UINT32_PRECISION;
    number->sign = 0;
    wchar* p = COMNumber::Int32ToDecChars(buffer + UINT32_PRECISION, value, 0);
    _ASSERT(p != NULL);
    int i = (int) (buffer + UINT32_PRECISION - p);
    number->scale = i;
    wchar* dst = number->digits;
    _ASSERT(dst != NULL);
    while (--i >= 0) *dst++ = *p++;
    *dst = 0;
}




#define LO32(x) ((unsigned int)(x))
#define HI32(x) ((unsigned int)(((x) & UI64(0xFFFFFFFF00000000)) >> 32))

STRINGREF Int64ToDecStr(__int64 value, int digits, STRINGREF sNegative)
{
    CONTRACTL {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    CQuickBytes buf;

    if (digits < 1) digits = 1;
    int sign = HI32(value);

    // digits as specified in the format string can be at most 99.
    UINT maxDigitsLength = (digits > 20) ? digits : 20; 
    UINT bufferLength = (maxDigitsLength > 100) ? maxDigitsLength : 100;

    if (sign < 0) {
        value = -value;
        _ASSERTE(sNegative);
        int negLength = sNegative->GetStringLength();
        if ((UINT) negLength > bufferLength - maxDigitsLength) {
            bufferLength = negLength + maxDigitsLength;
        }
    }

    wchar *buffer = (wchar*)buf.AllocThrows(bufferLength * sizeof(WCHAR));
    wchar* p = buffer + bufferLength;
    while (HI32(value)) {
        p = COMNumber::Int32ToDecChars(p, Int64DivMod1E9((unsigned __int64*)&value), 9);
        _ASSERTE(p != NULL);
        digits -= 9;
    }
    p = COMNumber::Int32ToDecChars(p, LO32(value), digits);
    _ASSERTE(p != NULL);
    if (sign < 0) {
        wchar* src = sNegative->GetBuffer();
        _ASSERTE(src != NULL);
        for (int i =sNegative->GetStringLength() - 1; i >= 0; i--)
        {
            *(--p) = *(src+i);
        }
    }
    return COMString::NewString(p, (int) (buffer + bufferLength - p));
}

STRINGREF UInt64ToDecStr(unsigned __int64 value, int digits)
{
    WRAPPER_CONTRACT

    wchar buffer[100];
    if (digits < 1) digits = 1;
    wchar* p = buffer + 100;
    while (HI32(value)) {
        p = COMNumber::Int32ToDecChars(p, Int64DivMod1E9(&value), 9);
        _ASSERTE(p != NULL);
        digits -= 9;
    }
    p = COMNumber::Int32ToDecChars(p, LO32(value), digits);
    _ASSERTE(p != NULL && p >= buffer && p < (buffer + 100));
    return COMString::NewString(p, (int) (buffer + 100 - p));
}

STRINGREF Int64ToHexStr(unsigned __int64 value, int hexBase, int digits)
{
    WRAPPER_CONTRACT

    wchar buffer[100];
    wchar* p;
    if (HI32(value)) {
        Int32ToHexChars(buffer + 100, LO32(value), hexBase, 8);
        p = Int32ToHexChars(buffer + 100 - 8, HI32(value), hexBase, digits - 8);
    }
    else {
        if (digits < 1) digits = 1;
        p = Int32ToHexChars(buffer + 100, LO32(value), hexBase, digits);
    }
    _ASSERTE(p != NULL && p >= buffer && p < (buffer + 100));
    return COMString::NewString(p, (int) (buffer + 100 - p));
}

void Int64ToNumber(__int64 value, NUMBER* number)
{
    WRAPPER_CONTRACT

    wchar buffer[INT64_PRECISION+1];
    number->precision = INT64_PRECISION;
    if (value >= 0) {
        number->sign = 0;
    }
    else {
        number->sign = 1;
        value = -value;
    }
    wchar* p = buffer + INT64_PRECISION;
    while (HI32(value)) {
        p = COMNumber::Int32ToDecChars(p, Int64DivMod1E9((unsigned __int64*)&value), 9);
        _ASSERTE(p != NULL);
    }
    p = COMNumber::Int32ToDecChars(p, LO32(value), 0);
    _ASSERTE(p != NULL);
    int i = (int) (buffer + INT64_PRECISION - p);
    number->scale = i;
    wchar* dst = number->digits;
    _ASSERTE(dst != NULL);
    while (--i >= 0) *dst++ = *p++;
    *dst = 0;
}

void UInt64ToNumber(unsigned __int64 value, NUMBER* number)
{
    WRAPPER_CONTRACT

    wchar buffer[UINT64_PRECISION+1];
    number->precision = UINT64_PRECISION;
    number->sign = 0;
    wchar* p = buffer + UINT64_PRECISION;
    while (HI32(value)) {
        p = COMNumber::Int32ToDecChars(p, Int64DivMod1E9(&value), 9);
        _ASSERTE(p != NULL);
    }
    p = COMNumber::Int32ToDecChars(p, LO32(value), 0);
    _ASSERTE(p != NULL);
    int i = (int) (buffer + UINT64_PRECISION - p);
    number->scale = i;
    wchar* dst = number->digits;
    _ASSERTE(dst != NULL);
    while (--i >= 0) *dst++ = *p++;
    *dst = 0;
}


void RoundNumber(NUMBER* number, int pos)
{
    LEAF_CONTRACT
    _ASSERTE(number != NULL);

    int i = 0;
    while (i < pos && number->digits[i] != 0) i++;
    if (i == pos && number->digits[i] >= '5') {
        while (i > 0 && number->digits[i - 1] == '9') i--;
        if (i > 0) {
            number->digits[i - 1]++;
        }
        else {
            number->scale++;
            number->digits[0] = '1';
            i = 1;
        }
    }
    else {
        while (i > 0 && number->digits[i - 1] == '0') i--;
    }
    if (i == 0) {
        number->scale = 0;
        number->sign = 0;
    }
    number->digits[i] = 0;
}

wchar ParseFormatSpecifier(STRINGREF str, int* digits)
{
    WRAPPER_CONTRACT
    _ASSERTE(digits != NULL);

    if (str != 0) {
        wchar* p = str->GetBuffer();
        _ASSERTE(p != NULL);
        wchar ch = *p;
        if (ch != 0) {
            if (ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z') {
                p++;
                int n = -1;
                if (*p >= '0' && *p <= '9') {
                    n = *p++ - '0';
                    while (*p >= '0' && *p <= '9') {
                        n = n * 10 + *p++ - '0';
                        if (n >= 10) break;
                    }
                }
                if (*p == 0) {
                    *digits = n;
                    return ch;
                }
            }
            return 0;
        }
    }
    *digits = -1;
    return 'G';
}

wchar* FormatExponent(wchar* buffer, int value, wchar expChar,
    STRINGREF posSignStr, STRINGREF negSignStr, int minDigits)
{
    WRAPPER_CONTRACT
    _ASSERTE(buffer != NULL);

    wchar digits[11];
    *buffer++ = expChar;
    if (value < 0) {
        _ASSERTE(negSignStr != NULL);
        AddStringRef(&buffer, negSignStr);
        value = -value;
    }
    else {
        if (posSignStr!= NULL) {
            AddStringRef(&buffer, posSignStr);
        }
    }
    wchar* p = COMNumber::Int32ToDecChars(digits + 10, value, minDigits);
    _ASSERTE(p != NULL);
    int i = (int) (digits + 10 - p);
    while (--i >= 0) *buffer++ = *p++;
    return buffer;
}

wchar* FormatGeneral(wchar* buffer, NUMBER* number, int digits, wchar expChar,
    NUMFMTREF numfmt, BOOL bSuppressScientific = FALSE)
{
        WRAPPER_CONTRACT
        _ASSERTE(number != NULL);
        _ASSERTE(buffer != NULL);
        _ASSERTE(numfmt != NULL);

    int digPos = number->scale;
    int scientific = 0;
    if (!bSuppressScientific) { // Don't switch to scientific notation
        if (digPos > digits || digPos < -3) {
            digPos = 1;
            scientific = 1;
        }
    }
    wchar* dig = number->digits;
    _ASSERT(dig != NULL);
    if (digPos > 0) {
        do {
            *buffer++ = *dig != 0? *dig++: '0';
        } while (--digPos > 0);
    }
    else {
        *buffer++ = '0';
    }
    if (*dig != 0 || digPos < 0) {
        AddStringRef(&buffer, numfmt->sNumberDecimal);
        while (digPos < 0) {
            *buffer++ = '0';
            digPos++;
        }
        while (*dig != 0) {
            *buffer++ = *dig++;
        }
    }
    if (scientific) buffer = FormatExponent(buffer, number->scale - 1, expChar, numfmt->sPositive, numfmt->sNegative, 2);
    return buffer;
}

wchar* FormatScientific(wchar* buffer, NUMBER* number, int digits, wchar expChar,
    NUMFMTREF numfmt)
{
        WRAPPER_CONTRACT
        _ASSERTE(number != NULL);
        _ASSERTE(buffer != NULL);
        _ASSERTE(numfmt != NULL);

    wchar* dig = number->digits;
    _ASSERTE(dig != NULL);
    *buffer++ = *dig != 0? *dig++: '0';
    if (digits != 1) // For E0 we would like to suppress the decimal point
        AddStringRef(&buffer, numfmt->sNumberDecimal);
    while (--digits > 0) *buffer++ = *dig != 0? *dig++: '0';
    int e = number->digits[0] == 0? 0: number->scale - 1;
    buffer = FormatExponent(buffer, e, expChar, numfmt->sPositive, numfmt->sNegative, 3);
    _ASSERTE(buffer != NULL);
    return buffer;
}

wchar* FormatFixed(wchar* buffer, NUMBER* number, int digits,
    I4ARRAYREF groupDigitsRef, STRINGREF sDecimal, STRINGREF sGroup)
{
    CONTRACTL {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckPointer(number));
    } CONTRACTL_END;

    int digPos = number->scale;
    wchar* dig = number->digits;
    const I4* groupDigits = NULL;
    if (groupDigitsRef != NULL) {
        groupDigits = groupDigitsRef->GetDirectConstPointerToNonObjectElements();
    }

    if (digPos > 0) {
        if (groupDigits != NULL) {

            int groupSizeIndex = 0;     // index into the groupDigits array.
            int groupSizeCount = groupDigits[groupSizeIndex];   // the current total of group size.
            int groupSizeLen   = groupDigitsRef->GetNumComponents();    // the length of groupDigits array.
            int bufferSize     = digPos;                        // the length of the result buffer string.
            int groupSeparatorLen = sGroup->GetStringLength();  // the length of the group separator string.
            int groupSize = 0;                                      // the current group size.

            //
            // Find out the size of the string buffer for the result.
            //
            if (groupSizeLen != 0) // You can pass in 0 length arrays
            {
                while (digPos > groupSizeCount) {
                    groupSize = groupDigits[groupSizeIndex];
                    if (groupSize == 0) {
                        break;
                    }

                    bufferSize += groupSeparatorLen;
                    if (groupSizeIndex < groupSizeLen - 1) {
                        groupSizeIndex++;
                    }
                    groupSizeCount += groupDigits[groupSizeIndex];
                    if (groupSizeCount < 0 || bufferSize < 0) {
                        COMPlusThrow(kArgumentOutOfRangeException); // if we overflow
                    }
                }
                if (groupSizeCount == 0) // If you passed in an array with one entry as 0, groupSizeCount == 0
                    groupSize = 0;
                else
                    groupSize = groupDigits[0];
            }

            groupSizeIndex = 0;
            int digitCount = 0;
            int digStart;
            int digLength = (int)wcslen(dig);
            digStart = (digPos<digLength)?digPos:digLength;
            wchar* p = buffer + bufferSize - 1;
            for (int i = digPos - 1; i >=0; i--) {
                *(p--) = (i<digStart)?dig[i]:'0';

                if (groupSize > 0) {
                    digitCount++;
                    if (digitCount == groupSize && i != 0) {
                        for (int j = groupSeparatorLen - 1; j >=0; j--) {
                            *(p--) = sGroup->GetBuffer()[j];
                        }

                        if (groupSizeIndex < groupSizeLen - 1) {
                            groupSizeIndex++;
                            groupSize = groupDigits[groupSizeIndex];
                        }
                        digitCount = 0;
                    }
                }
            }
            if (p < buffer - 1) {
                // This indicates a buffer underflow since we write in backwards. 
                DoJITFailFast();
            }
            buffer += bufferSize;
            dig += digStart;
        } else {
            do {
                *buffer++ = *dig != 0? *dig++: '0';
            } while (--digPos > 0);
        }
    }
    else {
        *buffer++ = '0';
    }
    if (digits > 0) {
        AddStringRef(&buffer, sDecimal);
        while (digPos < 0 && digits > 0) {
            *buffer++ = '0';
            digPos++;
            digits--;
        }
        while (digits > 0) {
            *buffer++ = *dig != 0? *dig++: '0';
            digits--;
        }
    }
    return buffer;
}

wchar* FormatNumber(wchar* buffer, NUMBER* number, int digits, NUMFMTREF numfmt)
{
    CONTRACTL {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckPointer(number));
    } CONTRACTL_END;

    char ch;
    const char* fmt;
    fmt = number->sign?
        negNumberFormats[numfmt->cNegativeNumberFormat]:
    posNumberFormat;

    while ((ch = *fmt++) != 0) {
        switch (ch) {
        case '#':
            buffer = FormatFixed(buffer, number, digits,
                numfmt->cNumberGroup,
                numfmt->sNumberDecimal, numfmt->sNumberGroup);
            break;
        case '-':
            AddStringRef(&buffer, numfmt->sNegative);
            break;
        default:
            *buffer++ = ch;
        }
    }
    return buffer;

}

wchar* FormatCurrency(wchar* buffer, NUMBER* number, int digits, NUMFMTREF numfmt)
{
    CONTRACTL {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckPointer(number));
    } CONTRACTL_END;

    char ch;
    const char* fmt;
    fmt = number->sign?
        negCurrencyFormats[numfmt->cNegCurrencyFormat]:
    posCurrencyFormats[numfmt->cPosCurrencyFormat];

    while ((ch = *fmt++) != 0) {
        switch (ch) {
        case '#':
            buffer = FormatFixed(buffer, number, digits,
                numfmt->cCurrencyGroup,
                numfmt->sCurrencyDecimal, numfmt->sCurrencyGroup);
            break;
        case '-':
            AddStringRef(&buffer, numfmt->sNegative);
            break;
        case '$':
            AddStringRef(&buffer, numfmt->sCurrency);
            break;
        default:
            *buffer++ = ch;
        }
    }
    return buffer;
}

wchar* FormatPercent(wchar* buffer, NUMBER* number, int digits, NUMFMTREF numfmt)
{
    CONTRACTL {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckPointer(number));
    } CONTRACTL_END;

    char ch;
    const char* fmt;
    fmt = number->sign?
        negPercentFormats[numfmt->cNegativePercentFormat]:
    posPercentFormats[numfmt->cPositivePercentFormat];

    while ((ch = *fmt++) != 0) {
        switch (ch) {
        case '#':
            buffer = FormatFixed(buffer, number, digits,
                numfmt->cPercentGroup,
                numfmt->sPercentDecimal, numfmt->sPercentGroup);
            break;
        case '-':
            AddStringRef(&buffer, numfmt->sNegative);
            break;
        case '%':
            AddStringRef(&buffer, numfmt->sPercent);
            break;
        default:
            *buffer++ = ch;
        }
    }
    return buffer;
}

STRINGREF NumberToString(NUMBER* number, wchar format, int digits, NUMFMTREF numfmt, BOOL bDecimal = FALSE )
{
        CONTRACTL {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(number));
    } CONTRACTL_END;


    // Do the worst case calculation
    /* US English - for Double.MinValue.ToString("C99"); we require 514 characters
    ----------
    2 paranthesis
    1 currency character
    308 characters
    103 group seperators
    1 decimal separator
    99 0's

        digPos + 99 + 6(slack) => digPos + 105
        C
        sNegative
        sCurrencyGroup
        sCurrencyDecimal
        sCurrency
        F
        sNegative
        sNumberDecimal
        N
        sNegative
        sNumberDecimal
        sNumberGroup
        E
        sNegative
        sPositive
        sNegative (for exponent)
        sPositive
        sNumberDecimal
        G
        sNegative
        sPositive
        sNegative (for exponent)
        sPositive
        sNumberDecimal
        P (+2 for some spaces)
        sNegative
        sPercentGroup
        sPercentDecimal
        sPercent
    */

    _ASSERTE(numfmt != NULL);
    INT64 newBufferLen = MIN_BUFFER_SIZE;

    CQuickBytesSpecifySize<LARGE_BUFFER_SIZE * sizeof(WCHAR)> buf;

    wchar *buffer = NULL;
    wchar* dst = NULL;
    wchar ftype = format & 0xFFDF;
    int digCount = 0;

    switch (ftype) {
    case 'C':
        if (digits < 0) digits = numfmt->cCurrencyDecimals;
        if (number->scale < 0)
            digCount = 0;
        else
            digCount = number->scale + digits;

        newBufferLen += digCount;
        newBufferLen += numfmt->sNegative->GetStringLength(); // For number and exponent
        newBufferLen += ((INT64)numfmt->sCurrencyGroup->GetStringLength() * digCount); // For all the grouping sizes
        newBufferLen += numfmt->sCurrencyDecimal->GetStringLength();
        newBufferLen += numfmt->sCurrency->GetStringLength();

        _ASSERTE(newBufferLen >= MIN_BUFFER_SIZE);
        if (newBufferLen > INT32_MAX) {
            COMPlusThrowOM();
        }
        newBufferLen = newBufferLen * sizeof(WCHAR);
        dst = buffer = (WCHAR*)buf.AllocThrows(newBufferLen);

        RoundNumber(number, number->scale + digits); // Don't change this line to use digPos since digCount could have its sign changed.
        dst = FormatCurrency(dst, number, digits, numfmt);
        break;
    case 'F':
        if (digits < 0) digits = numfmt->cNumberDecimals;

        if (number->scale < 0)
            digCount = 0;
        else
            digCount = number->scale + digits;


        newBufferLen += digCount;
        newBufferLen += numfmt->sNegative->GetStringLength(); // For number and exponent
        newBufferLen += numfmt->sNumberDecimal->GetStringLength();

        _ASSERTE(newBufferLen >= MIN_BUFFER_SIZE);
        if (newBufferLen > INT32_MAX) {
            COMPlusThrowOM();
        }
        newBufferLen = newBufferLen * sizeof(WCHAR);
        dst = buffer = (WCHAR*)buf.AllocThrows(newBufferLen);

        RoundNumber(number, number->scale + digits);
        if (number->sign) {
            AddStringRef(&dst, numfmt->sNegative);
        }
        dst = FormatFixed(dst, number, digits,
            NULL,
            numfmt->sNumberDecimal, NULL);
        break;
    case 'N':
        if (digits < 0) digits = numfmt->cNumberDecimals; // Since we are using digits in our calculation

        if (number->scale < 0)
            digCount = 0;
        else
            digCount = number->scale + digits;


        newBufferLen += digCount;
        newBufferLen += numfmt->sNegative->GetStringLength(); // For number and exponent
        newBufferLen += ((INT64)numfmt->sNumberGroup->GetStringLength()) * digCount; // For all the grouping sizes
        newBufferLen += numfmt->sNumberDecimal->GetStringLength();

        _ASSERTE(newBufferLen >= MIN_BUFFER_SIZE);
        if (newBufferLen > INT32_MAX) {
            COMPlusThrowOM();
        }
        newBufferLen = newBufferLen * sizeof(WCHAR);
        dst = buffer = (WCHAR*)buf.AllocThrows(newBufferLen);

        RoundNumber(number, number->scale + digits);
        dst = FormatNumber(dst, number, digits, numfmt);
        break;
    case 'E':
        if (digits < 0) digits = 6;
        digits++;

        newBufferLen += digits;
        newBufferLen += (((INT64)numfmt->sNegative->GetStringLength() + numfmt->sPositive->GetStringLength()) *2); // For number and exponent
        newBufferLen += numfmt->sNumberDecimal->GetStringLength();

        _ASSERTE(newBufferLen >= MIN_BUFFER_SIZE);
        if (newBufferLen > INT32_MAX) {
            COMPlusThrowOM();
        }
        newBufferLen = newBufferLen * sizeof(WCHAR);
        dst = buffer = (WCHAR*)buf.AllocThrows(newBufferLen);

        RoundNumber(number, digits);
        if (number->sign) {
            AddStringRef(&dst, numfmt->sNegative);
        }
        dst = FormatScientific(dst, number, digits, format, numfmt);
        break;
    case 'G':
        {
            bool enableRounding = true;
            if (digits < 1) {
                if (bDecimal && (digits == -1)) { // Default to 29 digits precision only for G formatting without a precision specifier
                    digits = DECIMAL_PRECISION;
                    enableRounding = false;  // Turn off rounding for ECMA compliance to output trailing 0's after decimal as significant
                }
                else {
                    digits = number->precision;
                }
            }

        newBufferLen += digits;
        newBufferLen += ((numfmt->sNegative->GetStringLength() + numfmt->sPositive->GetStringLength()) *2); // For number and exponent
        newBufferLen += numfmt->sNumberDecimal->GetStringLength();

        _ASSERTE(newBufferLen >= MIN_BUFFER_SIZE);
        if (newBufferLen > INT32_MAX) {
            COMPlusThrowOM();
        }
        newBufferLen = newBufferLen * sizeof(WCHAR);
        dst = buffer = (WCHAR*)buf.AllocThrows(newBufferLen);

            if (enableRounding) // Don't round for G formatting without precision
                RoundNumber(number, digits); // This also fixes up the minus zero case
            else {
                if (bDecimal && (number->digits[0] == 0)) { // Minus zero should be formatted as 0
                    number->sign = 0;
                }
            }
        if (number->sign) {
            AddStringRef(&dst, numfmt->sNegative);
        }
            dst = FormatGeneral(dst, number, digits, format - ('G' - 'E'), numfmt, !enableRounding);
        }
        break;
    case 'P':
        if (digits < 0) digits = numfmt->cPercentDecimals;
        number->scale += 2;

        if (number->scale < 0)
            digCount = 0;
        else
            digCount = number->scale + digits;


        newBufferLen += digCount;
        newBufferLen += numfmt->sNegative->GetStringLength(); // For number and exponent
        newBufferLen += ((INT64)numfmt->sPercentGroup->GetStringLength()) * digCount; // For all the grouping sizes
        newBufferLen += numfmt->sPercentDecimal->GetStringLength();
        newBufferLen += numfmt->sPercent->GetStringLength();

        _ASSERTE(newBufferLen >= MIN_BUFFER_SIZE);
        if (newBufferLen > INT32_MAX) {
            COMPlusThrowOM();
        }
        newBufferLen = newBufferLen * sizeof(WCHAR);
        dst = buffer = (WCHAR*)buf.AllocThrows(newBufferLen);

        RoundNumber(number, number->scale + digits);
        dst = FormatPercent(dst, number, digits, numfmt);
        break;
    default:
        COMPlusThrow(kFormatException, L"Format_BadFormatSpecifier");
    }
    if (!((dst - buffer >= 0) && (dst - buffer) <= newBufferLen)) {
        DoJITFailFast();
    }
    return COMString::NewString(buffer, (int) (dst - buffer));
}

LPCWSTR FindSection(LPCWSTR format, int section)
{
    LEAF_CONTRACT
    _ASSERTE(format != NULL);

    LPCWSTR src;
    wchar ch;
    if (section == 0) return format;
    src = format;
    for (;;) {
        switch (ch = *src++) {
        case '\'':
        case '"':
            while (*src != 0 && *src++ != ch);
            break;
        case '\\':
            if (*src != 0) src++;
            break;
        case ';':
            if (--section != 0) break;
            if (*src != 0 && *src != ';') return src;
        case 0:
            return format;
        }
    }
}

STRINGREF NumberToStringFormat(NUMBER* number, STRINGREF str, NUMFMTREF numfmt)
{
    CONTRACTL {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    int digitCount;
    int decimalPos;
    int firstDigit;
    int lastDigit;
    int digPos;
    int scientific;
    int percent;
    int permille;
    int thousandPos;
    int thousandCount = 0;
    int thousandSeps;
    int scaleAdjust;
    int adjust;
    wchar* format=NULL;
    LPCWSTR section=NULL;
    LPCWSTR src=NULL;
    wchar* dst=NULL;
    wchar* dig=NULL;
    wchar ch;
    wchar* buffer=NULL;
    CQuickBytes buf;

    _ASSERTE(str != NULL);
    _ASSERTE(numfmt != NULL);

    format = str->GetBuffer();

    section = FindSection(format, number->digits[0] == 0? 2: number->sign? 1: 0);

ParseSection:
    digitCount = 0;
    decimalPos = -1;
    firstDigit = 0x7FFFFFFF;
    lastDigit = 0;
    scientific = 0;
    percent = 0;
    permille = 0;
    thousandPos = -1;
    thousandSeps = 0;
    scaleAdjust = 0;
    src = section;
    _ASSERTE(src != NULL);
    while ((ch = *src++) != 0 && ch != ';') {
        switch (ch) {
        case '#':
            digitCount++;
            break;
        case '0':
            if (firstDigit == 0x7FFFFFFF) firstDigit = digitCount;
            digitCount++;
            lastDigit = digitCount;
            break;
        case '.':
            if (decimalPos < 0) {
                decimalPos = digitCount;
            }
            break;
        case ',':
            if (digitCount > 0 && decimalPos < 0) {
                if (thousandPos >= 0) {
                    if (thousandPos == digitCount) {
                        thousandCount++;
                        break;
                    }
                    thousandSeps = 1;
                }
                thousandPos = digitCount;
                thousandCount = 1;
            }
            break;
        case '%':
            percent++;
            scaleAdjust += 2;
            break;
        case 0x2030:
            permille++;
            scaleAdjust += 3;
            break;
        case '\'':
        case '"':
            while (*src != 0 && *src++ != ch);
            break;
        case '\\':
            if (*src != 0) src++;
            break;
        case 'E':
        case 'e':
            if (*src=='0' || ((*src == '+' || *src == '-') && src[1] == '0')) {
                while (*++src == '0');
                scientific = 1;
            }
            break;
        }
    }

    if (decimalPos < 0) decimalPos = digitCount;
    if (thousandPos >= 0) {
        if (thousandPos == decimalPos) {
            scaleAdjust -= thousandCount * 3;
        }
        else {
            thousandSeps = 1;
        }
    }
    if (number->digits[0] != 0) {
        number->scale += scaleAdjust;
        int pos = scientific? digitCount: number->scale + digitCount - decimalPos;
        RoundNumber(number, pos);
        if (number->digits[0] == 0) {
            src = FindSection(format, 2);
            if (src != section) {
                section = src;
                goto ParseSection;
            }
        }
    } else {
        number->sign = 0; // We need to format -0 without the sign set.
        number->scale = 0; // Decimals with scale ('0.00') should be rounded.
    }

    firstDigit = firstDigit < decimalPos? decimalPos - firstDigit: 0;
    lastDigit = lastDigit > decimalPos? decimalPos - lastDigit: 0;
    if (scientific) {
        digPos = decimalPos;
        adjust = 0;
    }
    else {
        digPos = number->scale > decimalPos? number->scale: decimalPos;
        adjust = number->scale - decimalPos;
    }
    src = section;
    dig = number->digits;

    // Find maximum number of characters that the destination string can grow by
    // in the following while loop.  Use this to avoid buffer overflows.
    // Longest strings are potentially +/- signs with 10 digit exponents,
    // or decimal numbers, or the while loops copying from a quote or a \ onwards.
    // Check for positive and negative
    UINT64 maxStrIncLen = 0; // We need this to be UINT64 since the percent computation could go beyond a UINT.
    if (number->sign) {
        maxStrIncLen = numfmt->sNegative->GetStringLength();
    }
    else {
        maxStrIncLen = numfmt->sPositive->GetStringLength();
    }

    // Add for any big decimal seperator
    maxStrIncLen += numfmt->sNumberDecimal->GetStringLength();

    // Add for scientific
    if (scientific) {
        int inc1 = numfmt->sPositive->GetStringLength();
        int inc2 = numfmt->sNegative->GetStringLength();
        maxStrIncLen +=(inc1>inc2)?inc1:inc2;
    }

    // Add for percent separator
    if (percent) {
        maxStrIncLen += ((INT64)numfmt->sPercent->GetStringLength()) * percent;
    }

    // Add for permilli separator
    if (permille) {
        maxStrIncLen += ((INT64)numfmt->sPerMille->GetStringLength()) * permille;
    }

    //adjust can be negative, so we make this an int instead of an unsigned int.
    // adjust represents the number of characters over the formatting eg. format string is "0000" and you are trying to
    // format 100000 (6 digits). Means adjust will be 2. On the other hand if you are trying to format 10 adjust will be
    // -2 and we'll need to fixup these digits with 0 padding if we have 0 formatting as in this example.
    INT64 adjustLen=(adjust>0)?adjust:0; // We need to add space for these extra characters anyway.
    CQuickBytes thousands;
    INT32 bufferLen2 = 125;
    INT32 *thousandsSepPos = NULL;
    INT32 thousandsSepCtr = -1;

    if (thousandSeps) { // Fixup possible buffer overrun problems
        // We need to precompute this outside the number formatting loop
        if(numfmt->cNumberGroup->GetNumComponents() == 0) {
            thousandSeps = 0; // Nothing to add
        }
        else {
            thousandsSepPos = (INT32 *)thousands.AllocThrows(bufferLen2 * sizeof(INT32));
        //                        - We need this array to figure out where to insert the thousands seperator. We would have to traverse the string
            // backwords. PIC formatting always traverses forwards. These indices are precomputed to tell us where to insert
            // the thousands seperator so we can get away with traversing forwards. Note we only have to compute upto digPos.
            // The max is not bound since you can have formatting strings of the form "000,000..", and this
            // should handle that case too.

            const I4* groupDigits = numfmt->cNumberGroup->GetDirectConstPointerToNonObjectElements();
            _ASSERTE(groupDigits != NULL);

            int groupSizeIndex = 0;     // index into the groupDigits array.
            INT64 groupTotalSizeCount = 0;
            int groupSizeLen   = numfmt->cNumberGroup->GetNumComponents();    // the length of groupDigits array.
            if (groupSizeLen != 0)
                groupTotalSizeCount = groupDigits[groupSizeIndex];   // the current running total of group size.
            int groupSize = groupTotalSizeCount;

            int totalDigits = digPos + ((adjust < 0)?adjust:0); // actual number of digits in o/p
            int numDigits = (firstDigit > totalDigits) ? firstDigit : totalDigits;
            while (numDigits > groupTotalSizeCount) {
                if (groupSize == 0)
                    break;
                thousandsSepPos[++thousandsSepCtr] = groupTotalSizeCount;
                if (groupSizeIndex < groupSizeLen - 1) {
                    groupSizeIndex++;
                    groupSize = groupDigits[groupSizeIndex];
                }
                groupTotalSizeCount += groupSize;
                if (bufferLen2 - thousandsSepCtr < 10) { // Slack of 10
                    bufferLen2 *= 2;
                    thousands.ReSizeThrows(bufferLen2*sizeof(INT32)); // memcopied by CQuickBytes automatically
                    thousandsSepPos = (INT32 *)thousands.Ptr();
                }
            }

            // We already have computed the number of separators above. Simply add space for them.
            adjustLen += ( (thousandsSepCtr + 1) * ((INT64)numfmt->sNumberGroup->GetStringLength()));
        }
    }

    maxStrIncLen += adjustLen;

    // Allocate temp buffer - gotta deal with Schertz' 500 MB strings.
    // Some computations like when you specify Int32.MaxValue-2 %'s and each percent is setup to be Int32.MaxValue in length
    // will generate a result that will be larget than an unsigned int can hold. This is to protect against overflow.
    UINT64 tempLen = str->GetStringLength() + maxStrIncLen + 10;  // Include a healthy amount of temp space.
    if (tempLen > 0x7FFFFFFF)
        COMPlusThrowOM(); // if we overflow

    unsigned int bufferLen = (UINT)tempLen;
    if (bufferLen < 250) // Stay under 512 bytes
        bufferLen = 250; // This is to prevent unneccessary calls to resize
    buffer = (wchar *) buf.AllocThrows(bufferLen* sizeof(WCHAR));
    dst = buffer;


    if (number->sign && section == format) {
        AddStringRef(&dst, numfmt->sNegative);
    }

    BOOL decimalWritten = FALSE;

    while ((ch = *src++) != 0 && ch != ';') {
        // Make sure temp buffer is big enough, else resize it.
        if (bufferLen - (unsigned int)(dst-buffer) < 10) {
            int offset = dst - buffer;
            bufferLen *= 2;
            buf.ReSizeThrows(bufferLen*sizeof(WCHAR));
            buffer = (wchar*)buf.Ptr(); // memcopied by QuickBytes automatically
            dst = buffer + offset;
        }

        if (adjust > 0) {
            switch (ch) {
            case '#':
            case '0':
            case '.':
                while (adjust > 0) { // digPos will be one greater than thousandsSepPos[thousandsSepCtr] since we are at
                    // the character after which the groupSeparator needs to be appended.
                    *dst++ = *dig != 0? *dig++: '0';
                    if (thousandSeps && digPos > 1 && thousandsSepCtr>=0) {
                        if (digPos == thousandsSepPos[thousandsSepCtr] + 1)  {
                            AddStringRef(&dst, numfmt->sNumberGroup);
                            thousandsSepCtr--;
                        }
                    }
                    digPos--;
                    adjust--;
                }
            }
        }

        switch (ch) {
        case '#':
        case '0':
            {
                if (adjust < 0) {
                    adjust++;
                    ch = digPos <= firstDigit? '0': 0;
                }
                else {
                    ch = *dig != 0? *dig++: digPos > lastDigit? '0': 0;
                }
                if (ch != 0) {
                    *dst++ = ch;
                    if (thousandSeps && digPos > 1 && thousandsSepCtr>=0) {
                        if (digPos == thousandsSepPos[thousandsSepCtr] + 1) {
                            AddStringRef(&dst, numfmt->sNumberGroup);
                            thousandsSepCtr--;
                        }
                    }
                }

                digPos--;
                break;
            }
        case '.':
            {
                if (digPos != 0 || decimalWritten) {
                    // For compatability, don't echo repeated decimals
                    break;
                }
                // If the format has trailing zeros or the format has a decimal and digits remain
                if (lastDigit < 0
                    || (decimalPos < digitCount && *dig != 0)) {
                    AddStringRef(&dst, numfmt->sNumberDecimal);
                    decimalWritten = TRUE;
                }
                break;
            }
        case 0x2030:
            AddStringRef(&dst, numfmt->sPerMille);
            break;
        case '%':
            AddStringRef(&dst, numfmt->sPercent);
            break;
        case ',':
            break;
        case '\'':
        case '"':
            // Buffer overflow possibility
            while (*src != 0 && *src != ch) {
                *dst++ = *src++;
                if ((unsigned int)(dst-buffer) == bufferLen-1) {
                    if (bufferLen - (unsigned int)(dst-buffer) < maxStrIncLen) {
                        int offset = dst - buffer;
                        bufferLen *= 2;
                        buf.ReSizeThrows(bufferLen*sizeof(WCHAR)); // memcopied by CQuickBytes automatically
                        buffer = (wchar *)buf.Ptr();
                        dst = buffer + offset;
                    }
                }
            }
            if (*src != 0) src++;
            break;
        case '\\':
            if (*src != 0) *dst++ = *src++;
            break;
        case 'E':
        case 'e':
            {
                STRINGREF sign = NULL;
                int i = 0;
                if (scientific) {
                    if (*src=='0') {
                        //Handles E0, which should format the same as E-0
                        i++;
                    } else if (*src == '+' && src[1] == '0') {
                        //Handles E+0
                        sign = numfmt->sPositive;
                    } else if (*src == '-' && src[1] == '0') {
                        //Handles E-0
                        //Do nothing, this is just a place holder s.t. we don't break out of the loop.
                    } else {
                        *dst++ = ch;
                        break;
                    }
                    while (*++src == '0') i++;
                    if (i > 10) i = 10;
                    int exp = number->digits[0] == 0? 0: number->scale - decimalPos;
                    dst = FormatExponent(dst, exp, ch, sign, numfmt->sNegative, i);
                    scientific = 0;
                }
                else
                {
                    *dst++ = ch; // Copy E or e to output
                    if (*src== '+' || *src == '-') {
                        *dst++ = *src++;
                    }
                    while (*src == '0') {
                        *dst++ = *src++;
                    }
                }
                break;
            }
        default:
            *dst++ = ch;
        }
    }
    if (!((dst - buffer >= 0) && (dst - buffer <= (int)bufferLen))) {
        DoJITFailFast();
    }
    STRINGREF newStr = COMString::NewString(buffer, (int)(dst - buffer));
    return newStr;
}

FCIMPL3_VII(Object*, COMNumber::FormatDecimal, DECIMAL value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;		
    }
    CONTRACTL_END;

    NUMBER number;

    wchar fmt;
    int digits;

    STRINGREF  refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    struct _gc
    {
        STRINGREF   format;
        NUMFMTREF   numfmt;
    } gc;

    gc.format = (STRINGREF) formatUNSAFE;
    gc.numfmt = (NUMFMTREF) numfmtUNSAFE;

    if (gc.numfmt == 0)
        COMPlusThrowArgumentNull(L"NumberFormatInfo");

    COMDecimal::DecimalToNumber(&value, &number);

    fmt = ParseFormatSpecifier(gc.format, &digits);
    if (fmt != 0) {
        refRetVal = NumberToString(&number, fmt, digits, gc.numfmt, TRUE);
    } else {
        refRetVal = NumberToStringFormat(&number, gc.format, gc.numfmt);
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL3_VII(Object*, COMNumber::FormatDouble, double value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    NUMBER number;
    int digits;
    double dTest;

    struct _gc
    {
        STRINGREF   format;
        NUMFMTREF   numfmt;
        STRINGREF   refRetVal;
    } gc;

    gc.format = (STRINGREF) formatUNSAFE;
    gc.numfmt = (NUMFMTREF) numfmtUNSAFE;
    gc.refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    if (gc.numfmt == 0) COMPlusThrowArgumentNull(L"NumberFormatInfo");
    wchar fmt = ParseFormatSpecifier(gc.format, &digits);
    wchar val = (fmt & 0xFFDF);
    int precision = DOUBLE_PRECISION;
    switch (val) {
    case 'R':
        //In order to give numbers that are both friendly to display and round-trippable,
        //we parse the number using 15 digits and then determine if it round trips to the same
        //value.  If it does, we convert that NUMBER to a string, otherwise we reparse using 17 digits
        //and display that.

        DoubleToNumber(value, DOUBLE_PRECISION, &number);

        if (number.scale == (int) SCALE_NAN) {
            gc.refRetVal = gc.numfmt->sNaN;
            goto lExit;
        }

        if (number.scale == SCALE_INF) {
            gc.refRetVal = (number.sign? gc.numfmt->sNegativeInfinity: gc.numfmt->sPositiveInfinity);
            goto lExit;
        }

        NumberToDouble(&number, &dTest);

        if (dTest == value) {
            gc.refRetVal = NumberToString(&number, 'G', DOUBLE_PRECISION, gc.numfmt);
            goto lExit;
        }

        DoubleToNumber(value, 17, &number);
        gc.refRetVal = NumberToString(&number, 'G', 17, gc.numfmt);
        goto lExit;
        break;

    case 'E':
        // Here we round values less than E14 to 15 digits
        if (digits > 14) {
            precision = 17;
        }
        break;

    case 'G':
        // Here we round values less than G15 to 15 digits, G16 and G17 will not be touched
        if (digits > 15) {
            precision = 17;
        }
        break;

    }

    DoubleToNumber(value, precision, &number);

    if (number.scale == (int) SCALE_NAN) {
        gc.refRetVal = gc.numfmt->sNaN;
        goto lExit;
    }

    if (number.scale == SCALE_INF) {
        gc.refRetVal = (number.sign? gc.numfmt->sNegativeInfinity: gc.numfmt->sPositiveInfinity);
        goto lExit;
    }

    if (fmt != 0) {
        gc.refRetVal = NumberToString( &number, fmt, digits, gc.numfmt);
    }
    else {
        gc.refRetVal = NumberToStringFormat( &number, gc.format, gc.numfmt);
    }

lExit: ;
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.refRetVal);
}
FCIMPLEND

//This function and the function pointer which we use to access are a really
//nasty hack to prevent VC7 from optimizing away our cast from double to float.
//We need this narrowing operation to verify whether or not we successfully round-tripped
//the single value.                                                                        

static void CvtToFloat(double val, volatile float* fltPtr)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;	

    *fltPtr = (float)val;
}

void (*CvtToFloatPtr)(double val, volatile float* fltPtr) = CvtToFloat;

FCIMPL3_VII(Object*, COMNumber::FormatSingle, float value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;		
    }
    CONTRACTL_END;

    NUMBER number;
    int digits;
    double dTest;
    double argsValue = value;

    struct _gc
    {
        STRINGREF   format;
        NUMFMTREF   numfmt;
        STRINGREF   refRetVal;
    } gc;

    gc.format = (STRINGREF) formatUNSAFE;
    gc.numfmt = (NUMFMTREF) numfmtUNSAFE;
    gc.refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    if (gc.numfmt == 0) COMPlusThrowArgumentNull(L"NumberFormatInfo");
    wchar fmt = ParseFormatSpecifier(gc.format, &digits);
    wchar val = fmt & 0xFFDF;
    int precision = FLOAT_PRECISION;
    switch (val) {
    case 'R':
        //In order to give numbers that are both friendly to display and round-trippable,
        //we parse the number using 7 digits and then determine if it round trips to the same
        //value.  If it does, we convert that NUMBER to a string, otherwise we reparse using 9 digits
        //and display that.

        DoubleToNumber(argsValue, FLOAT_PRECISION, &number);

            if (number.scale == (int) SCALE_NAN) {
                gc.refRetVal = gc.numfmt->sNaN;
                goto lExit;
        }
        if (number.scale == SCALE_INF) {
                gc.refRetVal = (number.sign? gc.numfmt->sNegativeInfinity: gc.numfmt->sPositiveInfinity);
                goto lExit;
        }

        NumberToDouble(&number, &dTest);

        volatile float fTest;

        (*CvtToFloatPtr)(dTest, &fTest);

            if (fTest == value) {
                gc.refRetVal = NumberToString(&number, 'G', FLOAT_PRECISION, gc.numfmt);
                goto lExit;
        }

        DoubleToNumber(argsValue, 9, &number);
            gc.refRetVal = NumberToString(&number, 'G', 9, gc.numfmt);
            goto lExit;
        break;
    case 'E':
        // Here we round values less than E14 to 15 digits
        if (digits > 6) {
            precision = 9;
        }
        break;


    case 'G':
        // Here we round values less than G15 to 15 digits, G16 and G17 will not be touched
        if (digits > 7) {
            precision = 9;
        }
        break;

    }

    DoubleToNumber(value, precision, &number);

    if (number.scale == (int) SCALE_NAN) {
        gc.refRetVal = gc.numfmt->sNaN;
        goto lExit;
    }

    if (number.scale == SCALE_INF) {
        gc.refRetVal = (number.sign? gc.numfmt->sNegativeInfinity: gc.numfmt->sPositiveInfinity);
        goto lExit;
    }

    if (fmt != 0) {
        gc.refRetVal = NumberToString( &number, fmt, digits, gc.numfmt);
    }
    else {
        gc.refRetVal = NumberToStringFormat( &number, gc.format, gc.numfmt);
    }

lExit: ;
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.refRetVal);
}
FCIMPLEND

FCIMPL3(Object*, COMNumber::FormatInt32, INT32 value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    wchar fmt;
    int digits;

    struct _gc
    {
        STRINGREF   refFormat;
        NUMFMTREF   refNumFmt;
        STRINGREF   refRetString;
    } gc;

    gc.refFormat    = (STRINGREF)formatUNSAFE;
    gc.refNumFmt    = (NUMFMTREF)numfmtUNSAFE;
    gc.refRetString = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);


    if (gc.refNumFmt == 0) COMPlusThrowArgumentNull(L"NumberFormatInfo");
    fmt = ParseFormatSpecifier(gc.refFormat, &digits);

    //ANDing fmt with FFDF has the effect of uppercasing the character because
    //we've removed the bit that marks lower-case.
    switch (fmt & 0xFFDF) {
    case 'G':
        if (digits > 0)
        {
            NUMBER number;
            Int32ToNumber(value, &number);
            if (fmt != 0) {
                gc.refRetString = NumberToString(&number, fmt, digits, gc.refNumFmt);
                break;
            }
            gc.refRetString = NumberToStringFormat(&number, gc.refFormat, gc.refNumFmt);
            break;
        }
        // fall through
    case 'D':
        gc.refRetString = Int32ToDecStr(value, digits, gc.refNumFmt->sNegative);
        break;
    case 'X':
        gc.refRetString = Int32ToHexStr(value, fmt - ('X' - 'A' + 10), digits);
        break;
    default:
    NUMBER number;
    Int32ToNumber(value, &number);
    if (fmt != 0) {
      gc.refRetString = NumberToString(&number, fmt, digits, gc.refNumFmt);
      break;
    }
    gc.refRetString = NumberToStringFormat(&number, gc.refFormat, gc.refNumFmt);
    break;

    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.refRetString);
}
FCIMPLEND

FCIMPL3(Object*, COMNumber::FormatUInt32, UINT32 value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;		
    }
    CONTRACTL_END;

    wchar fmt;
    int digits;

    struct _gc
    {
        STRINGREF   refFormat;
        NUMFMTREF   refNumFmt;
        STRINGREF   refRetString;
    } gc;

    gc.refFormat    = (STRINGREF)formatUNSAFE;
    gc.refNumFmt    = (NUMFMTREF)numfmtUNSAFE;
    gc.refRetString = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);


    if (gc.refNumFmt == 0) COMPlusThrowArgumentNull(L"NumberFormatInfo");
    fmt = ParseFormatSpecifier(gc.refFormat, &digits);
    switch (fmt & 0xFFDF)
    {
    case 'G':
        if (digits > 0)
        {
            NUMBER number;
            UInt32ToNumber(value, &number);
            if (fmt != 0) {
                gc.refRetString = NumberToString(&number, fmt, digits, gc.refNumFmt);
                break;
            }
            gc.refRetString = NumberToStringFormat(&number, gc.refFormat, gc.refNumFmt);
            break;
        }
        // fall through
    case 'D':
        gc.refRetString = UInt32ToDecStr(value, digits);
        break;
    case 'X':
        gc.refRetString = Int32ToHexStr(value, fmt - ('X' - 'A' + 10), digits);
        break;
    default:
        NUMBER number;
        UInt32ToNumber(value, &number);
        if (fmt != 0) {
          gc.refRetString = NumberToString(&number, fmt, digits, gc.refNumFmt);
          break;
        }
        gc.refRetString = NumberToStringFormat(&number, gc.refFormat, gc.refNumFmt);
        break;
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.refRetString);
}
FCIMPLEND

FCIMPL3_VII(Object*, COMNumber::FormatInt64, INT64 value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;		
    }
    CONTRACTL_END;

    wchar fmt;
    int digits;

    struct _gc
    {
        STRINGREF   refFormat;
        NUMFMTREF   refNumFmt;
        STRINGREF   refRetString;
    } gc;

    gc.refFormat    = ObjectToSTRINGREF(formatUNSAFE);
    gc.refNumFmt    = (NUMFMTREF)numfmtUNSAFE;
    gc.refRetString = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);


    if (gc.refNumFmt == 0) COMPlusThrowArgumentNull(L"NumberFormatInfo");
    fmt = ParseFormatSpecifier(gc.refFormat, &digits);
    switch (fmt & 0xFFDF)
    {
        case 'G':
            if (digits > 0)
            {
                NUMBER number;
                Int64ToNumber(value, &number);
                if (fmt != 0) {
                    gc.refRetString = NumberToString(&number, fmt, digits, gc.refNumFmt);
                    break;
                }
                gc.refRetString = NumberToStringFormat(&number, gc.refFormat, gc.refNumFmt);
                break;
            }
            // fall through
        case 'D':
            gc.refRetString = Int64ToDecStr(value, digits, gc.refNumFmt->sNegative);
            break;
        case 'X':
            gc.refRetString = Int64ToHexStr(value, fmt - ('X' - 'A' + 10), digits);
            break;
        default:
            NUMBER number;
            Int64ToNumber(value, &number);
            if (fmt != 0) {
                gc.refRetString = NumberToString(&number, fmt, digits, gc.refNumFmt);
                break;
            }
            gc.refRetString = NumberToStringFormat(&number, gc.refFormat, gc.refNumFmt);
            break;
    }
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.refRetString);
}
FCIMPLEND

FCIMPL3_VII(Object*, COMNumber::FormatUInt64, UINT64 value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;		
    }
    CONTRACTL_END;

    wchar fmt;
    int digits;

    struct _gc
    {
        STRINGREF   refFormat;
        NUMFMTREF   refNumFmt;
        STRINGREF   refRetString;
    } gc;

    gc.refFormat    = ObjectToSTRINGREF(formatUNSAFE);
    gc.refNumFmt    = (NUMFMTREF)numfmtUNSAFE;
    gc.refRetString = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    if (gc.refNumFmt == 0) COMPlusThrowArgumentNull(L"NumberFormatInfo");
    fmt = ParseFormatSpecifier(gc.refFormat, &digits);
    switch (fmt & 0xFFDF) {
        case 'G':
            if (digits > 0)
            {
                NUMBER number;
                UInt64ToNumber(value, &number);
                if (fmt != 0) {
                    gc.refRetString = NumberToString(&number, fmt, digits, gc.refNumFmt);
                    break;
                }
                gc.refRetString = NumberToStringFormat(&number, gc.refFormat, gc.refNumFmt);
                break;
            }
            // fall through
        case 'D':
            gc.refRetString = UInt64ToDecStr(value, digits);
            break;
        case 'X':
            gc.refRetString = Int64ToHexStr(value, fmt - ('X' - 'A' + 10), digits);
            break;
        default:
            NUMBER number;
            UInt64ToNumber(value, &number);
            if (fmt != 0) {
                gc.refRetString = NumberToString(&number, fmt, digits, gc.refNumFmt);
                break;
            }
            gc.refRetString = NumberToStringFormat(&number, gc.refFormat, gc.refNumFmt);
            break;
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.refRetString);
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, COMNumber::NumberBufferToDecimal, BYTE* number, DECIMAL* value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;	

    FC_RETURN_BOOL(COMDecimal::NumberToDecimal((NUMBER *) number, value) != 0);
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, COMNumber::NumberBufferToDouble, BYTE* number, double* value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    double d = 0;
    NumberToDouble((NUMBER*) number, &d);
    unsigned int e = ((FPDOUBLE*)&d)->exp;
    unsigned int fmntLow = ((FPDOUBLE*)&d)->mantLo;
    unsigned int fmntHigh = ((FPDOUBLE*)&d)->mantHi;
    if (e == 0x7FF) {
        FC_RETURN_BOOL(false);
    }
    if (e == 0 && fmntLow ==0 && fmntHigh == 0)  {
        d = 0;
    }
    *value = d;
    FC_RETURN_BOOL(true);
}
FCIMPLEND

