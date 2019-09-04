// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// class locale wide member functions

#include <istream>
#include <locale>
_STD_BEGIN

#pragma warning(disable : 4074)
#pragma init_seg(compiler)


using _Wtraits  = char_traits<wchar_t>;
using _Winiter  = istreambuf_iterator<wchar_t, _Wtraits>;
using _Woutiter = ostreambuf_iterator<wchar_t, _Wtraits>;

// facets associated with C categories
#define ADDFAC(Facet, cat, ptrimp, ptrloc)                                            \
    if ((_CATMASK(Facet::_Getcat()) & cat) == 0) {                                    \
        ;                                                                             \
    } else if (ptrloc == 0) {                                                         \
        ptrimp->_Addfac(new Facet(lobj), Facet::id);                                  \
    } else {                                                                          \
        ptrimp->_Addfac((locale::facet*) &_STD use_facet<Facet>(*ptrloc), Facet::id); \
    }

// moved from locale to ease subsetting
using _Tw1  = ctype<wchar_t>;
using _Tw2  = num_get<wchar_t, _Winiter>;
using _Tw3  = num_put<wchar_t, _Woutiter>;
using _Tw4  = numpunct<wchar_t>;
using _Tw5  = collate<wchar_t>;
using _Tw6  = messages<wchar_t>;
using _Tw7  = money_get<wchar_t, _Winiter>;
using _Tw8  = money_put<wchar_t, _Woutiter>;
using _Tw9  = moneypunct<wchar_t, false>;
using _Tw10 = moneypunct<wchar_t, true>;
using _Tw11 = time_get<wchar_t, _Winiter>;
using _Tw12 = time_put<wchar_t, _Woutiter>;
using _Tw13 = codecvt<wchar_t, char, _Mbstatet>;
__PURE_APPDOMAIN_GLOBAL locale::id time_put<wchar_t, _Woutiter>::id(0);


void __CLRCALL_OR_CDECL locale::_Locimp::_Makewloc(const _Locinfo& lobj, locale::category cat, _Locimp* ptrimp,
    const locale* ptrloc) { // setup wide part of a new locale
    ADDFAC(_Tw1, cat, ptrimp, ptrloc);
    ADDFAC(_Tw2, cat, ptrimp, ptrloc);
    ADDFAC(_Tw3, cat, ptrimp, ptrloc);
    ADDFAC(_Tw4, cat, ptrimp, ptrloc);
    ADDFAC(_Tw5, cat, ptrimp, ptrloc);
    ADDFAC(_Tw6, cat, ptrimp, ptrloc);
    ADDFAC(_Tw7, cat, ptrimp, ptrloc);
    ADDFAC(_Tw8, cat, ptrimp, ptrloc);
    ADDFAC(_Tw9, cat, ptrimp, ptrloc);
    ADDFAC(_Tw10, cat, ptrimp, ptrloc);
    ADDFAC(_Tw11, cat, ptrimp, ptrloc);
    ADDFAC(_Tw12, cat, ptrimp, ptrloc);
    ADDFAC(_Tw13, cat, ptrimp, ptrloc);
}

#ifdef _NATIVE_WCHAR_T_DEFINED
using _UShtraits  = char_traits<unsigned short>;
using _UShiniter  = istreambuf_iterator<unsigned short, _UShtraits>;
using _UShoutiter = ostreambuf_iterator<unsigned short, _UShtraits>;

// moved from locale to ease subsetting
using _Tush1  = ctype<unsigned short>;
using _Tush2  = num_get<unsigned short, _UShiniter>;
using _Tush3  = num_put<unsigned short, _UShoutiter>;
using _Tush4  = numpunct<unsigned short>;
using _Tush5  = collate<unsigned short>;
using _Tush6  = messages<unsigned short>;
using _Tush7  = money_get<unsigned short, _UShiniter>;
using _Tush8  = money_put<unsigned short, _UShoutiter>;
using _Tush9  = moneypunct<unsigned short, false>;
using _Tush10 = moneypunct<unsigned short, true>;
using _Tush11 = time_get<unsigned short, _UShiniter>;
using _Tush12 = time_put<unsigned short, _UShoutiter>;
using _Tush13 = codecvt<unsigned short, char, _Mbstatet>;

void __CLRCALL_OR_CDECL locale::_Locimp::_Makeushloc(const _Locinfo& lobj, locale::category cat, _Locimp* ptrimp,
    const locale* ptrloc) { // setup wide part of a new locale
    ADDFAC(_Tush1, cat, ptrimp, ptrloc);
    ADDFAC(_Tush2, cat, ptrimp, ptrloc);
    ADDFAC(_Tush3, cat, ptrimp, ptrloc);
    ADDFAC(_Tush4, cat, ptrimp, ptrloc);
    ADDFAC(_Tush5, cat, ptrimp, ptrloc);
    ADDFAC(_Tush6, cat, ptrimp, ptrloc);
    ADDFAC(_Tush7, cat, ptrimp, ptrloc);
    ADDFAC(_Tush8, cat, ptrimp, ptrloc);
    ADDFAC(_Tush9, cat, ptrimp, ptrloc);
    ADDFAC(_Tush10, cat, ptrimp, ptrloc);
    ADDFAC(_Tush11, cat, ptrimp, ptrloc);
    ADDFAC(_Tush12, cat, ptrimp, ptrloc);
    ADDFAC(_Tush13, cat, ptrimp, ptrloc);
}
#endif // _NATIVE_WCHAR_T_DEFINED

_STD_END
