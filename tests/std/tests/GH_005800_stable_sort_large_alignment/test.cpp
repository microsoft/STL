// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma warning(disable : 6262) // Function uses '16388' bytes of stack.

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>

#if _HAS_CXX17
#include <execution>
#endif // _HAS_CXX17

using namespace std;

template <size_t N>
struct alignas(N) large_element {
    array<unsigned char, N> elems;

#if _HAS_CXX20
    friend auto operator<=>(const large_element&, const large_element&) = default;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    friend bool operator==(const large_element& lhs, const large_element& rhs) {
        return lhs.elems == rhs.elems;
    }

    friend bool operator!=(const large_element& lhs, const large_element& rhs) {
        return lhs.elems != rhs.elems;
    }

    friend bool operator<(const large_element& lhs, const large_element& rhs) {
        return lhs.elems < rhs.elems;
    }

    friend bool operator>(const large_element& lhs, const large_element& rhs) {
        return lhs.elems > rhs.elems;
    }

    friend bool operator<=(const large_element& lhs, const large_element& rhs) {
        return lhs.elems <= rhs.elems;
    }

    friend bool operator>=(const large_element& lhs, const large_element& rhs) {
        return lhs.elems >= rhs.elems;
    }
#endif // ^^^ !_HAS_CXX20 ^^^
};

struct alignment_verifying_less {
    template <class T, class U>
    bool operator()(const T& t, const U& u) const {
        assert(reinterpret_cast<uintptr_t>(&t) % alignof(T) == 0);
        assert(reinterpret_cast<uintptr_t>(&u) % alignof(U) == 0);
        return t < u;
    }
};

struct alignment_verifying_truth {
    template <class T>
    bool operator()(const T& t) const {
        assert(reinterpret_cast<uintptr_t>(&t) % alignof(T) == 0);
        return true;
    }
};

template <size_t N>
void test() {
    {
        large_element<N> arr[2]{};

        stable_sort(begin(arr), end(arr), alignment_verifying_less{});
        stable_partition(begin(arr), end(arr), alignment_verifying_truth{});
        inplace_merge(begin(arr), begin(arr), end(arr), alignment_verifying_less{});
    }

#if _HAS_CXX17
    auto test_execution = [](const auto& execpol) {
        large_element<N> arr[2]{};

        stable_sort(execpol, begin(arr), end(arr), alignment_verifying_less{});
        stable_partition(execpol, begin(arr), end(arr), alignment_verifying_truth{});
        inplace_merge(execpol, begin(arr), begin(arr), end(arr), alignment_verifying_less{});
    };
    test_execution(execution::seq);
    test_execution(execution::par);
    test_execution(execution::par_unseq);
#if _HAS_CXX20
    test_execution(execution::unseq);
#endif // _HAS_CXX20
#endif // _HAS_CXX17

#if _HAS_CXX20
    {
        large_element<N> arr[2]{};

        ranges::stable_sort(arr, alignment_verifying_less{});
        ranges::stable_partition(arr, alignment_verifying_truth{});
        ranges::inplace_merge(arr, ranges::begin(arr), alignment_verifying_less{});
    }
#endif // _HAS_CXX20
}

int main() {
    test<1>();
    test<2>();
    test<4>();
    test<8>();
    test<16>();
    test<32>();
    test<64>();
    test<128>();
    test<256>();
    test<512>();
    test<1024>();
    test<2048>();
    test<4096>();
    test<8192>();
}
