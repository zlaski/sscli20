/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


--*/

////////////////////////////////////////////////////////////////////////
// Extensions to the usual posix header files
////////////////////////////////////////////////////////////////////////

#ifndef __PAL_MSTYPES_H__
#define __PAL_MSTYPES_H__

#ifdef  __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////
// calling convention stuff
////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif // __cplusplus

#ifndef _MSC_VER

// Note:  Win32-hosted GCC predefines __stdcall and __cdecl, but Unix-
// hosted GCC does not.

#ifdef __i386__

#if !defined(__stdcall)
#define __stdcall      __attribute__((stdcall))
#endif
#if !defined(_stdcall)
#define _stdcall       __stdcall
#endif

#if !defined(__cdecl)
#define __cdecl        __attribute__((cdecl))
#endif
#if !defined(_cdecl)
#define _cdecl         __cdecl
#endif
#if !defined(CDECL)
#define CDECL          __cdecl
#endif

#undef __fastcall
#define __fastcall      __stdcall
#undef _fastcall
#define _fastcall       __fastcall

#else   // !defined(__i386__)

#define __stdcall
#define _stdcall
#define __cdecl
#define _cdecl
#define CDECL
#define __fastcall
#define _fastcall

#endif  // !defined(__i386__)

#if !defined(__declspec)
#define __declspec(e) __attribute__((e))
#endif
#if !defined(_declspec)
#define _declspec(e)  __declspec(e)
#endif


#define __forceinline   inline

#endif // !_MSC_VER

#ifdef _MSC_VER

#if defined(PAL_IMPLEMENTATION) || defined(_MT)
#define PALIMPORT
#else
#define PALIMPORT   __declspec(dllimport)
#endif
#define PAL_NORETURN __declspec(noreturn)

#else

#define PALIMPORT
#define PAL_NORETURN    __attribute__((noreturn))

#endif

#define PALAPI      __stdcall
#define PALAPIV     __cdecl

////////////////////////////////////////////////////////////////////////
// Type attribute stuff
////////////////////////////////////////////////////////////////////////

#define CONST const
#define IN
#define OUT
#define OPTIONAL
#define FAR

#ifdef UNICODE
#define __TEXT(x) L##x
#else
#define __TEXT(x) x
#endif
#define TEXT(x) __TEXT(x)

////////////////////////////////////////////////////////////////////////
// Some special values
////////////////////////////////////////////////////////////////////////

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

////////////////////////////////////////////////////////////////////////
// Misc. type helpers
////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

// MSVC's way of declaring large integer constants
// If you define these in one step, without the _HELPER macros, you
// get extra whitespace when composing these with other concatenating macros.
#define I64_HELPER(x) x ## i64
#define I64(x)        I64_HELPER(x)

#define UI64_HELPER(x) x ## ui64
#define UI64(x)        UI64_HELPER(x)

#else // _MSC_VER

// GCC's way of declaring large integer constants
// If you define these in one step, without the _HELPER macros, you
// get extra whitespace when composing these with other concatenating macros.
#define I64_HELPER(x) x ## LL
#define I64(x)        I64_HELPER(x)

#define UI64_HELPER(x) x ## ULL
#define UI64(x)        UI64_HELPER(x)

#endif // _MSC_VER

////////////////////////////////////////////////////////////////////////
// Misc. types
////////////////////////////////////////////////////////////////////////

#ifndef _MSC_VER

// A bunch of source files (e.g. most of the ndp tree) include rotor_pal.h
// but are written to be LLP64, not LP64.  (LP64 => long = 64 bits
// LLP64 => longs = 32 bits, long long = 64 bits)
//
// To handle this difference, we #define long to be int (and thus 32 bits) when
// compiling those files.  (See the bottom of this file or search for 
// #define long to see where we do this.)  
//
// But this fix is more complicated than it seems, because we also use the 
// preprocessor to #define __int64 to long for LP64 architectures (__int64
// isn't a builtin in gcc).   We don't want __int64 to be an int (by cascading 
// macro rules).  So we play this little trick below where we add 
// __cppmungestrip before "long", which is what we're really #defining __int64 
// to.  The preprocessor sees __cppmungestriplong as something different than 
// long, so it doesn't replace it with int.  The during the cppmunge phase, we 
// remove the __cppmungestrip part, leaving long for the compiler to see.
//
// Note that we can't just use a typedef to define __int64 as long before 
// #defining long because typedefed types can't be signedness-agnostic (i.e. 
// they must be either signed or unsigned) and we want to be able to use 
// __int64 as though it were intrinsic

#define __int64     long long

#define __int32     int
#define __int16     short int
#define __int8      char        // assumes char is signed

#endif // _MSC_VER

#ifndef _MSC_VER


#endif // _MSC_VER

typedef void VOID;

#ifndef PLATFORM_UNIX
typedef long LONG;
typedef unsigned long ULONG;
#else
typedef int LONG;       // NOTE: diff from windows.h, for LP64 compat
typedef unsigned int ULONG; // NOTE: diff from windows.h, for LP64 compat
#endif

typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
typedef ULONGLONG DWORD64;
typedef DWORD64 *PDWORD64;
typedef LONGLONG *PLONG64;
typedef ULONGLONG *PULONG64;
typedef ULONG *PULONG;
typedef short SHORT;
typedef SHORT *PSHORT;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;
typedef ULONGLONG DWORDLONG;

#ifndef PLATFORM_UNIX
typedef unsigned long DWORD;
#else
typedef unsigned int DWORD; // NOTE: diff from  windows.h, for LP64 compat
#endif

typedef unsigned int DWORD32, *PDWORD32;

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;
typedef double DOUBLE;
typedef BOOL *PBOOL;
typedef BOOL *LPBOOL;
typedef BYTE *PBYTE;
typedef BYTE *LPBYTE;
typedef const BYTE *LPCBYTE;
typedef int *PINT;
typedef int *LPINT;
typedef WORD *PWORD;
typedef WORD *LPWORD;
typedef LONG *LPLONG;
typedef LPLONG PLONG;
typedef DWORD *PDWORD;
typedef DWORD *LPDWORD;
typedef void *PVOID;
typedef void *LPVOID;
typedef CONST void *LPCVOID;
typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;
typedef BYTE BOOLEAN;
typedef BOOLEAN *PBOOLEAN;

typedef unsigned __int8 UINT8;
typedef signed __int8 INT8;
typedef unsigned __int16 UINT16;
typedef signed __int16 INT16;
typedef unsigned __int32 UINT32, *PUINT32;
typedef signed __int32 INT32, *PINT32;
typedef unsigned __int64 UINT64, *PUINT64;
typedef signed __int64 INT64, *PINT64;

typedef unsigned __int32 ULONG32, *PULONG32;
typedef signed __int32 LONG32, *PLONG32;
typedef unsigned __int64 ULONG64;
typedef signed __int64 LONG64;

#if defined(_X86_) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif


typedef _W64 __int32 INT_PTR;
typedef _W64 unsigned __int32 UINT_PTR;

#ifndef _MINGW_

typedef _W64 __int32 LONG_PTR;
typedef _W64 unsigned __int32 ULONG_PTR, *PULONG_PTR;
typedef _W64 unsigned __int32 DWORD_PTR, *PDWORD_PTR;

#else // for MINGW build

typedef int LONG_PTR;
typedef unsigned int ULONG_PTR, *PULONG_PTR;
typedef unsigned int DWORD_PTR, *PDWORD_PTR;

#endif

#define __int3264   __int32

#define HandleToULong( h ) ((ULONG)(ULONG_PTR)(h) )
#define HandleToLong( h )  ((LONG)(LONG_PTR) (h) )
#define ULongToHandle( ul ) ((HANDLE)(ULONG_PTR) (ul) )
#define LongToHandle( h )   ((HANDLE)(LONG_PTR) (h) )
#define PtrToUlong( p ) ((ULONG)(ULONG_PTR) (p) )
#define PtrToLong( p )  ((LONG)(LONG_PTR) (p) )
#define PtrToUint( p ) ((UINT)(UINT_PTR) (p) )
#define PtrToInt( p )  ((INT)(INT_PTR) (p) )
#define PtrToUshort( p ) ((unsigned short)(ULONG_PTR)(p) )
#define PtrToShort( p )  ((short)(LONG_PTR)(p) )
#define IntToPtr( i )    ((VOID *)(INT_PTR)((int)i))
#define UIntToPtr( ui )  ((VOID *)(UINT_PTR)((unsigned int)ui))
#define LongToPtr( l )   ((VOID *)(LONG_PTR)((long)l))
#define ULongToPtr( ul ) ((VOID *)(ULONG_PTR)((unsigned long)ul))
#define ShortToPtr( s )  ((VOID *)(INT_PTR)((short)s))
#define UShortToPtr( us )  ((VOID *)(UINT_PTR)((unsigned short)s))


#define HandleToUlong(h)  HandleToULong(h)
#define UlongToHandle(ul) ULongToHandle(ul)
#define UlongToPtr(ul) ULongToPtr(ul)
#define UintToPtr(ui)  UIntToPtr(ui)

typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;

#if defined(__APPLE_CC__) || defined(_AIX) || defined(__hppa__)
typedef unsigned long size_t;
#else
typedef ULONG_PTR size_t;
#endif
#define _SIZE_T_DEFINED

typedef LONG_PTR ptrdiff_t;
#define _PTRDIFF_T_DEFINED
#ifdef _MINGW_
// We need to define _PTRDIFF_T to make sure ptrdiff_t doesn't get defined
// again by system headers - but only for MinGW.
#define _PTRDIFF_T
#endif

typedef INT_PTR intptr_t;
#define _INTPTR_T_DEFINED

typedef UINT_PTR uintptr_t;
#define _UINTPTR_T_DEFINED

typedef DWORD LCID;
typedef PDWORD PLCID;
typedef WORD LANGID;
typedef DWORD LCTYPE;

typedef wchar_t WCHAR;
typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR *LPWSTR, *PWSTR;

typedef CONST WCHAR *LPCWSTR, *PCWSTR;

typedef char CHAR;
typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;
typedef CONST CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;

#ifdef UNICODE
typedef WCHAR TCHAR;
typedef WCHAR _TCHAR;
#else
typedef CHAR TCHAR;
typedef CHAR _TCHAR;
#endif
typedef TCHAR *PTCHAR;
typedef TCHAR *LPTSTR, *PTSTR;
typedef CONST TCHAR *LPCTSTR;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))

typedef VOID *HANDLE;
typedef struct __PAL_RemoteHandle__ { HANDLE h; } *RHANDLE;
typedef HANDLE *PHANDLE;
typedef HANDLE *LPHANDLE;
#define INVALID_HANDLE_VALUE ((VOID *)(-1))
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_FILE_ATTRIBUTES ((DWORD) -1)
typedef HANDLE HMODULE;
typedef HANDLE HINSTANCE;
typedef HANDLE HGLOBAL;
typedef HANDLE HLOCAL;

typedef LONG HRESULT;
typedef LONG NTSTATUS;

typedef union _LARGE_INTEGER {
    struct {
#if BIGENDIAN
        LONG HighPart;
        DWORD LowPart;
#else
        DWORD LowPart;
        LONG HighPart;
#endif
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef struct _GUID {
    ULONG   Data1;    // NOTE: diff from Win32, for LP64
    USHORT  Data2;
    USHORT  Data3;
    UCHAR   Data4[ 8 ];
} GUID;
#define GUID_DEFINED

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

/* Code Page Default Values */
#define CP_ACP          0   /* default to ANSI code page */
#define CP_OEMCP        1   /* default to OEM code page */
#define CP_MACCP        2   /* default to MAC code page */
#define CP_THREAD_ACP   3   /* current thread's ANSI code page */
#define CP_WINUNICODE   1200
#define CP_UNICODE      1200 /* Unicode */
#define CP_UNICODESWAP  1201 /* Unicode Big-Endian */
#define CP_UTF7     65000   /* UTF-7 translation */
#define CP_UTF8     65001   /* UTF-8 translation */

#ifdef  __cplusplus
}
#endif

#endif // __PAL_MSTYPES_H__
