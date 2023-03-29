// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers ranges::min, ranges::max, ranges::minmax, ranges::clamp, ranges::min_element, ranges::max_element, and
// ranges::minmax_element

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <functional>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

#define ASSERT(...) assert((__VA_ARGS__))

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::min_element(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::min_element(borrowed<true>{})), int*>);

STATIC_ASSERT(same_as<decltype(ranges::max_element(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::max_element(borrowed<true>{})), int*>);

STATIC_ASSERT(
    same_as<decltype(ranges::minmax_element(borrowed<false>{})), ranges::minmax_element_result<ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::minmax_element(borrowed<true>{})), ranges::minmax_element_result<int*>>);

// Validate that minmax_result and minmax_element_result alias min_max_result
STATIC_ASSERT(same_as<ranges::minmax_result<int>, ranges::min_max_result<int>>);
STATIC_ASSERT(same_as<ranges::minmax_element_result<int>, ranges::min_max_result<int>>);

using P = pair<int, int>;

struct mm_element_empty {
    template <ranges::forward_range Fwd>
    static constexpr void call() {
        // Validate empty ranges
        const Fwd range{span<const P, 0>{}};

        ASSERT(ranges::min_element(range, ranges::less{}, get_first) == ranges::end(range));
        ASSERT(ranges::min_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first)
               == ranges::end(range));

        ASSERT(ranges::max_element(range, ranges::less{}, get_first) == ranges::end(range));
        ASSERT(ranges::max_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first)
               == ranges::end(range));

        {
            auto result = ranges::minmax_element(range, ranges::less{}, get_first);
            STATIC_ASSERT(same_as<decltype(result), ranges::minmax_element_result<ranges::iterator_t<Fwd>>>);
            ASSERT(result.min == ranges::end(range));
            ASSERT(result.max == ranges::end(range));
        }
        {
            auto result = ranges::minmax_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first);
            STATIC_ASSERT(same_as<decltype(result), ranges::minmax_element_result<ranges::iterator_t<Fwd>>>);
            ASSERT(result.min == ranges::end(range));
            ASSERT(result.max == ranges::end(range));
        }
    }
};

static constexpr array<P, 8> pairs = {P{5, 0}, P{5, 1}, P{5, 2}, P{5, 3}, P{5, 4}, P{5, 5}, P{5, 6}, P{5, 7}};

struct mm_element {
    template <ranges::forward_range Fwd>
    static constexpr void call() {
        constexpr auto N = pairs.size();
        auto elements    = pairs;
        const Fwd range{elements};

        ASSERT(*ranges::min_element(range, ranges::less{}, get_first) == P{5, 0});
        ASSERT(*ranges::min_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first) == P{5, 0});

        ASSERT(*ranges::max_element(range, ranges::less{}, get_first) == P{5, 0});
        ASSERT(*ranges::max_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first) == P{5, 0});

        {
            auto result = ranges::minmax_element(range, ranges::less{}, get_first);
            ASSERT(*result.min == P{5, 0});
            ASSERT(*result.max == P{5, 7});
        }
        {
            auto result = ranges::minmax_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first);
            ASSERT(*result.min == P{5, 0});
            ASSERT(*result.max == P{5, 7});
        }

        for (size_t i = 0; i < N; ++i) {
            elements[i] = P{0, 42};

            ASSERT(*ranges::min_element(range, ranges::less{}, get_first) == P{0, 42});
            ASSERT(
                *ranges::min_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first) == P{0, 42});

            ASSERT(*ranges::max_element(range, ranges::less{}, get_first) == (i == 0 ? P{5, 1} : P{5, 0}));
            ASSERT(*ranges::max_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first)
                   == (i == 0 ? P{5, 1} : P{5, 0}));

            {
                const auto result = ranges::minmax_element(range, ranges::less{}, get_first);
                ASSERT(*result.min == P{0, 42});
                ASSERT(*result.max == (i == 7 ? P{5, 6} : P{5, 7}));
            }
            {
                const auto result =
                    ranges::minmax_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first);
                ASSERT(*result.min == P{0, 42});
                ASSERT(*result.max == (i == 7 ? P{5, 6} : P{5, 7}));
            }

            for (size_t j = i + 1; j < N; ++j) {
                elements[j] = P{0, 13};

                ASSERT(*ranges::min_element(range, ranges::less{}, get_first) == P{0, 42});
                ASSERT(*ranges::min_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first)
                       == P{0, 42});

                ASSERT(*ranges::max_element(range, ranges::less{}, get_first)
                       == (i == 0 ? (j == 1 ? P{5, 2} : P{5, 1}) : P{5, 0}));
                ASSERT(*ranges::max_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first)
                       == (i == 0 ? (j == 1 ? P{5, 2} : P{5, 1}) : P{5, 0}));

                {
                    const auto result = ranges::minmax_element(range, ranges::less{}, get_first);
                    ASSERT(*result.min == P{0, 42});
                    ASSERT(*result.max == (j == 7 ? (i == 6 ? P{5, 5} : P{5, 6}) : P{5, 7}));
                }
                {
                    const auto result =
                        ranges::minmax_element(ranges::begin(range), ranges::end(range), ranges::less{}, get_first);
                    ASSERT(*result.min == P{0, 42});
                    ASSERT(*result.max == (j == 7 ? (i == 6 ? P{5, 5} : P{5, 6}) : P{5, 7}));
                }

                elements[j] = P{5, static_cast<int>(j)};
            }

            elements[i] = P{5, static_cast<int>(i)};
        }
    }
};

struct mm {
    template <ranges::input_range In>
    static constexpr void call() {
        constexpr auto N = pairs.size();
        auto elements    = pairs;

        ASSERT(ranges::min(In{elements}, ranges::less{}, get_first) == P{5, 0});
        ASSERT(ranges::max(In{elements}, ranges::less{}, get_first) == P{5, 0});
        {
            auto result = ranges::minmax(In{elements}, ranges::less{}, get_first);
            ASSERT(result.min == P{5, 0});
            ASSERT(result.max == P{5, 7});
        }

        for (size_t i = 0; i < N; ++i) {
            elements[i] = P{0, 42};

            ASSERT(ranges::min(In{elements}, ranges::less{}, get_first) == P{0, 42});
            ASSERT(ranges::max(In{elements}, ranges::less{}, get_first) == (i == 0 ? P{5, 1} : P{5, 0}));

            {
                auto result = ranges::minmax(In{elements}, ranges::less{}, get_first);
                STATIC_ASSERT(same_as<decltype(result), ranges::minmax_result<P>>);
                ASSERT(result.min == P{0, 42});
                ASSERT(result.max == (i == 7 ? P{5, 6} : P{5, 7}));
            }

            for (size_t j = i + 1; j < N; ++j) {
                elements[j] = P{0, 13};

                ASSERT(ranges::min(In{elements}, ranges::less{}, get_first) == P{0, 42});

                ASSERT(ranges::max(In{elements}, ranges::less{}, get_first)
                       == (i == 0 ? (j == 1 ? P{5, 2} : P{5, 1}) : P{5, 0}));

                {
                    auto result = ranges::minmax(In{elements}, ranges::less{}, get_first);
                    STATIC_ASSERT(same_as<decltype(result), ranges::minmax_result<P>>);
                    ASSERT(result.min == P{0, 42});
                    ASSERT(result.max == (j == 7 ? (i == 6 ? P{5, 5} : P{5, 6}) : P{5, 7}));
                }

                elements[j] = P{5, static_cast<int>(j)};
            }

            elements[i] = P{5, static_cast<int>(i)};
        }
    }
};

constexpr void nonrange_tests() {
    // validate overloads of ranges::min, ranges::max, ranges::minmax, and ranges::clamp which take values directly

    constexpr auto thirteen           = 13;
    constexpr auto also_thirteen      = thirteen;
    constexpr auto forty_two          = 42;
    constexpr auto thirteen_pair      = P{13, 200};
    constexpr auto also_thirteen_pair = thirteen_pair;
    constexpr auto forty_two_pair     = P{42, 100};

    ASSERT(&ranges::min(thirteen, forty_two) == &thirteen);
    ASSERT(&ranges::min(forty_two, thirteen) == &thirteen);
    ASSERT(&ranges::min(thirteen, also_thirteen) == &thirteen);

    ASSERT(&ranges::min(thirteen, forty_two, ranges::greater{}) == &forty_two);
    ASSERT(&ranges::min(forty_two, thirteen, ranges::greater{}) == &forty_two);
    ASSERT(&ranges::min(thirteen, also_thirteen, ranges::greater{}) == &thirteen);

    ASSERT(&ranges::min(thirteen_pair, forty_two_pair, ranges::greater{}, get_first) == &forty_two_pair);
    ASSERT(&ranges::min(forty_two_pair, thirteen_pair, ranges::greater{}, get_first) == &forty_two_pair);
    ASSERT(&ranges::min(thirteen_pair, P{thirteen_pair}, ranges::greater{}, get_first) == &thirteen_pair);

    ASSERT(&ranges::max(thirteen, forty_two) == &forty_two);
    ASSERT(&ranges::max(forty_two, thirteen) == &forty_two);
    ASSERT(&ranges::max(thirteen, also_thirteen) == &thirteen);

    ASSERT(&ranges::max(thirteen, forty_two, ranges::greater{}) == &thirteen);
    ASSERT(&ranges::max(forty_two, thirteen, ranges::greater{}) == &thirteen);
    ASSERT(&ranges::max(thirteen, also_thirteen, ranges::greater{}) == &thirteen);

    ASSERT(&ranges::max(thirteen_pair, forty_two_pair, ranges::greater{}, get_first) == &thirteen_pair);
    ASSERT(&ranges::max(forty_two_pair, thirteen_pair, ranges::greater{}, get_first) == &thirteen_pair);
    ASSERT(&ranges::max(thirteen_pair, P{thirteen_pair}, ranges::greater{}, get_first) == &thirteen_pair);

    ASSERT(&ranges::minmax(thirteen, forty_two).min == &thirteen);
    ASSERT(&ranges::minmax(thirteen, forty_two).max == &forty_two);
    ASSERT(&ranges::minmax(forty_two, thirteen).min == &thirteen);
    ASSERT(&ranges::minmax(forty_two, thirteen).max == &forty_two);
    ASSERT(&ranges::minmax(thirteen, also_thirteen).min == &thirteen);
    ASSERT(&ranges::minmax(thirteen, also_thirteen).max == &also_thirteen);

    ASSERT(&ranges::minmax(thirteen, forty_two, ranges::greater{}).min == &forty_two);
    ASSERT(&ranges::minmax(thirteen, forty_two, ranges::greater{}).max == &thirteen);
    ASSERT(&ranges::minmax(forty_two, thirteen, ranges::greater{}).min == &forty_two);
    ASSERT(&ranges::minmax(forty_two, thirteen, ranges::greater{}).max == &thirteen);
    ASSERT(&ranges::minmax(thirteen, also_thirteen, ranges::greater{}).min == &thirteen);
    ASSERT(&ranges::minmax(thirteen, also_thirteen, ranges::greater{}).max == &also_thirteen);

    ASSERT(&ranges::minmax(thirteen_pair, forty_two_pair, ranges::greater{}, get_first).min == &forty_two_pair);
    ASSERT(&ranges::minmax(thirteen_pair, forty_two_pair, ranges::greater{}, get_first).max == &thirteen_pair);
    ASSERT(&ranges::minmax(forty_two_pair, thirteen_pair, ranges::greater{}, get_first).min == &forty_two_pair);
    ASSERT(&ranges::minmax(forty_two_pair, thirteen_pair, ranges::greater{}, get_first).max == &thirteen_pair);
    ASSERT(&ranges::minmax(thirteen_pair, also_thirteen_pair, ranges::greater{}, get_first).min == &thirteen_pair);
    ASSERT(&ranges::minmax(thirteen_pair, also_thirteen_pair, ranges::greater{}, get_first).max == &also_thirteen_pair);

    constexpr auto also_forty_two                      = 42;
    constexpr auto less_than_thirteen                  = 11;
    constexpr auto between_thirteen_and_forty_two      = 37;
    constexpr auto greater_than_forty_two              = 43;
    constexpr auto also_forty_two_pair                 = P{42, 100};
    constexpr auto less_than_thirteen_pair             = P{11, 250};
    constexpr auto between_thirteen_and_forty_two_pair = P{37, 150};
    constexpr auto greater_than_forty_two_pair         = P{43, 50};

    ASSERT(&ranges::clamp(less_than_thirteen, thirteen, forty_two) == &thirteen);
    ASSERT(&ranges::clamp(also_thirteen, thirteen, forty_two) == &also_thirteen);
    ASSERT(&ranges::clamp(between_thirteen_and_forty_two, thirteen, forty_two) == &between_thirteen_and_forty_two);
    ASSERT(&ranges::clamp(also_forty_two, thirteen, forty_two) == &also_forty_two);
    ASSERT(&ranges::clamp(greater_than_forty_two, thirteen, forty_two) == &forty_two);


    ASSERT(&ranges::clamp(less_than_thirteen_pair, forty_two_pair, thirteen_pair, ranges::greater{}, get_first)
           == &thirteen_pair);
    ASSERT(&ranges::clamp(also_thirteen_pair, forty_two_pair, thirteen_pair, ranges::greater{}, get_first)
           == &also_thirteen_pair);
    ASSERT(
        &ranges::clamp(between_thirteen_and_forty_two_pair, forty_two_pair, thirteen_pair, ranges::greater{}, get_first)
        == &between_thirteen_and_forty_two_pair);
    ASSERT(&ranges::clamp(also_forty_two_pair, forty_two_pair, thirteen_pair, ranges::greater{}, get_first)
           == &also_forty_two_pair);
    ASSERT(&ranges::clamp(greater_than_forty_two_pair, forty_two_pair, thirteen_pair, ranges::greater{}, get_first)
           == &forty_two_pair);
}

constexpr void init_list_constexpr_tests() {
    ASSERT(
        ranges::min({P{5, 0}, P{5, 1}, P{5, 2}, P{5, 3}, P{5, 4}, P{5, 5}, P{5, 6}, P{5, 7}}, ranges::less{}, get_first)
        == P{5, 0});
    ASSERT(
        ranges::max({P{5, 0}, P{5, 1}, P{5, 2}, P{5, 3}, P{5, 4}, P{5, 5}, P{5, 6}, P{5, 7}}, ranges::less{}, get_first)
        == P{5, 0});
    auto result = ranges::minmax(
        {P{5, 0}, P{5, 1}, P{5, 2}, P{5, 3}, P{5, 4}, P{5, 5}, P{5, 6}, P{5, 7}}, ranges::less{}, get_first);
    STATIC_ASSERT(same_as<decltype(result), ranges::minmax_result<P>>);
    ASSERT(result.min == P{5, 0});
    ASSERT(result.max == P{5, 7});
}

constexpr void mm_element_constexpr_tests() {
    // (min|max|minmax)_element don't care about size, iterator difference, refinements of forward, commonality,
    // _or_ proxy vs. non-proxy reference. Let's take a couple variations of forward, and one
    // variation of each stronger category.

    using test::range, test::Sized, test::CanDifference, test::Common, test::CanCompare, test::ProxyRef, test::fwd;
    using E = const P;

    mm_element::call<range<fwd, E, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
    mm_element::call<range<fwd, E, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();

    mm_element::call<range<std::bidirectional_iterator_tag, E, Sized::no, CanDifference::no, Common::yes,
        CanCompare::yes, ProxyRef::yes>>();
    mm_element::call<range<std::random_access_iterator_tag, E, Sized::yes, CanDifference::yes, Common::yes,
        CanCompare::yes, ProxyRef::yes>>();
    mm_element::call<range<std::contiguous_iterator_tag, E, Sized::yes, CanDifference::yes, Common::yes,
        CanCompare::yes, ProxyRef::no>>();
}

constexpr void mm_constexpr_tests() {
    // Range overloads of (min|max|minmax) don't care about size, iterator difference, commonality, _or_ proxy vs.
    // non-proxy reference. They _do_ distinguish input vs. forward. Let's take a couple variations of input and
    // forward, and one variation of each stronger category.

    using test::range, test::Sized, test::CanDifference, test::Common, test::CanCompare, test::ProxyRef, test::input,
        test::fwd;
    using E = const P;

    mm::call<range<input, E, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
    mm::call<range<input, E, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();

    mm::call<range<fwd, E, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
    mm::call<range<fwd, E, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();

    mm::call<range<std::bidirectional_iterator_tag, E, Sized::no, CanDifference::no, Common::yes, CanCompare::yes,
        ProxyRef::yes>>();
    mm::call<range<std::random_access_iterator_tag, E, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
        ProxyRef::yes>>();
    mm::call<range<std::contiguous_iterator_tag, E, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
        ProxyRef::no>>();
}

void test_gh_1893() {
    // GH-1893: ranges::clamp was sometimes performing too many projections,
    // and we should conform at least in release mode.
    // the test protects us from the wrong implementation with std::move instead of std::forward in ranges::clamp
    // so reference_wrappers and the lambda are necessary.
    string val           = "meow";
    string low           = "m";
    string high          = "n";
    int projection_count = 0;
    const auto clamped   = ranges::clamp(
        ref(val), ref(low), ref(high), [](auto x, auto y) { return x < y; },
        [&projection_count](const auto& x) -> decltype(auto) {
            ++projection_count;
            return x.get();
        });
    (void) clamped;
#ifdef _DEBUG
    ASSERT(projection_count == 5);
#else
    ASSERT(projection_count == 3);
#endif
    ASSERT(val == "meow");
}

class input_move_iterator {
public:
    using iterator_category = input_iterator_tag;
    using iterator_concept  = input_iterator_tag;
    using difference_type   = ptrdiff_t;
    using value_type        = shared_ptr<int>;
    using pointer           = shared_ptr<int>*;
    using reference         = shared_ptr<int>&&;

    input_move_iterator() = default;
    explicit input_move_iterator(shared_ptr<int>* ptr) : m_ptr(ptr) {}

    reference operator*() const {
        return ranges::iter_move(m_ptr);
    }
    pointer operator->() const {
        return m_ptr;
    }

    input_move_iterator& operator++() {
        ++m_ptr;
        return *this;
    }
    input_move_iterator operator++(int) {
        input_move_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    friend bool operator==(const input_move_iterator&, const input_move_iterator&) = default;

private:
    shared_ptr<int>* m_ptr{nullptr};
};

void test_gh_2900() {
    // GH-2900: <algorithm>: ranges::minmax initializes minmax_result with the moved value
    {
        // check that the random access iterator isn't moved from multiple times
        const string str{"this long string will be dynamically allocated"};
        vector<string> v{str};
        ranges::subrange rng{move_iterator{v.begin()}, move_iterator{v.end()}};
        auto result = ranges::minmax(rng);
        assert(result.min == str);
        assert(result.max == str);
    }
    {
        // check that the input iterator isn't moved from multiple times
        shared_ptr<int> a[] = {make_shared<int>(42)};
        ranges::subrange rng{input_move_iterator{a}, input_move_iterator{a + 1}};
        auto result = ranges::minmax(rng);
        assert(a[0] == nullptr);
        assert(result.min != nullptr);
        assert(result.max == result.min);
        assert(*result.max == 42);
    }
}

int main() {
    STATIC_ASSERT((nonrange_tests(), true));
    nonrange_tests();

    STATIC_ASSERT((init_list_constexpr_tests(), true));
    init_list_constexpr_tests();

    STATIC_ASSERT((test_fwd<mm_element_empty, const P>(), true));
    test_fwd<mm_element_empty, const P>();

    STATIC_ASSERT((mm_element_constexpr_tests(), true));
    test_fwd<mm_element, const P>();

    STATIC_ASSERT((mm_constexpr_tests(), true));
    test_in<mm, const P>();

    test_gh_1893();
    test_gh_2900();
}
