// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

struct TmpInt {
    constexpr TmpInt() : TmpInt(0) {}
    constexpr TmpInt(int v) : val{v} {}

    constexpr TmpInt(const TmpInt&) = default;
    constexpr TmpInt(TmpInt&& other) : val{exchange(other.val, -1)} {}

    constexpr TmpInt& operator=(const TmpInt&) = default;
    constexpr TmpInt& operator=(TmpInt&& other) {
        if (this != &other) {
            val = exchange(other.val, -1);
        }
        return *this;
    }

    int val;
    constexpr bool operator==(const TmpInt&) const = default;
};

template <template <class> class PairLike>
constexpr bool test_pair_like_constructor() {
    // Test pair(pair-like) constructor with PairLike<int>&
    static_assert(constructible_from<pair<int, int>, PairLike<int>&>);
    static_assert(constructible_from<pair<int&, int&>, PairLike<int>&>);
    static_assert(constructible_from<pair<const int&, const int&>, PairLike<int>&>);
    static_assert(!constructible_from<pair<int&&, int&&>, PairLike<int>&>);
    static_assert(!constructible_from<pair<const int&&, const int&&>, PairLike<int>&>);

    // Test pair(pair-like) constructor with const PairLike<int>&
    static_assert(constructible_from<pair<int, int>, const PairLike<int>&>);
    static_assert(!constructible_from<pair<int&, int&>, const PairLike<int>&>);
    static_assert(constructible_from<pair<const int&, const int&>, const PairLike<int>&>);
    static_assert(!constructible_from<pair<int&&, int&&>, const PairLike<int>&>);
    static_assert(!constructible_from<pair<const int&&, const int&&>, const PairLike<int>&>);

    // Test pair(pair-like) constructor with PairLike<int>
    static_assert(constructible_from<pair<int, int>, PairLike<int>>);
    static_assert(!constructible_from<pair<int&, int&>, PairLike<int>>);
    static_assert(constructible_from<pair<const int&, const int&>, PairLike<int>>);
    static_assert(constructible_from<pair<int&&, int&&>, PairLike<int>>);
    static_assert(constructible_from<pair<const int&&, const int&&>, PairLike<int>>);

    // Test pair(pair-like) constructor with const PairLike<int>
    static_assert(constructible_from<pair<int, int>, const PairLike<int>>);
    static_assert(!constructible_from<pair<int&, int&>, const PairLike<int>>);
    static_assert(constructible_from<pair<const int&, const int&>, const PairLike<int>>);
    static_assert(!constructible_from<pair<int&&, int&&>, const PairLike<int>>);
    static_assert(constructible_from<pair<const int&&, const int&&>, const PairLike<int>>);

    PairLike<TmpInt> a = {1, 2};

    pair<TmpInt, TmpInt> p1(a);
    assert(p1.first == 1 && p1.second == 2 && get<0>(a) == 1 && get<1>(a) == 2);

    pair<TmpInt&, TmpInt&> p2(a);
    assert(&p2.first == &get<0>(a) && &p2.second == &get<1>(a));

    pair<const TmpInt&, const TmpInt&> p3(a);
    assert(&p3.first == &get<0>(a) && &p3.second == &get<1>(a));

    pair<TmpInt, TmpInt> p4(as_const(a));
    assert(p4.first == 1 && p4.second == 2 && get<0>(a) == 1 && get<1>(a) == 2);

    pair<const TmpInt&, const TmpInt&> p5(as_const(a));
    assert(&p5.first == &get<0>(a) && &p5.second == &get<1>(a));

    pair<TmpInt, TmpInt> p6(std::move(a));
    assert(p6.first == 1 && p6.second == 2 && get<0>(a) == -1 && get<1>(a) == -1);
    tie(get<0>(a), get<1>(a)) = std::move(p6);

    pair<const TmpInt&, const TmpInt&> p7(std::move(a));
    assert(&p7.first == &get<0>(a) && &p7.second == &get<1>(a));

    pair<TmpInt&&, TmpInt&&> p8(std::move(a));
    assert(&p8.first == &get<0>(a) && &p8.second == &get<1>(a));

    pair<const TmpInt&&, const TmpInt&&> p9(std::move(a));
    assert(&p9.first == &get<0>(a) && &p9.second == &get<1>(a));

    pair<TmpInt, TmpInt> p10(std::move(as_const(a)));
    assert(p10.first == 1 && p10.second == 2 && get<0>(a) == 1 && get<1>(a) == 2);

    pair<const TmpInt&, const TmpInt&> p11(std::move(as_const(a)));
    assert(p11.first == 1 && p11.second == 2 && get<0>(a) == 1 && get<1>(a) == 2);

    pair<const TmpInt&&, const TmpInt&&> p12(std::move(as_const(a)));
    assert(p12.first == 1 && p12.second == 2 && get<0>(a) == 1 && get<1>(a) == 2);

    return true;
}

template <template <class> class PairLike>
constexpr bool test_pair_like_assignment() {
    static_assert(is_assignable_v<pair<int, int>&, PairLike<int>&>);
    static_assert(is_assignable_v<pair<int, int>&, const PairLike<int>&>);
    static_assert(is_assignable_v<pair<int, int>&, PairLike<int>>);
    static_assert(is_assignable_v<pair<int, int>&, const PairLike<int>>);

    PairLike<TmpInt> a = {1, 2};

    pair<TmpInt, TmpInt> p1;
    p1 = a;
    assert(p1.first == 1 && p1.second == 2 && get<0>(a) == 1 && get<1>(a) == 2);

    pair<TmpInt, TmpInt> p2;
    p2 = as_const(a);
    assert(p2.first == 1 && p2.second == 2 && get<0>(a) == 1 && get<1>(a) == 2);

    pair<TmpInt, TmpInt> p3;
    p3 = std::move(a);
    assert(p3.first == 1 && p3.second == 2 && get<0>(a) == -1 && get<1>(a) == -1);
    tie(get<0>(a), get<1>(a)) = std::move(p3);

    pair<TmpInt, TmpInt> p4;
    p4 = std::move(as_const(a));
    assert(p4.first == 1 && p4.second == 2 && get<0>(a) == 1 && get<1>(a) == 2);

    return true;
}

template <template <class> class PairLike>
constexpr bool test_pair_like_const_assignment() {
    using Ref  = vector<bool>::reference;
    using Pair = pair<Ref, Ref>;

    static_assert(is_assignable_v<const Pair&, PairLike<bool>&>);
    static_assert(is_assignable_v<const Pair&, const PairLike<bool>&>);
    static_assert(is_assignable_v<const Pair&, PairLike<bool>>);
    static_assert(is_assignable_v<const Pair&, const PairLike<bool>>);

    vector<bool> bools = {false, true};
    PairLike<bool> a   = {true, false};

    const Pair p1{bools[0], bools[1]};
    p1 = a;
    assert(p1.first == true && p1.second == false);
    bools = {false, true};

    const Pair p2{bools[0], bools[1]};
    p2 = as_const(a);
    assert(p2.first == true && p2.second == false);
    bools = {false, true};

    const Pair p3{bools[0], bools[1]};
    p3 = std::move(a);
    assert(p3.first == true && p3.second == false);
    bools = {false, true};

    const Pair p4{bools[0], bools[1]};
    p4 = std::move(as_const(a));
    assert(p4.first == true && p4.second == false);

    return true;
}

template <class T>
using BinaryTuple = tuple<T, T>;

template <class T>
using BinaryArray = array<T, 2>;

int main() {
    static_assert(test_pair_like_constructor<BinaryArray>());
    assert((test_pair_like_constructor<BinaryArray>()));
    static_assert(test_pair_like_constructor<BinaryTuple>());
    assert((test_pair_like_constructor<BinaryTuple>()));

    static_assert(test_pair_like_assignment<BinaryArray>());
    assert((test_pair_like_assignment<BinaryArray>()));
    static_assert(test_pair_like_assignment<BinaryTuple>());
    assert((test_pair_like_assignment<BinaryTuple>()));

    static_assert(test_pair_like_const_assignment<BinaryArray>());
    assert((test_pair_like_const_assignment<BinaryArray>()));
    static_assert(test_pair_like_const_assignment<BinaryTuple>());
    assert((test_pair_like_const_assignment<BinaryTuple>()));
}
