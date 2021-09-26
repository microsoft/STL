// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ios>
#include <istream>
#include <ostream>
#include <streambuf>

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

int main() {
    test_istream_exceptions<char>();
    test_istream_exceptions<wchar_t>();

    test_ostream_exceptions<char>();
    test_ostream_exceptions<wchar_t>();
}
