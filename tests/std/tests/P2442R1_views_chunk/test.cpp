// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>
using namespace std;

template <class Rng>
concept CanViewChunk = requires(Rng&& r) { views::chunk(forward<Rng>(r), 2); };

constexpr auto equal_ranges = [](auto&& left, auto&& right) { return ranges::equal(left, right); };

template <ranges::input_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::bidirectional_range, ranges::common_range, ranges::forward_range, ranges::input_range,
        ranges::random_access_range, ranges::sized_range;
    using ranges::chunk_view, ranges::begin, ranges::end, ranges::equal, ranges::iterator_t, ranges::sentinel_t,
        ranges::prev;
    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using R = chunk_view<V>;

    static_assert(ranges::view<R>);
    static_assert(forward_range<R> == forward_range<Rng>);
    static_assert(bidirectional_range<R> == bidirectional_range<Rng>);
    static_assert(random_access_range<R> == random_access_range<Rng>);

    // Validate non-default-initializability
    static_assert(!is_default_constructible_v<R>);

    // Validate borrowed_range
    static_assert(ranges::borrowed_range<R> == (ranges::borrowed_range<V> && forward_range<V>) );

    // Validate range adaptor object and range adaptor closure
    constexpr auto closure = views::chunk(2);

    // ... with lvalue argument
    static_assert(CanViewChunk<Rng&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewChunk<Rng&>) {
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::chunk(rng, 2)), R>);
        static_assert(noexcept(views::chunk(rng, 2)) == is_noexcept);

        static_assert(same_as<decltype(rng | closure), R>);
        static_assert(noexcept(rng | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    static_assert(CanViewChunk<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewChunk<const remove_reference_t<Rng>&>) {
        using RC                   = chunk_view<views::all_t<const remove_reference_t<Rng>&>>;
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(!is_default_constructible_v<RC>);

        static_assert(same_as<decltype(views::chunk(as_const(rng), 2)), RC>);
        static_assert(noexcept(views::chunk(as_const(rng), 2)) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | closure), RC>);
        static_assert(noexcept(as_const(rng) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    static_assert(CanViewChunk<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewChunk<remove_reference_t<Rng>>) {
        using RS                   = chunk_view<views::all_t<remove_reference_t<Rng>>>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

        static_assert(!is_default_constructible_v<RS>);

        static_assert(same_as<decltype(views::chunk(move(rng), 2)), RS>);
        static_assert(noexcept(views::chunk(move(rng), 2)) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | closure), RS>);
        static_assert(noexcept(move(rng) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    static_assert(CanViewChunk<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewChunk<const remove_reference_t<Rng>>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::chunk(move(as_const(rng)), 2)), R>);
        static_assert(noexcept(views::chunk(move(as_const(rng)), 2)) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | closure), R>);
        static_assert(noexcept(move(as_const(rng)) | closure) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r   = chunk_view{forward<Rng>(rng), 2};
    const bool is_empty = ranges::empty(expected);

    // Validate chunk_view::size
    static_assert(CanMemberSize<R> == sized_range<V>);
    if constexpr (CanMemberSize<R>) {
        same_as<_Make_unsigned_like_t<ranges::range_difference_t<V>>> auto s = r.size();
        assert(s == ranges::size(expected));
        static_assert(noexcept(r.size()));
    }

    static_assert(CanMemberSize<const R> == sized_range<const V>);
    if constexpr (CanMemberSize<const R>) {
        same_as<_Make_unsigned_like_t<ranges::range_difference_t<const V>>> auto s = as_const(r).size();
        assert(s == ranges::size(expected));
        static_assert(noexcept(as_const(r).size()));
    }

    // Validate view_interface::empty and operator bool
    static_assert(CanMemberEmpty<R> == (sized_range<V> || forward_range<V>) );
    static_assert(CanBool<R> == CanEmpty<R>);
    if constexpr (CanMemberEmpty<R>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    } else {
        static_assert(CanEmpty<R> == CanSize<R>);
        if constexpr (CanEmpty<R>) {
            assert(ranges::empty(r) == is_empty);
            assert(static_cast<bool>(r) == !is_empty);
        }
    }

    static_assert(
        CanMemberEmpty<const R> == ((forward_range<Rng> && sized_range<const V>) || forward_range<const Rng>) );
    static_assert(CanBool<const R> == CanEmpty<const R>);
    if constexpr (CanMemberEmpty<const R>) {
        assert(as_const(r).empty() == is_empty);
        assert(static_cast<bool>(as_const(r)) == !is_empty);
    } else {
        static_assert(CanEmpty<const R> == CanSize<const R>);
        if constexpr (CanEmpty<const R>) {
            assert(ranges::empty(as_const(r)) == is_empty);
            assert(static_cast<bool>(as_const(r)) == !is_empty);
        }
    }

    // validate content
    assert(equal(r, expected, equal_ranges));
    if (!forward_range<V>) { // intentionally not if constexpr
        return true;
    }

    // Validate chunk_view::begin
    static_assert(CanMemberBegin<R>);
    {
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(equal(*i, *begin(expected)));
        }

        if constexpr (copy_constructible<V>) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
                assert(equal(*i2, *i));
            }
        }
    }

    static_assert(CanMemberBegin<const R> == forward_range<const V>);
    if constexpr (CanMemberBegin<const R>) {
        const same_as<iterator_t<const R>> auto ci = as_const(r).begin();
        if (!is_empty) {
            assert(equal(*ci, *begin(expected)));
        }

        if constexpr (copy_constructible<V>) {
            const auto r2                               = r;
            const same_as<iterator_t<const R>> auto ci2 = r2.begin();
            if (!is_empty) {
                assert(equal(*ci2, *ci));
            }
        }
    }

    // Validate chunk_view::end
    static_assert(CanMemberEnd<R>);
    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);
        static_assert(
            common_range<R> == (forward_range<V> && common_range<V> && (sized_range<V> || !bidirectional_range<V>) ));
        if constexpr (common_range<R> && bidirectional_range<V>) {
            if (!is_empty) {
                assert(equal(*prev(s), *prev(end(expected))));
            }

            if constexpr (copy_constructible<V>) {
                auto r2 = r;
                if (!is_empty) {
                    assert(equal(*prev(r2.end()), *prev(end(expected))));
                }
            }
        }
    }

    static_assert(CanMemberEnd<const R> == input_range<const V>);
    if constexpr (CanMemberEnd<const R>) {
        if constexpr (CanMemberBegin<const R>) {
            const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
            assert((r.begin() == cs) == is_empty);
            static_assert(common_range<const R>
                          == (forward_range<const V> && common_range<const V>
                              && (sized_range<const V> || !bidirectional_range<const V>) ));
            if constexpr (common_range<const R> && bidirectional_range<V>) {
                if (!is_empty) {
                    assert(equal(*prev(cs), *prev(end(expected))));
                }

                if constexpr (copy_constructible<V>) {
                    const auto r2 = r;
                    if (!is_empty) {
                        assert(equal(*prev(r2.end()), *prev(end(expected))));
                    }
                }
            }
        } else {
            static_assert(same_as<decltype(as_const(r).end()), default_sentinel_t>);
        }
    }

    if (is_empty) {
        return true;
    }

    // Validate view_interface::data
    static_assert(!CanData<R>);
    static_assert(!CanData<const R>);

    // Validate view_interface::operator[]
    static_assert(CanIndex<R> == random_access_range<V>);
    if constexpr (CanIndex<R>) {
        assert(equal(r[0], expected[0]));
    }

    static_assert(CanIndex<const R> == random_access_range<const V>);
    if constexpr (CanIndex<const R>) {
        assert(equal(as_const(r)[0], expected[0]));
    }

    // Validate view_interface::front
    static_assert(CanMemberFront<R> == forward_range<V>);
    if constexpr (CanMemberFront<R>) {
        assert(equal(r.front(), *begin(expected)));
    }

    static_assert(CanMemberFront<const R> == forward_range<const V>);
    if constexpr (CanMemberFront<const R>) {
        assert(equal(as_const(r).front(), *begin(expected)));
    }

    // Validate view_interface::back
    static_assert(CanMemberBack<R> == (bidirectional_range<V> && common_range<V> && sized_range<V>) );
    if constexpr (CanMemberBack<R>) {
        assert(equal(r.back(), *prev(end(expected))));
    }

    static_assert(
        CanMemberBack<const R> == (bidirectional_range<const V> && common_range<const V> && sized_range<const V>) );
    if constexpr (CanMemberBack<const R>) {
        assert(equal(as_const(r).back(), *prev(end(expected))));
    }

    // Validate chunk_view::iterator<NotConst>
    {
        if constexpr (forward_range<R>) {
            [[maybe_unused]] const iterator_t<R> defaulted;
        }
        same_as<iterator_t<R>> auto i = r.begin();

        if constexpr (forward_range<R>) {
            assert(equal(*i++, expected[0]));
        } else {
            i++;
        }
        assert(equal(*++i, expected[2]));

        if constexpr (bidirectional_range<R>) {
            assert(equal(*i--, expected[2]));
            assert(equal(*--i, expected[0]));
        }

        if constexpr (random_access_range<R>) {
            i += 2;
            assert(equal(*i, expected[2]));

            i -= 2;
            assert(equal(*i, expected[0]));

            assert(equal(i[2], expected[2]));

            const same_as<iterator_t<R>> auto i2 = i + 2;
            assert(equal(*i2, expected[2]));

            const same_as<iterator_t<R>> auto i3 = 2 + i;
            assert(equal(*i3, expected[2]));

            const same_as<iterator_t<R>> auto i4 = i3 - 2;
            assert(equal(*i4, expected[0]));

            const same_as<ranges::range_difference_t<V>> auto diff1 = i2 - i;
            assert(diff1 == 2);

            const same_as<ranges::range_difference_t<V>> auto diff2 = i - i2;
            assert(diff2 == -2);

            // comparisons
            assert(i == i4);
            assert(i != i2);

            assert(i < i2);
            assert(i <= i2);
            assert(i2 > i);
            assert(i2 >= i);

            if constexpr (three_way_comparable<iterator_t<V>>) {
                assert(i <=> i4 == strong_ordering::equal);
                assert(i <=> i2 == strong_ordering::less);
                assert(i2 <=> i == strong_ordering::greater);
            }
        }

        if constexpr (!common_range<R>) {
            if constexpr (forward_range<R>) {
                [[maybe_unused]] const sentinel_t<R> sentinel_defaulted;
            }

            const auto i2   = r.begin();
            const auto sen  = r.end();
            const auto size = ranges::ssize(expected);

            if constexpr (forward_range<R>) {
                assert(next(r.begin(), size) == sen);
            }
            assert(i2 != sen);

            if constexpr (sized_sentinel_for<sentinel_t<R>, iterator_t<R>>) {
                const same_as<ranges::range_difference_t<V>> auto diff3 = i2 - sen;
                assert(diff3 == -size);

                const same_as<ranges::range_difference_t<V>> auto diff4 = sen - i2;
                assert(diff4 == size);
            }
        }
    }

    // Validate chunk_view::iterator<Const>
    if constexpr (CanMemberBegin<const R>) {
        [[maybe_unused]] const iterator_t<const R> const_defaulted;
        auto i{r.begin()};
        auto ci{as_const(r).begin()};

        assert(equal(*ci++, expected[0]));
        assert(equal(*++ci, expected[2]));

        if constexpr (bidirectional_range<const R>) {
            assert(equal(*ci--, expected[2]));
            assert(equal(*--ci, expected[0]));
        }

        if constexpr (random_access_range<const R>) {
            ci += 2;
            assert(equal(*ci, expected[2]));

            ci -= 2;
            assert(equal(*ci, expected[0]));

            assert(equal(ci[2], expected[2]));

            const same_as<iterator_t<const R>> auto ci2 = ci + 2;
            assert(equal(*ci2, expected[2]));

            const same_as<iterator_t<const R>> auto ci3 = 2 + ci;
            assert(equal(*ci3, expected[2]));

            const same_as<iterator_t<const R>> auto ci4 = ci3 - 2;
            assert(equal(*ci4, expected[0]));

            const same_as<ranges::range_difference_t<V>> auto diff1 = ci2 - ci;
            assert(diff1 == 2);

            const same_as<ranges::range_difference_t<V>> auto diff2 = ci - ci2;
            assert(diff2 == -2);

            // comparisons
            assert(ci == ci4);
            assert(ci != ci2);

            assert(ci < ci2);
            assert(ci <= ci2);
            assert(ci2 > ci);
            assert(ci2 >= ci);

            // cross comparisons
            assert(ci == i);
            assert(ci2 != i);

            assert(i < ci2);
            assert(i <= ci2);
            assert(ci2 > i);
            assert(ci2 >= i);

            if constexpr (three_way_comparable<iterator_t<const V>>) {
                assert(ci <=> ci4 == strong_ordering::equal);
                assert(ci <=> ci2 == strong_ordering::less);
                assert(ci2 <=> ci == strong_ordering::greater);
            }
        }

        if constexpr (!common_range<const R>) {
            [[maybe_unused]] const sentinel_t<const R> sentinel_defaulted;

            const auto i2   = as_const(r).begin();
            const auto sen  = r.end();
            const auto size = ranges::ssize(expected);

            assert(next(i2, size) == sen);
            assert(i2 != sen);

            if constexpr (sized_sentinel_for<sentinel_t<const R>, iterator_t<const R>>) {
                const same_as<ranges::range_difference_t<const V>> auto diff3 = i2 - sen;
                assert(diff3 == -size);

                const same_as<ranges::range_difference_t<const V>> auto diff4 = sen - i2;
                assert(diff4 == size);
            }
        }
    }

    // Validate chunk_view::base() const&
    static_assert(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V>) {
        same_as<V> auto b1 = as_const(r).base();
        static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        assert(*b1.begin() == *begin(*begin(expected)));
    }

    // Validate chunk_view::base() && (NB: do this last since it leaves r moved-from)
    same_as<V> auto b2 = move(r).base();
    static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
    if (!is_empty) {
        assert(*b2.begin() == *begin(*begin(expected)));
    }

    return true;
}

static constexpr int some_ints[] = {0, 1, 2, 3, 4, 5, 6};
static constexpr span<const int> wrapped{some_ints};
static constexpr span<const int> chunks_of_two[] = {
    wrapped.subspan(0, 2), wrapped.subspan(2, 2), wrapped.subspan(4, 2), wrapped.subspan(6, 1)};

template <ranges::input_range Rng, class Expected>
constexpr bool test_input(Rng&& rng, Expected&& expected) {
    using ranges::chunk_view, ranges::equal, ranges::iterator_t, ranges::sentinel_t;

    using V  = views::all_t<Rng>;
    using BI = iterator_t<V>;
    using R  = chunk_view<V>;

    same_as<R> auto r = chunk_view{forward<Rng>(rng), 2};
    auto outer_iter   = r.begin();

    auto val_ty = *outer_iter;
    if constexpr (sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>) {
        const same_as<_Make_unsigned_like_t<ranges::range_difference_t<V>>> auto s = val_ty.size(); // test LWG-3707
        assert(s == 2);
    }

    auto inner_iter                            = val_ty.begin();
    same_as<default_sentinel_t> auto inner_sen = val_ty.end();

    { // Check iter_move (other tests are defined in 'test_lwg3851' function)
        same_as<ranges::range_rvalue_reference_t<Rng>> decltype(auto) rval = iter_move(as_const(inner_iter));
        assert(rval == expected[0][0]);
        static_assert(noexcept(iter_move(inner_iter)) == noexcept(ranges::iter_move(declval<const BI&>())));
    }

    if constexpr (indirectly_swappable<BI>) { // Check iter_swap (other tests are defined in 'test_lwg3851' function)
        static_assert(is_void_v<decltype(iter_swap(as_const(inner_iter), as_const(inner_iter)))>);
        static_assert(noexcept(iter_swap(inner_iter, inner_iter))
                      == noexcept(ranges::iter_swap(declval<const BI&>(), declval<const BI&>())));
    }

    assert(inner_iter != inner_sen);
    if constexpr (sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>) {
        assert(inner_sen - inner_iter == 2);
        assert(inner_iter - inner_sen == -2);
    }

    ++inner_iter;
    if constexpr (sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>) {
        assert(inner_sen - inner_iter == 1);
        assert(inner_iter - inner_sen == -1);
    }

    inner_iter++;
    if constexpr (sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>) {
        assert(inner_sen - inner_iter == 0);
        assert(inner_iter - inner_sen == 0);
    }
    assert(inner_iter == inner_sen);

    outer_iter++;
    assert(equal(*outer_iter, expected[1]));
    ++outer_iter;
    assert(equal(*outer_iter, expected[2]));
    ++outer_iter;

    same_as<default_sentinel_t> auto outer_sen = r.end();
    assert(outer_sen != outer_iter);

    if constexpr (sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>) {
        assert(outer_sen - outer_iter == 1);
        assert(outer_iter - outer_sen == -1);
    }

    ++outer_iter;
    assert(outer_sen == outer_iter);

    return true;
}

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, chunks_of_two);

        if constexpr (!ranges::forward_range<R>) {
            R r2{some_ints};
            test_input(r2, chunks_of_two);
        }
    }
};

template <test::CanDifference Difference>
using test_input_range = test::range<input_iterator_tag, const int, test::Sized::yes, Difference, test::Common::yes,
    test::CanCompare::yes, test::ProxyRef::no>;

template <class Category, test::Common IsCommon, test::Sized IsSized>
using test_range =
    test::range<Category, const int, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<instantiator, const int>();
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    using test::CanDifference, test::Common, test::Sized;

    // When the base range is an input range,
    // the view is sensitive to differencing.
    instantiator::call<test_input_range<CanDifference::yes>>();
    instantiator::call<test_input_range<CanDifference::no>>();

    // When the base range is a forward range,
    // the view is sensitive to category, commonality, and size, but oblivious to differencing and proxyness.
    instantiator::call<test_range<input_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<input_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<forward_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes, Sized::no>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::yes, Sized::no>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::yes, Sized::no>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::yes, Sized::no>>();
#endif // TEST_EVERYTHING
}

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

// Check LWG-3851: 'chunk_view::inner-iterator missing custom iter_move and iter_swap'
void test_lwg3851() {
    { // Check 'iter_move'
        istringstream ints{"0 1 2 3 4"};
        auto v = views::istream<int>(ints) | views::chunk(2);
        auto o = v.begin();
        auto c = *o;
        auto i = c.begin();

        same_as<int&&> decltype(auto) rval = iter_move(i);
        assert(rval == 0);
    }

    { // Check 'iter_swap'
        istringstream ints1{"0 1 2 3 4"};
        auto v1 = views::istream<int>(ints1) | views::chunk(2);
        auto o1 = v1.begin();
        auto c1 = *o1;
        auto i1 = c1.begin();

        istringstream ints2{"5 6 7 8 9"};
        auto v2 = views::istream<int>(ints2) | views::chunk(2);
        auto o2 = v2.begin();
        auto c2 = *o2;
        auto i2 = c2.begin();

        iter_swap(as_const(i1), as_const(i2));
        assert(*i1 == 5);
        assert(*i2 == 0);
    }
}

int main() {
    { // Validate views
        // ... copyable
        constexpr span<const int> s{some_ints};
        static_assert(test_one(s, chunks_of_two));
        test_one(s, chunks_of_two);
    }

    { // ... move-only
        test_one(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, chunks_of_two);
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, chunks_of_two);
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, chunks_of_two);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, chunks_of_two);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, chunks_of_two);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, chunks_of_two);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, chunks_of_two);
    }

    { // Validate non-views
        static_assert(test_one(some_ints, chunks_of_two));
        test_one(some_ints, chunks_of_two);
    }
    {
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(vec, chunks_of_two);
    }
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(lst, chunks_of_two);
    }

    { // empty range
        static_assert(test_one(span<const int, 0>{}, span<span<const int>>{}));
        test_one(span<const int, 0>{}, span<span<const int>>{});
    }

    static_assert((instantiation_test(), true));
    instantiation_test();

    test_lwg3851();
}
