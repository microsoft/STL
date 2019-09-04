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

    [[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL
    _Xbad_alloc();

struct _Fac_node { // node for lazy facet recording
    _Fac_node(_Fac_node* _Nextarg, _Facet_base* _Facptrarg)
        : _Next(_Nextarg), _Facptr(_Facptrarg) { // construct a node with value
    }

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

__PURE_APPDOMAIN_GLOBAL static _Fac_node* _Fac_head = 0;

struct _Fac_tidy_reg_t {
    ~_Fac_tidy_reg_t() noexcept { // destroy lazy facets
        while (_Fac_head != 0) { // destroy a lazy facet node
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

#include <locale>
#include <stdlib.h>

_EXTERN_C

void __CLRCALL_OR_CDECL _Deletegloballocale(void* ptr) { // delete a global locale reference
    std::locale::_Locimp* locptr = *(std::locale::_Locimp**) ptr;
    if (locptr != 0) {
        delete locptr->_Decref();
    }
}

__PURE_APPDOMAIN_GLOBAL static std::locale::_Locimp* global_locale = 0; // pointer to current locale

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
    global_locale = (std::locale::_Locimp*) ptr;
}

__PURE_APPDOMAIN_GLOBAL static locale classic_locale(_Noinit); // "C" locale object, uninitialized

__PURE_APPDOMAIN_GLOBAL locale::_Locimp* locale::_Locimp::_Clocptr = 0; // pointer to classic_locale

__PURE_APPDOMAIN_GLOBAL int locale::id::_Id_cnt = 0; // unique id counter for facets

__PURE_APPDOMAIN_GLOBAL locale::id ctype<char>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id ctype<wchar_t>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id codecvt<wchar_t, char, mbstate_t>::id(0);

#ifdef _NATIVE_WCHAR_T_DEFINED
__PURE_APPDOMAIN_GLOBAL locale::id ctype<unsigned short>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id codecvt<unsigned short, char, mbstate_t>::id(0);

#endif // _NATIVE_WCHAR_T_DEFINED

_MRTIMP2_PURE const locale& __CLRCALL_PURE_OR_CDECL locale::classic() { // get reference to "C" locale
    _Init();
    return classic_locale;
}

_MRTIMP2_PURE locale __CLRCALL_PURE_OR_CDECL locale::empty() { // make empty transparent locale
    _Init();
    return locale(_Locimp::_New_Locimp(true));
}

_MRTIMP2_PURE locale::_Locimp* __CLRCALL_PURE_OR_CDECL locale::_Init(bool _Do_incref) { // setup global and "C" locales
    locale::_Locimp* ptr = 0;

    _BEGIN_LOCK(_LOCK_LOCALE) // prevent double initialization

    ptr = _Getgloballocale();

    if (ptr == 0) { // create new locales
        _Setgloballocale(ptr = _Locimp::_New_Locimp());
        ptr->_Catmask = all; // set current locale to "C"
        ptr->_Name    = "C";

        _Locimp::_Clocptr = ptr; // set classic to match
        _Locimp::_Clocptr->_Incref();
        ::new (&classic_locale) locale(_Locimp::_Clocptr);
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
        if (_This->_Facetvec[--count] != 0) {
            delete _This->_Facetvec[count]->_Decref();
        }
    }

    free(_This->_Facetvec);
    _END_LOCK()
}

void __CLRCALL_PURE_OR_CDECL _Locinfo::_Locinfo_ctor(
    _Locinfo* pLocinfo, const char* locname) { // switch to a named locale
    const char* oldlocname = setlocale(LC_ALL, 0);

    pLocinfo->_Oldlocname = oldlocname == 0 ? "" : oldlocname;
    if (locname != 0) {
        locname = setlocale(LC_ALL, locname);
    }

    pLocinfo->_Newlocname = locname == 0 ? "*" : locname;
}

void __CLRCALL_PURE_OR_CDECL _Locinfo::_Locinfo_dtor(_Locinfo* pLocinfo) { // destroy a _Locinfo object, revert locale
    if (!pLocinfo->_Oldlocname._Empty()) {
        setlocale(LC_ALL, pLocinfo->_Oldlocname._C_str());
    }
}
_STD_END

#endif // !STDCPP_IMPLIB
