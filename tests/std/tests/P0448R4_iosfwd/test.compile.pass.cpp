// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iosfwd>

using namespace std;

// Needs to be separately declared as a simple alias of streampos
void test_forward_declaration(wstreampos*);
#ifdef __cpp_lib_char8_t
void test_forward_declaration(u8streampos*);
#endif // __cpp_lib_char8_t
void test_forward_declaration(u16streampos*);
void test_forward_declaration(u32streampos*);

struct test_aliases {
    static void test_forward_declaration(streampos*);

    static void test_forward_declaration(ios*);
    static void test_forward_declaration(wios*);

    static void test_forward_declaration(streambuf*);
    static void test_forward_declaration(istream*);
    static void test_forward_declaration(ostream*);
    static void test_forward_declaration(iostream*);

    static void test_forward_declaration(wstreambuf*);
    static void test_forward_declaration(wistream*);
    static void test_forward_declaration(wostream*);
    static void test_forward_declaration(wiostream*);

    static void test_forward_declaration(stringbuf*);
    static void test_forward_declaration(istringstream*);
    static void test_forward_declaration(ostringstream*);
    static void test_forward_declaration(stringstream*);

    static void test_forward_declaration(wstringbuf*);
    static void test_forward_declaration(wistringstream*);
    static void test_forward_declaration(wostringstream*);
    static void test_forward_declaration(wstringstream*);

#if _HAS_CXX23
    static void test_forward_declaration(spanbuf*);
    static void test_forward_declaration(ispanstream*);
    static void test_forward_declaration(ospanstream*);
    static void test_forward_declaration(spanstream*);

    static void test_forward_declaration(wspanbuf*);
    static void test_forward_declaration(wispanstream*);
    static void test_forward_declaration(wospanstream*);
    static void test_forward_declaration(wspanstream*);
#endif // _HAS_CXX23

    static void test_forward_declaration(filebuf*);
    static void test_forward_declaration(ifstream*);
    static void test_forward_declaration(ofstream*);
    static void test_forward_declaration(fstream*);

    static void test_forward_declaration(wfilebuf*);
    static void test_forward_declaration(wifstream*);
    static void test_forward_declaration(wofstream*);
    static void test_forward_declaration(wfstream*);

#if _HAS_CXX20
    static void test_forward_declaration(syncbuf*);
    static void test_forward_declaration(osyncstream*);

    static void test_forward_declaration(wsyncbuf*);
    static void test_forward_declaration(wosyncstream*);
#endif // _HAS_CXX20
};

template <class CharT>
struct test_forward_declarations {
    static void test_forward_declaration(allocator<CharT>*);

    static void test_forward_declaration(char_traits<CharT>*);
    static void test_forward_declaration(istreambuf_iterator<CharT>*);
    static void test_forward_declaration(ostreambuf_iterator<CharT>*);

    static void test_forward_declaration(basic_ios<CharT>*);

    static void test_forward_declaration(basic_streambuf<CharT>*);
    static void test_forward_declaration(basic_istream<CharT>*);
    static void test_forward_declaration(basic_ostream<CharT>*);
    static void test_forward_declaration(basic_iostream<CharT>*);

    static void test_forward_declaration(basic_stringbuf<CharT>*);
    static void test_forward_declaration(basic_istringstream<CharT>*);
    static void test_forward_declaration(basic_ostringstream<CharT>*);
    static void test_forward_declaration(basic_stringstream<CharT>*);

#if _HAS_CXX23
    static void test_forward_declaration(basic_spanbuf<CharT>*);
    static void test_forward_declaration(basic_ispanstream<CharT>*);
    static void test_forward_declaration(basic_ospanstream<CharT>*);
    static void test_forward_declaration(basic_spanstream<CharT>*);
#endif // _HAS_CXX23

    static void test_forward_declaration(basic_filebuf<CharT>*);
    static void test_forward_declaration(basic_ifstream<CharT>*);
    static void test_forward_declaration(basic_ofstream<CharT>*);
    static void test_forward_declaration(basic_fstream<CharT>*);

#if _HAS_CXX20
    static void test_forward_declaration(basic_syncbuf<CharT>*);
    static void test_forward_declaration(basic_osyncstream<CharT>*);
#endif // _HAS_CXX20
};

test_aliases aliases;
test_forward_declarations<char> forward_declarations_char;
test_forward_declarations<wchar_t> forward_declarations_wchar_t;
