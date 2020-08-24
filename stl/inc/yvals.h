// yvals.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _YVALS
#define _YVALS
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <crtdbg.h>
#include <crtdefs.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#if defined(MRTDLL) && defined(_CRTBLD)
// process-global is the default for code built with /clr or /clr:oldSyntax.
// appdomain-global is the default for code built with /clr:pure.
// Code in MSVCM is built with /clr, but is used by user code built with /clr:pure
// so it must conform to the expectations of /clr:pure clients.
// Use __PURE_APPDOMAIN_GLOBAL when a global needs to be appdomain-global for pure
// clients and process-global for mixed clients.
#define __PURE_APPDOMAIN_GLOBAL __declspec(appdomain)
#else
#define __PURE_APPDOMAIN_GLOBAL
#endif

// CURRENT DLL NAMES
#ifndef _CRT_MSVCP_CURRENT
#ifdef _CRT_WINDOWS
// Windows
#ifdef _DEBUG
#define _CRT_MSVCP_CURRENT "msvcpd_win.dll"
#else
#define _CRT_MSVCP_CURRENT "msvcp_win.dll"
#endif
#else
// Visual Studio
#ifdef _DEBUG
#define _CRT_MSVCP_CURRENT "msvcp140d.dll"
#else
#define _CRT_MSVCP_CURRENT "msvcp140.dll"
#endif
#endif
#endif

#ifdef _ITERATOR_DEBUG_LEVEL // A. _ITERATOR_DEBUG_LEVEL is already defined.

// A1. Validate _ITERATOR_DEBUG_LEVEL.
#if _ITERATOR_DEBUG_LEVEL > 2 && defined(_DEBUG)
#error _ITERATOR_DEBUG_LEVEL > 2 is not supported in debug mode.
#elif _ITERATOR_DEBUG_LEVEL > 1 && !defined(_DEBUG)
#error _ITERATOR_DEBUG_LEVEL > 1 is not supported in release mode.
#endif

// A2. Inspect _HAS_ITERATOR_DEBUGGING.
#ifdef _HAS_ITERATOR_DEBUGGING // A2i. _HAS_ITERATOR_DEBUGGING is already defined, validate it.
#if _ITERATOR_DEBUG_LEVEL == 2 && _HAS_ITERATOR_DEBUGGING != 1
#error _ITERATOR_DEBUG_LEVEL == 2 must imply _HAS_ITERATOR_DEBUGGING == 1.
#elif _ITERATOR_DEBUG_LEVEL < 2 && _HAS_ITERATOR_DEBUGGING != 0
#error _ITERATOR_DEBUG_LEVEL < 2 must imply _HAS_ITERATOR_DEBUGGING == 0.
#endif
#else // A2ii. _HAS_ITERATOR_DEBUGGING is not yet defined, derive it.
#if _ITERATOR_DEBUG_LEVEL == 2
#define _HAS_ITERATOR_DEBUGGING 1
#else
#define _HAS_ITERATOR_DEBUGGING 0
#endif
#endif // _HAS_ITERATOR_DEBUGGING

// A3. Inspect _SECURE_SCL.
#ifdef _SECURE_SCL // A3i. _SECURE_SCL is already defined, validate it.
#if _ITERATOR_DEBUG_LEVEL > 0 && _SECURE_SCL != 1
#error _ITERATOR_DEBUG_LEVEL > 0 must imply _SECURE_SCL == 1.
#elif _ITERATOR_DEBUG_LEVEL == 0 && _SECURE_SCL != 0
#error _ITERATOR_DEBUG_LEVEL == 0 must imply _SECURE_SCL == 0.
#endif
#else // A3ii. _SECURE_SCL is not yet defined, derive it.
#if _ITERATOR_DEBUG_LEVEL > 0
#define _SECURE_SCL 1
#else
#define _SECURE_SCL 0
#endif
#endif // _SECURE_SCL

#else // B. _ITERATOR_DEBUG_LEVEL is not yet defined.

// B1. Inspect _HAS_ITERATOR_DEBUGGING.
#ifdef _HAS_ITERATOR_DEBUGGING // B1i. _HAS_ITERATOR_DEBUGGING is already defined, validate it.
#if _HAS_ITERATOR_DEBUGGING > 1
#error _HAS_ITERATOR_DEBUGGING must be either 0 or 1.
#elif _HAS_ITERATOR_DEBUGGING == 1 && !defined(_DEBUG)
#error _HAS_ITERATOR_DEBUGGING == 1 is not supported in release mode.
#endif
#else // B1ii. _HAS_ITERATOR_DEBUGGING is not yet defined, default it.
#ifdef _DEBUG
#define _HAS_ITERATOR_DEBUGGING 1
#else
#define _HAS_ITERATOR_DEBUGGING 0
#endif
#endif // _HAS_ITERATOR_DEBUGGING

// B2. Inspect _SECURE_SCL.
#ifdef _SECURE_SCL // B2i. _SECURE_SCL is already defined, validate it.
#if _SECURE_SCL > 1
#error _SECURE_SCL must be either 0 or 1.
#endif
#else // B2ii. _SECURE_SCL is not yet defined, default it.
#if _HAS_ITERATOR_DEBUGGING == 1
#define _SECURE_SCL 1
#else
#define _SECURE_SCL 0
#endif
#endif // _SECURE_SCL

// B3. Derive _ITERATOR_DEBUG_LEVEL.
#if _HAS_ITERATOR_DEBUGGING
#define _ITERATOR_DEBUG_LEVEL 2
#elif _SECURE_SCL
#define _ITERATOR_DEBUG_LEVEL 1
#else
#define _ITERATOR_DEBUG_LEVEL 0
#endif

#endif // _ITERATOR_DEBUG_LEVEL

#ifdef __cplusplus
#ifndef _ALLOW_MSC_VER_MISMATCH
#pragma detect_mismatch("_MSC_VER", "1900")
#endif // _ALLOW_MSC_VER_MISMATCH

#ifndef _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
#pragma detect_mismatch("_ITERATOR_DEBUG_LEVEL", _STRINGIZE(_ITERATOR_DEBUG_LEVEL))
#endif // _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH

#ifndef _ALLOW_RUNTIME_LIBRARY_MISMATCH
#if !defined(_DLL) && !defined(_DEBUG)
#pragma detect_mismatch("RuntimeLibrary", "MT_StaticRelease")
#elif !defined(_DLL) && defined(_DEBUG)
#pragma detect_mismatch("RuntimeLibrary", "MTd_StaticDebug")
#elif defined(_DLL) && !defined(_DEBUG)
#pragma detect_mismatch("RuntimeLibrary", "MD_DynamicRelease")
#elif defined(_DLL) && defined(_DEBUG)
#pragma detect_mismatch("RuntimeLibrary", "MDd_DynamicDebug")
#endif // defined(_DLL) etc.
#endif // _ALLOW_RUNTIME_LIBRARY_MISMATCH
#endif // __cplusplus

#ifndef _CONTAINER_DEBUG_LEVEL
#if _ITERATOR_DEBUG_LEVEL == 0
#define _CONTAINER_DEBUG_LEVEL 0
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 0 // _ITERATOR_DEBUG_LEVEL != 0 vvv
#define _CONTAINER_DEBUG_LEVEL 1
#endif // _ITERATOR_DEBUG_LEVEL == 0
#endif // _CONTAINER_DEBUG_LEVEL

#if _ITERATOR_DEBUG_LEVEL != 0 && _CONTAINER_DEBUG_LEVEL == 0
#error _ITERATOR_DEBUG_LEVEL != 0 must imply _CONTAINER_DEBUG_LEVEL == 1.
#endif // _ITERATOR_DEBUG_LEVEL != 0 && _CONTAINER_DEBUG_LEVEL == 0

#define _STL_REPORT_ERROR(mesg)              \
    do {                                     \
        _RPTF0(_CRT_ASSERT, mesg);           \
        _CRT_SECURE_INVALID_PARAMETER(mesg); \
    } while (false)

#ifdef __clang__
#define _STL_VERIFY(cond, mesg)                                                            \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wassume\"") do { \
        if (cond) { /* contextually convertible to bool paranoia */                        \
        } else {                                                                           \
            _STL_REPORT_ERROR(mesg);                                                       \
        }                                                                                  \
                                                                                           \
        _Analysis_assume_(cond);                                                           \
    }                                                                                      \
    while (false)                                                                          \
    _Pragma("clang diagnostic pop")
#else // ^^^ Clang // MSVC vvv
#define _STL_VERIFY(cond, mesg)                                     \
    do {                                                            \
        if (cond) { /* contextually convertible to bool paranoia */ \
        } else {                                                    \
            _STL_REPORT_ERROR(mesg);                                \
        }                                                           \
                                                                    \
        _Analysis_assume_(cond);                                    \
    } while (false)
#endif // ^^^ MSVC ^^^

#ifdef _DEBUG
#define _STL_ASSERT(cond, mesg) _STL_VERIFY(cond, mesg)
#else // ^^^ _DEBUG ^^^ // vvv !_DEBUG vvv
#define _STL_ASSERT(cond, mesg) _Analysis_assume_(cond)
#endif // _DEBUG

#ifdef _ENABLE_STL_INTERNAL_CHECK
#define _STL_INTERNAL_CHECK(...)         _STL_VERIFY(__VA_ARGS__, "STL internal check: " #__VA_ARGS__)
#define _STL_INTERNAL_STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#else // ^^^ _ENABLE_STL_INTERNAL_CHECK ^^^ // vvv !_ENABLE_STL_INTERNAL_CHECK vvv
#define _STL_INTERNAL_CHECK(...) _Analysis_assume_(__VA_ARGS__)
#define _STL_INTERNAL_STATIC_ASSERT(...)
#endif // _ENABLE_STL_INTERNAL_CHECK

#ifndef _ENABLE_ATOMIC_REF_ALIGNMENT_CHECK
#ifdef _DEBUG
#define _ENABLE_ATOMIC_REF_ALIGNMENT_CHECK 1
#else // ^^^ _DEBUG ^^^ // vvv !_DEBUG vvv
#define _ENABLE_ATOMIC_REF_ALIGNMENT_CHECK 0
#endif // _DEBUG
#endif // _ENABLE_ATOMIC_REF_ALIGNMENT_CHECK

#if _ENABLE_ATOMIC_REF_ALIGNMENT_CHECK
#define _ATOMIC_REF_CHECK_ALIGNMENT(cond, mesg) _STL_VERIFY(cond, mesg)
#else
#define _ATOMIC_REF_CHECK_ALIGNMENT(cond, mesg) _Analysis_assume_(cond)
#endif


#include <use_ansi.h>

#define _WARNING_MESSAGE(NUMBER, MESSAGE) __FILE__ "(" _CRT_STRINGIZE(__LINE__) "): warning " NUMBER ": " MESSAGE

#ifdef _STATIC_CPPLIB
#ifndef _DISABLE_DEPRECATE_STATIC_CPPLIB
#ifdef _DLL
#pragma message(_WARNING_MESSAGE("STL4000", "_STATIC_CPPLIB is deprecated and will be REMOVED."))
#endif
#ifdef _M_CEE_MIXED
#error _STATIC_CPPLIB is not supported while building with /clr
#endif
#endif // !_DISABLE_DEPRECATE_STATIC_CPPLIB
#ifdef _M_CEE_PURE
#error _STATIC_CPPLIB cannot be used with /clr:pure (the resulting assembly would not be pure)
#endif
#endif // _STATIC_CPPLIB

#if defined(_M_CEE_PURE) && !defined(_SILENCE_CLR_PURE_DEPRECATION_WARNING)
#pragma message(_WARNING_MESSAGE("STL4001", "/clr:pure is deprecated and will be REMOVED."))
#endif

#ifndef _MRTIMP2_PURE
#ifdef _M_CEE_PURE
#define _MRTIMP2_PURE
#else
#define _MRTIMP2_PURE _MRTIMP2
#endif
#endif // _MRTIMP2_PURE

#if defined(_DLL) && !defined(_STATIC_CPPLIB) && !defined(_M_CEE_PURE)
#define _DLL_CPPLIB
#endif

#ifndef _CRTIMP2_PURE
#ifdef _M_CEE_PURE
#define _CRTIMP2_PURE
#else
#define _CRTIMP2_PURE _CRTIMP2
#endif
#endif // _CRTIMP2_PURE

#ifdef _CRTBLD
// These functions are for enabling STATIC_CPPLIB functionality
#define _cpp_stdin         (__acrt_iob_func(0))
#define _cpp_stdout        (__acrt_iob_func(1))
#define _cpp_stderr        (__acrt_iob_func(2))
#define _cpp_isleadbyte(c) (__pctype_func()[static_cast<unsigned char>(c)] & _LEADBYTE)
#endif // _CRTBLD

#ifndef _CRTIMP2_IMPORT
#if defined(CRTDLL2) && defined(_CRTBLD)
#define _CRTIMP2_IMPORT __declspec(dllexport)
#elif defined(_DLL) && !defined(_STATIC_CPPLIB)
#define _CRTIMP2_IMPORT __declspec(dllimport)
#else
#define _CRTIMP2_IMPORT
#endif
#endif // _CRTIMP2_IMPORT

#ifndef _CRTIMP2_PURE_IMPORT
#ifdef _M_CEE_PURE
#define _CRTIMP2_PURE_IMPORT
#else
#define _CRTIMP2_PURE_IMPORT _CRTIMP2_IMPORT
#endif
#endif // _CRTIMP2_PURE_IMPORT

#ifndef _CRTIMP2_PURE_IMPORT_UNLESS_CODECVT_ID_SATELLITE
#ifdef _BUILDING_SATELLITE_CODECVT_IDS
#define _CRTIMP2_PURE_IMPORT_UNLESS_CODECVT_ID_SATELLITE
#else
#define _CRTIMP2_PURE_IMPORT_UNLESS_CODECVT_ID_SATELLITE _CRTIMP2_PURE_IMPORT
#endif
#endif // _CRTIMP2_PURE_IMPORT_UNLESS_CODECVT_ID_SATELLITE

#ifndef _CRTDATA2_IMPORT
#if defined(MRTDLL) && defined(_CRTBLD)
#define _CRTDATA2_IMPORT
#else
#define _CRTDATA2_IMPORT _CRTIMP2_IMPORT
#endif
#endif // _CRTDATA2_IMPORT

// INTEGER PROPERTIES
#define _MAX_EXP_DIG    8 // for parsing numerics
#define _MAX_INT_DIG    32
#define _MAX_SIG_DIG_V1 36 // TRANSITION, ABI
#define _MAX_SIG_DIG_V2 768

// MULTITHREAD PROPERTIES
// LOCK MACROS
#define _LOCK_LOCALE         0
#define _LOCK_MALLOC         1
#define _LOCK_STREAM         2
#define _LOCK_DEBUG          3
#define _LOCK_AT_THREAD_EXIT 4

#ifndef _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B
#if _STL_WIN32_WINNT >= _STL_WIN32_WINNT_WINBLUE && defined(_WIN64)
#define _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B 1
#else // ^^^ modern 64-bit // less modern or 32-bit vvv
#define _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B 0
#endif // _STL_WIN32_WINNT >= _STL_WIN32_WINNT_WINBLUE && defined(_WIN64)
#endif // _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B

#if _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 0 && defined(_M_ARM64)
#error ARM64 requires _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B to be 1.
#endif // _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 0 && defined(_M_ARM64)

#if _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 1 && !defined(_WIN64)
#error _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 1 requires 64-bit.
#endif // _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 1 && !defined(_WIN64)

#ifdef __cplusplus
_STD_BEGIN
enum _Uninitialized { // tag for suppressing initialization
    _Noinit
};

// CLASS _Lockit
class _CRTIMP2_PURE_IMPORT _Lockit { // lock while object in existence -- MUST NEST
public:
#ifdef _M_CEE_PURE
    __CLR_OR_THIS_CALL _Lockit() noexcept : _Locktype(0) {
        _Lockit_ctor(this);
    }

    explicit __CLR_OR_THIS_CALL _Lockit(int _Kind) noexcept { // set the lock
        _Lockit_ctor(this, _Kind);
    }

    __CLR_OR_THIS_CALL ~_Lockit() noexcept { // clear the lock
        _Lockit_dtor(this);
    }

#else // _M_CEE_PURE
    __thiscall _Lockit() noexcept;
    explicit __thiscall _Lockit(int) noexcept; // set the lock
    __thiscall ~_Lockit() noexcept; // clear the lock
#endif // _M_CEE_PURE

    static void __cdecl _Lockit_ctor(int) noexcept;
    static void __cdecl _Lockit_dtor(int) noexcept;

private:
    static void __cdecl _Lockit_ctor(_Lockit*) noexcept;
    static void __cdecl _Lockit_ctor(_Lockit*, int) noexcept;
    static void __cdecl _Lockit_dtor(_Lockit*) noexcept;

public:
    __CLR_OR_THIS_CALL _Lockit(const _Lockit&) = delete;
    _Lockit& __CLR_OR_THIS_CALL operator=(const _Lockit&) = delete;

private:
    int _Locktype;
};

#ifdef _M_CEE
class _CRTIMP2_PURE_IMPORT _EmptyLockit { // empty lock class used for bin compat
private:
    int _Locktype;
};

#ifndef _PREPARE_CONSTRAINED_REGIONS
#ifdef _M_CEE_PURE
#define _PREPARE_CONSTRAINED_REGIONS 1
#else // _M_CEE_PURE
#define _PREPARE_CONSTRAINED_REGIONS 0
#endif // _M_CEE_PURE
#endif // _PREPARE_CONSTRAINED_REGIONS

#if _PREPARE_CONSTRAINED_REGIONS
#define _BEGIN_LOCK(_Kind)                                                                  \
    {                                                                                       \
        bool _MustReleaseLock = false;                                                      \
        int _LockKind         = _Kind;                                                      \
        System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();     \
        try {                                                                               \
            System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions(); \
            try {                                                                           \
            } finally {                                                                     \
                _STD _Lockit::_Lockit_ctor(_LockKind);                                      \
                _MustReleaseLock = true;                                                    \
            }

#define _END_LOCK()                                \
    }                                              \
    finally {                                      \
        if (_MustReleaseLock) {                    \
            _STD _Lockit::_Lockit_dtor(_LockKind); \
        }                                          \
    }                                              \
    }

#else // _PREPARE_CONSTRAINED_REGIONS
#define _BEGIN_LOCK(_Kind) \
    {                      \
        _STD _Lockit _Lock(_Kind);

#define _END_LOCK() }

#endif // _PREPARE_CONSTRAINED_REGIONS

#define _BEGIN_LOCINFO(_VarName) \
    _BEGIN_LOCK(_LOCK_LOCALE)    \
    _Locinfo _VarName;

#define _END_LOCINFO() _END_LOCK()

#define _RELIABILITY_CONTRACT                                                    \
    [System::Runtime::ConstrainedExecution::ReliabilityContract(                 \
        System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
        System::Runtime::ConstrainedExecution::Cer::Success)]

#else // _M_CEE
#define _BEGIN_LOCK(_Kind) \
    {                      \
        _STD _Lockit _Lock(_Kind);

#define _END_LOCK() }

#define _BEGIN_LOCINFO(_VarName) \
    {                            \
        _Locinfo _VarName;

#define _END_LOCINFO() }

#define _RELIABILITY_CONTRACT
#endif // _M_CEE

class _CRTIMP2_PURE_IMPORT _Init_locks { // initialize mutexes
public:
#ifdef _M_CEE_PURE
    __CLR_OR_THIS_CALL _Init_locks() noexcept {
        _Init_locks_ctor(this);
    }

    __CLR_OR_THIS_CALL ~_Init_locks() noexcept {
        _Init_locks_dtor(this);
    }

#else // _M_CEE_PURE
    __thiscall _Init_locks() noexcept;
    __thiscall ~_Init_locks() noexcept;
#endif // _M_CEE_PURE

private:
    static void __cdecl _Init_locks_ctor(_Init_locks*) noexcept;
    static void __cdecl _Init_locks_dtor(_Init_locks*) noexcept;
};

// EXCEPTION MACROS
#if _HAS_EXCEPTIONS
#define _TRY_BEGIN try {
#define _CATCH(x) \
    }             \
    catch (x) {
#define _CATCH_ALL \
    }              \
    catch (...) {
#define _CATCH_END }

#define _RERAISE  throw
#define _THROW(x) throw x

#else // _HAS_EXCEPTIONS
#define _TRY_BEGIN \
    {              \
        if (1) {
#define _CATCH(x) \
    }             \
    else if (0) {
#define _CATCH_ALL \
    }              \
    else if (0) {
#define _CATCH_END \
    }              \
    }

#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG

#define _RERAISE
#define _THROW(x) x._Raise()
#endif // _HAS_EXCEPTIONS
_STD_END
#endif // __cplusplus

#ifndef _RELIABILITY_CONTRACT
#define _RELIABILITY_CONTRACT
#endif // _RELIABILITY_CONTRACT

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _YVALS
