// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// class locale basic member functions

#include <crtdbg.h>
#include <internal_shared.h>
#include <xfacet>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

// This should probably go to a compiler section just after the locks - unfortunately we have per-appdomain
// and per-process variables to initialize
#pragma warning(disable : 4073)
#pragma init_seg(lib)

_STD_BEGIN
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc();

struct _Fac_node { // node for lazy facet recording
    _Fac_node(_Fac_node* _Nextarg, _Facet_base* _Facptrarg)
        : _Next(_Nextarg), _Facptr(_Facptrarg) {} // construct a node with value

    ~_Fac_node() noexcept { // destroy a facet
        delete _Facptr->_Decref();
    }

#ifdef _DEBUG
    void* operator new(size_t _Size) { // replace operator new
        void* _Ptr = _malloc_dbg(_Size > 0 ? _Size : 1, _CRT_BLOCK, __FILE__, __LINE__);
        if (!_Ptr) {
            _Xbad_alloc();
        }

        return _Ptr;
    }

    void operator delete(void* _Ptr) noexcept { // replace operator delete
        _free_dbg(_Ptr, _CRT_BLOCK);
    }
#endif // _DEBUG

    _Fac_node* _Next;
    _Facet_base* _Facptr;
};

__PURE_APPDOMAIN_GLOBAL static _Fac_node* _Fac_head = nullptr;

struct _Fac_tidy_reg_t {
    ~_Fac_tidy_reg_t() noexcept { // destroy lazy facets
        while (_Fac_head != nullptr) { // destroy a lazy facet node
            _Fac_node* nodeptr = _Fac_head;
            _Fac_head          = nodeptr->_Next;
            delete nodeptr;
        }
    }
};

__PURE_APPDOMAIN_GLOBAL const _Fac_tidy_reg_t _Fac_tidy_reg;

#if defined(_M_CEE)
void __CLRCALL_OR_CDECL _Facet_Register_m(_Facet_base* _This)
#else // defined(_M_CEE)
void __CLRCALL_OR_CDECL _Facet_Register(_Facet_base* _This)
#endif // defined(_M_CEE)
{ // queue up lazy facet for destruction
    _Fac_head = new _Fac_node(_Fac_head, _This);
}
_STD_END

#if !STDCPP_IMPLIB || defined(_M_CEE_PURE)

#include <cstdlib>
#include <locale>

_EXTERN_C

void __CLRCALL_OR_CDECL _Deletegloballocale(void* ptr) { // delete a global locale reference
    std::locale::_Locimp* locptr = *static_cast<std::locale::_Locimp**>(ptr);
    if (locptr != nullptr) {
        delete locptr->_Decref();
    }
}

__PURE_APPDOMAIN_GLOBAL static std::locale::_Locimp* global_locale = nullptr; // pointer to current locale

static void __CLRCALL_PURE_OR_CDECL tidy_global() { // delete static global locale reference
    _BEGIN_LOCK(_LOCK_LOCALE) // prevent double delete
    _Deletegloballocale(&global_locale);
    global_locale = nullptr;
    _END_LOCK()
}

_END_EXTERN_C

_MRTIMP2 void __cdecl _Atexit(void(__cdecl*)());

_STD_BEGIN

_MRTIMP2_PURE std::locale::_Locimp* __CLRCALL_PURE_OR_CDECL
    std::locale::_Getgloballocale() { // return pointer to current locale
    return global_locale;
}

_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL std::locale::_Setgloballocale(void* ptr) { // alter pointer to current locale
    __PURE_APPDOMAIN_GLOBAL static bool registered = false;

    if (!registered) { // register cleanup first time
        registered = true;
#if !defined(_M_CEE_PURE)
        ::_Atexit(&tidy_global);
#else
        _atexit_m_appdomain(tidy_global);
#endif
    }
    global_locale = static_cast<std::locale::_Locimp*>(ptr);
}

__PURE_APPDOMAIN_GLOBAL static locale classic_locale(_Noinit); // "C" locale object, uninitialized

__PURE_APPDOMAIN_GLOBAL locale::_Locimp* locale::_Locimp::_Clocptr = nullptr; // pointer to classic_locale

__PURE_APPDOMAIN_GLOBAL int locale::id::_Id_cnt = 0; // unique id counter for facets

__PURE_APPDOMAIN_GLOBAL locale::id ctype<char>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id ctype<wchar_t>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id codecvt<wchar_t, char, mbstate_t>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id ctype<unsigned short>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id codecvt<unsigned short, char, mbstate_t>::id(0);

#define _Compiler_barrier() _STL_DISABLE_DEPRECATED_WARNING _ReadWriteBarrier() _STL_RESTORE_DEPRECATED_WARNING

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
#define _Memory_barrier()             __dmb(0xB) // inner shared data memory barrier
#define _Compiler_or_memory_barrier() _Memory_barrier()
#elif defined(_M_IX86) || defined(_M_X64)
// x86/x64 hardware only emits memory barriers inside _Interlocked intrinsics
#define _Compiler_or_memory_barrier() _Compiler_barrier()
#else // ^^^ x86/x64 / unsupported hardware vvv
#error Unsupported hardware
#endif // hardware

_MRTIMP2_PURE const locale& __CLRCALL_PURE_OR_CDECL locale::classic() { // get reference to "C" locale
#if !defined(_M_CEE_PURE)
    const auto mem = reinterpret_cast<const intptr_t*>(&locale::_Locimp::_Clocptr);
    intptr_t as_bytes;
#ifdef _WIN64
#ifdef _M_ARM
    as_bytes = __ldrexd(_Mem);
#else // ^^^ ARM64 / x64 vvv
    as_bytes = __iso_volatile_load64(mem);
#endif // ^^^ x64
#else // ^^^ 64-bit / 32-bit vvv
    as_bytes = __iso_volatile_load32(mem);
#endif // ^^^ 32-bit ^^^
    _Compiler_or_memory_barrier();
    const auto ptr = reinterpret_cast<locale::_Locimp*>(as_bytes);
    if (ptr == nullptr)
#endif // !defined(_M_CEE_PURE)
    {
        _Init();
    }

    return classic_locale;
}

_MRTIMP2_PURE locale __CLRCALL_PURE_OR_CDECL locale::empty() { // make empty transparent locale
    _Init();
    return locale(_Locimp::_New_Locimp(true));
}

_MRTIMP2_PURE locale::_Locimp* __CLRCALL_PURE_OR_CDECL locale::_Init(bool _Do_incref) { // setup global and "C" locales
    locale::_Locimp* ptr = nullptr;

    _BEGIN_LOCK(_LOCK_LOCALE) // prevent double initialization

    ptr = _Getgloballocale();

    if (ptr == nullptr) { // create new locales
        _Setgloballocale(ptr = _Locimp::_New_Locimp());
        ptr->_Catmask = all; // set current locale to "C"
        ptr->_Name    = "C";

        // set classic to match
        ptr->_Incref();
        ::new (&classic_locale) locale(ptr);
#if defined(_M_CEE_PURE)
        locale::_Locimp::_Clocptr = ptr;
#else // ^^^ _M_CEE_PURE / !_M_CEE_PURE vvv
        const auto mem = reinterpret_cast<volatile intptr_t*>(&locale::_Locimp::_Clocptr);
        const auto as_bytes = reinterpret_cast<intptr_t>(ptr);
        _Compiler_or_memory_barrier();
#ifdef _WIN64
        __iso_volatile_store64(mem, as_bytes);
#else
        __iso_volatile_store32(mem, as_bytes);
#endif
#endif // ^^^ !defined(_M_CEE_PURE) ^^^
    }

    if (_Do_incref) {
        ptr->_Incref();
    }

    _END_LOCK()

    return ptr;
}

locale::_Locimp* __CLRCALL_PURE_OR_CDECL locale::_Locimp::_New_Locimp(bool _Transparent) {
    return new _Locimp(_Transparent);
}

locale::_Locimp* __CLRCALL_PURE_OR_CDECL locale::_Locimp::_New_Locimp(const _Locimp& _Right) {
    return new _Locimp(_Right);
}

void __CLRCALL_PURE_OR_CDECL locale::_Locimp::_Locimp_dtor(_Locimp* _This) { // destruct a _Locimp
    _BEGIN_LOCK(_LOCK_LOCALE) // prevent double delete
    for (size_t count = _This->_Facetcount; 0 < count;) {
        if (_This->_Facetvec[--count] != nullptr) {
            delete _This->_Facetvec[count]->_Decref();
        }
    }

    free(_This->_Facetvec);
    _END_LOCK()
}

void __CLRCALL_PURE_OR_CDECL _Locinfo::_Locinfo_ctor(
    _Locinfo* pLocinfo, const char* locname) { // switch to a named locale
    const char* oldlocname = setlocale(LC_ALL, nullptr);

    pLocinfo->_Oldlocname = oldlocname == nullptr ? "" : oldlocname;
    if (locname != nullptr) {
        locname = setlocale(LC_ALL, locname);
    }

    pLocinfo->_Newlocname = locname == nullptr ? "*" : locname;
}

void __CLRCALL_PURE_OR_CDECL _Locinfo::_Locinfo_dtor(_Locinfo* pLocinfo) { // destroy a _Locinfo object, revert locale
    if (!pLocinfo->_Oldlocname._Empty()) {
        setlocale(LC_ALL, pLocinfo->_Oldlocname._C_str());
    }
}
_STD_END

#endif // !STDCPP_IMPLIB || defined(_M_CEE_PURE)
