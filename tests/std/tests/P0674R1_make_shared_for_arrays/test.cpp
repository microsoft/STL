// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

int allocationCount = 0;
int canCreate       = 10; // Counter to force an exception when constructing a
                    // sufficiently large ReportAddress array

struct ReportAddress;
vector<ReportAddress*> ascendingAddressBuffer;
vector<ReportAddress*> descendingAddressBuffer;

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

    return result;
}

struct InitialValue {
    int value = 106;

    InitialValue() = default;

    InitialValue(int a, int b) : value(a + b) {}
};

struct ThreeIntWrap {
    int v1;
    int v2;
    int v3;
};

struct alignas(32) HighlyAligned {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
};

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

template <class T>
void assert_shared_use_get(const shared_ptr<T>& sp) {
    assert(sp.use_count() == 1);
    assert(sp.get() != nullptr);
}

template <class T, class... Args>
shared_ptr<T> make_shared_assert(Args&&... vals) {
    int count        = allocationCount;
    shared_ptr<T> sp = make_shared<T>(forward<Args>(vals)...);
    assert_shared_use_get(sp);
    assert(count + 1 == allocationCount);
    return sp;
}

template <class T, enable_if_t<extent_v<T> != 0, int> = 0>
shared_ptr<T> make_shared_init_assert(const remove_extent_t<T>& val) {
    return make_shared_assert<T>(val);
}

template <class T, enable_if_t<is_array_v<T> && extent_v<T> == 0, int> = 0>
shared_ptr<T> make_shared_init_assert(size_t size, const remove_extent_t<T>& val) {
    return make_shared_assert<T>(size, val);
}

template <class T, class... Args>
void test_make_init_destruct_order(Args&&... vals) {
    try {
        shared_ptr<T> sp = make_shared<T>(forward<Args>(vals)...);
        assert_shared_use_get(sp);
    } catch (const runtime_error& exc) {
        assert(exc.what() == "Can't create more ReportAddress objects."sv);
    }

    assert_ascending_init();
    assert_descending_destruct();
}

void test_make_shared_not_array() {
    shared_ptr<vector<int>> p0 = make_shared<vector<int>>();
    assert_shared_use_get(p0);
    assert(p0->empty());

    shared_ptr<InitialValue> p1 = make_shared_assert<InitialValue>();
    assert(p1->value == 106);

    shared_ptr<string> p2 = make_shared<string>("Meow!", 2u, 3u);
    assert_shared_use_get(p2);
    assert(p2->compare("ow!") == 0);

    shared_ptr<InitialValue> p3 = make_shared_assert<InitialValue>(40, 2);
    assert(p3->value == 42);

    shared_ptr<int> p4 = make_shared<int>();
    assert_shared_use_get(p4);
    assert(*p4 == 0);

    shared_ptr<HighlyAligned> p5 = make_shared<HighlyAligned>();
    assert_shared_use_get(p5);
    assert(reinterpret_cast<uintptr_t>(p5.get()) % alignof(HighlyAligned) == 0);
    assert(p5->a == 0 && p5->b == 0 && p5->c == 0 && p5->d == 0);
}

void test_make_shared_array_known_bounds() {
    shared_ptr<string[100]> p0 = make_shared<string[100]>();
    assert_shared_use_get(p0);
    for (int i = 0; i < 100; ++i) {
        assert(p0[i].empty());
    }

    shared_ptr<InitialValue[2][8][9]> p1 = make_shared_assert<InitialValue[2][8][9]>();
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 8; ++j) {
            for (int k = 0; k < 9; ++k) {
                assert(p1[i][j][k].value == 106);
            }
        }
    }

    shared_ptr<string[10][2]> p2 = make_shared<string[10][2]>({"Meow!", "Purr"});
    assert_shared_use_get(p2);
    for (int i = 0; i < 10; ++i) {
        assert(p2[i][0].compare("Meow!") == 0);
        assert(p2[i][1].compare("Purr") == 0);
    }

    shared_ptr<vector<int>[3]> p3 = make_shared<vector<int>[3]>({9, 9, 9});
    assert_shared_use_get(p3);
    for (int i = 0; i < 3; ++i) {
        assert(p3[i].size() == 3);
        for (const auto& val : p3[i]) {
            assert(val == 9);
        }
    }

    shared_ptr<ThreeIntWrap[5]> p4 = make_shared_init_assert<ThreeIntWrap[5]>({2, 8, 9});
    for (int i = 0; i < 5; ++i) {
        assert(p4[i].v1 == 2 && p4[i].v2 == 8 && p4[i].v3 == 9);
    }

    shared_ptr<int[1][7][2][9]> p5 = make_shared<int[1][7][2][9]>();
    assert_shared_use_get(p5);
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 9; ++k) {
                assert(p5[0][i][j][k] == 0);
            }
        }
    }

    shared_ptr<HighlyAligned[6]> p6 = make_shared<HighlyAligned[6]>();
    assert_shared_use_get(p6);
    assert(reinterpret_cast<uintptr_t>(p6.get()) % alignof(HighlyAligned) == 0);
    for (int i = 0; i < 6; ++i) {
        assert(p6[i].a == 0 && p6[i].b == 0 && p6[i].c == 0 && p6[i].d == 0);
    }

    test_make_init_destruct_order<ReportAddress[5]>(); // success one dimensional

    test_make_init_destruct_order<ReportAddress[20]>(); // failure one dimensional

    test_make_init_destruct_order<ReportAddress[2][2][2]>(); // success multidimensional

    test_make_init_destruct_order<ReportAddress[3][3][3]>(); // failure multidimensional

    shared_ptr<int[7]> p7 = make_shared<int[7]>(0);
    for (int i = 0; i < 7; ++i) {
        assert(p7[i] == 0);
    }
}

void test_make_shared_array_unknown_bounds() {
    shared_ptr<string[]> p0 = make_shared<string[]>(100);
    assert_shared_use_get(p0);
    for (int i = 0; i < 100; ++i) {
        assert(p0[i].empty());
    }

    shared_ptr<InitialValue[][8][9]> p1 = make_shared_assert<InitialValue[][8][9]>(2u);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 8; ++j) {
            for (int k = 0; k < 9; ++k) {
                assert(p1[i][j][k].value == 106);
            }
        }
    }

    shared_ptr<string[][2]> p2 = make_shared<string[][2]>(10, {"Meow!", "Purr"});
    assert_shared_use_get(p2);
    for (int i = 0; i < 10; ++i) {
        assert(p2[i][0].compare("Meow!") == 0);
        assert(p2[i][1].compare("Purr") == 0);
    }

    shared_ptr<vector<int>[]> p3 = make_shared<vector<int>[]>(3, {9, 9, 9});
    assert_shared_use_get(p3);
    for (int i = 0; i < 3; ++i) {
        assert(p3[i].size() == 3);
        for (const auto& val : p3[i]) {
            assert(val == 9);
        }
    }

    shared_ptr<ThreeIntWrap[]> p4 = make_shared_init_assert<ThreeIntWrap[]>(5, {2, 8, 9});
    for (int i = 0; i < 5; ++i) {
        assert(p4[i].v1 == 2 && p4[i].v2 == 8 && p4[i].v3 == 9);
    }

    shared_ptr<int[]> p5 = make_shared_assert<int[]>(0u); // p5 cannot be dereferenced

    shared_ptr<int[][5][6]> p6 = make_shared<int[][5][6]>(4u);
    assert_shared_use_get(p6);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 6; ++k) {
                assert(p6[i][j][k] == 0);
            }
        }
    }

    shared_ptr<HighlyAligned[]> p7 = make_shared<HighlyAligned[]>(7u);
    assert_shared_use_get(p7);
    assert(reinterpret_cast<uintptr_t>(p7.get()) % alignof(HighlyAligned) == 0);
    for (int i = 0; i < 7; ++i) {
        assert(p7[i].a == 0 && p7[i].b == 0 && p7[i].c == 0 && p7[i].d == 0);
    }

    test_make_init_destruct_order<ReportAddress[]>(5u); // success one dimensional

    test_make_init_destruct_order<ReportAddress[]>(20u); // failure one dimensional

    test_make_init_destruct_order<ReportAddress[][2][2]>(2u); // success multidimensional

    test_make_init_destruct_order<ReportAddress[][3][3]>(3u); // failure multidimensional

    shared_ptr<int[]> p8 = make_shared<int[]>(7u, 0);
    for (int i = 0; i < 7; ++i) {
        assert(p8[i] == 0);
    }
}

int constructCount = 0;
int destroyCount   = 0;

inline void assert_construct_destruct_equal() {
    assert(constructCount == destroyCount);
}

template <class T, class ConstructAssert>
struct ConstructConstrainingAllocator {
    using value_type = T;

    ConstructConstrainingAllocator() = default;
    template <class Other>
    ConstructConstrainingAllocator(const ConstructConstrainingAllocator<Other, ConstructAssert>&) {}
    ConstructConstrainingAllocator(const ConstructConstrainingAllocator&)            = default;
    ConstructConstrainingAllocator& operator=(const ConstructConstrainingAllocator&) = delete;

    T* allocate(size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) noexcept {
        return allocator<T>{}.deallocate(p, n);
    }

    template <class Other, class... Args>
    void construct(Other* p, Args&&... vals) {
        allocator<Other> a;
        static_assert(is_same_v<Other, value_type> && is_same_v<ConstructAssert, Other>, "incorrect construct call");
        allocator_traits<allocator<Other>>::construct(a, p, forward<Args>(vals)...);
        ++constructCount;
    }

    template <class Other>
    void destroy(Other* p) noexcept {
        allocator<Other> a;
        static_assert(is_same_v<Other, value_type> && is_same_v<ConstructAssert, Other>, "incorrect destroy call");
        allocator_traits<allocator<Other>>::destroy(a, p);
        ++destroyCount;
    }
};

template <typename T>
using CustomAlloc = ConstructConstrainingAllocator<void, T>;

template <class T, class... Args>
shared_ptr<T> allocate_shared_assert(int elemCount, Args&&... vals) {
    int aCount       = allocationCount;
    int cCount       = constructCount;
    shared_ptr<T> sp = allocate_shared<T>(forward<Args>(vals)...);
    assert_shared_use_get(sp);
    assert(aCount + 1 == allocationCount);
    assert(cCount + elemCount == constructCount);
    return sp;
}

template <class T, class A, enable_if_t<extent_v<T> != 0, int> = 0>
shared_ptr<T> allocate_shared_init_assert(int elemCount, const A& a, const remove_extent_t<T>& val) {
    return allocate_shared_assert<T>(elemCount, a, val);
}

template <class T, class A, enable_if_t<is_array_v<T> && extent_v<T> == 0, int> = 0>
shared_ptr<T> allocate_shared_init_assert(int elemCount, const A& a, size_t size, const remove_extent_t<T>& val) {
    return allocate_shared_assert<T>(elemCount, a, size, val);
}

template <class T, class... Args>
void test_allocate_init_destruct_order(Args&&... vals) {
    CustomAlloc<remove_all_extents_t<T>> a{};

    try {
        shared_ptr<T> sp = allocate_shared<T>(a, forward<Args>(vals)...);
        assert_shared_use_get(sp);
    } catch (const runtime_error& exc) {
        assert(exc.what() == "Can't create more ReportAddress objects."sv);
    }

    assert_construct_destruct_equal();
    assert_ascending_init();
    assert_descending_destruct();
}

void test_allocate_shared_not_array() {
    CustomAlloc<vector<int>> a0{};
    {
        shared_ptr<vector<int>> p0 = allocate_shared<vector<int>>(a0);
        assert_shared_use_get(p0);
        assert(p0->empty());
    }
    assert_construct_destruct_equal();

    CustomAlloc<InitialValue> a1{};
    {
        shared_ptr<InitialValue> p1 = allocate_shared_assert<InitialValue>(1, a1);
        assert(p1->value == 106);
    }
    assert_construct_destruct_equal();

    CustomAlloc<string> a2{};
    {
        shared_ptr<string> p2 = allocate_shared<string>(a2, "Meow!", 2u, 3u);
        assert_shared_use_get(p2);
        assert(p2->compare("ow!") == 0);
    }
    assert_construct_destruct_equal();

    {
        shared_ptr<InitialValue> p3 = allocate_shared_assert<InitialValue>(1, a1, 40, 2);
        assert(p3->value == 42);
    }
    assert_construct_destruct_equal();

    CustomAlloc<int> a4{};
    {
        shared_ptr<int> p4 = allocate_shared<int>(a4);
        assert_shared_use_get(p4);
        assert(*p4 == 0);
    }
    assert_construct_destruct_equal();

    CustomAlloc<HighlyAligned> a5{};
    {
        shared_ptr<HighlyAligned> p5 = allocate_shared<HighlyAligned>(a5);
        assert_shared_use_get(p5);
        assert(reinterpret_cast<uintptr_t>(p5.get()) % alignof(HighlyAligned) == 0);
        assert(p5->a == 0 && p5->b == 0 && p5->c == 0 && p5->d == 0);
    }
    assert_construct_destruct_equal();
}

void test_allocate_shared_array_known_bounds() {
    CustomAlloc<string> a0{};
    {
        shared_ptr<string[100]> p0 = allocate_shared<string[100]>(a0);
        assert_shared_use_get(p0);
        for (int i = 0; i < 100; ++i) {
            assert(p0[i].empty());
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<InitialValue> a1{};
    {
        shared_ptr<InitialValue[2][8][9]> p1 = allocate_shared_assert<InitialValue[2][8][9]>(144, a1);
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 8; ++j) {
                for (int k = 0; k < 9; ++k) {
                    assert(p1[i][j][k].value == 106);
                }
            }
        }
    }
    assert_construct_destruct_equal();

    {
        shared_ptr<string[10][2]> p2 = allocate_shared<string[10][2]>(a0, {"Meow!", "Purr"});
        assert_shared_use_get(p2);
        for (int i = 0; i < 10; ++i) {
            assert(p2[i][0].compare("Meow!") == 0);
            assert(p2[i][1].compare("Purr") == 0);
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<vector<int>> a3{};
    {
        shared_ptr<vector<int>[3]> p3 = allocate_shared<vector<int>[3]>(a3, {9, 9, 9});
        assert_shared_use_get(p3);
        for (int i = 0; i < 3; ++i) {
            assert(p3[i].size() == 3);
            for (const auto& val : p3[i]) {
                assert(val == 9);
            }
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<ThreeIntWrap> a4{};
    {
        shared_ptr<ThreeIntWrap[5]> p4 = allocate_shared_init_assert<ThreeIntWrap[5]>(5, a4, {2, 8, 9});
        for (int i = 0; i < 5; ++i) {
            assert(p4[i].v1 == 2 && p4[i].v2 == 8 && p4[i].v3 == 9);
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<int> a5{};
    {
        shared_ptr<int[1][7][2][9]> p5 = allocate_shared<int[1][7][2][9]>(a5);
        assert_shared_use_get(p5);
        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 9; ++k) {
                    assert(p5[0][i][j][k] == 0);
                }
            }
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<HighlyAligned> a6{};
    {
        shared_ptr<HighlyAligned[6]> p6 = allocate_shared<HighlyAligned[6]>(a6);
        assert_shared_use_get(p6);
        assert(reinterpret_cast<uintptr_t>(p6.get()) % alignof(HighlyAligned) == 0);
        for (int i = 0; i < 6; ++i) {
            assert(p6[i].a == 0 && p6[i].b == 0 && p6[i].c == 0 && p6[i].d == 0);
        }
    }
    assert_construct_destruct_equal();

    test_allocate_init_destruct_order<ReportAddress[5]>(); // success one dimensional

    test_allocate_init_destruct_order<ReportAddress[20]>(); // failure one dimensional

    test_allocate_init_destruct_order<ReportAddress[2][2][2]>(); // success multidimensional

    test_allocate_init_destruct_order<ReportAddress[3][3][3]>(); // failure multidimensional

    allocator<int> a7;
    shared_ptr<int[7]> p7 = allocate_shared<int[7]>(a7, 0);
    for (int i = 0; i < 7; ++i) {
        assert(p7[i] == 0);
    }
}

void test_allocate_shared_array_unknown_bounds() {
    CustomAlloc<string> a0{};
    {
        shared_ptr<string[]> p0 = allocate_shared<string[]>(a0, 100);
        assert_shared_use_get(p0);
        for (int i = 0; i < 100; ++i) {
            assert(p0[i].empty());
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<InitialValue> a1{};
    {
        shared_ptr<InitialValue[][8][9]> p1 = allocate_shared_assert<InitialValue[][8][9]>(144, a1, 2u);
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 8; ++j) {
                for (int k = 0; k < 9; ++k) {
                    assert(p1[i][j][k].value == 106);
                }
            }
        }
    }
    assert_construct_destruct_equal();

    {
        shared_ptr<string[][2]> p2 = allocate_shared<string[][2]>(a0, 10, {"Meow!", "Purr"});
        assert_shared_use_get(p2);
        for (int i = 0; i < 10; ++i) {
            assert(p2[i][0].compare("Meow!") == 0);
            assert(p2[i][1].compare("Purr") == 0);
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<vector<int>> a3{};
    {
        shared_ptr<vector<int>[]> p3 = allocate_shared<vector<int>[]>(a3, 3, {9, 9, 9});
        assert_shared_use_get(p3);
        for (int i = 0; i < 3; ++i) {
            assert(p3[i].size() == 3);
            for (const auto& val : p3[i]) {
                assert(val == 9);
            }
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<ThreeIntWrap> a4{};
    {
        shared_ptr<ThreeIntWrap[]> p4 = allocate_shared_init_assert<ThreeIntWrap[]>(5, a4, 5, {2, 8, 9});
        for (int i = 0; i < 5; ++i) {
            assert(p4[i].v1 == 2 && p4[i].v2 == 8 && p4[i].v3 == 9);
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<int> a5{};
    { shared_ptr<int[]> p5 = allocate_shared_assert<int[]>(0, a5, 0u); } // p5 cannot be dereferenced
    assert_construct_destruct_equal();

    {
        shared_ptr<int[][5][6]> p6 = allocate_shared<int[][5][6]>(a5, 4u);
        assert_shared_use_get(p6);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 5; ++j) {
                for (int k = 0; k < 6; ++k) {
                    assert(p6[i][j][k] == 0);
                }
            }
        }
    }
    assert_construct_destruct_equal();

    CustomAlloc<HighlyAligned> a7{};
    {
        shared_ptr<HighlyAligned[]> p7 = allocate_shared<HighlyAligned[]>(a7, 7u);
        assert_shared_use_get(p7);
        assert(reinterpret_cast<uintptr_t>(p7.get()) % alignof(HighlyAligned) == 0);
        for (int i = 0; i < 7; ++i) {
            assert(p7[i].a == 0 && p7[i].b == 0 && p7[i].c == 0 && p7[i].d == 0);
        }
    }
    assert_construct_destruct_equal();

    test_allocate_init_destruct_order<ReportAddress[]>(5u); // success one dimensional

    test_allocate_init_destruct_order<ReportAddress[]>(20u); // failure one dimensional

    test_allocate_init_destruct_order<ReportAddress[][2][2]>(2u); // success multidimensional

    test_allocate_init_destruct_order<ReportAddress[][3][3]>(3u); // failure multidimensional

    allocator<int> a8;
    shared_ptr<int[]> p8 = allocate_shared<int[]>(a8, 7u, 0);
    for (int i = 0; i < 7; ++i) {
        assert(p8[i] == 0);
    }
}

// Test GH-1733 "<memory>: error C2694 when calling make_shared on class with throwing destructor"
struct NontrivialThrowingDtor {
    ~NontrivialThrowingDtor() noexcept(false) {}
};
static_assert(!is_nothrow_destructible_v<NontrivialThrowingDtor>);
static_assert(!is_trivially_destructible_v<NontrivialThrowingDtor>);

struct TrivialThrowingDtor {
    ~TrivialThrowingDtor() noexcept(false) = default;
};

static_assert(!is_nothrow_destructible_v<TrivialThrowingDtor>);
static_assert(is_trivially_destructible_v<TrivialThrowingDtor>);

template <class T>
struct WeirdDeleter {
    void operator()(T* const ptr) const {
        delete ptr;
    }

    ~WeirdDeleter() noexcept(false) {}
};
static_assert(!is_nothrow_destructible_v<WeirdDeleter<int>>);

void test_gh_1733() {
    // GH-1733 <memory>: error C2694 when calling make_shared on class with throwing destructor
    WeirdDeleter<NontrivialThrowingDtor> del;
    allocator<int> al;

    // _Ref_count
    (void) shared_ptr<NontrivialThrowingDtor>{new NontrivialThrowingDtor};

    // _Ref_count_resource
    (void) shared_ptr<NontrivialThrowingDtor>{new NontrivialThrowingDtor, del};

    // _Ref_count_resource_alloc
    (void) shared_ptr<NontrivialThrowingDtor>{new NontrivialThrowingDtor, del, al};

    // _Ref_count_obj2
    (void) make_shared<NontrivialThrowingDtor>();

    // _Ref_count_obj_alloc3
    (void) allocate_shared<NontrivialThrowingDtor>(al);

    // _Ref_count_unbounded_array<_Ty, true>
    (void) make_shared<TrivialThrowingDtor[]>(10);

    // _Ref_count_unbounded_array<_Ty, false>
    (void) make_shared<NontrivialThrowingDtor[]>(10);

    // _Ref_count_bounded_array
    (void) make_shared<NontrivialThrowingDtor[10]>();

    // _Ref_count_unbounded_array_alloc
    (void) allocate_shared<NontrivialThrowingDtor[]>(al, 10);

    // _Ref_count_bounded_array_alloc
    (void) allocate_shared<NontrivialThrowingDtor[10]>(al);
}

int main() {
    test_make_shared_not_array();
    test_make_shared_array_known_bounds();
    test_make_shared_array_unknown_bounds();

    test_allocate_shared_not_array();
    test_allocate_shared_array_known_bounds();
    test_allocate_shared_array_unknown_bounds();

    test_gh_1733();
}
