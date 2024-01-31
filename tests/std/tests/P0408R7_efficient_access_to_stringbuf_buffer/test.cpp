// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <ios>
#include <memory>
#include <memory_resource>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <test_death.hpp>

using namespace std;

constexpr char empty_string[] = "";
constexpr char small_string[] = "a";
constexpr char large_string[] =
    "This is a long long long long long long long long string to avoid small string optimization.";

template <typename Stream>
struct test_rvalue {
    void operator()(const string& init_value) {
        test(init_value, ios_base::in);
        test(init_value, ios_base::out);
        test(init_value, ios_base::app);
        test(init_value, ios_base::ate);
    }

    void test(const string& init_value, const ios_base::openmode mode) {
        string buffer{init_value};
        size_t res = buffer.capacity();
        Stream stream{move(buffer), mode};

        // If the stream cannot be written or read we do nothing
        const auto buffer_view = stream.rdbuf()->_Get_buffer_view();
        if (!buffer_view._Ptr) {
            assert(!(mode & ios_base::in) && !(mode & ios_base::out));
            assert(buffer == init_value);
            return;
        }

        assert(stream.view() == init_value);
        assert(stream.str() == init_value);
        assert(stream.view() == init_value);
        assert(stream.rdbuf()->get_allocator() == init_value.get_allocator());
        // Move out the buffer, the underlying buffer should be empty.
        buffer = move(stream).str();
        if (buffer == large_string) {
            // stream doesn't actually have space for a null-terminator
            assert(buffer.capacity() == res);
        } else {
            assert(buffer.capacity() == res + 1);
        }
        assert(buffer == init_value);
        assert(stream.view().empty());
        assert(stream.str().empty());
        // Move in the buffer string
        stream.str(move(buffer));
        assert(stream.view() == init_value);
        assert(stream.str() == init_value);
        assert(stream.rdbuf()->get_allocator() == init_value.get_allocator());
        // Move to another stream
        Stream stream2 = move(stream);
        assert(stream.view().empty());
        assert(stream2.view() == init_value);
    }
};

template <typename Stream>
struct test_allocator {
    void operator()(const pmr::string& init_value) {
        Stream stream{init_value};
        assert(stream.view() == init_value);
        assert(stream.str(init_value.get_allocator()) == init_value);
        assert(stream.str() == string_view{init_value});
        assert(move(stream).str() == string_view{init_value});
        assert(stream.view().empty());
        stream.str(init_value);
        assert(stream.view() == init_value);
        assert(stream.str(init_value.get_allocator()) == init_value);
        assert(stream.str() == string_view{init_value});
    }
};

template <typename Stream>
struct test_pmr_allocator {
    pmr::polymorphic_allocator<char> alloc;

    void operator()(const pmr::string& init_value) {
        Stream stream{init_value, alloc};
        assert(stream.rdbuf()->get_allocator() != init_value.get_allocator());
        {
            pmr::string new_str = stream.str(init_value.get_allocator());
            assert(new_str == init_value);
            assert(new_str.get_allocator() == init_value.get_allocator());
        }
        {
            pmr::string new_str = stream.str();
            assert(new_str == init_value);
            assert(new_str.get_allocator() != init_value.get_allocator());
        }
        stream.str(init_value);
        assert(stream.rdbuf()->get_allocator() != init_value.get_allocator());
        {
            pmr::string new_str = stream.str(init_value.get_allocator());
            assert(new_str == init_value);
            assert(new_str.get_allocator() == init_value.get_allocator());
        }
        {
            pmr::string new_str = stream.str();
            assert(new_str == init_value);
            assert(new_str.get_allocator() != init_value.get_allocator());
        }
        Stream stream2{init_value, init_value.get_allocator()};
        assert(stream2.rdbuf()->get_allocator() == init_value.get_allocator());

        // GH-4232: <sstream>: basic_stringbuf shouldn't implement moving with swapping

        // Move to another stream buffer with different allocators
        using SBuf = remove_pointer_t<decltype(stream.rdbuf())>;
        SBuf& sbuf = *stream.rdbuf();
        SBuf sbuf2{move(sbuf), init_value.get_allocator()};
        assert(sbuf2.get_allocator() != sbuf.get_allocator());
        assert(stream.view().empty());
        assert(sbuf2.view() == init_value);

        // Move assignment between different memory resources
        sbuf = move(sbuf2);
        assert(sbuf2.get_allocator() != sbuf.get_allocator());
        assert(sbuf2.view().empty());
        assert(stream.view() == init_value);
    }
};

template <typename Test>
void run_test_util() {
    Test test{};
    test(empty_string);
    test(small_string);
    test(large_string);
}

template <template <typename> typename Test>
void run_test() {
    run_test_util<Test<stringstream>>();
    run_test_util<Test<istringstream>>();
    run_test_util<Test<ostringstream>>();
}

using pmr_stringstream  = basic_stringstream<char, char_traits<char>, pmr::polymorphic_allocator<char>>;
using pmr_istringstream = basic_istringstream<char, char_traits<char>, pmr::polymorphic_allocator<char>>;
using pmr_ostringstream = basic_ostringstream<char, char_traits<char>, pmr::polymorphic_allocator<char>>;

template <typename Test>
void run_pmr_allocator_test_util() {
    pmr::monotonic_buffer_resource resource{};
    Test test{pmr::polymorphic_allocator<char>{&resource}};
    test(empty_string);
    test(small_string);
    test(large_string);
}

void run_pmr_allocator_test() {
    run_pmr_allocator_test_util<test_pmr_allocator<pmr_stringstream>>();
    run_pmr_allocator_test_util<test_pmr_allocator<pmr_istringstream>>();
    run_pmr_allocator_test_util<test_pmr_allocator<pmr_ostringstream>>();
}

template <typename T>
struct counting_allocator {
    using value_type = T;

    shared_ptr<size_t> count{make_shared<size_t>(size_t{0})};

    T* allocate(size_t n) {
        ++*count;
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) {
        allocator<T>{}.deallocate(p, n);
    }

    counting_allocator() {}

    template <typename U>
    explicit counting_allocator(const counting_allocator<U>& c) : count(c.count) {}
};

template <typename T, typename U>
bool operator==(const counting_allocator<T>&, const counting_allocator<U>&) {
    return true;
}

using counting_string        = basic_string<char, char_traits<char>, counting_allocator<char>>;
using counting_stringstream  = basic_stringstream<char, char_traits<char>, counting_allocator<char>>;
using counting_istringstream = basic_istringstream<char, char_traits<char>, counting_allocator<char>>;
using counting_ostringstream = basic_ostringstream<char, char_traits<char>, counting_allocator<char>>;

template <typename Stream>
struct test_counting_allocator {
    void operator()(counting_string s) {
        shared_ptr<size_t> pcount = s.get_allocator().count;
        Stream stream{move(s)};
        s = move(stream).str();
        stream.str(move(s));
        assert(*pcount == 1);
    }
};

template <template <typename> typename Test>
void run_counting_test() {
    run_test_util<Test<counting_stringstream>>();
    run_test_util<Test<counting_istringstream>>();
    run_test_util<Test<counting_ostringstream>>();
}

template <const char* str>
void test_iterator_dereference_death() {
    string s      = str;
    auto iterator = s.begin();
    stringstream stream{move(s)};
    (void) *iterator; // cannot dereference invalid iterator
}

template <const char* str>
void test_iterator_operator_arrow_death() {
    string s      = str;
    auto iterator = s.begin();
    stringstream stream{move(s)};
    (void) iterator.operator->(); // cannot dereference invalid iterator
}

template <const char* str>
void test_iterator_increment_death() {
    string s      = str;
    auto iterator = s.begin();
    stringstream stream{move(s)};
    ++iterator; // cannot increase invalid iterator
}

void test_iterator_increment_zero(const char* str) {
    string s      = str;
    auto iterator = s.begin();
    stringstream stream{move(s)};
    iterator += 0; // OK
}

class test_stringbuf : public stringbuf {
public:
    using stringbuf::basic_stringbuf;

    using stringbuf::eback;
    using stringbuf::egptr;
    using stringbuf::epptr;
    using stringbuf::gptr;
    using stringbuf::pbase;
    using stringbuf::pptr;
};

void test_init_buf_ptrs_out(test_stringbuf& buf) {
    assert(*buf.pbase() == large_string[0]);
    assert(buf.epptr() >= buf.pbase() + buf.view().size());
    assert(buf.pptr() == buf.pbase());
}

void test_init_buf_ptrs_out_ate(test_stringbuf& buf) {
    assert(*buf.pbase() == large_string[0]);
    assert(buf.epptr() >= buf.pbase() + buf.view().size());
    assert(buf.pptr() == buf.pbase() + buf.view().size());
}

void test_init_buf_ptrs_in(test_stringbuf& buf) {
    assert(*buf.eback() == large_string[0]);
    assert(buf.gptr() == buf.eback() && buf.egptr() == buf.eback() + buf.view().size());
}

void test_init_buf_ptrs() {
    test_stringbuf buf1{string{large_string}, ios_base::out};
    test_init_buf_ptrs_out(buf1);

    test_stringbuf buf2{string{large_string}, ios_base::out | ios_base::ate};
    test_init_buf_ptrs_out_ate(buf2);

    test_stringbuf buf3{string{large_string}, ios_base::in};
    test_init_buf_ptrs_in(buf3);

    test_stringbuf buf4{string{large_string}, ios_base::in | ios_base::out};
    test_init_buf_ptrs_out(buf4);
    test_init_buf_ptrs_in(buf4);

    test_stringbuf buf5{string{large_string}, ios_base::in | ios_base::out | ios_base::ate};
    test_init_buf_ptrs_out_ate(buf5);
    test_init_buf_ptrs_in(buf5);
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        run_test<test_rvalue>();
        run_test<test_allocator>();
        run_pmr_allocator_test();

#if _ITERATOR_DEBUG_LEVEL == 0
        run_counting_test<test_counting_allocator>();
#endif // _ITERATOR_DEBUG_LEVEL == 0

        test_iterator_increment_zero(small_string);
        test_iterator_increment_zero(large_string);

        test_init_buf_ptrs();
    });

#if _ITERATOR_DEBUG_LEVEL > 0
    exec.add_death_tests({
        test_iterator_dereference_death<small_string>,
        test_iterator_operator_arrow_death<small_string>,
        test_iterator_increment_death<small_string>,
    });
#endif // _ITERATOR_DEBUG_LEVEL > 0

#if _ITERATOR_DEBUG_LEVEL > 1
    exec.add_death_tests({
        test_iterator_dereference_death<large_string>,
        test_iterator_operator_arrow_death<large_string>,
        test_iterator_increment_death<large_string>,
    });
#endif // _ITERATOR_DEBUG_LEVEL > 1


    return exec.run(argc, argv);
}
