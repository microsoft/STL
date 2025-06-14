// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^

using namespace std;

struct Cat {};
struct Leopard : Cat {
    int spots_;

    Leopard()                       = default;
    Leopard(const Leopard&)         = default;
    Leopard(Leopard&&)              = default;
    Leopard& operator=(Leopard&) && = delete;
    using Cat::operator=;
};

constexpr pair<int, int> expected_results[]{{5, 6}, {3, 4}, {1, 2}};

CONSTEXPR20 void test_reverse_copy() {
    {
        pair<int, int> src[]      = {{1, 2}, {3, 4}, {5, 6}};
        pair<int, int> dst[]      = {{3, 1}, {4, 1}, {5, 9}};
        pair<int&, int&> srcref[] = {
            {src[0].first, src[0].second}, {src[1].first, src[1].second}, {src[2].first, src[2].second}};
        pair<int&, int&> dstref[] = {
            {dst[0].first, dst[0].second}, {dst[1].first, dst[1].second}, {dst[2].first, dst[2].second}};

        reverse_copy(begin(srcref), end(srcref), dstref);
        assert(equal(begin(dst), end(dst), begin(expected_results), end(expected_results)));
    }
#if _HAS_CXX20
    {
        pair<int, int> src[]      = {{1, 2}, {3, 4}, {5, 6}};
        pair<int, int> dst[]      = {{3, 1}, {4, 1}, {5, 9}};
        pair<int&, int&> srcref[] = {
            {src[0].first, src[0].second}, {src[1].first, src[1].second}, {src[2].first, src[2].second}};
        pair<int&, int&> dstref[] = {
            {dst[0].first, dst[0].second}, {dst[1].first, dst[1].second}, {dst[2].first, dst[2].second}};

        ranges::reverse_copy(srcref, dstref);
        assert(ranges::equal(dst, expected_results));
    }
#endif // _HAS_CXX20
#if _HAS_CXX23
    {
        pair<int, int> src[]      = {{1, 2}, {3, 4}, {5, 6}};
        pair<int, int> dst[]      = {{3, 1}, {4, 1}, {5, 9}};
        pair<int&, int&> srcref[] = {src[0], src[1], src[2]};
        pair<int&, int&> dstref[] = {dst[0], dst[1], dst[2]};

        reverse_copy(begin(srcref), end(srcref), dstref);
        assert(equal(begin(dst), end(dst), begin(expected_results), end(expected_results)));
    }
    {
        pair<int, int> src[]      = {{1, 2}, {3, 4}, {5, 6}};
        pair<int, int> dst[]      = {{3, 1}, {4, 1}, {5, 9}};
        pair<int&, int&> srcref[] = {src[0], src[1], src[2]};
        pair<int&, int&> dstref[] = {dst[0], dst[1], dst[2]};

        ranges::reverse_copy(srcref, dstref);
        assert(ranges::equal(dst, expected_results));
    }
#endif // _HAS_CXX23
}

constexpr Leopard make_leopard(const int n) noexcept {
    Leopard result{};
    result.spots_ = n;
    return result;
}

CONSTEXPR20 void test_copy_move_leopards() {
    constexpr Leopard expected_leopards[]{
        make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
    constexpr Leopard zero_leopards[6]{};
    auto equal_leopard = [](const Leopard& lhs, const Leopard& rhs) { return lhs.spots_ == rhs.spots_; };
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        copy(begin(zero_leopards), end(zero_leopards), begin(dst));
        assert(equal(begin(dst), end(dst), begin(expected_leopards), end(expected_leopards), equal_leopard));
    }
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        copy_n(begin(zero_leopards), size(zero_leopards), begin(dst));
        assert(equal(begin(dst), end(dst), begin(expected_leopards), end(expected_leopards), equal_leopard));
    }
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        copy_backward(begin(zero_leopards), end(zero_leopards), end(dst));
        assert(equal(begin(dst), end(dst), begin(expected_leopards), end(expected_leopards), equal_leopard));
    }
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        move(begin(zero_leopards), end(zero_leopards), begin(dst));
        assert(equal(begin(dst), end(dst), begin(expected_leopards), end(expected_leopards), equal_leopard));
    }
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        move_backward(begin(zero_leopards), end(zero_leopards), end(dst));
        assert(equal(begin(dst), end(dst), begin(expected_leopards), end(expected_leopards), equal_leopard));
    }
#if _HAS_CXX20
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        ranges::copy(zero_leopards, dst);
        assert(ranges::equal(dst, expected_leopards, equal_leopard));
    }
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        ranges::copy_n(ranges::begin(zero_leopards), ranges::distance(zero_leopards), dst);
        assert(ranges::equal(dst, expected_leopards, equal_leopard));
    }
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        ranges::copy_backward(zero_leopards, ranges::end(dst));
        assert(ranges::equal(dst, expected_leopards, equal_leopard));
    }
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        ranges::move(zero_leopards, dst);
        assert(ranges::equal(dst, expected_leopards, equal_leopard));
    }
    {
        Leopard dst[]{
            make_leopard(3), make_leopard(1), make_leopard(4), make_leopard(1), make_leopard(5), make_leopard(9)};
        ranges::move_backward(zero_leopards, ranges::end(dst));
        assert(ranges::equal(dst, expected_leopards, equal_leopard));
    }
#endif // _HAS_CXX20
}

// Pedantically, all of MSVC, Clang, and EDG are currently wrong on this, see LLVM-37038.
// However, if compilers get corrected, the assignment operators of `DerivedLeopard` and `LeopardHouse` will be trivial
// but no-op, and the library side can't correctly conclude that assignments for them shouldn't be vectorized.
// As a result, we keep this as a regression test.
CONSTEXPR20 void test_llvm_37038() {
    struct DerivedLeopard : Leopard {};
    static_assert(is_trivially_move_assignable_v<DerivedLeopard>, "");

    auto make_derived_leopard = [](int n) {
        DerivedLeopard ret{};
        ret.spots_ = n;
        return ret;
    };

    auto equal_derived = [](const DerivedLeopard& lhs, const DerivedLeopard& rhs) { return lhs.spots_ == rhs.spots_; };

    {
        DerivedLeopard src[]{
            make_derived_leopard(1), make_derived_leopard(7), make_derived_leopard(2), make_derived_leopard(9)};
        DerivedLeopard dst[4]{};
        move(begin(src), end(src), dst);
        assert(equal(begin(dst), end(dst), begin(src), end(src), equal_derived));
    }
    {
        DerivedLeopard src[]{
            make_derived_leopard(1), make_derived_leopard(7), make_derived_leopard(2), make_derived_leopard(9)};
        DerivedLeopard dst[4]{};
        move_backward(begin(src), end(src), end(dst));
        assert(equal(begin(dst), end(dst), begin(src), end(src), equal_derived));
    }
#if _HAS_CXX20
    {
        DerivedLeopard src[]{
            make_derived_leopard(1), make_derived_leopard(7), make_derived_leopard(2), make_derived_leopard(9)};
        DerivedLeopard dst[4]{};
        ranges::move(src, dst);
        assert(ranges::equal(src, dst, equal_derived));
    }
    {
        DerivedLeopard src[]{
            make_derived_leopard(1), make_derived_leopard(7), make_derived_leopard(2), make_derived_leopard(9)};
        DerivedLeopard dst[4]{};
        ranges::move_backward(src, ranges::end(dst));
        assert(ranges::equal(src, dst, equal_derived));
    }
#endif // _HAS_CXX20

    struct LeopardHouse {
        Leopard bigcat_;
    };
    static_assert(is_trivially_move_assignable_v<LeopardHouse>, "");

    auto make_leopard_house = [](int n) {
        LeopardHouse ret{};
        ret.bigcat_.spots_ = n;
        return ret;
    };

    auto equal_house = [](const LeopardHouse& lhs, const LeopardHouse& rhs) {
        return lhs.bigcat_.spots_ == rhs.bigcat_.spots_;
    };

    {
        LeopardHouse src[]{make_leopard_house(1), make_leopard_house(7), make_leopard_house(2), make_leopard_house(9)};
        LeopardHouse dst[4]{};
        move(begin(src), end(src), dst);
        assert(equal(begin(dst), end(dst), begin(src), end(src), equal_house));
    }
    {
        LeopardHouse src[]{make_leopard_house(1), make_leopard_house(7), make_leopard_house(2), make_leopard_house(9)};
        LeopardHouse dst[4]{};
        move_backward(begin(src), end(src), end(dst));
        assert(equal(begin(dst), end(dst), begin(src), end(src), equal_house));
    }
#if _HAS_CXX20
    {
        LeopardHouse src[]{make_leopard_house(1), make_leopard_house(7), make_leopard_house(2), make_leopard_house(9)};
        LeopardHouse dst[4]{};
        ranges::move(src, dst);
        assert(ranges::equal(src, dst, equal_house));
    }
    {
        LeopardHouse src[]{make_leopard_house(1), make_leopard_house(7), make_leopard_house(2), make_leopard_house(9)};
        LeopardHouse dst[4]{};
        ranges::move_backward(src, ranges::end(dst));
        assert(ranges::equal(src, dst, equal_house));
    }
#endif // _HAS_CXX20
}

CONSTEXPR20 bool test() {
    test_reverse_copy();
    test_copy_move_leopards();
    test_llvm_37038();

    return true;
}

#if _HAS_CXX20
static_assert(test());
#endif // _HAS_CXX20

int main() {
    test();
}
