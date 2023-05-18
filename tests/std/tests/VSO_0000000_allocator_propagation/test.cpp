// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <forward_list>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <map>
#include <new>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

template <class Container>
_CONSTEXPR20 void assert_equal(const Container& cont, initializer_list<typename Container::value_type> il) {
    assert(equal(cont.begin(), cont.end(), il.begin(), il.end()));
}

template <class Container>
_CONSTEXPR20 void assert_is_permutation(const Container& cont, initializer_list<typename Container::value_type> il) {
    assert(is_permutation(cont.begin(), cont.end(), il.begin(), il.end()));
}

template <class T, class POCCA, class POCMA, class POCS, class EQUAL>
class MyAlloc {
private:
    size_t _id;

    [[nodiscard]] constexpr size_t equal_id() const noexcept {
        return is_always_equal::value ? 10 : _id;
    }

public:
    [[nodiscard]] constexpr size_t id() const noexcept {
        return _id;
    }

    using value_type = T;

    using propagate_on_container_copy_assignment = POCCA;
    using propagate_on_container_move_assignment = POCMA;
    using propagate_on_container_swap            = POCS;
    using is_always_equal                        = EQUAL;

    constexpr explicit MyAlloc(const size_t _id_) : _id(_id_) {}

    template <class U>
    constexpr MyAlloc(const MyAlloc<U, POCCA, POCMA, POCS, EQUAL>& other) noexcept : _id(other.id()) {}

    template <class U>
    [[nodiscard]] constexpr bool operator==(const MyAlloc<U, POCCA, POCMA, POCS, EQUAL>& other) const noexcept {
        return equal_id() == other.equal_id();
    }

    template <class U>
    [[nodiscard]] constexpr bool operator!=(const MyAlloc<U, POCCA, POCMA, POCS, EQUAL>& other) const noexcept {
        return equal_id() != other.equal_id();
    }

    [[nodiscard]] constexpr T* allocate(const size_t numElements) {
        return allocator<T>{}.allocate(numElements + equal_id()) + equal_id();
    }

    constexpr void deallocate(T* const first, const size_t numElements) noexcept {
        allocator<T>{}.deallocate(first - equal_id(), numElements + equal_id());
    }
};

template <class T>
using StationaryAlloc = MyAlloc<T, false_type, false_type, false_type, false_type>;
template <class T>
using CopyAlloc = MyAlloc<T, true_type, false_type, false_type, false_type>;
template <class T>
using CopyEqualAlloc = MyAlloc<T, true_type, false_type, false_type, true_type>;
template <class T>
using MoveAlloc = MyAlloc<T, false_type, true_type, false_type, false_type>;
template <class T>
using MoveEqualAlloc = MyAlloc<T, false_type, true_type, false_type, true_type>;
template <class T>
using SwapAlloc = MyAlloc<T, false_type, false_type, true_type, false_type>;
template <class T>
using SwapEqualAlloc = MyAlloc<T, false_type, false_type, true_type, true_type>;


template <template <class, class> class Sequence>
_CONSTEXPR20 void test_sequence_copy_ctor() {
    Sequence<int, StationaryAlloc<int>> src({10, 20, 30}, StationaryAlloc<int>(11));
    auto src_it = src.begin();

    Sequence<int, StationaryAlloc<int>> dst(src);
    auto dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_back(60);
    dst.push_back(70);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {40, 20, 30, 68});
    assert_equal(dst, {50, 20, 30, 79});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

template <template <class, class> class Sequence>
_CONSTEXPR20 void test_sequence_copy_alloc_ctor(const size_t id1, const size_t id2) {
    Sequence<int, StationaryAlloc<int>> src({10, 20, 30}, StationaryAlloc<int>(id1));
    auto src_it = src.begin();

    Sequence<int, StationaryAlloc<int>> dst(src, StationaryAlloc<int>(id2));
    auto dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_back(60);
    dst.push_back(70);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {40, 20, 30, 68});
    assert_equal(dst, {50, 20, 30, 79});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <template <class, class> class Sequence, class Alloc>
_CONSTEXPR20 void test_sequence_copy_assign(const size_t id1, const size_t id2, const size_t id3) {
    Sequence<int, Alloc> src({10, 20, 30}, Alloc(id1));
    Sequence<int, Alloc> dst({0, 0, 0}, Alloc(id2));

    auto src_it = src.begin();
    auto dst_it = dst.begin();

    dst = src;

    dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_back(60);
    dst.push_back(70);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {40, 20, 30, 68});
    assert_equal(dst, {50, 20, 30, 79});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <template <class, class> class Sequence>
_CONSTEXPR20 void test_sequence_move_ctor() {
    Sequence<int, StationaryAlloc<int>> src({10, 20, 30}, StationaryAlloc<int>(11));
    auto it1 = src.begin();

    Sequence<int, StationaryAlloc<int>> dst(move(src));
    auto it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_back(100);
    dst.push_back(110);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {90, 50, 60, 108});
    assert_equal(dst, {70, 80, 30, 119});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

template <template <class, class> class Sequence>
_CONSTEXPR20 void test_sequence_move_alloc_ctor(const size_t id1, const size_t id2) {
    Sequence<int, StationaryAlloc<int>> src({10, 20, 30}, StationaryAlloc<int>(id1));
    auto it1 = src.begin();

    Sequence<int, StationaryAlloc<int>> dst(move(src), StationaryAlloc<int>(id2));

    if (id1 != id2) {
        it1 = dst.begin();
    }

    auto it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_back(100);
    dst.push_back(110);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {90, 50, 60, 108});
    assert_equal(dst, {70, 80, 30, 119});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <template <class, class> class Sequence, class Alloc>
_CONSTEXPR20 void test_sequence_move_assign(const size_t id1, const size_t id2, const size_t id3) {
    Sequence<int, Alloc> src({10, 20, 30}, Alloc(id1));
    Sequence<int, Alloc> dst({0, 0, 0}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst = move(src);

    if (id1 != id3) {
        it1 = dst.begin();
    }

    it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_back(100);
    dst.push_back(110);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {90, 50, 60, 108});
    assert_equal(dst, {70, 80, 30, 119});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <template <class, class> class Sequence, class Alloc>
_CONSTEXPR20 void test_sequence_swap(const size_t id1, const size_t id2) {
    Sequence<int, Alloc> src({10, 20, 30}, Alloc(id1));
    Sequence<int, Alloc> dst({40, 50, 60}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst.swap(src);

    *it1 = 70;
    *it2 = 80;

    src.push_back(100);
    dst.push_back(110);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {80, 50, 60, 108});
    assert_equal(dst, {70, 20, 30, 119});

    assert(src.get_allocator().id() == id2);
    assert(dst.get_allocator().id() == id1);
}

template <template <class, class> class Sequence>
_CONSTEXPR20 bool test_sequence() {
    test_sequence_copy_ctor<Sequence>();

    test_sequence_copy_alloc_ctor<Sequence>(11, 11); // equal allocators
    test_sequence_copy_alloc_ctor<Sequence>(11, 22); // non-equal allocators

    test_sequence_copy_assign<Sequence, StationaryAlloc<int>>(11, 11, 11); // non-POCCA, equal allocators
    test_sequence_copy_assign<Sequence, StationaryAlloc<int>>(11, 22, 22); // non-POCCA, non-equal allocators
    test_sequence_copy_assign<Sequence, CopyAlloc<int>>(11, 11, 11); // POCCA, equal allocators
    test_sequence_copy_assign<Sequence, CopyAlloc<int>>(11, 22, 11); // POCCA, non-equal allocators
    test_sequence_copy_assign<Sequence, CopyEqualAlloc<int>>(11, 22, 11); // POCCA, always-equal allocators

    test_sequence_move_ctor<Sequence>();

    test_sequence_move_alloc_ctor<Sequence>(11, 11); // equal allocators
    test_sequence_move_alloc_ctor<Sequence>(11, 22); // non-equal allocators

    test_sequence_move_assign<Sequence, StationaryAlloc<int>>(11, 11, 11); // non-POCMA, equal allocators
    test_sequence_move_assign<Sequence, StationaryAlloc<int>>(11, 22, 22); // non-POCMA, non-equal allocators
    test_sequence_move_assign<Sequence, MoveAlloc<int>>(11, 11, 11); // POCMA, equal allocators
    test_sequence_move_assign<Sequence, MoveAlloc<int>>(11, 22, 11); // POCMA, non-equal allocators
    test_sequence_move_assign<Sequence, MoveEqualAlloc<int>>(11, 22, 11); // POCMA, always-equal allocators

    test_sequence_swap<Sequence, StationaryAlloc<int>>(11, 11); // non-POCS, equal allocators
    // UNDEFINED BEHAVIOR, NOT TESTED - non-POCS, non-equal allocators
    test_sequence_swap<Sequence, SwapAlloc<int>>(11, 11); // POCS, equal allocators
    test_sequence_swap<Sequence, SwapAlloc<int>>(11, 22); // POCS, non-equal allocators
    test_sequence_swap<Sequence, SwapEqualAlloc<int>>(11, 22); // POCS, always-equal allocators

    return true;
}


void test_flist_copy_ctor() {
    forward_list<int, StationaryAlloc<int>> src({10, 20, 30}, StationaryAlloc<int>(11));
    auto src_it = src.begin();

    forward_list<int, StationaryAlloc<int>> dst(src);
    auto dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_front(60);
    dst.push_front(70);

    *next(src.before_begin()) += 8;
    *next(dst.before_begin()) += 9;

    assert_equal(src, {68, 40, 20, 30});
    assert_equal(dst, {79, 50, 20, 30});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

void test_flist_copy_alloc_ctor(const size_t id1, const size_t id2) {
    forward_list<int, StationaryAlloc<int>> src({10, 20, 30}, StationaryAlloc<int>(id1));
    auto src_it = src.begin();

    forward_list<int, StationaryAlloc<int>> dst(src, StationaryAlloc<int>(id2));
    auto dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_front(60);
    dst.push_front(70);

    *next(src.before_begin()) += 8;
    *next(dst.before_begin()) += 9;

    assert_equal(src, {68, 40, 20, 30});
    assert_equal(dst, {79, 50, 20, 30});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <class Alloc>
void test_flist_copy_assign(const size_t id1, const size_t id2, const size_t id3) {

    forward_list<int, Alloc> src({10, 20, 30}, Alloc(id1));
    forward_list<int, Alloc> dst({0, 0, 0}, Alloc(id2));

    auto src_it = src.begin();
    auto dst_it = dst.begin();

    dst = src;

    dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_front(60);
    dst.push_front(70);

    *next(src.before_begin()) += 8;
    *next(dst.before_begin()) += 9;

    assert_equal(src, {68, 40, 20, 30});
    assert_equal(dst, {79, 50, 20, 30});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

void test_flist_move_ctor() {
    forward_list<int, StationaryAlloc<int>> src({10, 20, 30}, StationaryAlloc<int>(11));
    auto it1 = src.begin();

    forward_list<int, StationaryAlloc<int>> dst(move(src));
    auto it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_front(100);
    dst.push_front(110);

    *next(src.before_begin()) += 8;
    *next(dst.before_begin()) += 9;

    assert_equal(src, {108, 90, 50, 60});
    assert_equal(dst, {119, 70, 80, 30});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

void test_flist_move_alloc_ctor(const size_t id1, const size_t id2) {
    forward_list<int, StationaryAlloc<int>> src({10, 20, 30}, StationaryAlloc<int>(id1));
    auto it1 = src.begin();

    forward_list<int, StationaryAlloc<int>> dst(move(src), StationaryAlloc<int>(id2));

    if (id1 != id2) {
        it1 = dst.begin();
    }

    auto it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_front(100);
    dst.push_front(110);

    *next(src.before_begin()) += 8;
    *next(dst.before_begin()) += 9;

    assert_equal(src, {108, 90, 50, 60});
    assert_equal(dst, {119, 70, 80, 30});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <class Alloc>
void test_flist_move_assign(const size_t id1, const size_t id2, const size_t id3) {

    forward_list<int, Alloc> src({10, 20, 30}, Alloc(id1));
    forward_list<int, Alloc> dst({0, 0, 0}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst = move(src);

    if (id1 != id3) {
        it1 = dst.begin();
    }

    it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_front(100);
    dst.push_front(110);

    *next(src.before_begin()) += 8;
    *next(dst.before_begin()) += 9;

    assert_equal(src, {108, 90, 50, 60});
    assert_equal(dst, {119, 70, 80, 30});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <class Alloc>
void test_flist_swap(const size_t id1, const size_t id2) {

    forward_list<int, Alloc> src({10, 20, 30}, Alloc(id1));
    forward_list<int, Alloc> dst({40, 50, 60}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst.swap(src);

    *it1 = 70;
    *it2 = 80;

    src.push_front(100);
    dst.push_front(110);

    *next(src.before_begin()) += 8;
    *next(dst.before_begin()) += 9;

    assert_equal(src, {108, 80, 50, 60});
    assert_equal(dst, {119, 70, 20, 30});

    assert(src.get_allocator().id() == id2);
    assert(dst.get_allocator().id() == id1);
}

void test_flist() {
    test_flist_copy_ctor();

    test_flist_copy_alloc_ctor(11, 11); // equal allocators
    test_flist_copy_alloc_ctor(11, 22); // non-equal allocators

    test_flist_copy_assign<StationaryAlloc<int>>(11, 11, 11); // non-POCCA, equal allocators
    test_flist_copy_assign<StationaryAlloc<int>>(11, 22, 22); // non-POCCA, non-equal allocators
    test_flist_copy_assign<CopyAlloc<int>>(11, 11, 11); // POCCA, equal allocators
    test_flist_copy_assign<CopyAlloc<int>>(11, 22, 11); // POCCA, non-equal allocators
    test_flist_copy_assign<CopyEqualAlloc<int>>(11, 22, 11); // POCCA, always-equal allocators

    test_flist_move_ctor();

    test_flist_move_alloc_ctor(11, 11); // equal allocators
    test_flist_move_alloc_ctor(11, 22); // non-equal allocators

    test_flist_move_assign<StationaryAlloc<int>>(11, 11, 11); // non-POCMA, equal allocators
    test_flist_move_assign<StationaryAlloc<int>>(11, 22, 22); // non-POCMA, non-equal allocators
    test_flist_move_assign<MoveAlloc<int>>(11, 11, 11); // POCMA, equal allocators
    test_flist_move_assign<MoveAlloc<int>>(11, 22, 11); // POCMA, non-equal allocators
    test_flist_move_assign<MoveEqualAlloc<int>>(11, 22, 11); // POCMA, always-equal allocators

    test_flist_swap<StationaryAlloc<int>>(11, 11); // non-POCS, equal allocators
    // UNDEFINED BEHAVIOR, NOT TESTED - non-POCS, non-equal allocators
    test_flist_swap<SwapAlloc<int>>(11, 11); // POCS, equal allocators
    test_flist_swap<SwapAlloc<int>>(11, 22); // POCS, non-equal allocators
    test_flist_swap<SwapEqualAlloc<int>>(11, 22); // POCS, always-equal allocators
}


// NOTE: Having 4 elements of type char32_t bypasses the Small String Optimization.

void test_string_copy_ctor() {
    basic_string<char32_t, char_traits<char32_t>, StationaryAlloc<char32_t>> src(
        {5, 10, 20, 30}, StationaryAlloc<char32_t>(11));
    auto src_it = src.begin();

    basic_string<char32_t, char_traits<char32_t>, StationaryAlloc<char32_t>> dst(src);
    auto dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_back(60);
    dst.push_back(70);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {40, 10, 20, 30, 68});
    assert_equal(dst, {50, 10, 20, 30, 79});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

void test_string_copy_alloc_ctor(const size_t id1, const size_t id2) {
    basic_string<char32_t, char_traits<char32_t>, StationaryAlloc<char32_t>> src(
        {5, 10, 20, 30}, StationaryAlloc<char32_t>(id1));
    auto src_it = src.begin();

    basic_string<char32_t, char_traits<char32_t>, StationaryAlloc<char32_t>> dst(src, StationaryAlloc<char32_t>(id2));
    auto dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_back(60);
    dst.push_back(70);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {40, 10, 20, 30, 68});
    assert_equal(dst, {50, 10, 20, 30, 79});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <class Alloc>
void test_string_copy_assign(const size_t id1, const size_t id2, const size_t id3) {

    basic_string<char32_t, char_traits<char32_t>, Alloc> src({5, 10, 20, 30}, Alloc(id1));
    basic_string<char32_t, char_traits<char32_t>, Alloc> dst({0, 0, 0, 0}, Alloc(id2));

    auto src_it = src.begin();
    auto dst_it = dst.begin();

    dst = src;

    dst_it = dst.begin();

    *src_it = 40;
    *dst_it = 50;

    src.push_back(60);
    dst.push_back(70);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {40, 10, 20, 30, 68});
    assert_equal(dst, {50, 10, 20, 30, 79});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

void test_string_move_ctor() {
    basic_string<char32_t, char_traits<char32_t>, StationaryAlloc<char32_t>> src(
        {5, 10, 20, 30}, StationaryAlloc<char32_t>(11));
    auto it1 = src.begin();

    basic_string<char32_t, char_traits<char32_t>, StationaryAlloc<char32_t>> dst(move(src));
    it1      = dst.begin(); // basic_string doesn't preserve iterators here
    auto it2 = next(dst.begin());

    src         = {6, 40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_back(100);
    dst.push_back(110);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {90, 40, 50, 60, 108});
    assert_equal(dst, {70, 80, 20, 30, 119});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

void test_string_move_alloc_ctor(const size_t id1, const size_t id2) {
    basic_string<char32_t, char_traits<char32_t>, StationaryAlloc<char32_t>> src(
        {5, 10, 20, 30}, StationaryAlloc<char32_t>(id1));
    auto it1 = src.begin();

    basic_string<char32_t, char_traits<char32_t>, StationaryAlloc<char32_t>> dst(
        move(src), StationaryAlloc<char32_t>(id2));
    it1      = dst.begin(); // basic_string doesn't preserve iterators here
    auto it2 = next(dst.begin());

    src         = {6, 40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_back(100);
    dst.push_back(110);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {90, 40, 50, 60, 108});
    assert_equal(dst, {70, 80, 20, 30, 119});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <class Alloc>
void test_string_move_assign(const size_t id1, const size_t id2, const size_t id3) {

    basic_string<char32_t, char_traits<char32_t>, Alloc> src({5, 10, 20, 30}, Alloc(id1));
    basic_string<char32_t, char_traits<char32_t>, Alloc> dst({0, 0, 0, 0}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst = move(src);

    it1 = dst.begin(); // basic_string doesn't preserve iterators here
    it2 = next(dst.begin());

    src         = {6, 40, 50, 60};
    auto src_it = src.begin();

    *it1    = 70;
    *it2    = 80;
    *src_it = 90;

    src.push_back(100);
    dst.push_back(110);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {90, 40, 50, 60, 108});
    assert_equal(dst, {70, 80, 20, 30, 119});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <class Alloc>
void test_string_swap(const size_t id1, const size_t id2) {

    basic_string<char32_t, char_traits<char32_t>, Alloc> src({5, 10, 20, 30}, Alloc(id1));
    basic_string<char32_t, char_traits<char32_t>, Alloc> dst({6, 40, 50, 60}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst.swap(src);

    it1 = dst.begin(); // basic_string doesn't preserve iterators here
    it2 = src.begin(); // basic_string doesn't preserve iterators here

    *it1 = 70;
    *it2 = 80;

    src.push_back(100);
    dst.push_back(110);

    *prev(src.end()) += 8;
    *prev(dst.end()) += 9;

    assert_equal(src, {80, 40, 50, 60, 108});
    assert_equal(dst, {70, 10, 20, 30, 119});

    assert(src.get_allocator().id() == id2);
    assert(dst.get_allocator().id() == id1);
}

void test_string() {
    test_string_copy_ctor();

    test_string_copy_alloc_ctor(11, 11); // equal allocators
    test_string_copy_alloc_ctor(11, 22); // non-equal allocators

    test_string_copy_assign<StationaryAlloc<char32_t>>(11, 11, 11); // non-POCCA, equal allocators
    test_string_copy_assign<StationaryAlloc<char32_t>>(11, 22, 22); // non-POCCA, non-equal allocators
    test_string_copy_assign<CopyAlloc<char32_t>>(11, 11, 11); // POCCA, equal allocators
    test_string_copy_assign<CopyAlloc<char32_t>>(11, 22, 11); // POCCA, non-equal allocators
    test_string_copy_assign<CopyEqualAlloc<char32_t>>(11, 22, 11); // POCCA, always-equal allocators

    test_string_move_ctor();

    test_string_move_alloc_ctor(11, 11); // equal allocators
    test_string_move_alloc_ctor(11, 22); // non-equal allocators

    test_string_move_assign<StationaryAlloc<char32_t>>(11, 11, 11); // non-POCMA, equal allocators
    test_string_move_assign<StationaryAlloc<char32_t>>(11, 22, 22); // non-POCMA, non-equal allocators
    test_string_move_assign<MoveAlloc<char32_t>>(11, 11, 11); // POCMA, equal allocators
    test_string_move_assign<MoveAlloc<char32_t>>(11, 22, 11); // POCMA, non-equal allocators
    test_string_move_assign<MoveEqualAlloc<char32_t>>(11, 22, 11); // POCMA, always-equal allocators

    test_string_swap<StationaryAlloc<char32_t>>(11, 11); // non-POCS, equal allocators
    // UNDEFINED BEHAVIOR, NOT TESTED - non-POCS, non-equal allocators
    test_string_swap<SwapAlloc<char32_t>>(11, 11); // POCS, equal allocators
    test_string_swap<SwapAlloc<char32_t>>(11, 22); // POCS, non-equal allocators
    test_string_swap<SwapEqualAlloc<char32_t>>(11, 22); // POCS, always-equal allocators
}


constexpr bool O = false;
constexpr bool I = true;

void test_vb_copy_ctor() {
    vector<bool, StationaryAlloc<bool>> src({I, I, I, O, I, I, I}, StationaryAlloc<bool>(11));
    auto src_it = src.begin();

    vector<bool, StationaryAlloc<bool>> dst(src);
    auto dst_it = next(dst.begin());

    *src_it = O;
    *dst_it = O;

    src.push_back(O);
    dst.push_back(I);

    *prev(src.end(), 2) = O;
    *prev(dst.end(), 3) = O;

    assert_equal(src, {O, I, I, O, I, I, O, O});
    assert_equal(dst, {I, O, I, O, I, O, I, I});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

void test_vb_copy_alloc_ctor(const size_t id1, const size_t id2) {
    vector<bool, StationaryAlloc<bool>> src({I, I, I, O, I, I, I}, StationaryAlloc<bool>(id1));
    auto src_it = src.begin();

    vector<bool, StationaryAlloc<bool>> dst(src, StationaryAlloc<bool>(id2));
    auto dst_it = next(dst.begin());

    *src_it = O;
    *dst_it = O;

    src.push_back(O);
    dst.push_back(I);

    *prev(src.end(), 2) = O;
    *prev(dst.end(), 3) = O;

    assert_equal(src, {O, I, I, O, I, I, O, O});
    assert_equal(dst, {I, O, I, O, I, O, I, I});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <class Alloc>
void test_vb_copy_assign(const size_t id1, const size_t id2, const size_t id3) {

    vector<bool, Alloc> src({I, I, I, O, I, I, I}, Alloc(id1));
    vector<bool, Alloc> dst({O, O, O, O, O, O, O}, Alloc(id2));

    auto src_it = src.begin();
    auto dst_it = next(dst.begin());

    dst = src;

    dst_it = next(dst.begin());

    *src_it = O;
    *dst_it = O;

    src.push_back(O);
    dst.push_back(I);

    *prev(src.end(), 2) = O;
    *prev(dst.end(), 3) = O;

    assert_equal(src, {O, I, I, O, I, I, O, O});
    assert_equal(dst, {I, O, I, O, I, O, I, I});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

void test_vb_move_ctor() {
    vector<bool, StationaryAlloc<bool>> src({I, I, I, O, I, I, I}, StationaryAlloc<bool>(11));
    auto it1 = src.begin();

    vector<bool, StationaryAlloc<bool>> dst(move(src));
    auto it2 = next(dst.begin());

    src         = {O, O, O, I, O, O, O};
    auto src_it = src.begin();

    *it1    = O;
    *it2    = O;
    *src_it = I;

    src.push_back(O);
    dst.push_back(I);

    *prev(src.end(), 2) = I;
    *prev(dst.end(), 3) = O;

    assert_equal(src, {I, O, O, I, O, O, I, O});
    assert_equal(dst, {O, O, I, O, I, O, I, I});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

void test_vb_move_alloc_ctor(const size_t id1, const size_t id2) {
    vector<bool, StationaryAlloc<bool>> src({I, I, I, O, I, I, I}, StationaryAlloc<bool>(id1));
    auto it1 = src.begin();

    vector<bool, StationaryAlloc<bool>> dst(move(src), StationaryAlloc<bool>(id2));

    if (id1 != id2) {
        it1 = dst.begin();
    }

    auto it2 = next(dst.begin());

    src         = {O, O, O, I, O, O, O};
    auto src_it = src.begin();

    *it1    = O;
    *it2    = O;
    *src_it = I;

    src.push_back(O);
    dst.push_back(I);

    *prev(src.end(), 2) = I;
    *prev(dst.end(), 3) = O;

    assert_equal(src, {I, O, O, I, O, O, I, O});
    assert_equal(dst, {O, O, I, O, I, O, I, I});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <class Alloc>
void test_vb_move_assign(const size_t id1, const size_t id2, const size_t id3) {

    vector<bool, Alloc> src({I, I, I, O, I, I, I}, Alloc(id1));
    vector<bool, Alloc> dst({O, O, O, O, O, O, O}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst = move(src);

    if (id1 != id3) {
        it1 = dst.begin();
    }

    it2 = next(dst.begin());

    src         = {O, O, O, I, O, O, O};
    auto src_it = src.begin();

    *it1    = O;
    *it2    = O;
    *src_it = I;

    src.push_back(O);
    dst.push_back(I);

    *prev(src.end(), 2) = I;
    *prev(dst.end(), 3) = O;

    assert_equal(src, {I, O, O, I, O, O, I, O});
    assert_equal(dst, {O, O, I, O, I, O, I, I});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <class Alloc>
void test_vb_swap(const size_t id1, const size_t id2) {

    vector<bool, Alloc> src({I, I, I, O, I, I, I}, Alloc(id1));
    vector<bool, Alloc> dst({O, O, O, I, O, O, O}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst.swap(src);

    *it1 = O;
    *it2 = I;

    src.push_back(I);
    dst.push_back(O);

    *prev(src.end(), 2) = I;
    *prev(dst.end(), 3) = O;

    assert_equal(src, {I, O, O, I, O, O, I, I});
    assert_equal(dst, {O, I, I, O, I, O, I, O});

    assert(src.get_allocator().id() == id2);
    assert(dst.get_allocator().id() == id1);
}

void test_vb() {
    test_vb_copy_ctor();

    test_vb_copy_alloc_ctor(11, 11); // equal allocators
    test_vb_copy_alloc_ctor(11, 22); // non-equal allocators

    test_vb_copy_assign<StationaryAlloc<bool>>(11, 11, 11); // non-POCCA, equal allocators
    test_vb_copy_assign<StationaryAlloc<bool>>(11, 22, 22); // non-POCCA, non-equal allocators
    test_vb_copy_assign<CopyAlloc<bool>>(11, 11, 11); // POCCA, equal allocators
    test_vb_copy_assign<CopyAlloc<bool>>(11, 22, 11); // POCCA, non-equal allocators
    test_vb_copy_assign<CopyEqualAlloc<bool>>(11, 22, 11); // POCCA, always-equal allocators

    test_vb_move_ctor();

    test_vb_move_alloc_ctor(11, 11); // equal allocators
    test_vb_move_alloc_ctor(11, 22); // non-equal allocators

    test_vb_move_assign<StationaryAlloc<bool>>(11, 11, 11); // non-POCMA, equal allocators
    test_vb_move_assign<StationaryAlloc<bool>>(11, 22, 22); // non-POCMA, non-equal allocators
    test_vb_move_assign<MoveAlloc<bool>>(11, 11, 11); // POCMA, equal allocators
    test_vb_move_assign<MoveAlloc<bool>>(11, 22, 11); // POCMA, non-equal allocators
    test_vb_move_assign<MoveEqualAlloc<bool>>(11, 22, 11); // POCMA, always-equal allocators

    test_vb_swap<StationaryAlloc<bool>>(11, 11); // non-POCS, equal allocators
    // UNDEFINED BEHAVIOR, NOT TESTED - non-POCS, non-equal allocators
    test_vb_swap<SwapAlloc<bool>>(11, 11); // POCS, equal allocators
    test_vb_swap<SwapAlloc<bool>>(11, 22); // POCS, non-equal allocators
    test_vb_swap<SwapEqualAlloc<bool>>(11, 22); // POCS, always-equal allocators
}


using PCII = pair<const int, int>;

template <class K, class V, class C, class A>
auto GetIter(map<K, V, C, A>& c) {
    return prev(c.end());
}

template <class K, class V, class C, class A>
auto GetIter(multimap<K, V, C, A>& c) {
    return prev(c.end());
}

template <class K, class V, class H, class P, class A>
auto GetIter(unordered_map<K, V, H, P, A>& c) {
    return c.begin();
}

template <class K, class V, class H, class P, class A>
auto GetIter(unordered_multimap<K, V, H, P, A>& c) {
    return c.begin();
}

template <template <class> class Map>
void test_map_copy_ctor() {

    // Special: Test the (first, last, alloc) ctor.
    const PCII arr[] = {{10, 100}, {20, 200}, {30, 300}};

    typename Map<StationaryAlloc<PCII>>::type src(begin(arr), end(arr), StationaryAlloc<PCII>(11));
    auto src_it = src.begin();

    typename Map<StationaryAlloc<PCII>>::type dst(src);
    auto dst_it = dst.begin();

    assert(src_it->first * 10 == src_it->second);
    assert(dst_it->first * 10 == dst_it->second);

    src.emplace(40, 400);
    dst.emplace(50, 500);

    assert(GetIter(src)->first * 10 == GetIter(src)->second);
    assert(GetIter(dst)->first * 10 == GetIter(dst)->second);

    assert_is_permutation(src, {{10, 100}, {20, 200}, {30, 300}, {40, 400}});
    assert_is_permutation(dst, {{10, 100}, {20, 200}, {30, 300}, {50, 500}});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

template <template <class> class Map>
void test_map_copy_alloc_ctor(const size_t id1, const size_t id2) {

    typename Map<StationaryAlloc<PCII>>::type src({{10, 100}, {20, 200}, {30, 300}}, StationaryAlloc<PCII>(id1));
    auto src_it = src.begin();

    typename Map<StationaryAlloc<PCII>>::type dst(src, StationaryAlloc<PCII>(id2));
    auto dst_it = dst.begin();

    assert(src_it->first * 10 == src_it->second);
    assert(dst_it->first * 10 == dst_it->second);

    src.emplace(40, 400);
    dst.emplace(50, 500);

    assert(GetIter(src)->first * 10 == GetIter(src)->second);
    assert(GetIter(dst)->first * 10 == GetIter(dst)->second);

    assert_is_permutation(src, {{10, 100}, {20, 200}, {30, 300}, {40, 400}});
    assert_is_permutation(dst, {{10, 100}, {20, 200}, {30, 300}, {50, 500}});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <template <class> class Map, class Alloc>
void test_map_copy_assign(const size_t id1, const size_t id2, const size_t id3) {

    typename Map<Alloc>::type src({{10, 100}, {20, 200}, {30, 300}}, Alloc(id1));
    typename Map<Alloc>::type dst({{0, 0}, {0, 0}, {0, 0}}, Alloc(id2));

    auto src_it = src.begin();
    auto dst_it = dst.begin();

    dst = src;

    dst_it = dst.begin();

    assert(src_it->first * 10 == src_it->second);
    assert(dst_it->first * 10 == dst_it->second);

    src.emplace(40, 400);
    dst.emplace(50, 500);

    assert(GetIter(src)->first * 10 == GetIter(src)->second);
    assert(GetIter(dst)->first * 10 == GetIter(dst)->second);

    assert_is_permutation(src, {{10, 100}, {20, 200}, {30, 300}, {40, 400}});
    assert_is_permutation(dst, {{10, 100}, {20, 200}, {30, 300}, {50, 500}});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <template <class> class Map>
void test_map_move_ctor() {

    typename Map<StationaryAlloc<PCII>>::type src({{10, 100}, {20, 200}, {30, 300}}, StationaryAlloc<PCII>(11));
    auto it1 = src.begin();

    typename Map<StationaryAlloc<PCII>>::type dst(move(src));
    auto it2 = next(dst.begin());

    src         = {{40, 400}, {50, 500}, {60, 600}};
    auto src_it = src.begin();

    assert(it1->first * 10 == it1->second);
    assert(it2->first * 10 == it2->second);
    assert(src_it->first * 10 == src_it->second);

    src.emplace(70, 700);
    dst.emplace(80, 800);

    assert(GetIter(src)->first * 10 == GetIter(src)->second);
    assert(GetIter(dst)->first * 10 == GetIter(dst)->second);

    assert_is_permutation(src, {{40, 400}, {50, 500}, {60, 600}, {70, 700}});
    assert_is_permutation(dst, {{10, 100}, {20, 200}, {30, 300}, {80, 800}});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

template <template <class> class Map>
void test_map_move_alloc_ctor(const size_t id1, const size_t id2) {

    typename Map<StationaryAlloc<PCII>>::type src({{10, 100}, {20, 200}, {30, 300}}, StationaryAlloc<PCII>(id1));
    auto it1 = src.begin();

    typename Map<StationaryAlloc<PCII>>::type dst(move(src), StationaryAlloc<PCII>(id2));

    if (id1 != id2) {
        it1 = dst.begin();
    }

    auto it2 = next(dst.begin());

    src         = {{40, 400}, {50, 500}, {60, 600}};
    auto src_it = src.begin();

    assert(it1->first * 10 == it1->second);
    assert(it2->first * 10 == it2->second);
    assert(src_it->first * 10 == src_it->second);

    src.emplace(70, 700);
    dst.emplace(80, 800);

    assert(GetIter(src)->first * 10 == GetIter(src)->second);
    assert(GetIter(dst)->first * 10 == GetIter(dst)->second);

    assert_is_permutation(src, {{40, 400}, {50, 500}, {60, 600}, {70, 700}});
    assert_is_permutation(dst, {{10, 100}, {20, 200}, {30, 300}, {80, 800}});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <template <class> class Map, class Alloc>
void test_map_move_assign(const size_t id1, const size_t id2, const size_t id3) {

    typename Map<Alloc>::type src({{10, 100}, {20, 200}, {30, 300}}, Alloc(id1));
    typename Map<Alloc>::type dst({{0, 0}, {0, 0}, {0, 0}}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst = move(src);

    if (id1 != id3) {
        it1 = dst.begin();
    }

    it2 = next(dst.begin());

    src         = {{40, 400}, {50, 500}, {60, 600}};
    auto src_it = src.begin();

    assert(it1->first * 10 == it1->second);
    assert(it2->first * 10 == it2->second);
    assert(src_it->first * 10 == src_it->second);

    src.emplace(70, 700);
    dst.emplace(80, 800);

    assert(GetIter(src)->first * 10 == GetIter(src)->second);
    assert(GetIter(dst)->first * 10 == GetIter(dst)->second);

    assert_is_permutation(src, {{40, 400}, {50, 500}, {60, 600}, {70, 700}});
    assert_is_permutation(dst, {{10, 100}, {20, 200}, {30, 300}, {80, 800}});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <template <class> class Map, class Alloc>
void test_map_swap(const size_t id1, const size_t id2) {

    typename Map<Alloc>::type src({{10, 100}, {20, 200}, {30, 300}}, Alloc(id1));
    typename Map<Alloc>::type dst({{40, 400}, {50, 500}, {60, 600}}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst.swap(src);

    assert(it1->first * 10 == it1->second);
    assert(it2->first * 10 == it2->second);

    src.emplace(70, 700);
    dst.emplace(80, 800);

    assert(GetIter(src)->first * 10 == GetIter(src)->second);
    assert(GetIter(dst)->first * 10 == GetIter(dst)->second);

    assert_is_permutation(src, {{40, 400}, {50, 500}, {60, 600}, {70, 700}});
    assert_is_permutation(dst, {{10, 100}, {20, 200}, {30, 300}, {80, 800}});

    assert(src.get_allocator().id() == id2);
    assert(dst.get_allocator().id() == id1);
}

template <template <class> class Map>
void test_map() {

    test_map_copy_ctor<Map>();

    test_map_copy_alloc_ctor<Map>(11, 11); // equal allocators
    test_map_copy_alloc_ctor<Map>(11, 22); // non-equal allocators

    test_map_copy_assign<Map, StationaryAlloc<PCII>>(11, 11, 11); // non-POCCA, equal allocators
    test_map_copy_assign<Map, StationaryAlloc<PCII>>(11, 22, 22); // non-POCCA, non-equal allocators
    test_map_copy_assign<Map, CopyAlloc<PCII>>(11, 11, 11); // POCCA, equal allocators
    test_map_copy_assign<Map, CopyAlloc<PCII>>(11, 22, 11); // POCCA, non-equal allocators
    test_map_copy_assign<Map, CopyEqualAlloc<PCII>>(11, 22, 11); // POCCA, always-equal allocators

    test_map_move_ctor<Map>();

    test_map_move_alloc_ctor<Map>(11, 11); // equal allocators
    test_map_move_alloc_ctor<Map>(11, 22); // non-equal allocators

    test_map_move_assign<Map, StationaryAlloc<PCII>>(11, 11, 11); // non-POCMA, equal allocators
    test_map_move_assign<Map, StationaryAlloc<PCII>>(11, 22, 22); // non-POCMA, non-equal allocators
    test_map_move_assign<Map, MoveAlloc<PCII>>(11, 11, 11); // POCMA, equal allocators
    test_map_move_assign<Map, MoveAlloc<PCII>>(11, 22, 11); // POCMA, non-equal allocators
    test_map_move_assign<Map, MoveEqualAlloc<PCII>>(11, 22, 11); // POCMA, always-equal allocators

    test_map_swap<Map, StationaryAlloc<PCII>>(11, 11); // non-POCS, equal allocators
    // UNDEFINED BEHAVIOR, NOT TESTED - non-POCS, non-equal allocators
    test_map_swap<Map, SwapAlloc<PCII>>(11, 11); // POCS, equal allocators
    test_map_swap<Map, SwapAlloc<PCII>>(11, 22); // POCS, non-equal allocators
    test_map_swap<Map, SwapAlloc<PCII>>(11, 22); // POCS, always-equal allocators
}

template <class Alloc>
struct OrderedMap {
    using type = map<int, int, less<>, Alloc>;
};

template <class Alloc>
struct OrderedMultimap {
    using type = multimap<int, int, less<>, Alloc>;
};

template <class Alloc>
struct UnorderedMap {
    using type = unordered_map<int, int, hash<int>, equal_to<>, Alloc>;
};

template <class Alloc>
struct UnorderedMultimap {
    using type = unordered_multimap<int, int, hash<int>, equal_to<>, Alloc>;
};


template <class K, class C, class A>
auto GetIter(set<K, C, A>& c) {
    return prev(c.end());
}

template <class K, class C, class A>
auto GetIter(multiset<K, C, A>& c) {
    return prev(c.end());
}

template <class K, class H, class P, class A>
auto GetIter(unordered_set<K, H, P, A>& c) {
    return c.begin();
}

template <class K, class H, class P, class A>
auto GetIter(unordered_multiset<K, H, P, A>& c) {
    return c.begin();
}

template <template <class> class Set>
void test_set_copy_ctor() {

    // Special: Test the (first, last, alloc) ctor.
    const int arr[] = {10, 20, 30};

    typename Set<StationaryAlloc<int>>::type src(begin(arr), end(arr), StationaryAlloc<int>(11));
    auto src_it = src.begin();

    typename Set<StationaryAlloc<int>>::type dst(src);
    auto dst_it = dst.begin();

    assert(*src_it % 10 == 0);
    assert(*dst_it % 10 == 0);

    src.emplace(40);
    dst.emplace(50);

    assert(*GetIter(src) % 10 == 0);
    assert(*GetIter(dst) % 10 == 0);

    assert_is_permutation(src, {10, 20, 30, 40});
    assert_is_permutation(dst, {10, 20, 30, 50});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

template <template <class> class Set>
void test_set_copy_alloc_ctor(const size_t id1, const size_t id2) {

    typename Set<StationaryAlloc<int>>::type src({10, 20, 30}, StationaryAlloc<int>(id1));
    auto src_it = src.begin();

    typename Set<StationaryAlloc<int>>::type dst(src, StationaryAlloc<int>(id2));
    auto dst_it = dst.begin();

    assert(*src_it % 10 == 0);
    assert(*dst_it % 10 == 0);

    src.emplace(40);
    dst.emplace(50);

    assert(*GetIter(src) % 10 == 0);
    assert(*GetIter(dst) % 10 == 0);

    assert_is_permutation(src, {10, 20, 30, 40});
    assert_is_permutation(dst, {10, 20, 30, 50});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <template <class> class Set, class Alloc>
void test_set_copy_assign(const size_t id1, const size_t id2, const size_t id3) {

    typename Set<Alloc>::type src({10, 20, 30}, Alloc(id1));
    typename Set<Alloc>::type dst({0, 0, 0}, Alloc(id2));

    auto src_it = src.begin();
    auto dst_it = dst.begin();

    dst = src;

    dst_it = dst.begin();

    assert(*src_it % 10 == 0);
    assert(*dst_it % 10 == 0);

    src.emplace(40);
    dst.emplace(50);

    assert(*GetIter(src) % 10 == 0);
    assert(*GetIter(dst) % 10 == 0);

    assert_is_permutation(src, {10, 20, 30, 40});
    assert_is_permutation(dst, {10, 20, 30, 50});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <template <class> class Set>
void test_set_move_ctor() {

    typename Set<StationaryAlloc<int>>::type src({10, 20, 30}, StationaryAlloc<int>(11));
    auto it1 = src.begin();

    typename Set<StationaryAlloc<int>>::type dst(move(src));
    auto it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    assert(*it1 % 10 == 0);
    assert(*it2 % 10 == 0);
    assert(*src_it % 10 == 0);

    src.emplace(70);
    dst.emplace(80);

    assert(*GetIter(src) % 10 == 0);
    assert(*GetIter(dst) % 10 == 0);

    assert_is_permutation(src, {40, 50, 60, 70});
    assert_is_permutation(dst, {10, 20, 30, 80});

    assert(src.get_allocator().id() == 11);
    assert(dst.get_allocator().id() == 11);
}

template <template <class> class Set>
void test_set_move_alloc_ctor(const size_t id1, const size_t id2) {

    typename Set<StationaryAlloc<int>>::type src({10, 20, 30}, StationaryAlloc<int>(id1));
    auto it1 = src.begin();

    typename Set<StationaryAlloc<int>>::type dst(move(src), StationaryAlloc<int>(id2));

    if (id1 != id2) {
        it1 = dst.begin();
    }

    auto it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    assert(*it1 % 10 == 0);
    assert(*it2 % 10 == 0);
    assert(*src_it % 10 == 0);

    src.emplace(70);
    dst.emplace(80);

    assert(*GetIter(src) % 10 == 0);
    assert(*GetIter(dst) % 10 == 0);

    assert_is_permutation(src, {40, 50, 60, 70});
    assert_is_permutation(dst, {10, 20, 30, 80});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id2);
}

template <template <class> class Set, class Alloc>
void test_set_move_assign(const size_t id1, const size_t id2, const size_t id3) {

    typename Set<Alloc>::type src({10, 20, 30}, Alloc(id1));
    typename Set<Alloc>::type dst({0, 0, 0}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst = move(src);

    if (id1 != id3) {
        it1 = dst.begin();
    }

    it2 = next(dst.begin());

    src         = {40, 50, 60};
    auto src_it = src.begin();

    assert(*it1 % 10 == 0);
    assert(*it2 % 10 == 0);
    assert(*src_it % 10 == 0);

    src.emplace(70);
    dst.emplace(80);

    assert(*GetIter(src) % 10 == 0);
    assert(*GetIter(dst) % 10 == 0);

    assert_is_permutation(src, {40, 50, 60, 70});
    assert_is_permutation(dst, {10, 20, 30, 80});

    assert(src.get_allocator().id() == id1);
    assert(dst.get_allocator().id() == id3);
}

template <template <class> class Set, class Alloc>
void test_set_swap(const size_t id1, const size_t id2) {

    typename Set<Alloc>::type src({10, 20, 30}, Alloc(id1));
    typename Set<Alloc>::type dst({40, 50, 60}, Alloc(id2));

    auto it1 = src.begin();
    auto it2 = dst.begin();

    dst.swap(src);

    assert(*it1 % 10 == 0);
    assert(*it2 % 10 == 0);

    src.emplace(70);
    dst.emplace(80);

    assert(*GetIter(src) % 10 == 0);
    assert(*GetIter(dst) % 10 == 0);

    assert_is_permutation(src, {40, 50, 60, 70});
    assert_is_permutation(dst, {10, 20, 30, 80});

    assert(src.get_allocator().id() == id2);
    assert(dst.get_allocator().id() == id1);
}

template <template <class> class Set>
void test_set() {

    test_set_copy_ctor<Set>();

    test_set_copy_alloc_ctor<Set>(11, 11); // equal allocators
    test_set_copy_alloc_ctor<Set>(11, 22); // non-equal allocators

    test_set_copy_assign<Set, StationaryAlloc<int>>(11, 11, 11); // non-POCCA, equal allocators
    test_set_copy_assign<Set, StationaryAlloc<int>>(11, 22, 22); // non-POCCA, non-equal allocators
    test_set_copy_assign<Set, CopyAlloc<int>>(11, 11, 11); // POCCA, equal allocators
    test_set_copy_assign<Set, CopyAlloc<int>>(11, 22, 11); // POCCA, non-equal allocators
    test_set_copy_assign<Set, CopyEqualAlloc<int>>(11, 22, 11); // POCCA, always-equal allocators

    test_set_move_ctor<Set>();

    test_set_move_alloc_ctor<Set>(11, 11); // equal allocators
    test_set_move_alloc_ctor<Set>(11, 22); // non-equal allocators

    test_set_move_assign<Set, StationaryAlloc<int>>(11, 11, 11); // non-POCMA, equal allocators
    test_set_move_assign<Set, StationaryAlloc<int>>(11, 22, 22); // non-POCMA, non-equal allocators
    test_set_move_assign<Set, MoveAlloc<int>>(11, 11, 11); // POCMA, equal allocators
    test_set_move_assign<Set, MoveAlloc<int>>(11, 22, 11); // POCMA, non-equal allocators
    test_set_move_assign<Set, MoveEqualAlloc<int>>(11, 22, 11); // POCMA, always-equal allocators

    test_set_swap<Set, StationaryAlloc<int>>(11, 11); // non-POCS, equal allocators
    // UNDEFINED BEHAVIOR, NOT TESTED - non-POCS, non-equal allocators
    test_set_swap<Set, SwapAlloc<int>>(11, 11); // POCS, equal allocators
    test_set_swap<Set, SwapAlloc<int>>(11, 22); // POCS, non-equal allocators
    test_set_swap<Set, SwapEqualAlloc<int>>(11, 22); // POCS, always-equal allocators
}

template <class Alloc>
struct OrderedSet {
    using type = set<int, less<>, Alloc>;
};

template <class Alloc>
struct OrderedMultiset {
    using type = multiset<int, less<>, Alloc>;
};

template <class Alloc>
struct UnorderedSet {
    using type = unordered_set<int, hash<int>, equal_to<>, Alloc>;
};

template <class Alloc>
struct UnorderedMultiset {
    using type = unordered_multiset<int, hash<int>, equal_to<>, Alloc>;
};


// Verify that stateful comparators are updated by <xtree>'s copy assign, move assign, and swap.
void test_comparator_updates() {
    {
        set<int, function<bool(int, int)>, StationaryAlloc<int>> s1(
            {101, 701, 201, 901}, less<>{}, StationaryAlloc<int>(11));
        set<int, function<bool(int, int)>, StationaryAlloc<int>> s2(
            {102, 702, 202, 902}, greater<>{}, StationaryAlloc<int>(22));

        assert_equal(s1, {101, 201, 701, 901});
        assert_equal(s2, {902, 702, 202, 102});

        s1 = s2;

        s1.insert(803);
        s2.insert(804);

        assert_equal(s1, {902, 803, 702, 202, 102});
        assert_equal(s2, {902, 804, 702, 202, 102});
    }

    {
        set<int, function<bool(int, int)>, StationaryAlloc<int>> s1(
            {101, 701, 201, 901}, less<>{}, StationaryAlloc<int>(11));
        set<int, function<bool(int, int)>, StationaryAlloc<int>> s2(
            {102, 702, 202, 902}, greater<>{}, StationaryAlloc<int>(22));

        assert_equal(s1, {101, 201, 701, 901});
        assert_equal(s2, {902, 702, 202, 102});

        s1 = move(s2);
        s2.clear();

        s1.insert(803);

        s2.insert(10);
        s2.insert(70);
        s2.insert(20);
        s2.insert(90);

        assert_equal(s1, {902, 803, 702, 202, 102});
        assert_equal(s2, {90, 70, 20, 10});
    }

    {
        set<int, function<bool(int, int)>, SwapAlloc<int>> s1({101, 701, 201, 901}, less<>{}, SwapAlloc<int>(11));
        set<int, function<bool(int, int)>, SwapAlloc<int>> s2({102, 702, 202, 902}, greater<>{}, SwapAlloc<int>(22));

        assert_equal(s1, {101, 201, 701, 901});
        assert_equal(s2, {902, 702, 202, 102});

        s1.swap(s2);

        s1.insert(803);
        s2.insert(804);

        assert_equal(s1, {902, 803, 702, 202, 102});
        assert_equal(s2, {101, 201, 701, 804, 901});
    }
}


int main() {
    test_sequence<deque>();
    test_sequence<list>();
    test_sequence<vector>();
#if _HAS_CXX20
    static_assert(test_sequence<vector>());
#endif // _HAS_CXX20

    test_flist();
    test_string();
    test_vb();

    test_map<OrderedMap>();
    test_map<OrderedMultimap>();
    test_map<UnorderedMap>();
    test_map<UnorderedMultimap>();

    test_set<OrderedSet>();
    test_set<OrderedMultiset>();
    test_set<UnorderedSet>();
    test_set<UnorderedMultiset>();

    test_comparator_updates();
}
