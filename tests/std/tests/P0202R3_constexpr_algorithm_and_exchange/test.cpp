// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <assert.h>
#include <functional>
#include <iterator>
#include <stddef.h>

using namespace std;

template <class T>
struct input_pointer {
    using iterator_category = input_iterator_tag;
    using reference         = T&;
    using value_type        = T;
    using pointer           = T*;
    using difference_type   = ptrdiff_t;

    constexpr explicit input_pointer(T* const ptr_) : ptr(ptr_) {}

    constexpr T& operator*() const {
        return *ptr;
    }
    constexpr T* operator->() const {
        return ptr;
    }
    constexpr input_pointer& operator++() {
        ++ptr;
        return *this;
    }
    void operator++(int) = delete; // avoid postincrement
    constexpr bool operator==(const input_pointer& o) const {
        return ptr == o.ptr;
    }
    constexpr bool operator!=(const input_pointer& o) const {
        return ptr != o.ptr;
    }

    T* ptr;
};

template <class T>
struct forward_pointer {
    using iterator_category = forward_iterator_tag;
    using reference         = T&;
    using value_type        = T;
    using pointer           = T*;
    using difference_type   = ptrdiff_t;

    constexpr forward_pointer() : ptr{} {}
    constexpr explicit forward_pointer(T* const ptr_) : ptr(ptr_) {}

    constexpr T& operator*() const {
        return *ptr;
    }
    constexpr T* operator->() const {
        return ptr;
    }
    constexpr forward_pointer& operator++() {
        ++ptr;
        return *this;
    }
    void operator++(int) = delete; // avoid postincrement
    constexpr bool operator==(const forward_pointer& o) const {
        return ptr == o.ptr;
    }
    constexpr bool operator!=(const forward_pointer& o) const {
        return ptr != o.ptr;
    }

    T* ptr;
};

template <class T>
struct bidirectional_pointer {
    using iterator_category = bidirectional_iterator_tag;
    using reference         = T&;
    using value_type        = T;
    using pointer           = T*;
    using difference_type   = ptrdiff_t;

    constexpr bidirectional_pointer() : ptr{} {}
    constexpr explicit bidirectional_pointer(T* const ptr_) : ptr(ptr_) {}

    constexpr T& operator*() const {
        return *ptr;
    }
    constexpr T* operator->() const {
        return ptr;
    }
    constexpr bidirectional_pointer& operator++() {
        ++ptr;
        return *this;
    }
    void operator++(int) = delete; // avoid postincrement

    constexpr bidirectional_pointer& operator--() {
        --ptr;
        return *this;
    }
    void operator--(int) = delete; // avoid postdecrement

    constexpr bool operator==(const bidirectional_pointer& o) const {
        return ptr == o.ptr;
    }
    constexpr bool operator!=(const bidirectional_pointer& o) const {
        return ptr != o.ptr;
    }

    T* ptr;
};

template <class T>
struct output_pointer {
    using iterator_category = output_iterator_tag;

    constexpr explicit output_pointer(T* const ptr_) : ptr(ptr_) {}

    constexpr T& operator*() const {
        return *ptr;
    }
    constexpr T* operator->() const {
        return ptr;
    }
    constexpr output_pointer& operator++() {
        ++ptr;
        return *this;
    }
    void operator++(int) = delete; // avoid postincrement

    // note: output iterators aren't required to be comparable but we need to compare them for test
    // purposes
    constexpr bool operator==(const output_pointer& o) const {
        return ptr == o.ptr;
    }
    constexpr bool operator!=(const output_pointer& o) const {
        return ptr != o.ptr;
    }

    T* ptr;
};

// test all the algorithms that have is_constant_evaluated() calls

template <class I, class O>
constexpr bool test_copy() {
    const int a[] = {10, 20, 30};
    int b[3]{};
    assert(copy(I{begin(a)}, I{end(a)}, O{begin(b)}) == O{end(b)});
    assert(a[0] == 10);
    assert(a[1] == 20);
    assert(a[2] == 30);
    assert(b[0] == 10);
    assert(b[1] == 20);
    assert(b[2] == 30);
    return true;
}

template <class I, class O>
constexpr bool test_copy_n() {
    const int a[] = {10, 20, 30};
    int b[3]{};
    assert(copy_n(I{begin(a)}, 3, O{begin(b)}) == O{end(b)});
    assert(a[0] == 10);
    assert(a[1] == 20);
    assert(a[2] == 30);
    assert(b[0] == 10);
    assert(b[1] == 20);
    assert(b[2] == 30);
    return true;
}

template <class I, class O>
constexpr bool test_copy_backward() {
    const int a[] = {10, 20, 30};
    int b[3]{};
    assert(copy_backward(I{begin(a)}, I{end(a)}, O{end(b)}) == O{begin(b)});
    assert(a[0] == 10);
    assert(a[1] == 20);
    assert(a[2] == 30);
    assert(b[0] == 10);
    assert(b[1] == 20);
    assert(b[2] == 30);
    return true;
}

template <class I, class O>
constexpr bool test_move() {
    const int a[] = {10, 20, 30};
    int b[3]{};
    assert(move(I{begin(a)}, I{end(a)}, O{begin(b)}) == O{end(b)});
    assert(a[0] == 10);
    assert(a[1] == 20);
    assert(a[2] == 30);
    assert(b[0] == 10);
    assert(b[1] == 20);
    assert(b[2] == 30);
    return true;
}

template <class I, class O>
constexpr bool test_move_backward() {
    const int a[] = {10, 20, 30};
    int b[3]{};
    assert(move_backward(I{begin(a)}, I{end(a)}, O{end(b)}) == O{begin(b)});
    assert(a[0] == 10);
    assert(a[1] == 20);
    assert(a[2] == 30);
    assert(b[0] == 10);
    assert(b[1] == 20);
    assert(b[2] == 30);
    return true;
}

template <class O>
constexpr bool test_fill() {
    int a[] = {10, 20, 30};
    fill(O{begin(a)}, O{end(a)}, 20);
    assert(all_of(begin(a), end(a), [](int i) { return i == 20; }));
    return true;
}

template <class O>
constexpr bool test_fill_n() {
    int a[] = {10, 20, 30};
    fill_n(O{begin(a)}, size(a), 20);
    assert(all_of(begin(a), end(a), [](int i) { return i == 20; }));
    return true;
}

template <class I>
constexpr bool test_equal() {
    const int a[] = {10, 20, 30};
    const int b[] = {10, 20, 30};
    const int c[] = {40, 50, 60};
    assert(equal(I{begin(a)}, I{end(a)}, I{begin(a)}));
    assert(equal(I{begin(a)}, I{end(a)}, I{begin(b)}));
    assert(equal(I{begin(a)}, I{end(a)}, I{begin(c)}, not_equal_to{}));
    assert(equal(I{begin(a)}, I{end(a)}, I{begin(a)}, I{end(a)}));
    assert(equal(I{begin(a)}, I{end(a)}, I{begin(b)}, I{end(b)}));
    assert(equal(I{begin(a)}, I{end(a)}, I{begin(c)}, I{end(c)}, not_equal_to{}));

    assert(!equal(I{begin(a)}, I{end(a)}, I{begin(c)}));
    assert(!equal(I{begin(a)}, I{end(a)}, I{begin(c)}, I{end(c)}));
    assert(!equal(I{begin(a)}, I{end(a)}, I{begin(a)}, I{begin(a)}));
    assert(!equal(I{begin(a)}, I{end(a)}, I{begin(b)}, I{end(b)}, not_equal_to{}));
    assert(!equal(I{begin(a)}, I{end(a)}, I{begin(a)}, I{end(a)}, not_equal_to{}));
    return true;
}

template <class IntPtr, class CharPtr>
constexpr bool test_find() {
    const int a[] = {10, 20, 30};
    assert(find(IntPtr{begin(a)}, IntPtr{end(a)}, 20) == IntPtr{a + 1});

    const char b[] = {'\x0A', '\x14', '\x1E'};
    assert(find(CharPtr{begin(b)}, CharPtr{end(b)}, 20) == CharPtr{b + 1});
    return true;
}

template <class V, class IO>
constexpr bool test_reverse() {
    V a[] = {10, 20, 30};
    reverse(IO{begin(a)}, IO{end(a)});
    assert(a[0] == 30);
    assert(a[1] == 20);
    assert(a[2] == 10);
    return true;
}

// test remaining algorithms that had nothing tested by libcxx as of 2020-01-21

template <class I, class O>
constexpr bool test_rotate_copy() {
    const int a[] = {10, 20, 30};
    int b[3]{};
    assert(rotate_copy(I{begin(a)}, I{next(a)}, I{end(a)}, O{begin(b)}) == O{end(b)});
    assert(a[0] == 10);
    assert(a[1] == 20);
    assert(a[2] == 30);
    assert(b[0] == 20);
    assert(b[1] == 30);
    assert(b[2] == 10);
    return true;
}

template <class I, class O>
constexpr bool test_merge() {
    const int a[]        = {10, 20, 30, 40, 50, 60};
    const int b[]        = {20, 40, 60, 80, 100, 120};
    const int expected[] = {10, 20, 20, 30, 40, 40, 50, 60, 60, 80, 100, 120};
    int r[size(expected)]{};
    assert(merge(I{begin(a)}, I{end(a)}, I{begin(b)}, I{end(b)}, O{begin(r)}) == O{end(r)});
    assert(equal(begin(r), end(r), begin(expected), end(expected)));
    return true;
}

template <class I, class O>
constexpr bool test_set_union() {
    const int a[]        = {10, 20, 30, 40, 50, 60};
    const int b[]        = {20, 40, 60, 80, 100, 120};
    const int expected[] = {10, 20, 30, 40, 50, 60, 80, 100, 120};
    int r[size(expected)]{};
    assert(set_union(I{begin(a)}, I{end(a)}, I{begin(b)}, I{end(b)}, O{begin(r)}) == O{end(r)});
    assert(equal(begin(r), end(r), begin(expected), end(expected)));
    return true;
}

template <class I, class O>
constexpr bool test_set_difference() {
    const int a[] = {10, 20, 30, 40, 50, 60};
    const int b[] = {20, 40, 60, 80, 100, 120};
    int r[3]{};
    assert(set_difference(I{begin(a)}, I{end(a)}, I{begin(b)}, I{end(b)}, O{begin(r)}) == O{end(r)});
    assert(r[0] == 10);
    assert(r[1] == 30);
    assert(r[2] == 50);
    return true;
}

template <class I, class O>
constexpr bool test_set_symmetric_difference() {
    const int a[] = {10, 20, 30, 40, 50, 60};
    const int b[] = {20, 40, 60, 80, 100, 120};
    int r[6]{};
    assert(set_symmetric_difference(I{begin(a)}, I{end(a)}, I{begin(b)}, I{end(b)}, O{begin(r)}) == O{end(r)});
    assert(r[0] == 10);
    assert(r[1] == 30);
    assert(r[2] == 50);
    assert(r[3] == 80);
    assert(r[4] == 100);
    assert(r[5] == 120);
    return true;
}

// Also test P0879R0 constexpr for swapping functions

template <class IO>
constexpr bool test_rotate() {
    int a[] = {50, 60, 10, 20, 30, 40};
    assert(rotate(IO{begin(a)}, next(IO{begin(a)}, 2), IO{end(a)}) == IO{a + 4});
    assert(is_sorted(begin(a), end(a)));
    return true;
}

template <class IO>
constexpr bool test_partition() {
    const auto isNegative     = [](int b) { return b < 0; };
    int a[]                   = {10, 20, -10, -20, 100, 450, -1000};
    const auto partitionPoint = partition(IO{begin(a)}, IO{end(a)}, isNegative);
    assert(is_partitioned(begin(a), end(a), isNegative));
    assert(partitionPoint == IO{a + 3});
    assert(all_of(IO{begin(a)}, partitionPoint, isNegative));
    assert(none_of(partitionPoint, IO{end(a)}, isNegative));
    return true;
}

constexpr bool test_sort() {
    int a[] = {10, 20, -10, -20, 100, 450, -1000};
    sort(begin(a), end(a));
    assert(is_sorted(begin(a), end(a)));
    return true;
}

constexpr bool test_partial_sort() {
    int a[] = {1000, 10000, 2000, 20, 60, 80};
    partial_sort(begin(a), a + 3, end(a));
    assert(a[0] == 20);
    assert(a[1] == 60);
    assert(a[2] == 80);
    return true;
}

template <class I>
constexpr bool test_partial_sort_copy() {
    const int a[] = {1000, 10000, 2000, 20, 60, 80};
    int b[size(a) + 1]{};
    assert(partial_sort_copy(I{begin(a)}, I{end(a)}, begin(b), begin(b) + 3) == begin(b) + 3);
    assert(b[0] == 20);
    assert(b[1] == 60);
    assert(b[2] == 80);
    assert(partial_sort_copy(I{begin(a)}, I{end(a)}, begin(b), end(b)) == begin(b) + 6);
    assert(b[0] == 20);
    assert(b[1] == 60);
    assert(b[2] == 80);
    assert(b[3] == 1000);
    assert(b[4] == 2000);
    assert(b[5] == 10000);
    return true;
}

constexpr bool test_nth_element() {
    const auto isNegative = [](int b) { return b < 0; };
    int a[]               = {10, 20, -10, -20, 100, 450, -1000};
    nth_element(begin(a), begin(a) + 3, end(a));
    assert(all_of(begin(a), a + 2, isNegative));
    assert(a[2] == -10);
    assert(none_of(a + 4, end(a), isNegative));
    return true;
}

constexpr bool test_is_heap() {
    int buff[] = {
        1668617627,
        1429106719,
        -47163201,
        -441494788,
        -1200257975,
        -1459960308,
        -912489821,
        -2095681771,
        -1298559576,
        -1260655766,
    };

    assert(is_heap(begin(buff), end(buff)));
    swap(buff[0], buff[1]);
    assert(!is_heap(begin(buff), end(buff)));
    return true;
}

constexpr bool test_make_heap_and_sort_heap() {
    int buff[] = {
        -1200257975,
        -1260655766,
        -1298559576,
        -1459960308,
        -2095681771,
        -441494788,
        -47163201,
        -912489821,
        1429106719,
        1668617627,
    };

    make_heap(begin(buff), end(buff));
    assert(is_heap(begin(buff), end(buff)));
    sort_heap(begin(buff), end(buff));
    assert(is_sorted(begin(buff), end(buff)));
    return true;
}

constexpr bool test_pop_heap_and_push_heap() {
    int buff[] = {
        1668617627,
        1429106719,
        -47163201,
        -441494788,
        -1200257975,
        -1459960308,
        -912489821,
        -2095681771,
        -1298559576,
        -1260655766,
    };

    pop_heap(begin(buff), end(buff));
    const int expectedPopped[] = {
        1429106719,
        -441494788,
        -47163201,
        -1260655766,
        -1200257975,
        -1459960308,
        -912489821,
        -2095681771,
        -1298559576,
        1668617627,
    };

    assert(is_heap(begin(expectedPopped), end(expectedPopped) - 1));
    assert(equal(begin(buff), end(buff), begin(expectedPopped), end(expectedPopped)));

    push_heap(begin(buff), end(buff));
    const int expectedPushed[] = {
        1668617627,
        1429106719,
        -47163201,
        -1260655766,
        -441494788,
        -1459960308,
        -912489821,
        -2095681771,
        -1298559576,
        -1200257975,
    };
    assert(is_heap(begin(expectedPushed), end(expectedPushed)));
    assert(equal(begin(buff), end(buff), begin(expectedPushed), end(expectedPushed)));
    return true;
}

constexpr bool test_permutations() {
    int buff[] = {1, 2, 3, 4};
    // using std::array here is TRANSITION, DevCom-892153
    constexpr array<const int, 4> expected[] = {
        array<const int, 4>{10, 20, 30, 40},
        array<const int, 4>{10, 20, 40, 30},
        array<const int, 4>{10, 30, 20, 40},
        array<const int, 4>{10, 30, 40, 20},
        array<const int, 4>{10, 40, 20, 30},
        array<const int, 4>{10, 40, 30, 20},
        array<const int, 4>{20, 10, 30, 40},
        array<const int, 4>{20, 10, 40, 30},
        array<const int, 4>{20, 30, 10, 40},
        array<const int, 4>{20, 30, 40, 10},
        array<const int, 4>{20, 40, 10, 30},
        array<const int, 4>{20, 40, 30, 10},
        array<const int, 4>{30, 10, 20, 40},
        array<const int, 4>{30, 10, 40, 20},
        array<const int, 4>{30, 20, 10, 40},
        array<const int, 4>{30, 20, 40, 10},
        array<const int, 4>{30, 40, 10, 20},
        array<const int, 4>{30, 40, 20, 10},
        array<const int, 4>{40, 10, 20, 30},
        array<const int, 4>{40, 10, 30, 20},
        array<const int, 4>{40, 20, 10, 30},
        array<const int, 4>{40, 20, 30, 10},
        array<const int, 4>{40, 30, 10, 20},
        array<const int, 4>{40, 30, 20, 10},
    };

    auto cursor = begin(expected);
    do {
        assert(equal(begin(buff), end(buff), cursor->begin(), cursor->end()));
        ++cursor;
    } while (next_permutation(begin(buff), end(buff)));

    assert(cursor == end(expected));
    assert(!prev_permutation(begin(buff), end(buff)));

    do {
        assert(equal(begin(buff), end(buff), cursor->begin(), cursor->end()));
        --cursor;
    } while (prev_permutation(begin(buff), end(buff)));

    assert(is_sorted(begin(buff), end(buff)));
    return true;
}

constexpr bool test() {
    // clang-format off
    return
        test_copy<input_pointer<const int>, output_pointer<int>>()
        && test_copy<const int*, int*>()
        && test_copy_n<input_pointer<const int>, output_pointer<int>>()
        && test_copy_n<const int*, int*>()
        && test_copy_backward<bidirectional_pointer<const int>, bidirectional_pointer<int>>()
        && test_copy_backward<const int*, int*>()
        && test_move<input_pointer<const int>, output_pointer<int>>()
        && test_move<const int*, int*>()
        && test_move_backward<bidirectional_pointer<const int>, bidirectional_pointer<int>>()
        && test_move_backward<const int*, int*>()
        && test_fill<forward_pointer<int>>()
        && test_fill<int*>()
        && test_fill_n<forward_pointer<int>>()
        && test_fill_n<int*>()
        && test_equal<input_pointer<const int>>()
        && test_equal<const int*>()
        && test_find<input_pointer<const int>, input_pointer<const char>>()
        && test_find<const int*, const char*>()
        && test_reverse<char, bidirectional_pointer<char>>()
        && test_reverse<char, char*>()
        && test_reverse<short, bidirectional_pointer<short>>()
        && test_reverse<short, short*>()
        && test_reverse<int, bidirectional_pointer<int>>()
        && test_reverse<int, int*>()
        && test_reverse<long, bidirectional_pointer<long>>()
        && test_reverse<long, long*>()
        && test_reverse<long long, bidirectional_pointer<long long>>()
        && test_reverse<long long, long long*>()
        && test_rotate_copy<input_pointer<const int>, output_pointer<int>>()
        && test_rotate_copy<const int*, int*>()
        && test_merge<input_pointer<const int>, output_pointer<int>>()
        && test_merge<const int*, int*>()
        && test_set_union<input_pointer<const int>, output_pointer<int>>()
        && test_set_union<const int*, int*>()
        && test_set_difference<input_pointer<const int>, output_pointer<int>>()
        && test_set_difference<const int*, int*>()
        && test_set_symmetric_difference<input_pointer<const int>, output_pointer<int>>()
        && test_set_symmetric_difference<const int*, int*>()
        && test_rotate<bidirectional_pointer<int>>()
        && test_rotate<int*>()
        && test_partition<forward_pointer<int>>()
        && test_partition<bidirectional_pointer<int>>()
        && test_partition<int*>()
        && test_sort()
        && test_partial_sort()
        && test_partial_sort_copy<input_pointer<const int>>()
        && test_partial_sort_copy<const int*>()
        && test_nth_element()
        && test_is_heap()
        && test_make_heap_and_sort_heap()
        && test_pop_heap_and_push_heap()
        ;
    // clang-format on
}

int main() {
    test();
    static_assert(test());
}
