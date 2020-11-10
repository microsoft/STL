// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// class locale member functions

#if !defined(STDCPP_IMPLIB)
#define STDCPP_IMPLIB 0
#endif

#include <cstdlib>
#include <internal_shared.h>
#include <istream>
#include <xlocale>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

_STD_BEGIN

_MRTIMP2_PURE locale __CLRCALL_PURE_OR_CDECL locale::global(const locale& loc) { // change global locale
    locale _Oldglobal;
    _BEGIN_LOCK(_LOCK_LOCALE)
    locale::_Locimp* ptr = _Getgloballocale();

    if (ptr != loc._Ptr) { // set new global locale
        delete ptr->_Decref();
        _Setgloballocale(ptr = loc._Ptr);
        ptr->_Incref();
        category _Cmask = ptr->_Catmask & all;
        if (_Cmask == all) {
            setlocale(LC_ALL, ptr->_Name._C_str());
        } else {
            for (int catindex = 0; catindex <= _X_MAX; ++catindex) {
                if ((_CATMASK(catindex) & _Cmask) != 0) {
                    setlocale(catindex, ptr->_Name._C_str());
                }
            }
        }
    }
    return _Oldglobal;
    _END_LOCK()
}


#if STDCPP_IMPLIB || !defined(_M_CEE_PURE)
// facets associated with C categories
#define ADDFAC(Facet, cat, ptrimp, ptrloc)                                                                  \
    if ((_CATMASK(Facet::_Getcat()) & cat) == 0) {                                                          \
        ;                                                                                                   \
    } else if (ptrloc == nullptr) {                                                                         \
        ptrimp->_Addfac(new Facet(lobj), Facet::id);                                                        \
    } else {                                                                                                \
        ptrimp->_Addfac(                                                                                    \
            const_cast<locale::facet*>(static_cast<const locale::facet*>(&_STD use_facet<Facet>(*ptrloc))), \
            Facet::id);                                                                                     \
    }

using _Tc1 = ctype<char>;
using _Tc2 = num_get<char>;
using _Tc3 = num_put<char>;
using _Tc4 = numpunct<char>;
using _Tc5 = codecvt<char, char, _Mbstatet>;
// others moved to wlocale and xlocale to ease subsetting

locale::_Locimp* __CLRCALL_OR_CDECL locale::_Locimp::_Makeloc(
    const _Locinfo& lobj, locale::category cat, _Locimp* ptrimp, const locale* ptrloc) { // setup a new locale
    ADDFAC(_Tc1, cat, ptrimp, ptrloc);
    ADDFAC(_Tc2, cat, ptrimp, ptrloc);
    ADDFAC(_Tc3, cat, ptrimp, ptrloc);
    ADDFAC(_Tc4, cat, ptrimp, ptrloc);
    ADDFAC(_Tc5, cat, ptrimp, ptrloc);
    _Locimp::_Makexloc(lobj, cat, ptrimp, ptrloc);
    _Locimp::_Makewloc(lobj, cat, ptrimp, ptrloc);
#ifdef _NATIVE_WCHAR_T_DEFINED
    _Locimp::_Makeushloc(lobj, cat, ptrimp, ptrloc);
#endif
    ptrimp->_Catmask |= cat;
    ptrimp->_Name = lobj._Getname();
    return ptrimp;
}

void __CLRCALL_PURE_OR_CDECL locale::_Locimp::_Locimp_ctor(
    locale::_Locimp* _This, const locale::_Locimp& imp) { // construct a _Locimp from a copy
    if (&imp == _This->_Clocptr) {
        _BEGIN_LOCINFO(_Lobj)
        _Makeloc(_Lobj, locale::all, _This, nullptr);
        _END_LOCINFO()
    } else { // lock to keep facets from disappearing
        _BEGIN_LOCK(_LOCK_LOCALE)
        if (0 < _This->_Facetcount) { // copy over nonempty facet vector
            _This->_Facetvec = static_cast<locale::facet**>(_malloc_crt(_This->_Facetcount * sizeof(locale::facet*)));
            if (_This->_Facetvec == nullptr) { // report no memory
                _Xbad_alloc();
            }

            for (size_t count = _This->_Facetcount; 0 < count;) { // copy over facet pointers
                locale::facet* ptrfac = imp._Facetvec[--count];
                if ((_This->_Facetvec[count] = ptrfac) != nullptr) {
                    ptrfac->_Incref();
                }
            }
        }
        _END_LOCK()
    }
}
#endif // STDCPP_IMPLIB || !defined(_M_CEE_PURE)


void __CLRCALL_PURE_OR_CDECL locale::_Locimp::_Locimp_Addfac(
    _Locimp* _This, locale::facet* ptrfac, size_t id) { // add a facet to a locale
    _BEGIN_LOCK(_LOCK_LOCALE)
    const size_t MINCAT = 40; // minimum number of facets in a locale

    if (_This->_Facetcount <= id) { // make facet vector larger
        size_t count = id + 1;
        if (count < MINCAT) {
            count = MINCAT;
        }

        locale::facet** ptrnewvec =
            static_cast<locale::facet**>(_realloc_crt(_This->_Facetvec, count * sizeof(locale::facet**)));
        if (ptrnewvec == nullptr) { // report no memory
            _Xbad_alloc();
        }
        _This->_Facetvec = ptrnewvec;
        for (; _This->_Facetcount < count; ++_This->_Facetcount) {
            _This->_Facetvec[_This->_Facetcount] = nullptr;
        }
    }
    ptrfac->_Incref();
#pragma warning(suppress : 6001) // PREfast isn't following through _realloc_crt here
    if (_This->_Facetvec[id] != nullptr) {
        delete _This->_Facetvec[id]->_Decref();
    }

    _This->_Facetvec[id] = ptrfac;
    _END_LOCK()
}

void __CLRCALL_PURE_OR_CDECL _Locinfo::_Locinfo_ctor(
    _Locinfo* pLocinfo, int cat, const char* locname) { // capture a named locale
    const char* oldlocname = setlocale(LC_ALL, nullptr);

    pLocinfo->_Oldlocname = oldlocname == nullptr ? "" : oldlocname;
    _Locinfo_Addcats(pLocinfo, cat, locname);
}

_Locinfo& __CLRCALL_PURE_OR_CDECL _Locinfo::_Locinfo_Addcats(
    _Locinfo* pLocinfo, int cat, const char* locname) { // merge in another named locale
    const char* oldlocname = nullptr;

    if (locname == nullptr) {
        _Xruntime_error("bad locale name");
    }

    if (locname[0] != '*' || locname[1] != '\0') {
        if (cat == 0) {
            oldlocname = setlocale(LC_ALL, nullptr);
        } else if (cat == _M_ALL) {
            oldlocname = setlocale(LC_ALL, locname);
        } else { // alter selected categories
            for (int catindex = 0; catindex <= _X_MAX; ++catindex) {
                if ((_CATMASK(catindex) & cat) != 0) {
                    setlocale(catindex, locname);
                }
            }

            oldlocname = setlocale(LC_ALL, locname);
        }
    }

    if (oldlocname == nullptr) {
        pLocinfo->_Newlocname = "*";
    } else if (strcmp(pLocinfo->_Newlocname._C_str(), "*") != 0) {
        pLocinfo->_Newlocname = oldlocname;
    }

    return *pLocinfo;
}

_STD_END
