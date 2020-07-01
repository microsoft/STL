// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
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

int main() {
    run_test<test_rvalue>();
    run_test<test_allocator>();
}
