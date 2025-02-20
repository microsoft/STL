// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <type_traits>
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

template <class SetContainer>
void test_emplace_for_unique_set() {
    int lvalueInt{42};
    test_emplaces<SetContainer>(lvalueInt);
}

template <class MapContainer>
void test_emplace_for_unique_map() {
    int lvalueInt{42};
    test_emplaces<MapContainer>(lvalueInt, lvalueInt);

    pair<const int, int> lvalueConstPair{42, 65};
    pair<int, int> lvaluePair{42, 65};
    pair<const int&, int> lvalueConstRefPair{lvalueInt, 65};
    pair<int&, int> lvalueRefPair{lvalueInt, 65};
    pair<const int&&, int> lvalueConstRefRefPair{move(lvalueInt), 65};
    pair<int&&, int> lvalueRefRefPair{move(lvalueInt), 65};
    test_emplaces<MapContainer>(lvalueConstPair);
    test_emplaces<MapContainer>(lvaluePair);
    test_emplaces<MapContainer>(lvalueConstRefPair);
    test_emplaces<MapContainer>(lvalueRefPair);
    test_emplaces<MapContainer>(lvalueConstRefRefPair);
    test_emplaces<MapContainer>(lvalueRefRefPair);

#if _HAS_CXX23
    tuple<const int, int> lvalueConstTuple{42, 65};
    tuple<int, int> lvalueTuple{42, 65};
    tuple<const int&, int> lvalueConstRefTuple{lvalueInt, 65};
    tuple<int&, int> lvalueRefTuple{lvalueInt, 65};
    tuple<const int&&, int> lvalueConstRefRefTuple{move(lvalueInt), 65};
    tuple<int&&, int> lvalueRefRefTuple{move(lvalueInt), 65};
    test_emplaces<MapContainer>(lvalueConstTuple);
    test_emplaces<MapContainer>(lvalueTuple);
    test_emplaces<MapContainer>(lvalueConstRefTuple);
    test_emplaces<MapContainer>(lvalueRefTuple);
    test_emplaces<MapContainer>(lvalueConstRefRefTuple);
    test_emplaces<MapContainer>(lvalueRefRefTuple);

    array<int, 2> arr{42, 65};
    array<const int, 2> constArr{42, 65};
    test_emplaces<MapContainer>(arr);
    test_emplaces<MapContainer>(constArr);
#endif // _HAS_CXX23

    tuple<int> tupleIntSixtyFive{65};

    tuple<const int> lvalueConstOneTuple{42};
    tuple<int> lvalueOneTuple{42};
    tuple<const int&> lvalueConstRefOneTuple{lvalueInt};
    tuple<int&> lvalueRefOneTuple{lvalueInt};
    test_emplaces<MapContainer>(piecewise_construct, lvalueConstOneTuple, tupleIntSixtyFive);
    test_emplaces<MapContainer>(piecewise_construct, lvalueOneTuple, tupleIntSixtyFive);
    test_emplaces<MapContainer>(piecewise_construct, lvalueConstRefOneTuple, tupleIntSixtyFive);
    test_emplaces<MapContainer>(piecewise_construct, lvalueRefOneTuple, tupleIntSixtyFive);
}

// also test that the optimization strategy does not mishandle volatile arguments
template <class SetContainer>
void test_volatile_arguments_for_unique_set() {
    using Key = typename SetContainer::value_type;

    SetContainer s;

    volatile Key x         = 0;
    const volatile Key& cx = x;

    s.emplace(x);
    x = 1;
    s.emplace(move(x));
    x = 2;
    s.emplace(cx);
    x = 3;
    s.emplace(move(cx));

    x = 4;
    s.emplace_hint(s.end(), x);
    x = 5;
    s.emplace_hint(s.end(), move(x));
    x = 6;
    s.emplace_hint(s.end(), cx);
    x = 7;
    s.emplace_hint(s.end(), move(cx));

    assert((s == SetContainer{0, 1, 2, 3, 4, 5, 6, 7}));
}

template <class MapContainer, class PairLike>
void test_pair_like_volatile_for_unique_map() {
    using First  = tuple_element_t<0, PairLike>;
    using Second = tuple_element_t<1, PairLike>;

    MapContainer m;

    volatile remove_cv_t<remove_reference_t<First>> x = 0;
    {
        PairLike p{static_cast<First>(x), Second{}};
        m.emplace(p);
    }

    x = 1;
    {
        PairLike p{static_cast<First>(x), Second{}};
        m.emplace(move(p));
    }

    x = 2;
    {
        PairLike p{static_cast<First>(x), Second{}};
        const auto& cp = p;
        m.emplace(cp);
    }

    x = 3;
    {
        PairLike p{static_cast<First>(x), Second{}};
        const auto& cp = p;
        m.emplace(move(cp));
    }

    x = 4;
    {
        PairLike p{static_cast<First>(x), Second{}};
        m.emplace_hint(m.end(), p);
    }

    x = 5;
    {
        PairLike p{static_cast<First>(x), Second{}};
        m.emplace_hint(m.end(), move(p));
    }

    x = 6;
    {
        PairLike p{static_cast<First>(x), Second{}};
        const auto& cp = p;
        m.emplace_hint(m.end(), cp);
    }

    x = 7;
    {
        PairLike p{static_cast<First>(x), Second{}};
        const auto& cp = p;
        m.emplace_hint(m.end(), move(cp));
    }

    assert((m == MapContainer{{0, {}}, {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}}));
}

template <class MapContainer, class Tuple>
void test_piecewise_volatile_for_unique_map() {
    using First = tuple_element_t<0, Tuple>;

    MapContainer m;

    volatile remove_cv_t<remove_reference_t<First>> x = 0;
    {
        Tuple tp{static_cast<First>(x)};
        m.emplace(piecewise_construct, tp, tuple<>{});
    }

    x = 1;
    {
        Tuple tp{static_cast<First>(x)};
        m.emplace(piecewise_construct, move(tp), tuple<>{});
    }

    x = 2;
    {
        Tuple tp{static_cast<First>(x)};
        const auto& ctp = tp;
        m.emplace(piecewise_construct, ctp, tuple<>{});
    }

    x = 3;
    {
        Tuple tp{static_cast<First>(x)};
        const auto& ctp = tp;
        m.emplace(piecewise_construct, move(ctp), tuple<>{});
    }

    x = 4;
    {
        Tuple tp{static_cast<First>(x)};
        m.emplace_hint(m.end(), piecewise_construct, tp, tuple<>{});
    }

    x = 5;
    {
        Tuple tp{static_cast<First>(x)};
        m.emplace_hint(m.end(), piecewise_construct, move(tp), tuple<>{});
    }

    x = 6;
    {
        Tuple tp{static_cast<First>(x)};
        const auto& ctp = tp;
        m.emplace_hint(m.end(), piecewise_construct, ctp, tuple<>{});
    }

    x = 7;
    {
        Tuple tp{static_cast<First>(x)};
        const auto& ctp = tp;
        m.emplace_hint(m.end(), piecewise_construct, move(ctp), tuple<>{});
    }

    assert((m == MapContainer{{0, {}}, {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}}));
}

template <class MapContainer>
void test_volatile_arguments_for_unique_map() {
    using Key    = typename MapContainer::key_type;
    using Mapped = typename MapContainer::mapped_type;

    {
        volatile Key x         = 0;
        const volatile Key& cx = x;

        MapContainer m;

        m.emplace(x, Mapped{});
        x = 1;
        m.emplace(move(x), Mapped{});
        x = 2;
        m.emplace(cx, Mapped{});
        x = 3;
        m.emplace(move(cx), Mapped{});

        x = 4;
        m.emplace_hint(m.end(), x, Mapped{});
        x = 5;
        m.emplace_hint(m.end(), move(x), Mapped{});
        x = 6;
        m.emplace_hint(m.end(), cx, Mapped{});
        x = 7;
        m.emplace_hint(m.end(), move(cx), Mapped{});

        assert((m == MapContainer{{0, {}}, {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}}));
    }

    test_pair_like_volatile_for_unique_map<MapContainer, pair<volatile Key, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, pair<const volatile Key, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, pair<volatile Key&, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, pair<const volatile Key&, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, pair<volatile Key&&, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, pair<const volatile Key&&, Mapped>>();
#if _HAS_CXX23
    test_pair_like_volatile_for_unique_map<MapContainer, tuple<volatile Key, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, tuple<const volatile Key, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, tuple<volatile Key&, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, tuple<const volatile Key&, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, tuple<volatile Key&&, Mapped>>();
    test_pair_like_volatile_for_unique_map<MapContainer, tuple<const volatile Key&&, Mapped>>();

    test_pair_like_volatile_for_unique_map<MapContainer, array<volatile Key, 2>>();
    test_pair_like_volatile_for_unique_map<MapContainer, array<const volatile Key, 2>>();
#endif // _HAS_CXX23

    test_piecewise_volatile_for_unique_map<MapContainer, tuple<volatile Key>>();
    test_piecewise_volatile_for_unique_map<MapContainer, tuple<const volatile Key>>();
    test_piecewise_volatile_for_unique_map<MapContainer, tuple<volatile Key&>>();
    test_piecewise_volatile_for_unique_map<MapContainer, tuple<const volatile Key&>>();
}

int main() {
    // This tests that unordered_(map|set) detect cases they can avoid allocating.
    // It isn't strictly required by the standard, but we think it should be applicable to good implementations.
    test_emplace_for_unique_set<set<int, less<>, TestAlloc<int>>>();
    test_emplace_for_unique_set<unordered_set<int, hash<int>, equal_to<>, TestAlloc<int>>>();

    test_emplace_for_unique_map<map<int, int, less<>, TestAlloc<pair<const int, int>>>>();
    test_emplace_for_unique_map<unordered_map<int, int, hash<int>, equal_to<>, TestAlloc<pair<const int, int>>>>();

    test_volatile_arguments_for_unique_set<set<int>>();
    test_volatile_arguments_for_unique_set<unordered_set<int>>();

    test_volatile_arguments_for_unique_map<map<int, long>>();
    test_volatile_arguments_for_unique_map<unordered_map<int, long>>();
}
