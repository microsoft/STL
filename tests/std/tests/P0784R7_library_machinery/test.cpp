// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#pragma warning(disable : 4582) // '%s': constructor is not implicitly called
#pragma warning(disable : 4583) // '%s': destructor is not implicitly called

using namespace std;

struct int_wrapper_copy {
    constexpr int_wrapper_copy() = default;
    constexpr int_wrapper_copy(const int v) : _val(v) {}

    constexpr int_wrapper_copy(const int_wrapper_copy& other) : _val(other._val) {}
    constexpr int_wrapper_copy& operator=(const int_wrapper_copy& other) {
        _val = other._val;
        return *this;
    }

    int_wrapper_copy(int_wrapper_copy&&)            = delete;
    int_wrapper_copy& operator=(int_wrapper_copy&&) = delete;

    constexpr bool operator==(const int_wrapper_copy&) const = default;

    int _val = 0;
};

struct int_wrapper_move {
    constexpr int_wrapper_move() = default;
    constexpr int_wrapper_move(const int v) : _val(v) {}

    int_wrapper_move(const int_wrapper_move&)            = delete;
    int_wrapper_move& operator=(const int_wrapper_move&) = delete;

    constexpr int_wrapper_move(int_wrapper_move&& other) : _val(exchange(other._val, -1)) {}
    constexpr int_wrapper_move& operator=(int_wrapper_move&& other) {
        _val = exchange(other._val, -1);
        return *this;
    }

    constexpr bool operator==(const int_wrapper_move&) const = default;

    int _val = 0;
};

static constexpr int_wrapper_copy expected_copy[]       = {1, 2, 3, 4};
static constexpr int_wrapper_move expected_move[]       = {1, 2, 3, 4};
static constexpr int_wrapper_move expected_after_move[] = {-1, -1, -1, -1};

constexpr bool test() {
    { // _Copy_unchecked
        int_wrapper_copy input[]   = {1, 2, 3, 4};
        int_wrapper_copy output[4] = {5, 6, 7, 8};

        const auto result = _STD _Copy_unchecked(begin(input), end(input), begin(output));
        static_assert(is_same_v<remove_const_t<decltype(result)>, int_wrapper_copy*>);
        assert(result == end(output));
        assert(equal(begin(expected_copy), end(expected_copy), begin(output), end(output)));
    }

    { // _Copy_n_unchecked4
        int_wrapper_copy input[]   = {1, 2, 3, 4};
        int_wrapper_copy output[4] = {5, 6, 7, 8};

        const auto result = _Copy_n_unchecked4(begin(input), size(input), begin(output));
        static_assert(is_same_v<remove_const_t<decltype(result)>, int_wrapper_copy*>);
        assert(result == end(output));
        assert(equal(begin(expected_copy), end(expected_copy), begin(output), end(output)));
    }

    { // _Copy_backward_unchecked
        int_wrapper_copy input[]   = {1, 2, 3, 4};
        int_wrapper_copy output[4] = {5, 6, 7, 8};

        const auto result = _Copy_backward_unchecked(begin(input), end(input), end(output));
        static_assert(is_same_v<remove_const_t<decltype(result)>, int_wrapper_copy*>);
        assert(result == begin(output));
        assert(equal(begin(expected_copy), end(expected_copy), begin(output), end(output)));
    }

    { // _Uninitialized_copy_unchecked
        int_wrapper_copy input[] = {1, 2, 3, 4};
        int_wrapper_copy output[4];

        const auto result = _Uninitialized_copy_unchecked(begin(input), end(input), begin(output));
        static_assert(is_same_v<remove_const_t<decltype(result)>, int_wrapper_copy*>);
        assert(result == end(output));
        assert(equal(begin(expected_copy), end(expected_copy), begin(output), end(output)));
    }

    { // _Move_unchecked
        int_wrapper_move input[]   = {1, 2, 3, 4};
        int_wrapper_move output[4] = {5, 6, 7, 8};

        const auto result = _STD _Move_unchecked(begin(input), end(input), begin(output));
        static_assert(is_same_v<remove_const_t<decltype(result)>, int_wrapper_move*>);
        assert(result == end(output));
        assert(equal(begin(expected_move), end(expected_move), begin(output), end(output)));
        if (is_constant_evaluated()) {
            assert(equal(begin(input), end(input), begin(expected_after_move), end(expected_after_move)));
        }
    }

    { // _Move_backward_unchecked
        int_wrapper_move input[]   = {1, 2, 3, 4};
        int_wrapper_move output[4] = {5, 6, 7, 8};

        const auto result = _Move_backward_unchecked(begin(input), end(input), end(output));
        static_assert(is_same_v<remove_const_t<decltype(result)>, int_wrapper_move*>);
        assert(result == begin(output));
        assert(equal(begin(expected_move), end(expected_move), begin(output), end(output)));
        if (is_constant_evaluated()) {
            assert(equal(begin(input), end(input), begin(expected_after_move), end(expected_after_move)));
        }
    }

#ifdef __cpp_lib_concepts
    { // _Move_backward_common
        int_wrapper_move input[]   = {1, 2, 3, 4};
        int_wrapper_move output[4] = {5, 6, 7, 8};

        const auto result = ranges::_Move_backward_common(begin(input), end(input), end(output));
        static_assert(is_same_v<remove_const_t<decltype(result)>, int_wrapper_move*>);
        assert(result == begin(output));
        assert(equal(begin(expected_move), end(expected_move), begin(output), end(output)));
        if (is_constant_evaluated()) {
            assert(equal(begin(input), end(input), begin(expected_after_move), end(expected_after_move)));
        }
    }
#endif // __cpp_lib_concepts

    { // _Uninitialized_move_unchecked
        int_wrapper_move input[] = {1, 2, 3, 4};
        int_wrapper_move output[4];

        const auto result = _Uninitialized_move_unchecked(begin(input), end(input), begin(output));
        static_assert(is_same_v<remove_const_t<decltype(result)>, int_wrapper_move*>);
        assert(result == end(output));
        assert(equal(begin(expected_move), end(expected_move), begin(output), end(output)));
        if (is_constant_evaluated()) {
            assert(equal(begin(input), end(input), begin(expected_after_move), end(expected_after_move)));
        }
    }

    return true;
}

int main() {
    test();
    static_assert(test());
}
