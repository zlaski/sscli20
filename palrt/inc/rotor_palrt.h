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
// ===========================================================================
// File: rotor_palrt.h
// 
// =========================================================================== 

/*++


Abstract:

    Rotor runtime functions.  These are functions which are ordinarily
    implemented as part of the Win32 API set, but for Rotor, are
    implemented as a runtime library on top of the PAL.

Author:


Revision History:

--*/

#ifndef __ROTOR_PALRT_H__
#define __ROTOR_PALRT_H__

/******************* HRESULTs *********************************************/

#ifdef RC_INVOKED
#define _HRESULT_TYPEDEF_(_sc) _sc
#else // RC_INVOKED
#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
#endif // RC_INVOKED

#define S_OK                             _HRESULT_TYPEDEF_(0x00000000L)
#define S_FALSE                          _HRESULT_TYPEDEF_(0x00000001L)

#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80004001L)
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)
#define E_UNEXPECTED                     _HRESULT_TYPEDEF_(0x8000FFFFL)
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80070006L)
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80004004L)
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80070005L)
#define E_PENDING                        _HRESULT_TYPEDEF_(0x8000000AL)

#define DISP_E_PARAMNOTFOUND             _HRESULT_TYPEDEF_(0x80020004L)
#define DISP_E_TYPEMISMATCH              _HRESULT_TYPEDEF_(0x80020005L)
#define DISP_E_BADVARTYPE                _HRESULT_TYPEDEF_(0x80020008L)
#define DISP_E_OVERFLOW                  _HRESULT_TYPEDEF_(0x8002000AL)
#define DISP_E_DIVBYZERO                 _HRESULT_TYPEDEF_(0x80020012L)

#define CLASS_E_CLASSNOTAVAILABLE        _HRESULT_TYPEDEF_(0x80040111L)
#define CLASS_E_NOAGGREGATION            _HRESULT_TYPEDEF_(0x80040110L)

#define CO_E_CLASSSTRING                 _HRESULT_TYPEDEF_(0x800401F3L)

#define URL_E_INVALID_SYNTAX             _HRESULT_TYPEDEF_(0x80041001L)
#define MK_E_SYNTAX                      _HRESULT_TYPEDEF_(0x800401E4L)

#define STG_E_INVALIDFUNCTION            _HRESULT_TYPEDEF_(0x80030001L)
#define STG_E_FILENOTFOUND               _HRESULT_TYPEDEF_(0x80030002L)
#define STG_E_PATHNOTFOUND               _HRESULT_TYPEDEF_(0x80030003L)
#define STG_E_WRITEFAULT                 _HRESULT_TYPEDEF_(0x8003001DL)
#define STG_E_FILEALREADYEXISTS          _HRESULT_TYPEDEF_(0x80030050L)
#define STG_E_ABNORMALAPIEXIT            _HRESULT_TYPEDEF_(0x800300FAL)

#define NTE_BAD_UID                      _HRESULT_TYPEDEF_(0x80090001L)
#define NTE_BAD_HASH                     _HRESULT_TYPEDEF_(0x80090002L)
#define NTE_BAD_KEY                      _HRESULT_TYPEDEF_(0x80090003L)
#define NTE_BAD_LEN                      _HRESULT_TYPEDEF_(0x80090004L)
#define NTE_BAD_DATA                     _HRESULT_TYPEDEF_(0x80090005L)
#define NTE_BAD_SIGNATURE                _HRESULT_TYPEDEF_(0x80090006L)
#define NTE_BAD_VER                      _HRESULT_TYPEDEF_(0x80090007L)
#define NTE_BAD_ALGID                    _HRESULT_TYPEDEF_(0x80090008L)
#define NTE_BAD_FLAGS                    _HRESULT_TYPEDEF_(0x80090009L)
#define NTE_BAD_TYPE                     _HRESULT_TYPEDEF_(0x8009000AL)
#define NTE_BAD_KEY_STATE                _HRESULT_TYPEDEF_(0x8009000BL)
#define NTE_BAD_HASH_STATE               _HRESULT_TYPEDEF_(0x8009000CL)
#define NTE_NO_KEY                       _HRESULT_TYPEDEF_(0x8009000DL)
#define NTE_NO_MEMORY                    _HRESULT_TYPEDEF_(0x8009000EL)
#define NTE_SIGNATURE_FILE_BAD           _HRESULT_TYPEDEF_(0x8009001CL)
#define NTE_FAIL                         _HRESULT_TYPEDEF_(0x80090020L)

#define CRYPT_E_HASH_VALUE               _HRESULT_TYPEDEF_(0x80091007L)

#define TYPE_E_SIZETOOBIG                _HRESULT_TYPEDEF_(0x800288C5L)
#define TYPE_E_DUPLICATEID               _HRESULT_TYPEDEF_(0x800288C6L)

#define STD_CTL_SCODE(n) MAKE_SCODE(SEVERITY_ERROR, FACILITY_CONTROL, n)
#define CTL_E_OVERFLOW                  STD_CTL_SCODE(6)
#define CTL_E_OUTOFMEMORY               STD_CTL_SCODE(7)
#define CTL_E_DIVISIONBYZERO            STD_CTL_SCODE(11)
#define CTL_E_OUTOFSTACKSPACE           STD_CTL_SCODE(28)
#define CTL_E_FILENOTFOUND              STD_CTL_SCODE(53)
#define CTL_E_DEVICEIOERROR             STD_CTL_SCODE(57)
#define CTL_E_PERMISSIONDENIED          STD_CTL_SCODE(70)
#define CTL_E_PATHFILEACCESSERROR       STD_CTL_SCODE(75)
#define CTL_E_PATHNOTFOUND              STD_CTL_SCODE(76)

#define INET_E_CANNOT_CONNECT            _HRESULT_TYPEDEF_(0x800C0004L)
#define INET_E_RESOURCE_NOT_FOUND        _HRESULT_TYPEDEF_(0x800C0005L)
#define INET_E_OBJECT_NOT_FOUND          _HRESULT_TYPEDEF_(0x800C0006L)
#define INET_E_DATA_NOT_AVAILABLE        _HRESULT_TYPEDEF_(0x800C0007L)
#define INET_E_DOWNLOAD_FAILURE          _HRESULT_TYPEDEF_(0x800C0008L)
#define INET_E_CONNECTION_TIMEOUT        _HRESULT_TYPEDEF_(0x800C000BL)
#define INET_E_UNKNOWN_PROTOCOL          _HRESULT_TYPEDEF_(0x800C000DL)

/**************************************************************************/

#define STATUS_INVALID_PARAMETER_3       (0xC00000F1L)
#define STATUS_INVALID_PARAMETER_4       (0xC00000F2L)
#define STATUS_UNSUCCESSFUL              (0xC0000001L)
#define STATUS_SUCCESS                   (0x00000000L)

/**************************************************************************/

#ifndef RC_INVOKED

#include "rotor_pal.h"

#ifdef __cplusplus
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void *__cdecl operator new(size_t, void *_P)
{
    return (_P);
}
#endif // __PLACEMENT_NEW_INLINE
#endif

#include <pal_assert.h>

#define IsDebuggerPresent() FALSE

#if defined(_DEBUG)
#define ROTOR_PAL_CTOR_TEST_BODY(TESTNAME)                              \
    class TESTNAME ## _CTOR_TEST {                                      \
    public:                                                             \
        class HelperClass {                                             \
        public:                                                         \
            HelperClass(const char *String) {                           \
                _ASSERTE (m_s == NULL);                                 \
                m_s = String;                                           \
            }                                                           \
                                                                        \
            void Validate (const char *String) {                        \
                _ASSERTE (m_s);                                         \
                _ASSERTE (m_s == String);                               \
                _ASSERTE (!strncmp (                                    \
                              m_s,                                      \
                              String,                                   \
                              1000));                                   \
            }                                                           \
                                                                        \
        private:                                                        \
            const char *m_s;                                            \
        };                                                              \
                                                                        \
        TESTNAME ## _CTOR_TEST() {                                      \
            _ASSERTE (m_This == NULL);                                  \
            m_This = this;                                              \
        }                                                               \
                                                                        \
        void Validate () {                                              \
            _ASSERTE (m_This == this);                                  \
            m_String.Validate(#TESTNAME "_CTOR_TEST");                  \
        }                                                               \
                                                                        \
    private:                                                            \
        void              *m_This;                                      \
        static HelperClass m_String;                                    \
    };                                                                  \
                                                                        \
    static TESTNAME ## _CTOR_TEST                                       \
      g_ ## TESTNAME ## _CTOR_TEST;                                     \
    TESTNAME ## _CTOR_TEST::HelperClass                                 \
      TESTNAME ## _CTOR_TEST::m_String(#TESTNAME "_CTOR_TEST");

#define ROTOR_PAL_CTOR_TEST_RUN(TESTNAME)                               \
    g_ ## TESTNAME ##_CTOR_TEST.Validate()

#else // DEBUG

#define ROTOR_PAL_CTOR_TEST_BODY(TESTNAME) 
#define ROTOR_PAL_CTOR_TEST_RUN(TESTNAME)  do {} while (0)

#endif // DEBUG

#define NTAPI       __stdcall
#define WINAPI      __stdcall
#define CALLBACK    __stdcall

#define _WINNT_

// C++ standard, 18.1.5 - offsetof requires a POD (plain old data) struct or
// union. Since offsetof is a macro, gcc doesn't actually check for improper
// use of offsetof, it keys off of the -> from NULL (which is also invalid for
// non-POD types by 18.1.5)
//
// As we have numerous examples of this behavior in our codebase,
// making an offsetof which doesn't use 0.

// PAL_safe_offsetof is a version of offsetof that protects against an
// overridden operator&

#define FIELD_OFFSET(type, field) (((LONG)(LONG_PTR)&(((type *)64)->field)) - 64)
#define offsetof(s,m)          ((size_t)((ptrdiff_t)&(((s *)64)->m)) - 64)
#define PAL_safe_offsetof(s,m) ((size_t)((ptrdiff_t)&(char&)(((s *)64)->m))-64)

#define ARGUMENT_PRESENT(ArgumentPointer)    (\
    (CHAR *)(ArgumentPointer) != (CHAR *)(NULL) )

#define MAX_NATURAL_ALIGNMENT sizeof(ULONG)

#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name

#ifndef COM_NO_WINDOWS_H
#define COM_NO_WINDOWS_H
#endif

#define interface struct

#define STDMETHODCALLTYPE    __stdcall
#define STDMETHODVCALLTYPE   __cdecl

#define STDAPICALLTYPE       __stdcall
#define STDAPIVCALLTYPE      __cdecl

#define STDMETHODIMP         HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)  type STDMETHODCALLTYPE

#define STDMETHODIMPV        HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_(type) type STDMETHODVCALLTYPE

#define STDMETHOD(method)       virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method) virtual type STDMETHODCALLTYPE method

#define STDMETHODV(method)       virtual HRESULT STDMETHODVCALLTYPE method
#define STDMETHODV_(type,method) virtual type STDMETHODVCALLTYPE method

#define STDAPI               EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)        EXTERN_C type STDAPICALLTYPE

#define STDAPIV              EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type)       EXTERN_C type STDAPIVCALLTYPE

#define PURE                    = 0
#define THIS_
#define THIS                void

#if _MSC_VER
#define DECLSPEC_NOVTABLE   __declspec(novtable)
#define DECLSPEC_IMPORT     __declspec(dllimport)
#else
#define DECLSPEC_NOVTABLE
#define DECLSPEC_IMPORT
#endif

#define DECLARE_INTERFACE(iface)    interface DECLSPEC_NOVTABLE iface
#define DECLARE_INTERFACE_(iface, baseiface)    interface DECLSPEC_NOVTABLE iface : public baseiface

#ifdef __cplusplus
#define REFGUID const GUID &
#else
#define REFGUID const GUID *
#endif

extern const GUID GUID_NULL;

typedef const GUID FAR *LPCGUID;

#ifdef __cplusplus
extern "C++" {
inline int IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
    { return !memcmp(&rguid1, &rguid2, sizeof(GUID)); }
inline int operator==(REFGUID guidOne, REFGUID guidOther)
    { return IsEqualGUID(guidOne,guidOther); }
inline int operator!=(REFGUID guidOne, REFGUID guidOther)
    { return !IsEqualGUID(guidOne,guidOther); }
};
#endif // __cplusplus

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID FAR name

typedef GUID IID;
#ifdef __cplusplus
#define REFIID const IID &
#else
#define REFIID const IID *
#endif
#define IID_NULL GUID_NULL
#define IsEqualIID(riid1, riid2) IsEqualGUID(riid1, riid2)

#define __IID_DEFINED__

typedef GUID CLSID;
#define CLSID_DEFINED
#ifdef __cplusplus
#define REFCLSID const CLSID &
#else
#define REFCLSID const CLSID *
#endif
#define CLSID_NULL GUID_NULL
#define IsEqualCLSID(rclsid1, rclsid2) IsEqualGUID(rclsid1, rclsid2)

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

typedef LONG SCODE;


typedef union _ULARGE_INTEGER {
    struct {
#if BIGENDIAN
        DWORD HighPart;
        DWORD LowPart;
#else
        DWORD LowPart;
        DWORD HighPart;
#endif
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

/******************* HRESULT types ****************************************/

#define FACILITY_WINDOWS                 8
#define FACILITY_URT                     19
#define FACILITY_UMI                     22
#define FACILITY_SXS                     23
#define FACILITY_STORAGE                 3
#define FACILITY_SSPI                    9
#define FACILITY_SCARD                   16
#define FACILITY_SETUPAPI                15
#define FACILITY_SECURITY                9
#define FACILITY_RPC                     1
#define FACILITY_WIN32                   7
#define FACILITY_CONTROL                 10
#define FACILITY_NULL                    0
#define FACILITY_MSMQ                    14
#define FACILITY_MEDIASERVER             13
#define FACILITY_INTERNET                12
#define FACILITY_ITF                     4
#define FACILITY_DPLAY                   21
#define FACILITY_DISPATCH                2
#define FACILITY_COMPLUS                 17
#define FACILITY_CERT                    11
#define FACILITY_ACS                     20
#define FACILITY_AAF                     18

#define NO_ERROR 0L

#define SEVERITY_SUCCESS    0
#define SEVERITY_ERROR      1

#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
#define FAILED(Status) ((HRESULT)(Status)<0)
#define IS_ERROR(Status) ((ULONG)(Status) >> 31 == SEVERITY_ERROR) // diff from win32
#define HRESULT_CODE(hr)    ((hr) & 0xFFFF)
#define SCODE_CODE(sc)      ((sc) & 0xFFFF)
#define HRESULT_FACILITY(hr)  (((hr) >> 16) & 0x1fff)
#define SCODE_FACILITY(sc)    (((sc) >> 16) & 0x1fff)
#define HRESULT_SEVERITY(hr)  (((hr) >> 31) & 0x1)
#define SCODE_SEVERITY(sc)    (((sc) >> 31) & 0x1)

// both macros diff from Win32
#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((ULONG)(sev)<<31) | ((ULONG)(fac)<<16) | ((ULONG)(code))) )
#define MAKE_SCODE(sev,fac,code) \
    ((SCODE) (((ULONG)(sev)<<31) | ((ULONG)(fac)<<16) | ((LONG)(code))) )

#define FACILITY_NT_BIT                 0x10000000
#define HRESULT_FROM_WIN32(x) ((HRESULT)(x) <= 0 ? ((HRESULT)(x)) : ((HRESULT) (((x) & 0x0000FFFF) | (FACILITY_WIN32 << 16) | 0x80000000)))

#define HRESULT_FROM_NT(x)      ((HRESULT) ((x) | FACILITY_NT_BIT))

/******************* OLE, BSTR, VARIANT *************************/

STDAPI_(LPVOID) CoTaskMemAlloc(SIZE_T cb);
STDAPI_(void) CoTaskMemFree(LPVOID pv);

typedef SHORT VARIANT_BOOL;
#define VARIANT_TRUE ((VARIANT_BOOL)-1)
#define VARIANT_FALSE ((VARIANT_BOOL)0)

typedef WCHAR OLECHAR;
typedef OLECHAR* LPOLESTR;
typedef const OLECHAR* LPCOLESTR;

typedef WCHAR *BSTR;

STDAPI_(BSTR) SysAllocString(const OLECHAR*);
STDAPI_(BSTR) SysAllocStringLen(const OLECHAR*, UINT);
STDAPI_(BSTR) SysAllocStringByteLen(const char *, UINT);
STDAPI_(void) SysFreeString(BSTR);
STDAPI_(UINT) SysStringLen(BSTR);
STDAPI_(UINT) SysStringByteLen(BSTR);

typedef double DATE;

typedef union tagCY {
    struct {
#if BIGENDIAN
        LONG    Hi;
        ULONG   Lo;
#else
        ULONG   Lo;
        LONG    Hi;
#endif
    } u;
    LONGLONG int64;
} CY, *LPCY;

typedef CY CURRENCY;

typedef struct tagDEC {
    // Decimal.cs treats the first two shorts as one long
    // And they seriable the data so we need to little endian
    // seriliazation
    // The wReserved overlaps with Variant's vt member
#if BIGENDIAN
    union {
        struct {
            BYTE sign;
            BYTE scale;
        } u;
        USHORT signscale;
    } u;
    USHORT wReserved;
#else
    USHORT wReserved;
    union {
        struct {
            BYTE scale;
            BYTE sign;
        } u;
        USHORT signscale;
    } u;
#endif
    ULONG Hi32;
    union {
        struct {
            ULONG Lo32;
            ULONG Mid32;
        } v;
        // Don't use Lo64 in the PAL: ULONGLONG Lo64;
    } v;
} DECIMAL, *LPDECIMAL;

#define DECIMAL_NEG ((BYTE)0x80)
#define DECIMAL_SCALE(dec)       ((dec).u.u.scale)
#define DECIMAL_SIGN(dec)        ((dec).u.u.sign)
#define DECIMAL_SIGNSCALE(dec)   ((dec).u.signscale)
#define DECIMAL_LO32(dec)        ((dec).v.v.Lo32)
#define DECIMAL_MID32(dec)       ((dec).v.v.Mid32)
#define DECIMAL_HI32(dec)        ((dec).Hi32)

#define DECIMAL_SETZERO(dec) {DECIMAL_LO32(dec) = 0; DECIMAL_MID32(dec) = 0; DECIMAL_HI32(dec) = 0; DECIMAL_SIGNSCALE(dec) = 0;}

typedef struct tagBLOB {
    ULONG cbSize;
    BYTE *pBlobData;
} BLOB, *LPBLOB;

interface IUnknown;
interface IStream;
interface IRecordInfo;

typedef unsigned short VARTYPE;

enum VARENUM {
    VT_EMPTY    = 0,
    VT_NULL = 1,
    VT_I2   = 2,
    VT_I4   = 3,
    VT_R4   = 4,
    VT_R8   = 5,
    VT_CY   = 6,
    VT_DATE = 7,
    VT_BSTR = 8,
    VT_DISPATCH = 9,
    VT_ERROR    = 10,
    VT_BOOL = 11,
    VT_VARIANT  = 12,
    VT_UNKNOWN  = 13,
    VT_DECIMAL  = 14,
    VT_I1   = 16,
    VT_UI1  = 17,
    VT_UI2  = 18,
    VT_UI4  = 19,
    VT_I8   = 20,
    VT_UI8  = 21,
    VT_INT  = 22,
    VT_UINT = 23,
    VT_VOID = 24,
    VT_HRESULT  = 25,
    VT_PTR  = 26,
    VT_SAFEARRAY    = 27,
    VT_CARRAY   = 28,
    VT_USERDEFINED  = 29,
    VT_LPSTR    = 30,
    VT_LPWSTR   = 31,
    VT_RECORD   = 36,

    VT_FILETIME        = 64,
    VT_BLOB            = 65,
    VT_STREAM          = 66,
    VT_STORAGE         = 67,
    VT_STREAMED_OBJECT = 68,
    VT_STORED_OBJECT   = 69,
    VT_BLOB_OBJECT     = 70,
    VT_CF              = 71,
    VT_CLSID           = 72,

    VT_VECTOR   = 0x1000,
    VT_ARRAY    = 0x2000,
    VT_BYREF    = 0x4000,
    VT_TYPEMASK = 0xfff,
};

typedef struct tagVARIANT VARIANT, *LPVARIANT;

struct tagVARIANT
    {
    union
        {
        struct
            {
#if BIGENDIAN
            // We need to make sure vt overlaps with DECIMAL's wReserved.
            // See the DECIMAL type for details.
            WORD wReserved1;
            VARTYPE vt;
#else
            VARTYPE vt;
            WORD wReserved1;
#endif
            WORD wReserved2;
            WORD wReserved3;
            union
                {
                LONGLONG llVal;
                LONG lVal;
                BYTE bVal;
                SHORT iVal;
                FLOAT fltVal;
                DOUBLE dblVal;
                VARIANT_BOOL boolVal;
                SCODE scode;
                CY cyVal;
                DATE date;
                BSTR bstrVal;
                interface IUnknown *punkVal;
                BYTE *pbVal;
                SHORT *piVal;
                LONG *plVal;
                LONGLONG *pllVal;
                FLOAT *pfltVal;
                DOUBLE *pdblVal;
                VARIANT_BOOL *pboolVal;
                SCODE *pscode;
                CY *pcyVal;
                DATE *pdate;
                BSTR *pbstrVal;
                interface IUnknown **ppunkVal;
                VARIANT *pvarVal;
                PVOID byref;
                CHAR cVal;
                USHORT uiVal;
                ULONG ulVal;
                ULONGLONG ullVal;
                INT intVal;
                UINT uintVal;
                DECIMAL *pdecVal;
                CHAR *pcVal;
                USHORT *puiVal;
                ULONG *pulVal;
                ULONGLONG *pullVal;
                INT *pintVal;
                UINT *puintVal;
                struct __tagBRECORD
                    {
                    PVOID pvRecord;
                    interface IRecordInfo *pRecInfo;
                    } brecVal;
                } n3;
            } n2;
        DECIMAL decVal;
        } n1;
    };

typedef VARIANT VARIANTARG, *LPVARIANTARG;

STDAPI_(void) VariantInit(VARIANT * pvarg);
STDAPI_(HRESULT) VariantClear(VARIANT * pvarg);

#define V_VT(X)         ((X)->n1.n2.vt)
#define V_UNION(X, Y)   ((X)->n1.n2.n3.Y)
#define V_RECORDINFO(X) ((X)->n1.n2.n3.brecVal.pRecInfo)
#define V_RECORD(X)     ((X)->n1.n2.n3.brecVal.pvRecord)

#define V_UI1(X)         V_UNION(X, bVal)
#define V_UI1REF(X)      V_UNION(X, pbVal)
#define V_I2(X)          V_UNION(X, iVal)
#define V_I2REF(X)       V_UNION(X, piVal)
#define V_I4(X)          V_UNION(X, lVal)
#define V_I4REF(X)       V_UNION(X, plVal)
#define V_I8(X)          V_UNION(X, llVal)
#define V_I8REF(X)       V_UNION(X, pllVal)
#define V_R4(X)          V_UNION(X, fltVal)
#define V_R4REF(X)       V_UNION(X, pfltVal)
#define V_R8(X)          V_UNION(X, dblVal)
#define V_R8REF(X)       V_UNION(X, pdblVal)
#define V_I1(X)          V_UNION(X, cVal)
#define V_I1REF(X)       V_UNION(X, pcVal)
#define V_UI2(X)         V_UNION(X, uiVal)
#define V_UI2REF(X)      V_UNION(X, puiVal)
#define V_UI4(X)         V_UNION(X, ulVal)
#define V_UI4REF(X)      V_UNION(X, pulVal)
#define V_UI8(X)         V_UNION(X, ullVal)
#define V_UI8REF(X)      V_UNION(X, pullVal)
#define V_INT(X)         V_UNION(X, intVal)
#define V_INTREF(X)      V_UNION(X, pintVal)
#define V_UINT(X)        V_UNION(X, uintVal)
#define V_UINTREF(X)     V_UNION(X, puintVal)

#define V_INT_PTR(X)        V_UNION(X, lVal)
#define V_UINT_PTR(X)       V_UNION(X, ulVal)
#define V_INT_PTRREF(X)     V_UNION(X, plVal)
#define V_UINT_PTRREF(X)    V_UNION(X, pulVal)

#define V_CY(X)          V_UNION(X, cyVal)
#define V_CYREF(X)       V_UNION(X, pcyVal)
#define V_DATE(X)        V_UNION(X, date)
#define V_DATEREF(X)     V_UNION(X, pdate)
#define V_BSTR(X)        V_UNION(X, bstrVal)
#define V_BSTRREF(X)     V_UNION(X, pbstrVal)
#define V_UNKNOWN(X)     V_UNION(X, punkVal)
#define V_UNKNOWNREF(X)  V_UNION(X, ppunkVal)
#define V_VARIANTREF(X)  V_UNION(X, pvarVal)
#define V_ERROR(X)       V_UNION(X, scode)
#define V_ERRORREF(X)    V_UNION(X, pscode)
#define V_BOOL(X)        V_UNION(X, boolVal)
#define V_BOOLREF(X)     V_UNION(X, pboolVal)
#define V_BYREF(X)       V_UNION(X, byref)

#define V_DECIMAL(X)     ((X)->n1.decVal)
#define V_DECIMALREF(X)    V_UNION(X, pdecVal)

#define V_ISBYREF(X)     (V_VT(X)&VT_BYREF)

STDAPI CreateStreamOnHGlobal(PVOID hGlobal, BOOL fDeleteOnRelease, interface IStream** ppstm);

#define STGM_DIRECT             0x00000000L

#define STGM_READ               0x00000000L
#define STGM_WRITE              0x00000001L
#define STGM_READWRITE          0x00000002L

#define STGM_SHARE_DENY_NONE    0x00000040L
#define STGM_SHARE_DENY_READ    0x00000030L
#define STGM_SHARE_DENY_WRITE   0x00000020L
#define STGM_SHARE_EXCLUSIVE    0x00000010L

#define STGM_DELETEONRELEASE    0x04000000L

#define STGM_CREATE             0x00001000L
#define STGM_CONVERT            0x00020000L
#define STGM_FAILIFTHERE        0x00000000L

#define STGM_NOSNAPSHOT         0x00200000L

STDAPI IIDFromString(LPOLESTR lpsz, IID* lpiid);
STDAPI_(int) StringFromGUID2(REFGUID rguid, LPOLESTR lpsz, int cchMax); 

STDAPI CoCreateGuid(OUT GUID * pguid);

/******************* CRYPT **************************************/

typedef unsigned int ALG_ID;

#define CALG_RSA_SIGN           ((1 << 13) | (2 << 9) | 0)

#define CALG_MD2                ((4 << 13) | 1)
#define CALG_MD4                ((4 << 13) | 2)
#define CALG_MD5                ((4 << 13) | 3)
#define CALG_SHA                ((4 << 13) | 4)
#define CALG_SHA1               ((4 << 13) | 4)
#define CALG_MAC                ((4 << 13) | 5)
#define CALG_SSL3_SHAMD5        ((4 << 13) | 8)
#define CALG_HMAC               ((4 << 13) | 9)

#define GET_ALG_CLASS(x)        (x & (7 << 13))
#define ALG_CLASS_SIGNATURE     (1 << 13)
#define ALG_CLASS_HASH          (4 << 13)

#define MAX_SIZE_SECURITY_ID    512

#define PROV_RSA_FULL           1
#define CRYPT_VERIFYCONTEXT     0xF0000000

#define PUBLICKEYBLOB           0x6
#define PRIVATEKEYBLOB          0x7

#define HP_ALGID                0x0001
#define HP_HASHVAL              0x0002
#define HP_HASHSIZE             0x0004

#define KP_ALGID                0x0007
#define KP_KEYLEN               0x0009

#define AT_SIGNATURE            2

#define CRYPT_EXPORTABLE        0x00000001

typedef ULONG_PTR HCRYPTPROV;
typedef ULONG_PTR HCRYPTKEY;
typedef ULONG_PTR HCRYPTHASH;

EXTERN_C
BOOL
PALAPI
CryptAcquireContextW(
    HCRYPTPROV *phProv,
    LPCWSTR szContainer,
    LPCWSTR szProvider,
    DWORD dwProvType,
    DWORD dwFlags);

EXTERN_C
BOOL
PALAPI
CryptAcquireContextA(
    HCRYPTPROV *phProv,
    LPCSTR szContainer,
    LPCSTR szProvider,
    DWORD dwProvType,
    DWORD dwFlags);

#ifdef UNICODE
#define CryptAcquireContext  CryptAcquireContextW
#else
#define CryptAcquireContext  CryptAcquireContextA
#endif

EXTERN_C
BOOL
PALAPI
CryptReleaseContext(
    HCRYPTPROV hProv,
    DWORD dwFlags);

EXTERN_C
BOOL
PALAPI
CryptCreateHash(
    HCRYPTPROV hProv,
    ALG_ID Algid,
    HCRYPTKEY hKey,
    DWORD dwFlags,
    HCRYPTHASH *phHash);

EXTERN_C
BOOL
PALAPI
CryptDestroyHash(
    HCRYPTHASH hHash);

EXTERN_C
BOOL
PALAPI
CryptHashData(
    HCRYPTHASH hHash,
    CONST BYTE *pbData,
    DWORD dwDataLen,
    DWORD dwFlags);

EXTERN_C
BOOL
PALAPI
CryptGetHashParam(
    HCRYPTHASH hHash,
    DWORD dwParam,
    BYTE *pbData,
    DWORD *pdwDataLen,
    DWORD dwFlags);

EXTERN_C
BOOL
PALAPI
CryptGenKey(
    HCRYPTPROV hProv,
    ALG_ID Algid,
    DWORD dwFlags,
    HCRYPTKEY *phKey);

EXTERN_C
BOOL
PALAPI
CryptDestroyKey(
    HCRYPTKEY hKey);

EXTERN_C
BOOL
PALAPI
CryptImportKey(
    HCRYPTPROV hProv,
    CONST BYTE *pbData,
    DWORD dwDataLen,
    HCRYPTKEY hPubKey,
    DWORD dwFlags,
    HCRYPTKEY *phKey);

EXTERN_C
BOOL
PALAPI
CryptExportKey(
    HCRYPTKEY hKey,
    HCRYPTKEY hExpKey,
    DWORD dwBlobType,
    DWORD dwFlags,
    BYTE *pbData,
    DWORD *pdwDataLen);

EXTERN_C
BOOL
PALAPI
CryptGetKeyParam(
    HCRYPTKEY hKey,
    DWORD dwParam,
    BYTE *pbData,
    DWORD *pdwDataLen,
    DWORD dwFlags);

EXTERN_C
BOOL
PALAPI
CryptVerifySignatureW(
    HCRYPTHASH hHash,
    CONST BYTE *pbSignature,
    DWORD dwSigLen,
    HCRYPTKEY hPubKey,
    LPCWSTR szDescription,
    DWORD dwFlags);

EXTERN_C
BOOL
PALAPI
CryptVerifySignatureA(
    HCRYPTHASH hHash,
    CONST BYTE *pbSignature,
    DWORD dwSigLen,
    HCRYPTKEY hPubKey,
    LPCSTR szDescription,
    DWORD dwFlags);

#ifdef UNICODE
#define CryptVerifySignature CryptVerifySignatureW
#else
#define CryptVerifySignature CryptVerifySignatureA
#endif

EXTERN_C
BOOL
PALAPI
CryptSignHashW(
    HCRYPTHASH hHash,
    DWORD dwKeySpec,
    LPCWSTR szDescription,
    DWORD dwFlags,
    BYTE *pbSignature,
    DWORD *pdwSigLen);

EXTERN_C
BOOL
PALAPI
CryptSignHashA(
    HCRYPTHASH hHash,
    DWORD dwKeySpec,
    LPCSTR szDescription,
    DWORD dwFlags,
    BYTE *pbSignature,
    DWORD *pdwSigLen);

#ifdef UNICODE
#define CryptSignHash  CryptSignHashW
#else
#define CryptSignHash  CryptSignHashA
#endif

/******************* NLS ****************************************/

typedef 
enum tagMIMECONTF {
    MIMECONTF_MAILNEWS  = 0x1,
    MIMECONTF_BROWSER   = 0x2,
    MIMECONTF_MINIMAL   = 0x4,
    MIMECONTF_IMPORT    = 0x8,
    MIMECONTF_SAVABLE_MAILNEWS  = 0x100,
    MIMECONTF_SAVABLE_BROWSER   = 0x200,
    MIMECONTF_EXPORT    = 0x400,
    MIMECONTF_PRIVCONVERTER = 0x10000,
    MIMECONTF_VALID = 0x20000,
    MIMECONTF_VALID_NLS = 0x40000,
    MIMECONTF_MIME_IE4  = 0x10000000,
    MIMECONTF_MIME_LATEST   = 0x20000000,
    MIMECONTF_MIME_REGISTRY = 0x40000000
    }   MIMECONTF;

#define LCMAP_LOWERCASE           0x00000100
#define LCMAP_UPPERCASE           0x00000200
#define LCMAP_SORTKEY             0x00000400
#define LCMAP_BYTEREV             0x00000800

#define LCMAP_HIRAGANA            0x00100000
#define LCMAP_KATAKANA            0x00200000
#define LCMAP_HALFWIDTH           0x00400000
#define LCMAP_FULLWIDTH           0x00800000

#define LCMAP_LINGUISTIC_CASING   0x01000000

#define SORTIDFROMLCID(lcid)   ((WORD  )((((DWORD)(lcid)) >> 16) & 0xf))

#define LOCALE_SCOUNTRY           0x00000006
#define LOCALE_SENGCOUNTRY        0x00001002

#define LOCALE_SLANGUAGE          0x00000002
#define LOCALE_SENGLANGUAGE       0x00001001

#define LOCALE_SDATE              0x0000001D
#define LOCALE_STIME              0x0000001E

#define CSTR_LESS_THAN            1
#define CSTR_EQUAL                2
#define CSTR_GREATER_THAN         3

#define NORM_IGNORENONSPACE       0x00000002

#define WC_COMPOSITECHECK         0x00000000 // NOTE: diff from winnls.h

/******************* shlwapi ************************************/

// note: diff in NULL handing and calling convetion
#define StrCpyW                 wcscpy
#define StrCpyNW                lstrcpynW // note: can't be wcsncpy!
#define StrCatW                 wcscat
#define StrChrW                 (WCHAR*)wcschr
#define StrCmpW                 wcscmp
#define StrCmpIW                _wcsicmp
#define StrCmpNW                wcsncmp
#define StrCmpNIW               _wcsnicmp

STDAPI_(LPWSTR) StrNCatW(LPWSTR lpFront, LPCWSTR lpBack, int cchMax);
STDAPI_(int) StrToIntW(LPCWSTR lpSrc);
STDAPI_(LPWSTR) StrStrIW(LPCWSTR lpFirst, LPCWSTR lpSrch);
STDAPI_(LPWSTR) StrRChrW(LPCWSTR lpStart, LPCWSTR lpEnd, WCHAR wMatch);
STDAPI_(LPWSTR) StrCatBuffW(LPWSTR pszDest, LPCWSTR pszSrc, int cchDestBuffSize);

#define lstrcmpW                wcscmp
#define lstrcmpiW               _wcsicmp
#define wnsprintfW              _snwprintf // note: not 100% compatible (wsprintf should be subset of sprintf...)
#define wvnsprintfW             _vsnwprintf // note: not 100% compatible (wsprintf should be subset of sprintf...)

#ifdef UNICODE
#define StrCpy                  StrCpyW
#define StrCpyN                 StrCpyNW
#define StrCat                  StrCatW
#define StrNCat                 StrNCatW
#define StrChr                  StrChrW
#define StrCmp                  StrCmpW
#define StrCmpN                 StrCmpNW
#define StrCmpI                 StrCmpIW
#define StrCmpNI                StrCmpNIW

#define StrToInt                StrToIntW
#define StrStrI                 StrStrIW
#define StrRChr                 StrRChrW
#define StrCatBuff              StrCatBuffW

#define lstrcmp                 lstrcmpW
#define lstrcmpi                lstrcmpiW
#define wnsprintf               wnsprintfW
#endif


#ifdef __cplusplus
/*
  Safe CRT functions are not available (yet) on all platforms, so we use our own implementations from safecrt.h.
*/
#define _CRT_ALTERNATIVE_INLINES
#define _SAFECRT_NO_INCLUDES 1
#define _SAFECRT_USE_INLINES 1
#define _SAFECRT_SET_ERRNO 0
#define _SAFECRT_DEFINE_MBS_FUNCTIONS 0
/*
#define _SAFECRT__ISMBBLEAD(_Character) 0
#define _SAFECRT__MBSDEC(_String, _Current) (_Current - 1)
*/
#include "safecrt.h"
#include "specstrings.h"

/*
The wrappers below are simple implementations that may not be as robust as complete functions in the Secure CRT library.
Remember to fix the errcode defintion in safecrt.h.
*/

#define _wcslwr_s _wcslwr_unsafe
#define _snwprintf_s _snwprintf_unsafe
#define _vsnwprintf_s _vsnwprintf_unsafe
#define _snprintf_s _snprintf_unsafe
#define _vsnprintf_s _vsnprintf_unsafe
#define swscanf_s _swscanf_unsafe
#define sscanf_s _sscanf_unsafe

#define _wfopen_s _wfopen_unsafe
#define fopen_s _fopen_unsafe

#define _strlwr_s _strlwr_unsafe

#define _vscprintf _vscprintf_unsafe
#define _vscwprintf _vscwprintf_unsafe

#define sprintf_s _snprintf
#define swprintf_s _snwprintf
#define vsprintf_s _vsnprintf
#define vswprintf_s _vsnwprintf

extern "C++" {

inline errno_t __cdecl _wcslwr_unsafe(wchar_t *str, size_t sz)
{
    wchar_t *copy = (wchar_t *)malloc(sz * sizeof(wchar_t));
    if(copy == NULL)
        return 1;

    errno_t retCode = wcscpy_s(copy, sz, str);
    if(retCode) {
        free(copy);
        return 1;
    }

    _wcslwr(copy);
    wcscpy_s(str, sz, copy);
    free(copy);
	
    return 0;
}
inline errno_t __cdecl _strlwr_unsafe(char *str, size_t sz)
{
    char *copy = (char *)malloc(sz);
    if(copy == NULL)
        return 1;

    errno_t retCode = strcpy_s(copy, sz, str);
    if(retCode) {
        free(copy);
        return 1;
    }

    _strlwr(copy);
    strcpy_s(str, sz, copy);
    free(copy);
	
    return 0;
}

inline int __cdecl _vscprintf_unsafe(const char *_Format, va_list _ArgList)
{
    int guess = 10;

    for (;;)
    {
        char *buf = (char *)malloc(guess * sizeof(char));
        if(buf == NULL)
            return 0;

        int ret = _vsnprintf(buf, guess, _Format, _ArgList);
        free(buf);

        if ((ret != -1) && (ret < guess))
            return ret;

        guess *= 2;
    }
}

inline int __cdecl _vscwprintf_unsafe(const wchar_t *_Format, va_list _ArgList)
{
    int guess = 10;

    for (;;)
    {
        wchar_t *buf = (wchar_t *)malloc(guess * sizeof(wchar_t));
        if(buf == NULL)
            return 0;

        int ret = _vsnwprintf(buf, guess, _Format, _ArgList);
        free(buf);

        if ((ret != -1) && (ret < guess))
            return ret;

        guess *= 2;
    }
}

inline int __cdecl _vsnwprintf_unsafe(wchar_t *_Dst, size_t _SizeInWords, size_t _Count, const wchar_t *_Format, va_list _ArgList)
{
    if (_Count == _TRUNCATE) _Count = _SizeInWords - 1;
    int ret = _vsnwprintf(_Dst, _Count, _Format, _ArgList);
    _Dst[_SizeInWords - 1] = L'\0';
    if (ret < 0 && errno == 0)
    {
        errno = ERANGE;
    }
    return ret;
}

inline int __cdecl _snwprintf_unsafe(wchar_t *_Dst, size_t _SizeInWords, size_t _Count, const wchar_t *_Format, ...)
{
    va_list _ArgList;
    va_start(_ArgList, _Format);
    return _vsnwprintf_unsafe(_Dst, _SizeInWords, _Count, _Format, _ArgList);
}

inline int __cdecl _vsnprintf_unsafe(char *_Dst, size_t _SizeInWords, size_t _Count, const char *_Format, va_list _ArgList)
{
    if (_Count == _TRUNCATE) _Count = _SizeInWords - 1;
    int ret = _vsnprintf(_Dst, _Count, _Format, _ArgList);
    _Dst[_SizeInWords - 1] = L'\0';
    if (ret < 0 && errno == 0)
    {
        errno = ERANGE;
    }
    return ret;
}

inline int __cdecl _snprintf_unsafe(char *_Dst, size_t _SizeInWords, size_t _Count, const char *_Format, ...)
{
    va_list _ArgList;
    va_start(_ArgList, _Format);
    return _vsnprintf_unsafe(_Dst, _SizeInWords, _Count, _Format, _ArgList);
}

inline int __cdecl _swscanf_unsafe(const wchar_t *_Dst, const wchar_t *_Format,...)
{
    va_list _ArgList;
    va_start(_ArgList, _Format);
    wchar_t *tempFormat;

    tempFormat = (wchar_t*) _Format;
    
    while (*tempFormat != L'\0') {

        if (*tempFormat == L'%') {
            
            //
            // If scanf takes parameters other than numbers, return error.
            //
            
            if (! ((*(tempFormat+1)==L'x') || (*(tempFormat+1)==L'd') ||
                   (*(tempFormat+1)==L'X') || (*(tempFormat+1)==L'D')) ) {
                
                _ASSERTE(FALSE);
                return -1;    
                
            } 
        }
           
        tempFormat++;
       }

    return swscanf(_Dst, _Format, _ArgList);
}

inline int __cdecl _sscanf_unsafe(const char *_Dst, const char *_Format,...)
{
    char *tempFormat;

    va_list _ArgList;
    va_start(_ArgList, _Format);

    tempFormat = (char*) _Format;

    while (*tempFormat != '\0') {
        
        if (*tempFormat == '%') {
            
            //
            // If scanf takes parameters other than numbers, return error.
            //
            
            if (! ((*(tempFormat+1)=='x') || (*(tempFormat+1)=='d') ||
                   (*(tempFormat+1)=='X') || (*(tempFormat+1)=='D')) ) {

                _ASSERTE(FALSE);
                return -1;    

            } 
        }
        
        tempFormat++;
    }


    return sscanf(_Dst, _Format, _ArgList);
}

inline errno_t __cdecl _wfopen_unsafe(FILE * *ff, const wchar_t *fileName, const wchar_t *mode)
{
    FILE *result = _wfopen(fileName, mode);
    if(result == 0) {
        return 1;
    } else {
        *ff = result;
        return 0;
    }
}

inline errno_t __cdecl _fopen_unsafe(FILE * *ff, const char *fileName, const char *mode)
{
  FILE *result = fopen(fileName, mode);
  if(result == 0) {
    return 1;
  } else {
    *ff = result;
    return 0;
  }
}

/* _itow_s */
_SAFECRT__EXTERN_C
errno_t __cdecl _itow_s(int _Value, wchar_t *_Dst, size_t _SizeInWords, int _Radix);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl _itow_s(int _Value, wchar_t (&_Dst)[_SizeInWords], int _Radix)
{
    return _itow_s(_Value, _Dst, _SizeInWords, _Radix);
}
#endif

#if _SAFECRT_USE_INLINES

__inline
errno_t __cdecl _itow_s(int _Value, wchar_t *_Dst, size_t _SizeInWords, int _Radix)
{
    /* validation section */
    _SAFECRT__VALIDATE_STRING(_Dst, _SizeInWords);

    /* TODO: do not write past buffer size */
    _itow(_Value, _Dst, _Radix);
    return 0;
}

#endif

/* _i64tow_s */
_SAFECRT__EXTERN_C
errno_t __cdecl _i64tow_s(__int64 _Value, wchar_t *_Dst, size_t _SizeInWords, int _Radix);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl _i64tow_s(__int64 _Value, wchar_t (&_Dst)[_SizeInWords], int _Radix)
{
    return _i64tow_s(_Value, _Dst, _SizeInWords, _Radix);
}
#endif

#if _SAFECRT_USE_INLINES

__inline
errno_t __cdecl _i64tow_s(__int64 _Value, wchar_t *_Dst, size_t _SizeInWords, int _Radix)
{
    /* validation section */
    _SAFECRT__VALIDATE_STRING(_Dst, _SizeInWords);

    /* TODO: do not write past buffer size */
    _i64tow(_Value, _Dst, _Radix);
    return 0;
}

#endif

/* getenv_s */
/*
 * _ReturnValue indicates if the variable has been found and size needed
 */
_SAFECRT__EXTERN_C
errno_t __cdecl getenv_s(size_t *_ReturnValue, char *_Dst, size_t _SizeInWords, const char *_Name);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl getenv_s(size_t *_ReturnValue, char *_Dst, size_t _SizeInWords, const char *_Name)
{
    return getenv_s(_ReturnValue, _Dst, _SizeInWords, _Name);
}
#endif

#if _SAFECRT_USE_INLINES

__inline
errno_t __cdecl getenv_s(size_t *_ReturnValue, char *_Dst, size_t _SizeInWords, const char *_Name)
{
    char *szFound;

    /* validation section */
    _SAFECRT__VALIDATE_STRING(_Dst, _SizeInWords);

    szFound = getenv(_Name);
    if (szFound == NULL)
    {
        *_ReturnValue = 0;
        return 0;
    }
    *_ReturnValue = strlen(szFound) + 1;
    return strcpy_s(_Dst, _SizeInWords, szFound);
}

#endif
 
}
#endif /* __cplusplus */


STDAPI_(BOOL) PathAppendW(LPWSTR pszPath, LPCWSTR pszMore);
STDAPI_(int) PathCommonPrefixW(LPCWSTR pszFile1, LPCWSTR pszFile2, LPWSTR  pszPath);
STDAPI_(LPWSTR) PathFindFileNameW(LPCWSTR pPath);
STDAPI_(LPWSTR) PathFindExtensionW(LPCWSTR pszPath);
STDAPI_(int) PathGetDriveNumberW(LPCWSTR lpsz);
STDAPI_(BOOL) PathIsRelativeW(LPCWSTR lpszPath);
STDAPI_(BOOL) PathIsUNCW(LPCWSTR pszPath);
STDAPI_(LPWSTR) PathAddBackslashW(LPWSTR lpszPath);
STDAPI_(LPWSTR) PathRemoveBackslashW(LPWSTR lpszPath);
STDAPI_(void) PathRemoveExtensionW(LPWSTR pszPath);
STDAPI_(LPWSTR) PathCombineW(LPWSTR lpszDest, LPCWSTR lpszDir, LPCWSTR lpszFile);
STDAPI_(BOOL) PathCanonicalizeW(LPWSTR lpszDst, LPCWSTR lpszSrc);
STDAPI_(BOOL) PathRelativePathToW(LPWSTR pszPath, LPCWSTR pszFrom, DWORD dwAttrFrom, LPCWSTR pszTo, DWORD dwAttrTo);
STDAPI_(BOOL) PathRenameExtensionW(LPWSTR pszPath, LPCWSTR pszExt);
STDAPI_(BOOL) PathRemoveFileSpecW(LPWSTR pFile);
STDAPI_(void) PathStripPathW (LPWSTR pszPath);

STDAPI PathCreateFromUrlW(LPCWSTR pszUrl, LPWSTR pszPath, LPDWORD pcchPath, DWORD dwFlags);
STDAPI_(BOOL) PathIsURLW(LPCWSTR pszPath);


#define URL_UNESCAPE                    0x10000000
#define URL_ESCAPE_PERCENT              0x00001000

typedef enum {
    URLIS_FILEURL = 3,
} URLIS;

typedef enum {
    URL_PART_SCHEME     = 1,
    URL_PART_HOSTNAME   = 2,
} URL_PART;

STDAPI UrlCanonicalizeW(LPCWSTR pszUrl, LPWSTR pszCanonicalized, LPDWORD pcchCanonicalized, DWORD dwFlags);
STDAPI UrlCombineW(LPCWSTR pszBase, LPCWSTR pszRelative, LPWSTR pszCombined, LPDWORD pcchCombined, DWORD dwFlags);
STDAPI UrlEscapeW(LPCWSTR pszUrl, LPWSTR pszEscaped, LPDWORD pcchEscaped, DWORD dwFlags);
STDAPI UrlUnescapeW(LPWSTR pszURL, LPWSTR pszUnescaped, LPDWORD pcchUnescaped, DWORD dwFlags);
STDAPI_(BOOL) UrlIsW(LPCWSTR pszUrl, URLIS dwUrlIs);
STDAPI UrlGetPartW(LPCWSTR pszIn, LPWSTR pszOut, LPDWORD pcchOut, DWORD dwPart, DWORD dwFlags);

#ifdef UNICODE
#define PathAppend          PathAppendW
#define PathCommonPrefix    PathCommonPrefixW
#define PathFindFileName    PathFindFileNameW
#define PathIsRelative      PathIsRelativeW
#define PathGetDriveNumber  PathGetDriveNumberW
#define PathIsUNC           PathIsUNCW
#define PathAddBackslash    PathAddBackslashW
#define PathRemoveBackslash PathRemoveBackslashW
#define PathRemoveExtension PathRemoveExtensionW
#define PathCombine         PathCombineW
#define PathSkipRoot        PathSkipRootW
#define PathFindExtension   PathFindExtensionW
#define PathCanonicalize    PathCanonicalizeW
#define PathRelativePathTo  PathRelativePathToW
#define PathRemoveFileSpec  PathRemoveFileSpecW
#define PathRenameExtension PathRenameExtensionW
#define PathStripPath       PathStripPathW

#define PathCreateFromUrl   PathCreateFromUrlW
#define PathIsURL           PathIsURLW

#define UrlCanonicalize     UrlCanonicalizeW
#define UrlCombine          UrlCombineW
#define UrlEscape           UrlEscapeW
#define UrlUnescape         UrlUnescapeW 
#define UrlIs               UrlIsW
#define UrlGetPart          UrlGetPartW

#endif // UNICODE

/******************* misc ***************************************/

#define PF_COMPARE_EXCHANGE_DOUBLE          2

typedef VOID (NTAPI * WAITORTIMERCALLBACKFUNC) (PVOID, BOOLEAN );

typedef HANDLE HWND;

#define IS_TEXT_UNICODE_SIGNATURE             0x0008
#define IS_TEXT_UNICODE_UNICODE_MASK          0x000F

#define FILE_READ_DATA                        0x0001

typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

typedef VOID (__stdcall *WAITORTIMERCALLBACK)(PVOID, BOOL);


/******************* SAFEARRAY ************************/

#define	FADF_VARIANT	( 0x800 )

typedef struct tagSAFEARRAYBOUND
    {
    ULONG cElements;
    LONG lLbound;
    } 	SAFEARRAYBOUND;

typedef struct tagSAFEARRAYBOUND *LPSAFEARRAYBOUND;

typedef struct tagSAFEARRAY
    {
    USHORT cDims;
    USHORT fFeatures;
    ULONG cbElements;
    ULONG cLocks;
    PVOID pvData;
    SAFEARRAYBOUND rgsabound[ 1 ];
    } 	SAFEARRAY;

typedef SAFEARRAY *LPSAFEARRAY;


STDAPI_(SAFEARRAY *) SafeArrayCreateVector(VARTYPE vt, LONG lLbound, ULONG cElements);
STDAPI_(UINT) SafeArrayGetDim(SAFEARRAY * psa);
STDAPI SafeArrayGetElement(SAFEARRAY * psa, LONG * rgIndices, void * pv);
STDAPI SafeArrayGetLBound(SAFEARRAY * psa, UINT nDim, LONG * plLbound);
STDAPI SafeArrayGetUBound(SAFEARRAY * psa, UINT nDim, LONG * plUbound);
STDAPI SafeArrayGetVartype(SAFEARRAY * psa, VARTYPE * pvt);
STDAPI SafeArrayPutElement(SAFEARRAY * psa, LONG * rgIndices, void * pv);
STDAPI SafeArrayDestroy(SAFEARRAY * psa);

EXTERN_C void * _stdcall _lfind(const void *, const void *, unsigned int *, unsigned int,
        int (__cdecl *)(const void *, const void *));


/*                                                                     */


interface IDispatch;
interface ITypeInfo;
interface ITypeLib;
interface IMoniker;

typedef VOID (WINAPI *LPOVERLAPPED_COMPLETION_ROUTINE)( 
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPVOID lpOverlapped);
typedef int DEBUG_EVENT;
typedef DEBUG_EVENT *LPDEBUG_EVENT;


#define _EXCEPTION_REGISTRATION_RECORD _PAL_EXCEPTION_REGISTRATION
#define EXCEPTION_REGISTRATION_RECORD PAL_EXCEPTION_REGISTRATION
#define PEXCEPTION_REGISTRATION_RECORD PPAL_EXCEPTION_REGISTRATION

#define PEXCEPTION_ROUTINE PFN_PAL_EXCEPTION_FILTER

#define EXCEPTION_DISPOSITION LONG

#define DISPATCHER_CONTEXT    LPVOID

#define ExceptionContinueSearch     EXCEPTION_CONTINUE_SEARCH
#define ExceptionStackUnwind        EXCEPTION_EXECUTE_HANDLER
#define ExceptionContinueExecution  EXCEPTION_CONTINUE_EXECUTION

#define EXCEPTION_CHAIN_END ((PEXCEPTION_REGISTRATION_RECORD) NULL)


typedef LPVOID HKEY;
typedef LPVOID PACL;
typedef LPVOID LPBC;
typedef LPVOID PSECURITY_DESCRIPTOR;

typedef struct _EXCEPTION_RECORD64 {
    DWORD ExceptionCode;
    ULONG ExceptionFlags;
    ULONG64 ExceptionRecord;
    ULONG64 ExceptionAddress;
    ULONG NumberParameters;
    ULONG __unusedAlignment;
    ULONG64 ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD64, *PEXCEPTION_RECORD64;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _STRING64 {
    USHORT   Length;
    USHORT   MaximumLength;
    ULONGLONG  Buffer;
} STRING64;
typedef STRING64 *PSTRING64;

typedef STRING64 UNICODE_STRING64;
typedef UNICODE_STRING64 *PUNICODE_STRING64;

typedef STRING64 ANSI_STRING64;
typedef ANSI_STRING64 *PANSI_STRING64;

#define UNICODE_NULL ((WCHAR)0) // winnt

/******************** Lists *************************/

//
// These are needed for portable debugger support.
//

typedef struct LIST_ENTRY32 {
    ULONG Flink;
    ULONG Blink;
} LIST_ENTRY32;
typedef LIST_ENTRY32 *PLIST_ENTRY32;

typedef struct LIST_ENTRY64 {
    ULONGLONG Flink;
    ULONGLONG Blink;
} LIST_ENTRY64;
typedef LIST_ENTRY64 *PLIST_ENTRY64;

/******************** PAL RT APIs *******************************/

typedef struct _HSATELLITE *HSATELLITE;

EXTERN_C HSATELLITE PALAPI PAL_LoadSatelliteResourceW(LPCWSTR SatelliteResourceFileName);
EXTERN_C HSATELLITE PALAPI PAL_LoadSatelliteResourceA(LPCSTR SatelliteResourceFileName);
EXTERN_C BOOL PALAPI PAL_FreeSatelliteResource(HSATELLITE SatelliteResource);
EXTERN_C UINT PALAPI PAL_LoadSatelliteStringW(HSATELLITE SatelliteResource,
             UINT uID,
             LPWSTR lpBuffer,
             UINT nBufferMax);
EXTERN_C UINT PALAPI PAL_LoadSatelliteStringA(HSATELLITE SatelliteResource,
             UINT uID,
             LPSTR lpBuffer,
             UINT nBufferMax);

EXTERN_C HRESULT PALAPI PAL_CoCreateInstance(REFCLSID   rclsid,
                             REFIID     riid,
                             void     **ppv);

// So we can have CoCreateInstance in most of the code base, 
// instead of spreading around of if'def FEATURE_PALs for PAL_CoCreateInstance.
#define CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv) PAL_CoCreateInstance(rclsid, riid, ppv)

EXTERN_C BOOL PALAPI PAL_GetMachineConfigurationDirectoryW(
                      OUT LPWSTR lpDirectoryName,
                      IN UINT cbDirectoryName);

#ifdef UNICODE
#define PAL_GetMachineConfigurationDirectory PAL_GetMachineConfigurationDirectoryW
#endif

typedef struct _HINI *HINI;

EXTERN_C HINI PALAPI PAL_IniCreate();
EXTERN_C BOOL PALAPI PAL_IniClose(HINI hIni);
EXTERN_C BOOL PALAPI PAL_IniLoad(HINI hIni, 
             LPCWSTR pszFileName);
EXTERN_C BOOL PALAPI PAL_IniSave(HINI hIni, 
             LPCWSTR pszFileName, 
             BOOL fForce);
EXTERN_C DWORD PALAPI PAL_IniReadString(HINI hIni, 
             LPCWSTR pszSection, 
             LPCWSTR pszKey, 
             LPWSTR pszValue, 
             DWORD dwSize);
EXTERN_C BOOL PALAPI PAL_IniWriteString(HINI hIni, 
            LPCWSTR pszSection, 
            LPCWSTR pszKey, 
            LPCWSTR pszValue);
EXTERN_C BOOL PALAPI PAL_IniGetString(HINI hIni, 
             LPCWSTR pszSection, 
             LPCWSTR pszKey, 
             LPWSTR pszValue, 
             DWORD dwSize);

EXTERN_C
BOOL
PALAPI
PAL_FetchConfigurationStringW(
                    IN BOOL bPerMachine,
                    IN LPCWSTR lpParameterName,
                    OUT LPWSTR lpParameterValue,
                    IN UINT cbParameterValueLength);
                    
EXTERN_C
BOOL
PALAPI
PAL_SetConfigurationStringW(
                    IN BOOL bPerMachine,
                    IN LPCWSTR lpParameterName,
                    IN LPCWSTR lpParameterValue);

#ifdef UNICODE
#define PAL_FetchConfigurationString PAL_FetchConfigurationStringW
#define PAL_SetConfigurationString PAL_SetConfigurationStringW
#endif

/************** Byte swapping & unaligned access ******************/

#include <pal_endian.h>

/******************** external includes *************************/

#include <ntimage.h>

// ROTORTODO: Need a big-endian version of IMAGE_FILE_MACHINE_POWERPC
#define IMAGE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian

// ROTOROTODO: IMAGE_FILE_MACHINE_SPARC - just a made up value for now.
#define IMAGE_FILE_MACHINE_SPARC             0x0800 // SPARC

#include <atlcom.h>
#include <atlstr.h>

#include "sscli_version.h"

#endif // RC_INVOKED

#endif // __ROTOR_PALRT_H__
