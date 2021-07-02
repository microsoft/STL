// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <istream>
#include <ostream>

using namespace std;

template <class CharT>
class throwing_buffer : public basic_streambuf<CharT> {
public:
    streampos seekoff(streamoff, ios::seekdir, ios_base::openmode = ios_base::in | ios_base::out) override {
        throw 42;
    }

    streampos seekpos(streampos, ios_base::openmode = ios_base::in | ios_base::out) override {
        throw 42;
    }

    int sync() override {
        throw 42;
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

    { // seekg
        basic_istream<CharT> is(buffer.to_buf());
        assert(!is.bad());
        is.seekg(0);
        assert(is.bad());
    }

    { // seekg
        basic_istream<CharT> is(buffer.to_buf());
        assert(!is.bad());
        is.seekg(0, ios_base::beg);
        assert(is.bad());
    }

    { // tellg
        basic_istream<CharT> is(buffer.to_buf());
        assert(!is.bad());
        assert(is.tellg() == -1);
        assert(is.bad());
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

    { // seekp
        basic_ostream<CharT> os(buffer.to_buf());
        assert(!os.bad());
        os.seekp(0);
        assert(os.bad());
    }

    { // seekp
        basic_ostream<CharT> os(buffer.to_buf());
        assert(!os.bad());
        os.seekp(0, ios_base::beg);
        assert(os.bad());
    }

    { // tellp
        basic_ostream<CharT> os(buffer.to_buf());
        assert(!os.bad());
        assert(os.tellp() == -1);
        assert(os.bad());
    }
}

int main() {
    test_istream_exceptions<char>();
    test_istream_exceptions<wchar_t>();

    test_ostream_exceptions<char>();
    test_ostream_exceptions<wchar_t>();
}
