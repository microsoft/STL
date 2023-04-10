// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <iterator>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <class Rng>
concept CanViewAsRvalue = requires(Rng&& r) { views::as_rvalue(forward<Rng>(r)); };

template <ranges::input_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::as_rvalue_view, ranges::begin, ranges::end, ranges::iterator_t, ranges::sentinel_t, ranges::prev,
        ranges::input_range, ranges::forward_range, ranges::bidirectional_range, ranges::random_access_range,
        ranges::common_range, ranges::sized_range;
    using V = views::all_t<Rng>;
    using R = as_rvalue_view<V>;

    constexpr bool is_view           = ranges::view<remove_cvref_t<Rng>>;
    constexpr bool is_already_rvalue = same_as<ranges::range_rvalue_reference_t<V>, ranges::range_reference_t<V>>;

    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(input_range<R> == input_range<V>);
    STATIC_ASSERT(forward_range<R> == forward_range<V>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<V>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<V>);
    STATIC_ASSERT(!ranges::contiguous_range<R>);

    // Validate default-initializability
    STATIC_ASSERT(default_initializable<R> == default_initializable<V>);

    // Validate borrowed_range
    STATIC_ASSERT(ranges::borrowed_range<R> == ranges::borrowed_range<V>);

    // Validate range adaptor object
    if constexpr (!is_already_rvalue) { // range adaptor results in as_rvalue_view
        // ... with lvalue argument
        STATIC_ASSERT(CanViewAsRvalue<Rng&> == (!is_view || copy_constructible<V>) );
        if constexpr (CanViewAsRvalue<Rng&>) {
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_rvalue(std::forward<Rng>(rng))), R>);
            STATIC_ASSERT(noexcept(views::as_rvalue(std::forward<Rng>(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(std::forward<Rng>(rng) | views::as_rvalue), R>);
            STATIC_ASSERT(noexcept(std::forward<Rng>(rng) | views::as_rvalue) == is_noexcept);
        }

        // ... with const lvalue argument
        STATIC_ASSERT(CanViewAsRvalue<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
        if constexpr (CanViewAsRvalue<const remove_reference_t<Rng>&>) {
            using RC                   = as_rvalue_view<views::all_t<const remove_reference_t<Rng>&>>;
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_rvalue(as_const(rng))), RC>);
            STATIC_ASSERT(noexcept(views::as_rvalue(as_const(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(as_const(rng) | views::as_rvalue), RC>);
            STATIC_ASSERT(noexcept(as_const(rng) | views::as_rvalue) == is_noexcept);
        }

        // ... with rvalue argument
        STATIC_ASSERT(CanViewAsRvalue<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
        if constexpr (CanViewAsRvalue<remove_reference_t<Rng>>) {
            using RS                   = as_rvalue_view<views::all_t<remove_reference_t<Rng>>>;
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_rvalue(std::move(rng))), RS>);
            STATIC_ASSERT(noexcept(views::as_rvalue(std::move(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(std::move(rng) | views::as_rvalue), RS>);
            STATIC_ASSERT(noexcept(std::move(rng) | views::as_rvalue) == is_noexcept);
        }

        // ... with const rvalue argument
        STATIC_ASSERT(CanViewAsRvalue<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
        if constexpr (CanViewAsRvalue<const remove_reference_t<Rng>>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_rvalue(std::move(as_const(rng)))), R>);
            STATIC_ASSERT(noexcept(views::as_rvalue(std::move(as_const(rng)))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(std::move(as_const(rng)) | views::as_rvalue), R>);
            STATIC_ASSERT(noexcept(std::move(as_const(rng)) | views::as_rvalue) == is_noexcept);
        }
    } else { // range adaptor results in views::all_t
        // ... with lvalue argument
        STATIC_ASSERT(CanViewAsRvalue<Rng&> == (!is_view || copy_constructible<V>) );
        if constexpr (CanViewAsRvalue<Rng&>) {
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_rvalue(std::forward<Rng>(rng))), V>);
            STATIC_ASSERT(noexcept(views::as_rvalue(std::forward<Rng>(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(std::forward<Rng>(rng) | views::as_rvalue), V>);
            STATIC_ASSERT(noexcept(std::forward<Rng>(rng) | views::as_rvalue) == is_noexcept);
        }

        // ... with const lvalue argument
        STATIC_ASSERT(CanViewAsRvalue<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
        if constexpr (CanViewAsRvalue<const remove_reference_t<Rng>&>) {
            using VC                   = views::all_t<const remove_reference_t<Rng>&>;
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_rvalue(as_const(rng))), VC>);
            STATIC_ASSERT(noexcept(views::as_rvalue(as_const(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(as_const(rng) | views::as_rvalue), VC>);
            STATIC_ASSERT(noexcept(as_const(rng) | views::as_rvalue) == is_noexcept);
        }

        // ... with rvalue argument
        STATIC_ASSERT(CanViewAsRvalue<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
        if constexpr (CanViewAsRvalue<remove_reference_t<Rng>>) {
            using VS                   = views::all_t<remove_reference_t<Rng>>;
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_rvalue(std::move(rng))), VS>);
            STATIC_ASSERT(noexcept(views::as_rvalue(std::move(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(std::move(rng) | views::as_rvalue), VS>);
            STATIC_ASSERT(noexcept(std::move(rng) | views::as_rvalue) == is_noexcept);
        }

        // ... with const rvalue argument
        STATIC_ASSERT(CanViewAsRvalue<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
        if constexpr (CanViewAsRvalue<const remove_reference_t<Rng>>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_rvalue(std::move(as_const(rng)))), V>);
            STATIC_ASSERT(noexcept(views::as_rvalue(std::move(as_const(rng)))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(std::move(as_const(rng)) | views::as_rvalue), V>);
            STATIC_ASSERT(noexcept(std::move(as_const(rng)) | views::as_rvalue) == is_noexcept);
        }
    }

    // Validate deduction guide
    same_as<R> auto r = as_rvalue_view{std::forward<Rng>(rng)};

    // Validate as_rvalue_view::size
    STATIC_ASSERT(CanMemberSize<R> == sized_range<V>);
    if constexpr (CanMemberSize<R>) {
        same_as<ranges::range_size_t<V>> auto s = r.size();
        assert(_To_unsigned_like(s) == ranges::size(expected));
        STATIC_ASSERT(noexcept(r.size()) == noexcept(ranges::size(as_const(rng))));
    }

    // Validate as_rvalue_view::size (const)
    STATIC_ASSERT(CanMemberSize<const R> == sized_range<const V>);
    if constexpr (CanMemberSize<const R>) {
        same_as<ranges::range_size_t<const V>> auto s = as_const(r).size();
        assert(_To_unsigned_like(s) == ranges::size(expected));
        STATIC_ASSERT(noexcept(as_const(r).size()) == noexcept(ranges::size(rng)));
    }

    const bool is_empty = ranges::empty(expected);

    // Validate view_interface::empty and operator bool
    STATIC_ASSERT(CanMemberEmpty<R> == (forward_range<V> || sized_range<V>) );
    STATIC_ASSERT(CanBool<R> == CanEmpty<R>);
    if constexpr (CanMemberEmpty<R>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    }

    // Validate view_interface::empty and operator bool (const)
    STATIC_ASSERT(CanMemberEmpty<const R> == (forward_range<const Rng> || sized_range<const V>) );
    STATIC_ASSERT(CanBool<const R> == CanEmpty<const R>);
    if constexpr (CanMemberEmpty<const R>) {
        assert(as_const(r).empty() == is_empty);
        assert(static_cast<bool>(as_const(r)) == !is_empty);
    }

    assert(ranges::equal(r, expected));
    if (!forward_range<V>) { // intentionally not if constexpr
        return true;
    }

    // Validate as_rvalue_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    STATIC_ASSERT(same_as<iterator_t<R>, move_iterator<iterator_t<V>>>);
    {
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected));
        }

        if constexpr (copy_constructible<V>) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }
    }

    // Validate as_rvalue_view::begin (const)
    STATIC_ASSERT(CanMemberBegin<const R> == ranges::range<const V>);
    if constexpr (CanMemberBegin<const R>) {
        const same_as<iterator_t<const R>> auto ci = as_const(r).begin();
        if (!is_empty) {
            assert(*ci == *begin(expected));
        }

        if constexpr (copy_constructible<V>) {
            const auto cr2                              = r;
            const same_as<iterator_t<const R>> auto ci2 = cr2.begin();
            if (!is_empty) {
                assert(*ci2 == *ci);
            }
        }
    }

    // Validate as_rvalue_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    if constexpr (common_range<V>) {
        STATIC_ASSERT(same_as<sentinel_t<R>, move_iterator<iterator_t<V>>>);
    } else {
        STATIC_ASSERT(same_as<sentinel_t<R>, move_sentinel<sentinel_t<V>>>);
    }

    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);
        STATIC_ASSERT(common_range<R> == common_range<V>);
        if constexpr (common_range<R> && bidirectional_range<V>) {
            if (!is_empty) {
                assert(*prev(s) == *prev(end(expected)));
            }

            if constexpr (copy_constructible<V>) {
                auto r2 = r;
                if (!is_empty) {
                    assert(*prev(r2.end()) == *prev(end(expected)));
                }
            }
        }
    }

    // Validate as_rvalue_view::end (const)
    STATIC_ASSERT(CanMemberEnd<const R> == ranges::range<const V>);
    if constexpr (CanMemberEnd<const R>) {
        const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
        assert((as_const(r).begin() == cs) == is_empty);
        STATIC_ASSERT(common_range<const R> == common_range<const V>);
        if constexpr (common_range<const R> && bidirectional_range<const V>) {
            if (!is_empty) {
                assert(*prev(cs) == *prev(end(expected)));
            }

            if constexpr (copy_constructible<V>) {
                const auto r2 = r;
                if (!is_empty) {
                    assert(*prev(r2.end()) == *prev(end(expected)));
                }
            }
        }
    }

    // Validate view_interface::data
    STATIC_ASSERT(!CanData<R>);
    STATIC_ASSERT(!CanData<const R>);

    if (!is_empty) {
        // Validate view_interface::operator[]
        STATIC_ASSERT(CanIndex<R> == random_access_range<V>);
        if constexpr (CanIndex<R>) {
            assert(r[0] == expected[0]);
        }

        // Validate view_interface::operator[] (const)
        STATIC_ASSERT(CanIndex<const R> == random_access_range<const V>);
        if constexpr (CanIndex<const R>) {
            assert(as_const(r)[0] == expected[0]);
        }

        // Validate view_interface::front
        STATIC_ASSERT(CanMemberFront<R> == forward_range<V>);
        if constexpr (CanMemberFront<R>) {
            assert(r.front() == *begin(expected));
        }

        // Validate view_interface::front (const)
        STATIC_ASSERT(CanMemberFront<const R> == forward_range<const V>);
        if constexpr (CanMemberFront<const R>) {
            assert(as_const(r).front() == *begin(expected));
        }

        // Validate view_interface::back
        STATIC_ASSERT(CanMemberBack<R> == (bidirectional_range<V> && common_range<V>) );
        if constexpr (CanMemberBack<R>) {
            assert(r.back() == *prev(end(expected)));
        }

        // Validate view_interface::back (const)
        STATIC_ASSERT(CanMemberBack<const R> == (bidirectional_range<const V> && common_range<const V>) );
        if constexpr (CanMemberBack<const R>) {
            assert(as_const(r).back() == *prev(end(expected)));
        }
    }

    // Validate as_rvalue_view::base() const&
    STATIC_ASSERT(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V>) {
        same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert(*b1.begin() == *begin(expected));
        }
    }

    // Validate as_rvalue::base() &&
    same_as<V> auto b2 = std::move(r).base();
    STATIC_ASSERT(noexcept(std::move(r).base()) == is_nothrow_move_constructible_v<V>);
    if (!is_empty) {
        assert(*b2.begin() == *begin(expected));
    }

    return true;
}

constexpr int some_ints[] = {0, 3, 6, 9, 12, 15};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, span{some_ints});
    }
};

template <class Category, test::Common IsCommon, test::Sized IsSized>
using test_range =
    test::range<Category, const int, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr bool instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<instantiator, const int>();
#else // ^^^ test all input permutations / test only "interesting" permutations vvv
    using test::Common, test::Sized;

    // The view is sensitive to category, commonality, and size, but oblivious to differencing and proxyness
    instantiator::call<test_range<input_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<input_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<input_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<input_iterator_tag, Common::yes, Sized::no>>();

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

    return true;
}

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare::yes, test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>},
    test::CanView::yes, test::Copyability::move_only>;

void test_example_from_p2446r2() {
    const vector<string> pattern = {"the", "quick", "brown", "fox", "ate", "a", "pterodactyl"};

    vector<string> words = pattern;
    vector<string> new_words;
    ranges::copy(words | views::as_rvalue, back_inserter(new_words)); // moves each string from words into new_words

    assert(ranges::equal(new_words, pattern));
    assert(words.size() == pattern.size()); // size of words is preserved
    assert(ranges::all_of(words, ranges::empty)); // all strings from words are empty (implementation assumption)
}

int main() {
    { // Validate views
        // ... copyable
        constexpr span<const int> s{some_ints};
        STATIC_ASSERT(test_one(s, some_ints));
        test_one(s, some_ints);
    }

    { // ... move-only
        test_one(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_one(move_only_view<input_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, some_ints);
    }

    { // Validate non-views
        STATIC_ASSERT(test_one(some_ints, some_ints));
        test_one(some_ints, some_ints);
        test_one(some_ints | ranges::to<vector>(), some_ints);
        test_one(some_ints | ranges::to<forward_list>(), some_ints);
    }

    { // Validate some views
        STATIC_ASSERT(test_one(views::iota(0, 10), views::iota(0, 10)));
        test_one(views::iota(0, 10), views::iota(0, 10));

        const string some_strings[] = {"0", "3", "6", "9", "12", "15"};
        auto transformed            = some_ints | views::transform([](int x) { return to_string(x); });
        test_one(transformed, some_strings);
    }

    { // empty range
        using Span = span<string_view>;
        STATIC_ASSERT(test_one(Span{}, Span{}));
        test_one(Span{}, Span{});
    }

    STATIC_ASSERT(instantiation_test());
    instantiation_test();

    test_example_from_p2446r2();
}
