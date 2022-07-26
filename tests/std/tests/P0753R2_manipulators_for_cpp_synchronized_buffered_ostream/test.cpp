// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ios>
#include <memory>
#include <sstream>
#include <streambuf>
#include <string>
#include <syncstream>
#include <type_traits>

using namespace std;

template <class Ty, bool ThrowOnSync = false>
class string_buffer : public basic_streambuf<Ty, char_traits<Ty>> { // represents the wrapped object in syncbuf
public:
    string_buffer()  = default;
    ~string_buffer() = default;

    streamsize xsputn(const Ty* ptr, streamsize n) override {
        str.append(ptr, static_cast<string::size_type>(n));
        return n;
    }

    int sync() override {
        if constexpr (ThrowOnSync) {
            return -1;
        } else {
            return 0;
        }
    }

    string str;
};


template <class Ty, class Alloc = void, bool ThrowOnSync = false>
void test_osyncstream_manipulators(
    string_buffer<typename Ty::char_type, ThrowOnSync>* buf = nullptr, bool buffer_can_sync = true) {
    using char_type   = typename Ty::char_type;
    using traits_type = typename Ty::traits_type;

    static_assert(is_base_of_v<basic_ostream<char_type, traits_type>, Ty>);

    Ty os{buf};
    os << "Some input";

    assert(addressof(emit_on_flush(os)) == addressof(os));
    if constexpr (is_base_of_v<basic_osyncstream<char_type, traits_type, Alloc>, Ty>) {
        const auto aSyncbuf = static_cast<basic_syncbuf<char_type, traits_type, Alloc>*>(os.rdbuf());
        assert(aSyncbuf->_Stl_internal_check_get_emit_on_sync() == true);
        if (buf) {
            assert(buf->str == "");
        }
    }

    assert(addressof(flush_emit(os)) == addressof(os));
    if constexpr (is_base_of_v<basic_osyncstream<char_type, traits_type, Alloc>, Ty>) {
        if constexpr (ThrowOnSync) {
            assert(os.rdstate() == ios_base::badbit);
        } else {
            if (buf) {
                assert(os.rdstate() == ios_base::goodbit);
            } else {
                assert(os.rdstate() == ios_base::badbit);
            }
        }

        if (buf) {
            assert(buf->str == "Some input");
            buf->str.clear();
        }
        os.clear();
        os << "Another input";
    }

    assert(addressof(noemit_on_flush(os)) == addressof(os));
    if constexpr (is_base_of_v<basic_osyncstream<char_type, traits_type, Alloc>, Ty>) {
        const auto aSyncbuf = static_cast<basic_syncbuf<char_type, traits_type, Alloc>*>(os.rdbuf());
        assert(aSyncbuf->_Stl_internal_check_get_emit_on_sync() == false);
    }

    assert(addressof(flush_emit(os)) == addressof(os));
    if constexpr (is_base_of_v<basic_osyncstream<char_type, traits_type, Alloc>, Ty>) {
        const auto state = (buf && buffer_can_sync) ? ios_base::goodbit : ios_base::badbit;
        assert(os.rdstate() == state);
        if (buf) {
            assert(buf->str == "Another input");
        }
        os.clear();
    }

    if (buf) {
        buf->str.clear();
    }
}

template <class Ty>
class throwing_string_buffer : public basic_streambuf<Ty, char_traits<Ty>> {
public:
    throwing_string_buffer()  = default;
    ~throwing_string_buffer() = default;

    streamsize xsputn(const Ty*, streamsize) override {
        throw ios_base::failure{"test exception"};
    }
};

void test_lwg_3571() {
    // LWG-3571: "flush_emit should set badbit if the emit call fails"
    {
        osyncstream stream(nullptr);
        stream << flush_emit;
        assert(stream.rdstate() == ios_base::badbit);
    }
    {
        stringstream inner;
        osyncstream stream(inner);
        stream << "Hello World";
        stream.setstate(ios_base::failbit);
        assert((stream.rdstate() & ios_base::badbit) == ios_base::goodbit);
        stream << flush_emit;
        assert(stream.rdstate() & ios_base::badbit);
        assert(inner.str() == "");
    }
    {
        throwing_string_buffer<char> inner;
        osyncstream stream(&inner);
        stream << "Hello World";
        stream.exceptions(ios_base::failbit | ios_base::badbit);
        assert((stream.rdstate() & ios_base::badbit) == ios_base::goodbit);
        try {
            stream << flush_emit;
            assert(false);
        } catch (const ios_base::failure&) {
            assert(stream.rdstate() & ios_base::badbit);
        }
    }
}

void test_lwg_3570() {
    // LWG-3570: "basic_osyncstream::emit should be an unformatted output function"
    {
        stringstream inner;
        osyncstream stream(inner);
        stream << "Hello World";
        stream.setstate(ios_base::failbit);
        assert((stream.rdstate() & ios_base::badbit) == ios_base::goodbit);
        stream.emit();
        assert(stream.rdstate() & ios_base::badbit);
        assert(inner.str() == "");
    }
    {
        throwing_string_buffer<char> inner;
        osyncstream stream(&inner);
        stream << "Hello World";
        stream.exceptions(ios_base::failbit | ios_base::badbit);
        assert((stream.rdstate() & ios_base::badbit) == ios_base::goodbit);
        try {
            stream.emit();
            assert(false);
        } catch (const ios_base::failure&) {
            assert(stream.rdstate() & ios_base::badbit);
        }
    }
}

int main() {
    string_buffer<char> char_buffer{};
    string_buffer<char, true> no_sync_char_buffer{};

    test_osyncstream_manipulators<basic_ostream<char>>();
    test_osyncstream_manipulators<basic_osyncstream<char, char_traits<char>, allocator<char>>, allocator<char>>();

    test_osyncstream_manipulators<basic_ostream<char>>(&char_buffer);
    test_osyncstream_manipulators<basic_osyncstream<char, char_traits<char>, allocator<char>>, allocator<char>>(
        &char_buffer);

    test_osyncstream_manipulators<basic_ostream<char>>(&no_sync_char_buffer);
    test_osyncstream_manipulators<basic_osyncstream<char, char_traits<char>, allocator<char>>, allocator<char>>(
        &no_sync_char_buffer, false);

    test_lwg_3571();
    test_lwg_3570();
}
