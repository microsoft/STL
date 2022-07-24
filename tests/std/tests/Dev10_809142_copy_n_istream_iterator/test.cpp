// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <forward_list>
#include <iterator>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void assert_same(const vector<int>& v, const array<int, 9>& a) {
    assert(equal(v.begin(), v.end(), a.begin(), a.end()));
}

template <typename Container>
void assert_zeroes(const Container& a) {
    assert(all_of(a.begin(), a.end(), [](const auto& c) { return c == 0; }));
}

// Compile time test covers:
// VSO-227957 - _Destroy_range fails to compile for iterator types
template <typename Container>
void compile_time_test_impl() {
    Container c{};
    uninitialized_copy_n(begin(c), 1, begin(c));
    uninitialized_copy(begin(c), end(c), begin(c));
    uninitialized_fill(begin(c), end(c), typename Container::value_type{});
    uninitialized_fill_n(begin(c), 1, typename Container::value_type{});
}

void compile_time_test() {
    compile_time_test_impl<forward_list<int>>();
    compile_time_test_impl<forward_list<string>>();
    compile_time_test_impl<list<int>>();
    compile_time_test_impl<list<string>>();
    compile_time_test_impl<vector<int>>();
    compile_time_test_impl<vector<string>>();
}

int main() {


    {
        istringstream iss("10 20 30 40 50 60 70 80 90");


        vector<int> v(9, 0);
        assert_zeroes(v);

        istream_iterator<int> isi(iss); // Load a fresh value into isi.

        // should be no-ops
        assert(copy_n(isi, -1, v.begin()) == v.begin());
        assert(copy_n(isi, 0, v.begin()) == v.begin());

        assert(copy_n(isi, 3, v.begin()) == v.begin() + 3);

        {
            const array<int, 9> arr = {{10, 20, 30, 0, 0, 0, 0, 0, 0}};
            assert_same(v, arr);
        }


        ++isi; // Load a fresh value into isi.

        assert(copy_n(isi, 2, v.begin() + 3) == v.begin() + 5);

        {
            const array<int, 9> arr = {{10, 20, 30, 40, 50, 0, 0, 0, 0}};
            assert_same(v, arr);
        }


        ++isi; // Load a fresh value into isi.

        assert(copy_n(isi, 1, v.begin() + 5) == v.begin() + 6);

        {
            const array<int, 9> arr = {{10, 20, 30, 40, 50, 60, 0, 0, 0}};
            assert_same(v, arr);
        }


        ++isi; // Load a fresh value into isi.

        assert(copy_n(isi, 0, v.begin() + 6) == v.begin() + 6);

        {
            const array<int, 9> arr = {{10, 20, 30, 40, 50, 60, 0, 0, 0}};
            assert_same(v, arr);
        }


        // A fresh value has already been loaded into isi.

        assert(copy_n(isi, 3, v.begin() + 6) == v.end());

        {
            const array<int, 9> arr = {{10, 20, 30, 40, 50, 60, 70, 80, 90}};
            assert_same(v, arr);
        }
    }

    // Note: uninitialized_copy_n is required to increment the input N times
    // see LWG-2471
    {
        istringstream iss("10 20 30 40 50 60 70 80 90 100 110 120");


        vector<int> v(9, 0);
        assert_zeroes(v);

        istream_iterator<int> isi(iss); // Load a fresh value into isi.

        // should be no-ops
        assert(uninitialized_copy_n(isi, -1, v.begin()) == v.begin());
        assert(uninitialized_copy_n(isi, 0, v.begin()) == v.begin());

        assert(uninitialized_copy_n(isi, 3, v.begin()) == v.begin() + 3);

        {
            const array<int, 9> arr = {{10, 20, 30, 0, 0, 0, 0, 0, 0}};
            assert_same(v, arr);
        }


        ++isi; // Load a fresh value into isi.

        assert(uninitialized_copy_n(isi, 2, v.begin() + 3) == v.begin() + 5);

        {
            const array<int, 9> arr = {{10, 20, 30, 50, 60, 0, 0, 0, 0}};
            assert_same(v, arr);
        }


        ++isi; // Load a fresh value into isi.

        assert(uninitialized_copy_n(isi, 1, v.begin() + 5) == v.begin() + 6);

        {
            const array<int, 9> arr = {{10, 20, 30, 50, 60, 80, 0, 0, 0}};
            assert_same(v, arr);
        }


        ++isi; // Load a fresh value into isi.

        assert(uninitialized_copy_n(isi, 0, v.begin() + 6) == v.begin() + 6);

        {
            const array<int, 9> arr = {{10, 20, 30, 50, 60, 80, 0, 0, 0}};
            assert_same(v, arr);
        }


        // A fresh value has already been loaded into isi.

        assert(uninitialized_copy_n(isi, 3, v.begin() + 6) == v.end());

        {
            const array<int, 9> arr = {{10, 20, 30, 50, 60, 80, 100, 110, 120}};
            assert_same(v, arr);
        }
    }

    // Verify that negative counts are handled properly
    {
        // engage memmove case
        const array<int, 9> arr = {{10, 20, 30, 50, 60, 80, 100, 110, 120}};
        array<int, 9> dest{};

        assert(copy_n(arr.begin(), 0, dest.begin()) == dest.begin());
        assert_zeroes(dest);
        assert(copy_n(arr.begin(), -1, dest.begin()) == dest.begin());
        assert_zeroes(dest);

        assert(uninitialized_copy_n(arr.begin(), 0, dest.begin()) == dest.begin());
        assert_zeroes(dest);
        assert(uninitialized_copy_n(arr.begin(), -1, dest.begin()) == dest.begin());
        assert_zeroes(dest);

        // forward iterator case
        forward_list<int> empty_flist;
        // following would trigger iterator debug checks in the bad case
        assert(copy_n(empty_flist.begin(), -1, dest.begin()) == dest.begin());
        assert(copy_n(empty_flist.begin(), 0, dest.begin()) == dest.begin());
        assert(uninitialized_copy_n(empty_flist.begin(), -1, dest.begin()) == dest.begin());
        assert(uninitialized_copy_n(empty_flist.begin(), 0, dest.begin()) == dest.begin());


        assert(fill_n(dest.begin(), 0, 42) == dest.begin());
        assert_zeroes(dest);
        assert(fill_n(dest.begin(), -1, 42) == dest.begin());
        assert_zeroes(dest);

        assert(uninitialized_fill_n(dest.begin(), 0, 42) == dest.begin());
        assert_zeroes(dest);
        assert(uninitialized_fill_n(dest.begin(), -1, 42) == dest.begin());
        assert_zeroes(dest);

        auto gen = []() -> int { abort(); };
        assert(generate_n(dest.begin(), 0, gen) == dest.begin());
        assert(generate_n(dest.begin(), -1, gen) == dest.begin());
    }
}
