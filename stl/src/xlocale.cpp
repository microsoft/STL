// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// class locale exotic char member functions

#include <istream>
#include <locale>
_STD_BEGIN

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

using _Traits  = char_traits<char>;
using _Initer  = istreambuf_iterator<char, _Traits>;
using _Outiter = ostreambuf_iterator<char, _Traits>;

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
using _Tx1 = collate<char>;
using _Tx2 = messages<char>;
using _Tx3 = money_get<char, _Initer>;
using _Tx4 = money_put<char, _Outiter>;
using _Tx5 = moneypunct<char, false>;
using _Tx6 = moneypunct<char, true>;
using _Tx7 = time_get<char, _Initer>;
using _Tx8 = time_put<char, _Outiter>;

void __CLRCALL_OR_CDECL locale::_Locimp::_Makexloc(const _Locinfo& lobj, locale::category cat, _Locimp* ptrimp,
    const locale* ptrloc) { // setup exotic char part of a new locale
    ADDFAC(_Tx1, cat, ptrimp, ptrloc);
    ADDFAC(_Tx2, cat, ptrimp, ptrloc);
    ADDFAC(_Tx3, cat, ptrimp, ptrloc);
    ADDFAC(_Tx4, cat, ptrimp, ptrloc);
    ADDFAC(_Tx5, cat, ptrimp, ptrloc);
    ADDFAC(_Tx6, cat, ptrimp, ptrloc);
    ADDFAC(_Tx7, cat, ptrimp, ptrloc);
    ADDFAC(_Tx8, cat, ptrimp, ptrloc);
}
_STD_END
