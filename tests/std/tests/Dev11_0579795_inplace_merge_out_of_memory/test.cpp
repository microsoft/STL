// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <new>

#ifdef __cpp_lib_execution
#include <execution>
#endif // __cpp_lib_execution

using namespace std;

constexpr auto size_max = numeric_limits<size_t>::max();

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

constexpr size_t prohibit_attempts_to_allocate = size_max; // extension provided by our STL
constexpr size_t max_allocate                  = size_max - 1;
size_t g_max_memory                            = max_allocate;

void* operator new(size_t size) {
    void* const p = ::operator new(size, nothrow);

    if (p) {
        return p;
    }

    abort();
}

void* operator new(size_t size, const nothrow_t&) noexcept {
    assert(size != prohibit_attempts_to_allocate);
    if (size >= g_max_memory) {
        return nullptr;
    } else {
        return malloc(size == 0 ? 1 : size);
    }
}

void operator delete(void* ptr) noexcept {
    ::operator delete(ptr, nothrow);
}

void operator delete(void* ptr, const nothrow_t&) noexcept {
    free(ptr);
}

void* operator new[](size_t size) {
    return ::operator new(size);
}

void* operator new[](size_t size, const nothrow_t&) noexcept {
    return ::operator new(size, nothrow);
}

void operator delete[](void* ptr) noexcept {
    ::operator delete(ptr);
}

void operator delete[](void* ptr, const nothrow_t&) noexcept {
    ::operator delete(ptr, nothrow);
}

void test() {
    array<int, 9> a = {{2, 4, 6, 8, 10, 6, -100, -200, -300}};

    inplace_merge(a.begin(), a.begin() + 5, a.begin() + 6);

    const array<int, 9> correct = {{2, 4, 6, 6, 8, 10, -100, -200, -300}};

    assert(a == correct);
}

void do_one_test(const array<int, 9>& orig, const size_t i, const size_t k) {
    array<int, 9> a = orig;

    inplace_merge(a.begin(), a.begin() + static_cast<ptrdiff_t>(i), a.begin() + static_cast<ptrdiff_t>(i + k));

    assert(is_permutation(a.begin(), a.begin() + static_cast<ptrdiff_t>(i + k), orig.begin()));
    assert(is_sorted(a.begin(), a.begin() + static_cast<ptrdiff_t>(i + k)));
    assert(equal(a.begin() + static_cast<ptrdiff_t>(i + k), a.end(), orig.begin() + static_cast<ptrdiff_t>(i + k)));
}

void test_every_permutation_with_available_memory_of(size_t mem) {
    g_max_memory    = mem * sizeof(int);
    array<int, 9> a = {{10, 20, 20, 30, 30, 30, 40, 40, 50}};

    do {
        for (size_t i = 0; i <= a.size(); ++i) {
            for (size_t k = 0; k <= a.size() - i; ++k) {
                if (is_sorted(a.begin(), a.begin() + static_cast<ptrdiff_t>(i))
                    && is_sorted(a.begin() + static_cast<ptrdiff_t>(i), a.begin() + static_cast<ptrdiff_t>(i + k))) {
                    do_one_test(a, i, k);
                }
            }
        }
    } while (next_permutation(a.begin(), a.end()));
}

void test_more() {
    for (size_t mem = 0; mem < 9; ++mem) {
        test_every_permutation_with_available_memory_of(mem);
    }
}

struct int_ish {
    /* implicit */ int_ish(int init) : value(init) {}
    int_ish(const int_ish&)            = delete;
    int_ish(int_ish&&)                 = default;
    int_ish& operator=(const int_ish&) = delete;

    int_ish& operator=(int_ish&& rhs) {
        assert(this != &rhs); // tests VSO-254106
        value = rhs.value;
        return *this;
    }

    bool operator==(const int_ish& rhs) const {
        return value == rhs.value;
    }

    int value;
};

void test_stability() {
    auto comp = [](int x, int y) { return x / 10 < y / 10; };

    {
        array<int, 100> a = {{20, 13, 58, 85, 29, 91, 28, 99, 64, 17, 60, 44, 48, 86, 26, 89, 47, 43, 20, 51, 96, 66,
            75, 19, 35, 65, 99, 42, 39, 80, 66, 18, 87, 64, 76, 33, 96, 55, 26, 98, 63, 95, 40, 65, 79, 16, 63, 68, 55,
            27, 99, 55, 77, 59, 77, 11, 77, 92, 77, 16, 28, 83, 74, 77, 21, 55, 50, 73, 65, 81, 53, 68, 82, 72, 92, 64,
            30, 94, 80, 86, 21, 37, 20, 90, 56, 71, 41, 15, 42, 70, 50, 90, 26, 72, 47, 41, 46, 41, 58, 94}};
#ifdef __cpp_lib_execution
        auto b = a;
#endif

        stable_sort(a.begin(), a.end(), comp);

        const array<int, 100> correct = {
            {13, 17, 19, 18, 16, 11, 16, 15, 20, 29, 28, 26, 20, 26, 27, 28, 21, 21, 20, 26, 35, 39, 33, 30, 37, 44, 48,
                47, 43, 42, 40, 41, 42, 47, 41, 46, 41, 58, 51, 55, 55, 55, 59, 55, 50, 53, 56, 50, 58, 64, 60, 66, 65,
                66, 64, 63, 65, 63, 68, 65, 68, 64, 75, 76, 79, 77, 77, 77, 77, 74, 77, 73, 72, 71, 70, 72, 85, 86, 89,
                80, 87, 83, 81, 82, 80, 86, 91, 99, 96, 99, 96, 98, 95, 99, 92, 92, 94, 90, 90, 94}};

        assert(a == correct);

#ifdef __cpp_lib_execution
        stable_sort(execution::par, b.begin(), b.end(), comp);

        assert(b == correct);
#endif
    }

    {
        array<int, 100> a = {{13, 17, 19, 18, 16, 20, 29, 28, 26, 20, 26, 27, 35, 39, 33, 44, 48, 47, 43, 42, 40, 58,
            51, 55, 55, 64, 60, 66, 65, 66, 64, 63, 65, 63, 68, 75, 76, 79, 85, 86, 89, 80, 87, 91, 99, 96, 99, 96, 98,
            95, 11, 16, 15, 28, 21, 21, 20, 26, 30, 37, 41, 42, 47, 41, 46, 41, 55, 59, 55, 50, 53, 56, 50, 58, 65, 68,
            64, 77, 77, 77, 77, 74, 77, 73, 72, 71, 70, 72, 83, 81, 82, 80, 86, 99, 92, 92, 94, 90, 90, 94}};

        inplace_merge(a.begin(), a.begin() + 50, a.end(), comp);

        const array<int, 100> correct = {
            {13, 17, 19, 18, 16, 11, 16, 15, 20, 29, 28, 26, 20, 26, 27, 28, 21, 21, 20, 26, 35, 39, 33, 30, 37, 44, 48,
                47, 43, 42, 40, 41, 42, 47, 41, 46, 41, 58, 51, 55, 55, 55, 59, 55, 50, 53, 56, 50, 58, 64, 60, 66, 65,
                66, 64, 63, 65, 63, 68, 65, 68, 64, 75, 76, 79, 77, 77, 77, 77, 74, 77, 73, 72, 71, 70, 72, 85, 86, 89,
                80, 87, 83, 81, 82, 80, 86, 91, 99, 96, 99, 96, 98, 95, 99, 92, 92, 94, 90, 90, 94}};

        assert(a == correct);
    }

    {
        array<int_ish, 100> a = {{20, 13, 58, 85, 29, 91, 28, 99, 64, 17, 60, 44, 48, 86, 26, 89, 47, 43, 20, 51, 96,
            66, 75, 19, 35, 65, 99, 42, 39, 80, 66, 18, 87, 64, 76, 33, 96, 55, 26, 98, 63, 95, 40, 65, 79, 16, 63, 68,
            55, 27, 99, 55, 77, 59, 77, 11, 77, 92, 77, 16, 28, 83, 74, 77, 21, 55, 50, 73, 65, 81, 53, 68, 82, 72, 92,
            64, 30, 94, 80, 86, 21, 37, 20, 90, 56, 71, 41, 15, 42, 70, 50, 90, 26, 72, 47, 41, 46, 41, 58, 94}};

        size_t predCalls{};
        const auto partitionPoint         = stable_partition(a.begin(), a.end(), [&](const int_ish& x) {
            ++predCalls;
            return x.value < 50;
        });
        const array<int_ish, 100> correct = {
            {20, 13, 29, 28, 17, 44, 48, 26, 47, 43, 20, 19, 35, 42, 39, 18, 33, 26, 40, 16, 27, 11, 16, 28, 21, 30, 21,
                37, 20, 41, 15, 42, 26, 47, 41, 46, 41, 58, 85, 91, 99, 64, 60, 86, 89, 51, 96, 66, 75, 65, 99, 80, 66,
                87, 64, 76, 96, 55, 98, 63, 95, 65, 79, 63, 68, 55, 99, 55, 77, 59, 77, 77, 92, 77, 83, 74, 77, 55, 50,
                73, 65, 81, 53, 68, 82, 72, 92, 64, 94, 80, 86, 90, 56, 71, 70, 50, 90, 72, 58, 94}};

        assert(predCalls == 100);
        assert(partitionPoint == a.begin() + 37);
        assert(a == correct);
    }
}

void test_after_skipping_in_position_overhaul() {
    g_max_memory = prohibit_attempts_to_allocate;
    { // check that 1 element trivial cases don't fail
        array<int, 1> a = {{5}};
        inplace_merge(a.begin(), a.begin(), a.begin()); // empty degenerate case
        inplace_merge(a.begin(), a.begin(), a.end()); // first partition eliminates everything degenerate case
        inplace_merge(a.begin(), a.end(), a.end()); // second partition eliminates everything degenerate case
    }

    // already sorted case
    do_one_test({{0, 1, 2, 3, 4, 5, 6, 7, 8}}, 4, 5);
    // move 1 element from left partition to right partition case
    do_one_test({{9, 1, 2, 3, 4, 5, 6, 7, 8}}, 1, 8);
    // move 1 element from right partition to left partition case
    do_one_test({{1, 2, 3, 4, 5, 6, 7, 8, 0}}, 8, 1);

    g_max_memory = max_allocate;
    // buffer left and merge left case
    do_one_test({{7, 8, 0, 1, 2, 3, 4, 5, 6}}, 2, 7);
    // buffer right and merge right case
    do_one_test({{2, 3, 4, 5, 6, 7, 8, 0, 1}}, 7, 2);

    // (divide and conquer cases tested in test_more)
}

int main() {
    test();
    test_stability();

    g_max_memory = 0;

    test();
    test_stability();

    test_after_skipping_in_position_overhaul();
    test_more();
}
