// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <iostream>
#include <memory_resource>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

using namespace std;

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
    test("");
    test("a");
    test("This is a long long long long long long long long string to avoid small string optimization.");
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

int main() {
    run_test<test_rvalue>();
    run_test<test_allocator>();
    run_counting_test<test_counting_allocator>();
}
