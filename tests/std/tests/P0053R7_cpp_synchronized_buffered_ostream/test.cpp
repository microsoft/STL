// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test.hpp"

using namespace std;

static_assert(is_default_constructible_v<syncbuf>);
static_assert(is_constructible_v<syncbuf, typename syncbuf::streambuf_type*>);
static_assert(is_constructible_v<syncbuf, typename syncbuf::streambuf_type*, typename syncbuf::allocator_type>);
static_assert(is_move_constructible_v<syncbuf>);
static_assert(is_move_assignable_v<syncbuf>);
static_assert(is_destructible_v<syncbuf>);

template <class Elem, class Traits, class Alloc>
class test_syncbuf : public basic_syncbuf<Elem, Traits, Alloc> {
public:
    using size_type      = typename Alloc::size_type;
    using value_type     = typename Alloc::value_type;
    using _Mybase        = basic_syncbuf<Elem, Traits, Alloc>;
    using streambuf_type = typename _Mybase::streambuf_type;

    using _Mybase::epptr;
    using _Mybase::pbase;
    using _Mybase::pptr;

    test_syncbuf() = default;

    explicit test_syncbuf(streambuf_type* _Strbuf) : _Mybase(_Strbuf) {}

    test_syncbuf(streambuf_type* _Strbuf, const Alloc& _Al) : _Mybase(_Strbuf, _Al) {}

    test_syncbuf(test_syncbuf&&) = default;
    test_syncbuf& operator=(test_syncbuf&&) = default;
    ~test_syncbuf()                         = default;

    auto Test_get_buffer_size() const noexcept {
        return static_cast<size_type>(epptr() - pbase());
    }
    auto Test_get_data_size() const noexcept {
        return static_cast<size_type>(pptr() - pbase());
    }

    test_syncbuf(const test_syncbuf&) = delete;
    test_syncbuf& operator=(const test_syncbuf&) = delete;
};

template <class Alloc>
void test_syncbuf_member_functions(string_buffer<typename Alloc::value_type>* buf = nullptr) {

    using value_type = typename Alloc::value_type;
    using Syncbuf    = test_syncbuf<value_type, char_traits<value_type>, Alloc>;
    using OStream    = basic_ostream<value_type, char_traits<value_type>>;

    Alloc alloc{};

    Syncbuf aSyncbuf{buf, alloc};

    // test construction post-conditions
    assert(aSyncbuf.get_wrapped() == buf);
    assert(aSyncbuf.get_allocator() == alloc);
    assert(aSyncbuf.Test_get_data_size() == 0);
    assert(aSyncbuf.Test_get_buffer_size() == _Min_syncbuf_size);

    // check emit post-conditions with no input
    if (buf) {
        assert(aSyncbuf.emit() == true);
    } else {
        assert(aSyncbuf.emit() == false);
    }

    OStream os{&aSyncbuf};

    os << "A small string\n";
    assert(aSyncbuf.Test_get_data_size() == 15);
    if (buf) {
        assert(aSyncbuf.emit() == true);
        assert(buf->str == "A small string\n");
        buf->str.clear();
    } else {
        assert(os.rdstate() == ios::goodbit);
        assert(aSyncbuf.emit() == false);
    }

    os << "A string holds more than 32 characters"; // requires one re-allocation
    if (buf) {
        assert(aSyncbuf.Test_get_data_size() == 38);
        assert(aSyncbuf.emit() == true);
        assert(buf->str == "A string holds more than 32 characters");
        buf->str.clear();
    } else {
        assert(aSyncbuf.Test_get_data_size() == _Min_syncbuf_size); // if _Wrapped is nullptr, re-allocation will not
                                                                    // occur and will return a _Traits::eof bit
        assert(os.rdstate() == ios::badbit);
        os.setstate(ios::goodbit);
        assert(aSyncbuf.emit() == false);
    }

    os << "A string that will definitely overflow the small_size_allocator"; // requires more than one re-allocation
    if (buf) {
        if constexpr (is_base_of_v<small_size_allocation, Alloc>) { // fail to allocate enough memory
            assert(aSyncbuf.Test_get_data_size() == _Min_size_allocation);
            assert(os.rdstate() == ios::badbit);
            os.setstate(ios::goodbit);
            assert(aSyncbuf.emit() == true);
            assert(buf->str == "A string that will definitely overflow the small_s");
        } else {
            assert(aSyncbuf.Test_get_data_size() == 63);
            assert(os.rdstate() == ios::goodbit);
            assert(aSyncbuf.emit() == true);
            assert(buf->str == "A string that will definitely overflow the small_size_allocator");
        }
        buf->str.clear();
    } else {
        assert(aSyncbuf.Test_get_data_size() == _Min_syncbuf_size);
        assert(os.rdstate() == ios::badbit);
        os.setstate(ios::goodbit);
        assert(aSyncbuf.emit() == false);
    }
}

template <class Alloc, bool ThrowOnSync = false>
void test_syncbuf_synchronization(string_buffer<typename Alloc::value_type, ThrowOnSync>* buf) {
    assert(buf); // meaningless to run with nullptr

    using value_type = typename Alloc::value_type;
    using Syncbuf    = test_syncbuf<value_type, char_traits<value_type>, Alloc>;
    using OStream    = basic_ostream<value_type, char_traits<value_type>>;

    {
        Syncbuf buf1{buf};
        OStream os1{&buf1};
        os1 << "Last element ";
        {
            Syncbuf buf2{buf};
            buf2.set_emit_on_sync(false);
            OStream os2{&buf2};
            os2 << "Second element ";
            int syncResult = buf2.pubsync(); // trigger a sync
            assert(syncResult == 0);
            {
                Syncbuf buf3{buf};
                OStream{&buf3} << "First element to be presented!\n";
            }
            os2 << "to be presented!\n";
        }
        os1 << "to be presented!\n";
    }
    assert(
        buf->str == "First element to be presented!\nSecond element to be presented!\nLast element to be presented!\n");
    buf->str.clear();
    {
        Syncbuf buf1{buf};
        OStream os1{&buf1};
        os1 << "Last element ";
        {
            Syncbuf buf2{buf};
            buf2.set_emit_on_sync(true);
            OStream os2{&buf2};
            os2 << "First element to by emitted by sync!\n";
            int syncResult = buf2.pubsync(); // trigger a sync
            if constexpr (ThrowOnSync) {
                assert(syncResult == -1);
            } else {
                assert(syncResult == 0);
            }
            {
                Syncbuf buf3{buf};
                OStream{&buf3} << "Second element to be presented!\n";
            }
            os2 << "Third element to be presented!\n";
        }
        os1 << "to be presented!\n";
    }
    assert(buf->str
           == "First element to by emitted by sync!\nSecond element to be presented!\nThird element to be "
              "presented!\nLast element to be presented!\n");
    buf->str.clear();
}

template <class Alloc>
void test_syncbuf_move_swap_operations(string_buffer<typename Alloc::value_type>* buf) {

    using value_type = typename Alloc::value_type;
    using Syncbuf    = test_syncbuf<value_type, char_traits<value_type>, Alloc>;
    using OStream    = basic_ostream<value_type, char_traits<value_type>>;

    { // test move constructor
        Syncbuf buf1{buf};
        OStream(&buf1) << "Some input";
        auto buf1WrappedObject = buf1.get_wrapped();
        auto buf1BufferSize    = buf1.Test_get_buffer_size();
        auto buf1DataSize      = buf1.Test_get_data_size();
        Syncbuf buf2{move(buf1)};
        assert(buf->str == "");

        // move constructor post-conditions
        assert(buf2.get_wrapped() == buf1WrappedObject);
        assert(buf2.Test_get_buffer_size() == buf1BufferSize);
        assert(buf2.Test_get_data_size() == buf1DataSize);
        assert(buf1.get_wrapped() == nullptr);
        assert(buf1.pbase() == buf1.pptr());
        assert(buf1.Test_get_buffer_size() == 0);
        assert(buf1.Test_get_data_size() == 0);
    }
    assert(buf->str == "Some input");
    buf->str.clear();
    { // test move assignment
        Syncbuf buf1{buf};
        OStream{&buf1} << "Some input";
        Syncbuf buf2{nullptr};
        auto buf1WrappedObject = buf1.get_wrapped();
        auto buf1BufferSize    = buf1.Test_get_buffer_size();
        auto buf1DataSize      = buf1.Test_get_data_size();
        buf2                   = move(buf1);

        // move assignment post-conditions
        assert(buf2.get_wrapped() == buf1WrappedObject);
        assert(buf2.Test_get_buffer_size() == buf1BufferSize);
        assert(buf2.Test_get_data_size() == buf1DataSize);
        assert(buf1.get_wrapped() == nullptr);
        assert(buf1.Test_get_buffer_size() == 0);
        assert(buf1.Test_get_data_size() == 0);

        if constexpr (allocator_traits<Alloc>::propagate_on_container_move_assignment::value
                      && is_same_v<allocator_traits<Alloc>::is_always_equal, true_type>) {
            assert(buf1.get_allocator() == buf2.get_allocator());
        } else if constexpr (is_same_v<allocator_traits<Alloc>::is_always_equal, true_type>) {
            assert(buf1.get_allocator() == buf2.get_allocator());
        } else {
            assert(buf1.get_allocator() != buf2.get_allocator());
        }
    }
    assert(buf->str == "Some input");
    buf->str.clear();
    { // test swap
        Syncbuf buf1{buf};
        OStream{&buf1} << "Some input that requires re-allocation";
        Syncbuf buf2{nullptr};
        auto buf1WrappedObject = buf1.get_wrapped();
        auto buf1BufferSize    = buf1.Test_get_buffer_size();
        auto buf1DataSize      = buf1.Test_get_data_size();
        auto buf2WrappedObject = buf2.get_wrapped();
        auto buf2BufferSize    = buf2.Test_get_buffer_size();
        auto buf2DataSize      = buf2.Test_get_data_size();
        if constexpr (allocator_traits<Alloc>::propagate_on_container_swap::value
                      || is_same_v<allocator_traits<Alloc>::is_always_equal, true_type>) {
            buf1.swap(buf2);
            assert(buf2.get_wrapped() == buf1WrappedObject);
            assert(buf2.Test_get_buffer_size() == buf1BufferSize);
            assert(buf2.Test_get_data_size() == buf1DataSize);
            assert(buf1.get_wrapped() == buf2WrappedObject);
            assert(buf1.Test_get_buffer_size() == buf2BufferSize);
            assert(buf1.Test_get_data_size() == buf2DataSize);
        }
    }
    assert(buf->str == "Some input that requires re-allocation");
    buf->str.clear();
}

int main() {
    string_buffer<char> char_buffer{};
    string_buffer<char, true> no_sync_char_buffer{};

    test_syncbuf_member_functions<allocator<char>>();
    test_syncbuf_member_functions<small_size_allocator<char>>();
    test_syncbuf_member_functions<fancy_ptr_allocator<char>>();
    test_syncbuf_member_functions<small_size_fancy_ptr_allocator<char>>();

    test_syncbuf_member_functions<small_size_allocator<char>>(&char_buffer);
    test_syncbuf_member_functions<allocator<char>>(&char_buffer);
    test_syncbuf_member_functions<fancy_ptr_allocator<char>>(&char_buffer);
    test_syncbuf_member_functions<small_size_fancy_ptr_allocator<char>>(&char_buffer);

    test_syncbuf_synchronization<allocator<char>>(&char_buffer);
    test_syncbuf_synchronization<allocator<char>>(&no_sync_char_buffer);
    test_syncbuf_synchronization<fancy_ptr_allocator<char>>(&char_buffer);
    test_syncbuf_synchronization<fancy_ptr_allocator<char>>(&no_sync_char_buffer);

    test_syncbuf_move_swap_operations<allocator<char>>(&char_buffer);
    test_syncbuf_move_swap_operations<fancy_ptr_allocator<char>>(&char_buffer);
    test_syncbuf_move_swap_operations<non_move_assignable_non_equal_allocator<char>>(&char_buffer);
    test_syncbuf_move_swap_operations<non_move_assignable_equal_allocator<char>>(&char_buffer);
    test_syncbuf_move_swap_operations<non_swapable_equal_allocator<char>>(&char_buffer);
}
