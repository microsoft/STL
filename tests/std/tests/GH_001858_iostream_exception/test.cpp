// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING

#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <strstream>
#include <type_traits>
#include <utility>

#if _HAS_CXX20
#include <syncstream>
#endif // _HAS_CXX20

#if _HAS_CXX23
#include <spanstream>
#endif // _HAS_CXX23

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

struct test_exception {};

template <class CharT>
class throwing_buffer : public basic_streambuf<CharT> {
public:
    streampos seekoff(streamoff, ios_base::seekdir, ios_base::openmode = ios_base::in | ios_base::out) override {
        throw test_exception{};
    }

    streampos seekpos(streampos, ios_base::openmode = ios_base::in | ios_base::out) override {
        throw test_exception{};
    }

    int sync() override {
        throw test_exception{};
    }

    basic_streambuf<CharT>* to_buf() {
        return this;
    }
};

template <class CharT>
void test_istream_exceptions() {
    throwing_buffer<CharT> buffer;

    { // sync
        basic_istream<CharT> is(buffer.to_buf());
        assert(!is.bad());
        assert(is.sync() == -1);
        assert(is.bad());
    }

    { // sync with exceptions
        basic_istream<CharT> is(buffer.to_buf());
        is.exceptions(ios_base::badbit);

        try {
            is.sync();
            assert(false);
        } catch (const ios_base::failure&) {
            assert(false);
        } catch (const test_exception&) {
            // Expected case
        }
    }

    { // seekg
        basic_istream<CharT> is(buffer.to_buf());
        assert(!is.bad());
        is.seekg(0);
        assert(is.bad());
    }

    { // seekg with exceptions
        basic_istream<CharT> is(buffer.to_buf());
        is.exceptions(ios_base::badbit);

        try {
            is.seekg(0);
            assert(false);
        } catch (const ios_base::failure&) {
            assert(false);
        } catch (const test_exception&) {
            // Expected case
        }
    }

    { // seekg
        basic_istream<CharT> is(buffer.to_buf());
        assert(!is.bad());
        is.seekg(0, ios_base::beg);
        assert(is.bad());
    }

    { // seekg with exceptions
        basic_istream<CharT> is(buffer.to_buf());
        is.exceptions(ios_base::badbit);

        try {
            is.seekg(0, ios_base::beg);
            assert(false);
        } catch (const ios_base::failure&) {
            assert(false);
        } catch (const test_exception&) {
            // Expected case
        }
    }

    { // tellg
        basic_istream<CharT> is(buffer.to_buf());
        assert(!is.bad());
        assert(is.tellg() == -1);
        assert(is.bad());
    }

    { // tellg with exceptions
        basic_istream<CharT> is(buffer.to_buf());
        is.exceptions(ios_base::badbit);

        try {
            is.tellg();
            assert(false);
        } catch (const ios_base::failure&) {
            assert(false);
        } catch (const test_exception&) {
            // Expected case
        }
    }
}

template <class CharT>
void test_ostream_exceptions() {
    throwing_buffer<CharT> buffer;

    { // flush
        basic_ostream<CharT> os(buffer.to_buf());
        assert(!os.bad());
        os.flush();
        assert(os.bad());
    }

    { // flush with exceptions
        basic_ostream<CharT> os(buffer.to_buf());
        os.exceptions(ios_base::badbit);

        try {
            os.flush();
            assert(false);
        } catch (const ios_base::failure&) {
            assert(false);
        } catch (const test_exception&) {
            // Expected case
        }
    }

    { // seekp
        basic_ostream<CharT> os(buffer.to_buf());
        assert(!os.bad());
        os.seekp(0);
        assert(os.bad());
    }

    { // seekp with exceptions
        basic_ostream<CharT> os(buffer.to_buf());
        os.exceptions(ios_base::badbit);

        try {
            os.seekp(0);
            assert(false);
        } catch (const ios_base::failure&) {
            assert(false);
        } catch (const test_exception&) {
            // Expected case
        }
    }

    { // seekp
        basic_ostream<CharT> os(buffer.to_buf());
        assert(!os.bad());
        os.seekp(0, ios_base::beg);
        assert(os.bad());
    }

    { // seekp with exceptions
        basic_ostream<CharT> os(buffer.to_buf());
        os.exceptions(ios_base::badbit);

        try {
            os.seekp(0, ios_base::beg);
            assert(false);
        } catch (const ios_base::failure&) {
            assert(false);
        } catch (const test_exception&) {
            // Expected case
        }
    }

    { // tellp
        basic_ostream<CharT> os(buffer.to_buf());
        assert(!os.bad());
        assert(os.tellp() == -1);
        assert(os.bad());
    }

    { // tellp with exceptions
        basic_ostream<CharT> os(buffer.to_buf());
        os.exceptions(ios_base::badbit);

        try {
            os.tellp();
            assert(false);
        } catch (const ios_base::failure&) {
            assert(false);
        } catch (const test_exception&) {
            // Expected case
        }
    }
}

// Also test strengthened and mandatory exception specifications.
template <class, class = void>
constexpr bool has_nothrow_rdbuf = false;

template <class T>
constexpr bool has_nothrow_rdbuf<T, void_t<decltype(declval<const T&>().rdbuf())>> =
    noexcept(declval<const T&>().rdbuf());

template <class, class = void>
constexpr bool has_nothrow_is_open = false;

template <class T>
constexpr bool has_nothrow_is_open<T, void_t<decltype(declval<const T&>().is_open())>> =
    noexcept(declval<const T&>().is_open());

template <class, class = void>
constexpr bool is_nothrow_std_swappable = false;

template <class T>
constexpr bool is_nothrow_std_swappable<T, void_t<decltype(std::swap(declval<T&>(), declval<T&>()))>> =
    noexcept(std::swap(declval<T&>(), declval<T&>()));

STATIC_ASSERT(noexcept(static_cast<bool>(cin)));
STATIC_ASSERT(noexcept(!cin));

STATIC_ASSERT(noexcept(cin.rdstate()));
STATIC_ASSERT(noexcept(cin.good()));
STATIC_ASSERT(noexcept(cin.eof()));
STATIC_ASSERT(noexcept(cin.fail()));
STATIC_ASSERT(noexcept(cin.bad()));
STATIC_ASSERT(noexcept(cin.exceptions()));
STATIC_ASSERT(noexcept(cin.flags()));
STATIC_ASSERT(noexcept(cin.flags(ios_base::fmtflags{})));
STATIC_ASSERT(noexcept(cin.setf(ios_base::fmtflags{})));
STATIC_ASSERT(noexcept(cin.setf(ios_base::fmtflags{}, ios_base::fmtflags{})));
STATIC_ASSERT(noexcept(cin.unsetf(ios_base::fmtflags{})));
STATIC_ASSERT(noexcept(cin.precision()));
STATIC_ASSERT(noexcept(cin.precision(0)));
STATIC_ASSERT(noexcept(cin.width()));
STATIC_ASSERT(noexcept(cin.width(0)));
STATIC_ASSERT(noexcept(cin.getloc()));

STATIC_ASSERT(noexcept(cin.tie()));
STATIC_ASSERT(noexcept(cin.tie(nullptr)));

STATIC_ASSERT(noexcept(cin.fill()));
STATIC_ASSERT(noexcept(cin.fill('*')));

STATIC_ASSERT(noexcept(cin.gcount()));

STATIC_ASSERT(!has_nothrow_rdbuf<ios_base>);
STATIC_ASSERT(!has_nothrow_is_open<ios_base>);

STATIC_ASSERT(!is_nothrow_std_swappable<ios_base>);

STATIC_ASSERT(!is_nothrow_std_swappable<streambuf>);
STATIC_ASSERT(!is_nothrow_std_swappable<wstreambuf>);

STATIC_ASSERT(has_nothrow_rdbuf<ios>);
STATIC_ASSERT(has_nothrow_rdbuf<wios>);

STATIC_ASSERT(has_nothrow_rdbuf<ifstream>);
STATIC_ASSERT(has_nothrow_rdbuf<wifstream>);
STATIC_ASSERT(has_nothrow_rdbuf<ofstream>);
STATIC_ASSERT(has_nothrow_rdbuf<wofstream>);
STATIC_ASSERT(has_nothrow_rdbuf<fstream>);
STATIC_ASSERT(has_nothrow_rdbuf<wfstream>);

STATIC_ASSERT(is_nothrow_std_swappable<filebuf>);
STATIC_ASSERT(is_nothrow_std_swappable<wfilebuf>);
STATIC_ASSERT(is_nothrow_std_swappable<ifstream>);
STATIC_ASSERT(is_nothrow_std_swappable<wifstream>);
STATIC_ASSERT(is_nothrow_std_swappable<ofstream>);
STATIC_ASSERT(is_nothrow_std_swappable<wofstream>);
STATIC_ASSERT(is_nothrow_std_swappable<fstream>);
STATIC_ASSERT(is_nothrow_std_swappable<wfstream>);

STATIC_ASSERT(has_nothrow_is_open<filebuf>);
STATIC_ASSERT(has_nothrow_is_open<wfilebuf>);
STATIC_ASSERT(has_nothrow_is_open<ifstream>);
STATIC_ASSERT(has_nothrow_is_open<wifstream>);
STATIC_ASSERT(has_nothrow_is_open<ofstream>);
STATIC_ASSERT(has_nothrow_is_open<wofstream>);
STATIC_ASSERT(has_nothrow_is_open<fstream>);
STATIC_ASSERT(has_nothrow_is_open<wfstream>);

STATIC_ASSERT(is_nothrow_move_assignable_v<stringbuf>);
STATIC_ASSERT(is_nothrow_move_assignable_v<wstringbuf>);
STATIC_ASSERT(is_nothrow_move_assignable_v<istringstream>);
STATIC_ASSERT(is_nothrow_move_assignable_v<wistringstream>);
STATIC_ASSERT(is_nothrow_move_assignable_v<ostringstream>);
STATIC_ASSERT(is_nothrow_move_assignable_v<wostringstream>);
STATIC_ASSERT(is_nothrow_move_assignable_v<stringstream>);
STATIC_ASSERT(is_nothrow_move_assignable_v<wstringstream>);

STATIC_ASSERT(is_nothrow_std_swappable<stringbuf>);
STATIC_ASSERT(is_nothrow_std_swappable<wstringbuf>);
STATIC_ASSERT(is_nothrow_std_swappable<istringstream>);
STATIC_ASSERT(is_nothrow_std_swappable<wistringstream>);
STATIC_ASSERT(is_nothrow_std_swappable<ostringstream>);
STATIC_ASSERT(is_nothrow_std_swappable<wostringstream>);
STATIC_ASSERT(is_nothrow_std_swappable<stringstream>);
STATIC_ASSERT(is_nothrow_std_swappable<wstringstream>);

STATIC_ASSERT(has_nothrow_rdbuf<istringstream>);
STATIC_ASSERT(has_nothrow_rdbuf<wistringstream>);
STATIC_ASSERT(has_nothrow_rdbuf<ostringstream>);
STATIC_ASSERT(has_nothrow_rdbuf<wostringstream>);
STATIC_ASSERT(has_nothrow_rdbuf<stringstream>);
STATIC_ASSERT(has_nothrow_rdbuf<wstringstream>);

STATIC_ASSERT(is_nothrow_move_assignable_v<strstreambuf>);
STATIC_ASSERT(is_nothrow_move_assignable_v<istrstream>);
STATIC_ASSERT(is_nothrow_move_assignable_v<ostrstream>);
STATIC_ASSERT(is_nothrow_move_assignable_v<strstream>);

STATIC_ASSERT(noexcept(declval<strstreambuf&>().clear()));

STATIC_ASSERT(noexcept(declval<strstreambuf&>().freeze()));
STATIC_ASSERT(noexcept(declval<ostrstream&>().freeze()));
STATIC_ASSERT(noexcept(declval<strstream&>().freeze()));

STATIC_ASSERT(noexcept(declval<strstreambuf&>().str()));
STATIC_ASSERT(noexcept(declval<istrstream&>().str()));
STATIC_ASSERT(noexcept(declval<ostrstream&>().str()));
STATIC_ASSERT(noexcept(declval<strstream&>().str()));

STATIC_ASSERT(noexcept(declval<strstreambuf&>().pcount()));
STATIC_ASSERT(noexcept(declval<ostrstream&>().pcount()));
STATIC_ASSERT(noexcept(declval<strstream&>().pcount()));

#if !_HAS_CXX23 // non-Standard overloads
STATIC_ASSERT(is_nothrow_std_swappable<strstreambuf>);
STATIC_ASSERT(is_nothrow_std_swappable<istrstream>);
STATIC_ASSERT(is_nothrow_std_swappable<ostrstream>);
STATIC_ASSERT(is_nothrow_std_swappable<strstream>);
#endif // !_HAS_CXX23

#if _HAS_CXX17
static_assert(is_nothrow_swappable_v<filebuf>);
static_assert(is_nothrow_swappable_v<wfilebuf>);
static_assert(is_nothrow_swappable_v<ifstream>);
static_assert(is_nothrow_swappable_v<wifstream>);
static_assert(is_nothrow_swappable_v<ofstream>);
static_assert(is_nothrow_swappable_v<wofstream>);
static_assert(is_nothrow_swappable_v<fstream>);
static_assert(is_nothrow_swappable_v<wfstream>);

static_assert(is_nothrow_swappable_v<stringbuf>);
static_assert(is_nothrow_swappable_v<wstringbuf>);
static_assert(is_nothrow_swappable_v<istringstream>);
static_assert(is_nothrow_swappable_v<wistringstream>);
static_assert(is_nothrow_swappable_v<ostringstream>);
static_assert(is_nothrow_swappable_v<wostringstream>);
static_assert(is_nothrow_swappable_v<stringstream>);
static_assert(is_nothrow_swappable_v<wstringstream>);
#endif // _HAS_CXX17

#if _HAS_CXX20
static_assert(is_nothrow_swappable_v<syncbuf>);
static_assert(is_nothrow_swappable_v<wsyncbuf>);

static_assert(has_nothrow_rdbuf<osyncstream>); // mandatory
static_assert(has_nothrow_rdbuf<wosyncstream>); // mandatory
#endif // _HAS_CXX20

#if _HAS_CXX23
static_assert(is_nothrow_swappable_v<spanbuf>);
static_assert(is_nothrow_swappable_v<wspanbuf>);
static_assert(is_nothrow_swappable_v<ispanstream>);
static_assert(is_nothrow_swappable_v<wispanstream>);
static_assert(is_nothrow_swappable_v<ospanstream>);
static_assert(is_nothrow_swappable_v<wospanstream>);
static_assert(is_nothrow_swappable_v<spanstream>);
static_assert(is_nothrow_swappable_v<wspanstream>);

static_assert(is_nothrow_move_assignable_v<spanbuf>);
static_assert(is_nothrow_move_assignable_v<wspanbuf>);
static_assert(is_nothrow_move_assignable_v<ispanstream>);
static_assert(is_nothrow_move_assignable_v<wispanstream>);
static_assert(is_nothrow_move_assignable_v<ospanstream>);
static_assert(is_nothrow_move_assignable_v<wospanstream>);
static_assert(is_nothrow_move_assignable_v<spanstream>);
static_assert(is_nothrow_move_assignable_v<wspanstream>);

static_assert(has_nothrow_rdbuf<ispanstream>); // mandatory
static_assert(has_nothrow_rdbuf<wispanstream>); // mandatory
static_assert(has_nothrow_rdbuf<ospanstream>); // mandatory
static_assert(has_nothrow_rdbuf<wospanstream>); // mandatory
static_assert(has_nothrow_rdbuf<spanstream>); // mandatory
static_assert(has_nothrow_rdbuf<wspanstream>); // mandatory
#endif // _HAS_CXX23

int main() {
    test_istream_exceptions<char>();
    test_istream_exceptions<wchar_t>();

    test_ostream_exceptions<char>();
    test_ostream_exceptions<wchar_t>();
}
