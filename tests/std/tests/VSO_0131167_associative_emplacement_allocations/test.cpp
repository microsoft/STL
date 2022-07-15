// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

template <class Iter>
void assert_same_obj(const Iter& lhs, const Iter& rhs) {
    assert(&*lhs == &*rhs);
}

static bool g_allow_allocations = true;

template <class T>
struct TestAlloc {
    using value_type = T;

    TestAlloc()                            = default;
    TestAlloc(const TestAlloc&)            = default;
    TestAlloc& operator=(const TestAlloc&) = default;

    template <class U>
    TestAlloc(const TestAlloc<U>&) {}

    T* allocate(const size_t n) {
        assert(g_allow_allocations);
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) {
        allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    bool operator==(const TestAlloc<U>&) const {
        return true;
    }

    template <class U>
    bool operator!=(const TestAlloc<U>&) const {
        return false;
    }
};

struct NoAllocGuard {
    NoAllocGuard() {
        g_allow_allocations = false;
    }

    NoAllocGuard(const NoAllocGuard&)            = delete;
    NoAllocGuard& operator=(const NoAllocGuard&) = delete;

    ~NoAllocGuard() {
        g_allow_allocations = true;
    }
};

template <class Container, class FirstValue, class... Values>
void test_emplaces(FirstValue& val1, Values&... valn) {
    const FirstValue& constVal1 = val1;

    {
        Container c;
        const auto insertPos = c.emplace(val1, valn...).first;
        {
            NoAllocGuard guard;
            assert_same_obj(insertPos, c.emplace(val1, valn...).first);
        }
    }

    {
        Container c;
        const auto insertPos = c.emplace(val1, valn...).first;
        {
            NoAllocGuard guard;
            assert_same_obj(insertPos, c.emplace(constVal1, valn...).first);
        }
    }

    {
        Container c;
        const auto insertPos = c.emplace(val1, valn...).first;
        {
            NoAllocGuard guard;
            assert_same_obj(insertPos, c.emplace(move(val1), valn...).first);
        }
    }

    {
        Container c;
        const auto insertPos = c.emplace(val1, valn...).first;
        {
            NoAllocGuard guard;
            assert_same_obj(insertPos, c.emplace(move(constVal1), valn...).first);
        }
    }

    {
        Container c;
        const auto insertPos = c.emplace(val1, valn...).first;
        {
            NoAllocGuard guard;
            assert_same_obj(insertPos, c.emplace_hint(insertPos, val1, valn...));
            assert_same_obj(insertPos, c.emplace_hint(c.end(), val1, valn...));
        }
    }

    {
        Container c;
        const auto insertPos = c.emplace(val1, valn...).first;
        {
            NoAllocGuard guard;
            assert_same_obj(insertPos, c.emplace_hint(insertPos, constVal1, valn...));
            assert_same_obj(insertPos, c.emplace_hint(c.end(), constVal1, valn...));
        }
    }

    {
        Container c;
        const auto insertPos = c.emplace(val1, valn...).first;
        {
            NoAllocGuard guard;
            assert_same_obj(insertPos, c.emplace_hint(insertPos, move(val1), valn...));
            assert_same_obj(insertPos, c.emplace_hint(c.end(), move(val1), valn...));
        }
    }

    {
        Container c;
        const auto insertPos = c.emplace(val1, valn...).first;
        {
            NoAllocGuard guard;
            assert_same_obj(insertPos, c.emplace_hint(insertPos, move(constVal1), valn...));
            assert_same_obj(insertPos, c.emplace_hint(c.end(), move(constVal1), valn...));
        }
    }
}

int main() {
    // This tests that unordered_(map|set) detect cases they can avoid allocating.
    // It isn't strictly required by the standard, but we think it should be applicable to good implementations.
    int lvalueInt{42};
    test_emplaces<unordered_set<int, hash<int>, equal_to<>, TestAlloc<int>>>(lvalueInt);
    using MapType = unordered_map<int, int, hash<int>, equal_to<>, TestAlloc<pair<const int, int>>>;
    test_emplaces<MapType>(lvalueInt, lvalueInt);
    pair<const int, int> lvalueConstPair{42, 65};
    pair<int, int> lvaluePair{42, 65};
    pair<const int&, int> lvalueConstRefPair{lvalueInt, 65};
    pair<int&, int> lvalueRefPair{lvalueInt, 65};
    pair<const int&&, int> lvalueConstRefRefPair{move(lvalueInt), 65};
    pair<int&&, int> lvalueRefRefPair{move(lvalueInt), 65};
    test_emplaces<MapType>(lvalueConstPair);
    test_emplaces<MapType>(lvaluePair);
    test_emplaces<MapType>(lvalueConstRefPair);
    test_emplaces<MapType>(lvalueRefPair);
    test_emplaces<MapType>(lvalueConstRefRefPair);
    test_emplaces<MapType>(lvalueRefRefPair);
}
