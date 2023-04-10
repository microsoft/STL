// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// class locale wide member functions

#include <istream>
#include <locale>
_STD_BEGIN

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

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

// moved from locale to ease subsetting
using _Tw1  = ctype<wchar_t>;
using _Tw2  = num_get<wchar_t>;
using _Tw3  = num_put<wchar_t>;
using _Tw4  = numpunct<wchar_t>;
using _Tw5  = collate<wchar_t>;
using _Tw6  = messages<wchar_t>;
using _Tw7  = money_get<wchar_t>;
using _Tw8  = money_put<wchar_t>;
using _Tw9  = moneypunct<wchar_t, false>;
using _Tw10 = moneypunct<wchar_t, true>;
using _Tw11 = time_get<wchar_t>;
using _Tw12 = time_put<wchar_t>;
using _Tw13 = codecvt<wchar_t, char, _Mbstatet>;
__PURE_APPDOMAIN_GLOBAL locale::id time_put<wchar_t>::id(0);

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

// moved from locale to ease subsetting
using _Tu1  = ctype<unsigned short>;
using _Tu2  = num_get<unsigned short>;
using _Tu3  = num_put<unsigned short>;
using _Tu4  = numpunct<unsigned short>;
using _Tu5  = collate<unsigned short>;
using _Tu6  = messages<unsigned short>;
using _Tu7  = money_get<unsigned short>;
using _Tu8  = money_put<unsigned short>;
using _Tu9  = moneypunct<unsigned short, false>;
using _Tu10 = moneypunct<unsigned short, true>;
using _Tu11 = time_get<unsigned short>;
using _Tu12 = time_put<unsigned short>;
using _Tu13 = codecvt<unsigned short, char, _Mbstatet>;

void __CLRCALL_OR_CDECL locale::_Locimp::_Makeushloc(const _Locinfo& lobj, locale::category cat, _Locimp* ptrimp,
    const locale* ptrloc) { // setup wide part of a new locale
    ADDFAC(_Tu1, cat, ptrimp, ptrloc);
    ADDFAC(_Tu2, cat, ptrimp, ptrloc);
    ADDFAC(_Tu3, cat, ptrimp, ptrloc);
    ADDFAC(_Tu4, cat, ptrimp, ptrloc);
    ADDFAC(_Tu5, cat, ptrimp, ptrloc);
    ADDFAC(_Tu6, cat, ptrimp, ptrloc);
    ADDFAC(_Tu7, cat, ptrimp, ptrloc);
    ADDFAC(_Tu8, cat, ptrimp, ptrloc);
    ADDFAC(_Tu9, cat, ptrimp, ptrloc);
    ADDFAC(_Tu10, cat, ptrimp, ptrloc);
    ADDFAC(_Tu11, cat, ptrimp, ptrloc);
    ADDFAC(_Tu12, cat, ptrimp, ptrloc);
    ADDFAC(_Tu13, cat, ptrimp, ptrloc);
}

_STD_END
