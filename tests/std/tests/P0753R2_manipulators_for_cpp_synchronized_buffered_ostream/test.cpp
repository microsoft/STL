#include <assert.h>
#include <string>
#include <syncstream>
#include <type_traits>

using namespace std;

template <class Ty, bool ThrowOnSync = false>
class string_buffer : public basic_streambuf<Ty, char_traits<Ty>> { // represents the wrapped object in syncbuf
public:
    string_buffer()  = default;
    ~string_buffer() = default;

    streamsize xsputn(const Ty* _Ptr, streamsize _Count) override {
        str.append(_Ptr, static_cast<string::size_type>(_Count));
        return _Count;
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

template <class Elem, class Traits>
class Test_Basic_syncbuf : public _Basic_syncbuf_impl<Elem, Traits> {
    using Mybase = _Basic_syncbuf_impl<Elem, Traits>;

public:
    Test_Basic_syncbuf()                            = default;
    Test_Basic_syncbuf(Test_Basic_syncbuf&& _Right) = default;

    using Mybase::_Emit_on_sync;
};

template <class Ty, class Alloc = void, bool ThrowOnSync = false>
requires(is_base_of_v<basic_ostream<typename Ty::char_type, typename Ty::traits_type>,
    Ty>) void test_osyncstream_manipulators(string_buffer<typename Ty::char_type, ThrowOnSync>* buf = nullptr) {
    using char_type   = typename Ty::char_type;
    using traits_type = typename Ty::traits_type;

    Ty os{buf};
    os << "Some input";

    assert(addressof(emit_on_flush(os)) == addressof(os));
    if constexpr (is_base_of_v<basic_osyncstream<char_type, traits_type, Alloc>, Ty>) {
        auto* aSyncbuf = reinterpret_cast<Test_Basic_syncbuf<char_type, traits_type>*>(os.rdbuf());
        assert(aSyncbuf->_Emit_on_sync == true);
        if (buf) {
            assert(buf->str == "");
        }
    }

    assert(addressof(flush_emit(os)) == addressof(os));
    if constexpr (is_base_of_v<basic_osyncstream<char_type, traits_type, Alloc>, Ty>) {
        if constexpr (ThrowOnSync) {
            assert(os.rdstate() == ios::badbit);
        } else {
            assert(os.rdstate() == (buf ? ios::goodbit : ios::badbit));
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
        auto* aSyncbuf = reinterpret_cast<Test_Basic_syncbuf<char_type, traits_type>*>(os.rdbuf());
        assert(aSyncbuf->_Emit_on_sync == false);
    }

    assert(addressof(flush_emit(os)) == addressof(os));
    if constexpr (is_base_of_v<basic_osyncstream<char_type, traits_type, Alloc>, Ty>) {
        if constexpr (ThrowOnSync) {
            assert(os.rdstate() == ios::badbit);
        } else {
            assert(os.rdstate() == (buf ? ios::goodbit : ios::badbit));
        }
        if (buf) {
            assert(buf->str == "Another input");
        }
        os.clear();
    }

    if (buf) {
        buf->str.clear();
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
        &no_sync_char_buffer);
}
