// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

constexpr int uninitializedValue = 0xEE;
constexpr int initializedValue   = 106;
size_t allocationCount           = 0;

struct ReportAddress;
vector<ReportAddress*> ascendingAddressBuffer;
vector<ReportAddress*> descendingAddressBuffer;

#pragma warning(disable : 28251) // Inconsistent annotation for 'new'

// According to N4849, the default behavior of operator new[](size) is to return
// operator new(size), so only the latter needs to be replaced.
void* operator new(size_t size) {
    void* const p = ::operator new(size, nothrow);

    if (p) {
        return p;
    } else {
        throw bad_alloc();
    }
}

void* operator new(size_t size, const nothrow_t&) noexcept {
    void* const result = malloc(size == 0 ? 1 : size);
    ++allocationCount;
    if (result) {
        memset(result, uninitializedValue, size);
    }

    return result;
}

void* operator new(size_t size, align_val_t align) {
    void* const p = ::operator new(size, align, nothrow);

    if (p) {
        return p;
    } else {
        throw bad_alloc();
    }
}

void* operator new(size_t size, align_val_t align, const nothrow_t&) noexcept {
    void* const result = ::_aligned_malloc(size, static_cast<size_t>(align));
    ++allocationCount;
    if (result) {
        memset(result, uninitializedValue, size);
    }

    return result;
}

template <typename T, typename = void>
struct unique_is_for_overwritable : false_type {};

template <typename T>
struct unique_is_for_overwritable<T, void_t<decltype(make_unique_for_overwrite<T>())>> : true_type {};

template <typename T>
constexpr bool unique_is_for_overwritable_v = unique_is_for_overwritable<T>::value;

struct DefaultInitializableInt {
    int value;
    DefaultInitializableInt() : value(initializedValue) {}
};

struct alignas(32) HighlyAligned {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
};

size_t canCreate = 10; // Counter to force an exception when constructing a sufficiently large ReportAddress array

struct ReportAddress {
    ReportAddress() {
        if (canCreate > 0) {
            ascendingAddressBuffer.push_back(this);
            --canCreate;
        } else {
            throw runtime_error("Can't create more ReportAddress objects.");
        }
    }

    ~ReportAddress() {
        ++canCreate;
        descendingAddressBuffer.push_back(this);
    }
};

void assert_ascending_init() {
    for (size_t i = 1; i < ascendingAddressBuffer.size(); ++i) {
        assert(ascendingAddressBuffer[i - 1] < ascendingAddressBuffer[i]);
    }

    ascendingAddressBuffer.clear();
}

void assert_descending_destruct() {
    for (size_t i = 1; i < descendingAddressBuffer.size(); ++i) {
        assert(descendingAddressBuffer[i - 1] > descendingAddressBuffer[i]);
    }

    descendingAddressBuffer.clear();
}

void assert_uninitialized(void* p, size_t size) {
    unsigned char* chPtr = reinterpret_cast<unsigned char*>(p);
    for (size_t offset = 0; offset < size; ++offset) {
        assert(chPtr[offset] == uninitializedValue);
    }
}

template <class T>
void assert_shared_use_get(const shared_ptr<T>& sp) {
    assert(sp.use_count() == 1);
    assert(sp.get() != nullptr);
}

template <class T, class... Args>
shared_ptr<T> make_shared_for_overwrite_assert(Args&&... vals) {
    size_t aCount    = allocationCount;
    shared_ptr<T> sp = make_shared_for_overwrite<T>(forward<Args>(vals)...);
    assert_shared_use_get(sp);
    assert(aCount + 1 == allocationCount);
    return sp;
}

template <class T, class... Args>
void test_make_shared_init_destruct_order(Args&&... vals) {
    try {
        shared_ptr<T> sp = make_shared_for_overwrite<T>(forward<Args>(vals)...);
        assert_shared_use_get(sp);
    } catch (const runtime_error& exc) {
        assert(exc.what() == "Can't create more ReportAddress objects."sv);
    }

    assert_ascending_init();
    assert_descending_destruct();
}

void test_make_unique_for_overwrite() {
    static_assert(unique_is_for_overwritable_v<char>);
    static_assert(!unique_is_for_overwritable_v<int[100]>);

    auto p0 = make_unique_for_overwrite<int>();
    assert_uninitialized(p0.get(), sizeof(int));

    auto p1 = make_unique_for_overwrite<int[]>(100u);
    assert_uninitialized(p1.get(), sizeof(int) * 100u);

    auto p2 = make_unique_for_overwrite<DefaultInitializableInt>();
    assert(p2->value == initializedValue);

    auto p3 = make_unique_for_overwrite<DefaultInitializableInt[][89]>(2u);
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 89; ++j) {
            assert(p3[i][j].value == initializedValue);
        }
    }

    auto p4 = make_unique_for_overwrite<int[]>(0u); // p4 cannot be dereferenced
}

void test_make_shared_for_overwrite() {
    auto p0 = make_shared_for_overwrite_assert<int>();
    assert_uninitialized(addressof(*p0), sizeof(int));

    auto p1 = make_shared_for_overwrite_assert<DefaultInitializableInt>();
    assert(p1->value == initializedValue);

    auto p2 = make_shared_for_overwrite_assert<HighlyAligned>();
    assert(reinterpret_cast<uintptr_t>(p2.get()) % alignof(HighlyAligned) == 0);
    assert_uninitialized(addressof(*p2), sizeof(HighlyAligned));

    auto p3 = make_shared_for_overwrite_assert<int[100]>();
    assert_uninitialized(addressof(p3[0]), sizeof(int) * 100u);

    auto p4 = make_shared_for_overwrite_assert<DefaultInitializableInt[2][8]>();
    for (ptrdiff_t i = 0; i < 2; ++i) {
        for (ptrdiff_t j = 0; j < 8; ++j) {
            assert(p4[i][j].value == initializedValue);
        }
    }

    auto p5 = make_shared_for_overwrite_assert<HighlyAligned[10]>();
    assert(reinterpret_cast<uintptr_t>(p5.get()) % alignof(HighlyAligned) == 0);
    assert_uninitialized(addressof(p5[0]), sizeof(HighlyAligned) * 10u);

    auto p6 = make_shared_for_overwrite_assert<DefaultInitializableInt[]>(100u);
    for (ptrdiff_t i = 0; i < 100; ++i) {
        assert(p6[i].value == initializedValue);
    }

    auto p7 = make_shared_for_overwrite_assert<DefaultInitializableInt[][8][9]>(2u);
    for (ptrdiff_t i = 0; i < 2; ++i) {
        for (ptrdiff_t j = 0; j < 8; ++j) {
            for (ptrdiff_t k = 0; k < 9; ++k) {
                assert(p7[i][j][k].value == initializedValue);
            }
        }
    }

    auto p8 = make_shared_for_overwrite_assert<int[]>(100u);
    assert_uninitialized(addressof(p8[0]), sizeof(int) * 100u);

    auto p9 = make_shared_for_overwrite_assert<int[]>(0u); // p9 cannot be dereferenced

    auto p10 = make_shared_for_overwrite_assert<HighlyAligned[]>(10u);
    assert(reinterpret_cast<uintptr_t>(p10.get()) % alignof(HighlyAligned) == 0);
    assert_uninitialized(addressof(p10[0]), sizeof(HighlyAligned) * 10u);

    test_make_shared_init_destruct_order<ReportAddress[5]>(); // success one dimensional

    test_make_shared_init_destruct_order<ReportAddress[20]>(); // failure one dimensional

    test_make_shared_init_destruct_order<ReportAddress[2][2][2]>(); // success multidimensional

    test_make_shared_init_destruct_order<ReportAddress[3][3][3]>(); // failure multidimensional

    test_make_shared_init_destruct_order<ReportAddress[]>(5u); // success one dimensional

    test_make_shared_init_destruct_order<ReportAddress[]>(20u); // failure one dimensional

    test_make_shared_init_destruct_order<ReportAddress[][2][2]>(2u); // success multidimensional

    test_make_shared_init_destruct_order<ReportAddress[][3][3]>(3u); // failure multidimensional
}

template <class T, class... Args>
shared_ptr<T> allocate_shared_for_overwrite_assert(Args&&... vals) {
    size_t aCount    = allocationCount;
    shared_ptr<T> sp = allocate_shared_for_overwrite<T>(forward<Args>(vals)...);
    assert_shared_use_get(sp);
    assert(aCount + 1 == allocationCount);
    return sp;
}

template <class T, class... Args>
void test_allocate_shared_init_destruct_order(Args&&... vals) {
    allocator<remove_all_extents_t<T>> a{};

    try {
        shared_ptr<T> sp = allocate_shared_for_overwrite<T>(a, forward<Args>(vals)...);
        assert_shared_use_get(sp);
    } catch (const runtime_error& exc) {
        assert(exc.what() == "Can't create more ReportAddress objects."sv);
    }

    assert_ascending_init();
    assert_descending_destruct();
}

void test_allocate_shared_for_overwrite() {
    allocator<int> a0{};
    auto p0 = allocate_shared_for_overwrite_assert<int>(a0);
    assert_uninitialized(addressof(*p0), sizeof(int));

    allocator<DefaultInitializableInt> a1{};
    auto p1 = allocate_shared_for_overwrite_assert<DefaultInitializableInt>(a1);
    assert(p1->value == initializedValue);

    allocator<HighlyAligned> a2{};
    auto p2 = allocate_shared_for_overwrite_assert<HighlyAligned>(a2);
    assert(reinterpret_cast<uintptr_t>(p2.get()) % alignof(HighlyAligned) == 0);
    assert_uninitialized(addressof(*p2), sizeof(HighlyAligned));

    auto p3 = allocate_shared_for_overwrite_assert<int[100]>(a0);
    assert_uninitialized(addressof(p3[0]), sizeof(int) * 100u);

    auto p4 = allocate_shared_for_overwrite_assert<DefaultInitializableInt[2][8]>(a1);
    for (ptrdiff_t i = 0; i < 2; ++i) {
        for (ptrdiff_t j = 0; j < 8; ++j) {
            assert(p4[i][j].value == initializedValue);
        }
    }

    auto p5 = allocate_shared_for_overwrite_assert<HighlyAligned[10]>(a2);
    assert(reinterpret_cast<uintptr_t>(p5.get()) % alignof(HighlyAligned) == 0);
    assert_uninitialized(addressof(p5[0]), sizeof(HighlyAligned) * 10u);

    auto p6 = allocate_shared_for_overwrite_assert<DefaultInitializableInt[]>(a1, 100u);
    for (ptrdiff_t i = 0; i < 100; ++i) {
        assert(p6[i].value == initializedValue);
    }

    auto p7 = allocate_shared_for_overwrite_assert<DefaultInitializableInt[][8][9]>(a1, 2u);
    for (ptrdiff_t i = 0; i < 2; ++i) {
        for (ptrdiff_t j = 0; j < 8; ++j) {
            for (ptrdiff_t k = 0; k < 9; ++k) {
                assert(p7[i][j][k].value == initializedValue);
            }
        }
    }

    auto p8 = allocate_shared_for_overwrite_assert<int[]>(a0, 100u);
    assert_uninitialized(addressof(p8[0]), sizeof(int) * 100u);

    auto p9 = allocate_shared_for_overwrite_assert<int[]>(a0, 0u); // p9 cannot be dereferenced

    auto p10 = allocate_shared_for_overwrite_assert<HighlyAligned[]>(a2, 10u);
    assert(reinterpret_cast<uintptr_t>(p10.get()) % alignof(HighlyAligned) == 0);
    assert_uninitialized(addressof(p10[0]), sizeof(HighlyAligned) * 10u);

    test_allocate_shared_init_destruct_order<ReportAddress[5]>(); // success one dimensional

    test_allocate_shared_init_destruct_order<ReportAddress[20]>(); // failure one dimensional

    test_allocate_shared_init_destruct_order<ReportAddress[2][2][2]>(); // success multidimensional

    test_allocate_shared_init_destruct_order<ReportAddress[3][3][3]>(); // failure multidimensional

    test_allocate_shared_init_destruct_order<ReportAddress[]>(5u); // success one dimensional

    test_allocate_shared_init_destruct_order<ReportAddress[]>(20u); // failure one dimensional

    test_allocate_shared_init_destruct_order<ReportAddress[][2][2]>(2u); // success multidimensional

    test_allocate_shared_init_destruct_order<ReportAddress[][3][3]>(3u); // failure multidimensional
}

int main() {
    test_make_unique_for_overwrite();
    test_make_shared_for_overwrite();
    test_allocate_shared_for_overwrite();
}
