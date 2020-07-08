// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory_resource>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <test_death.hpp>

using namespace std;

constexpr const char empty_string[] = "";
constexpr const char small_string[] = "a";
constexpr const char large_string[] =
    "This is a long long long long long long long long string to avoid small string optimization.";

template <typename Stream>
struct test_rvalue {
    void operator()(const string& init_value) {
        string buffer{init_value};
        Stream stream{move(buffer)};
        assert(stream.view() == init_value);
        assert(stream.str() == init_value);
        assert(stream.view() == init_value);
        assert(stream.rdbuf()->get_allocator() == init_value.get_allocator());
        // Move out the buffer, the underlying buffer should be empty.
        buffer = move(stream).str();
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

template <typename T>
struct counting_allocator {
    using value_type = T;

    std::allocator<T> allocator{};
    std::size_t count{0};

    T* allocate(std::size_t n) {
        count++;
        return allocator.allocate(n);
    }

    void deallocate(T* p, std::size_t n) {
        allocator.deallocate(p, n);
    }

    counting_allocator() {}

    template <typename U>
    explicit counting_allocator(const counting_allocator<U>& c) : count(c.count) {}
};

template <typename T, typename U>
bool operator==(const counting_allocator<T>& c1, const counting_allocator<U>& c2) {
    return c1.allocator == c2.allocator;
}

using counting_string        = std::basic_string<char, std::char_traits<char>, counting_allocator<char>>;
using counting_stringstream  = std::basic_stringstream<char, std::char_traits<char>, counting_allocator<char>>;
using counting_istringstream = std::basic_istringstream<char, std::char_traits<char>, counting_allocator<char>>;
using counting_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, counting_allocator<char>>;

template <typename Stream>
struct test_counting_allocator {
    void operator()(counting_string s) {
        Stream stream{move(s)};
        s = move(stream).str();
        stream.str(move(s));
        if constexpr (!std::is_same_v<Stream, counting_istringstream>) {
            assert(stream.rdbuf()->get_allocator().count == 1);
        }
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

template <const char* str>
void test_iterator_increment_zero() {
    string s      = str;
    auto iterator = s.begin();
    stringstream stream{move(s)};
    iterator += 0; // OK
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        run_test<test_rvalue>();
        run_test<test_allocator>();
        run_counting_test<test_counting_allocator>();

        test_iterator_increment_zero<small_string>();
        test_iterator_increment_zero<large_string>();
    });

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({test_iterator_dereference_death<small_string>, test_iterator_dereference_death<large_string>,
        test_iterator_operator_arrow_death<small_string>, test_iterator_operator_arrow_death<large_string>,
        test_iterator_increment_death<small_string>, test_iterator_increment_death<large_string>});
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
