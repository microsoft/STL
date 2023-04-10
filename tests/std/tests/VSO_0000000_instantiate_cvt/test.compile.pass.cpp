// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _USE_NAMED_IDL_NAMESPACE 1

#ifndef _M_CEE_PURE // chk builds of /clr:pure compile this test extremely slowly.

#include <cvt/8859_10>
#include <cvt/8859_13>
#include <cvt/8859_14>
#include <cvt/8859_15>
#include <cvt/8859_16>
#include <cvt/8859_1>
#include <cvt/8859_2>
#include <cvt/8859_3>
#include <cvt/8859_4>
#include <cvt/8859_5>
#include <cvt/8859_6>
#include <cvt/8859_7>
#include <cvt/8859_8>
#include <cvt/8859_9>
#include <cvt/baltic>
#include <cvt/big5>
#include <cvt/cp037>
#include <cvt/cp1006>
#include <cvt/cp1026>
#include <cvt/cp1250>
#include <cvt/cp1251>
#include <cvt/cp1252>
#include <cvt/cp1253>
#include <cvt/cp1254>
#include <cvt/cp1255>
#include <cvt/cp1256>
#include <cvt/cp1257>
#include <cvt/cp1258>
#include <cvt/cp424>
#include <cvt/cp437>
#include <cvt/cp500>
#include <cvt/cp737>
#include <cvt/cp775>
#include <cvt/cp850>
#include <cvt/cp852>
#include <cvt/cp855>
#include <cvt/cp856>
#include <cvt/cp857>
#include <cvt/cp860>
#include <cvt/cp861>
#include <cvt/cp862>
#include <cvt/cp863>
#include <cvt/cp864>
#include <cvt/cp865>
#include <cvt/cp866>
#include <cvt/cp869>
#include <cvt/cp874>
#include <cvt/cp875>
#include <cvt/cp932>
#include <cvt/cp936>
#include <cvt/cp949>
#include <cvt/cp950>
#include <cvt/cyrillic>
#include <cvt/ebcdic>
#include <cvt/euc>
#include <cvt/euc_0208>
#include <cvt/gb12345>
#include <cvt/gb2312>
#include <cvt/greek>
#include <cvt/iceland>
#include <cvt/jis0201>
#include <cvt/jis>
#include <cvt/jis_0208>
#include <cvt/ksc5601>
#include <cvt/latin2>
#include <cvt/one_one>
#include <cvt/roman>
#include <cvt/sjis>
#include <cvt/sjis_0208>
#include <cvt/turkish>
#include <cvt/utf16>
#include <cvt/utf8>
#include <cvt/utf8_utf16>
#include <cvt/wbuffer>
#include <cvt/wstring>
#include <cvt/xjis>
#include <cvt/xone_byte>
#include <cvt/xtwo_byte>

#define INSTANTIATE(...)   \
    do {                   \
        __VA_ARGS__ val{}; \
        (void) val;        \
    } while (0)

template <typename CharType>
void cvt_test_impl() {
    // use explicit namespace to avoid disambiguation for
    // types with similar names in std (like codecvt_utf16)
    INSTANTIATE(stdext::cvt::codecvt_8859_1<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_10<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_13<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_14<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_15<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_16<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_2<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_3<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_4<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_5<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_6<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_7<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_8<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_8859_9<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_baltic<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_big5<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp037<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1006<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1026<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1250<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1251<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1252<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1253<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1254<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1255<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1256<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1257<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp1258<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp424<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp437<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp500<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp737<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp775<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp850<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp852<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp855<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp856<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp857<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp860<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp861<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp862<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp863<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp864<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp865<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp866<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp869<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp874<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp875<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp932<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp936<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp949<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cp950<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_cyrillic<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_ebcdic<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_euc<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_euc_0208<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_gb12345<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_gb2312<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_greek<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_iceland<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_jis<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_jis0201<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_jis_0208<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_ksc5601<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_latin2<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_one_one<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_roman<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_sjis<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_sjis_0208<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_turkish<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_utf16<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_utf8<CharType>);
    INSTANTIATE(stdext::cvt::codecvt_utf8_utf16<CharType>);
    INSTANTIATE(stdext::cvt::wbuffer_convert<stdext::cvt::codecvt_utf16<CharType>>);
    INSTANTIATE(stdext::cvt::wstring_convert<stdext::cvt::codecvt_utf16<CharType>>);
}

void cvt_test() {
    cvt_test_impl<wchar_t>();
}

#endif // _M_CEE_PURE
