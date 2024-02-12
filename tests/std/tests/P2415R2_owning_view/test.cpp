// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <memory>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    template <ranges::input_range ImmobileR>
    static constexpr void call() {
        using R = ImmobileR::RebindAsMoveOnly;
        using ranges::owning_view, ranges::begin, ranges::end, ranges::bidirectional_range, ranges::common_range,
            ranges::contiguous_range, ranges::forward_range, ranges::input_range, ranges::random_access_range;
        int input[3] = {0, 1, 2};

        // properties
        STATIC_ASSERT(ranges::enable_borrowed_range<owning_view<R>> == ranges::enable_borrowed_range<R>);

        STATIC_ASSERT(ranges::view<owning_view<R>>);
        STATIC_ASSERT(input_range<owning_view<R>>);
        STATIC_ASSERT(forward_range<owning_view<R>> == forward_range<R>);
        STATIC_ASSERT(bidirectional_range<owning_view<R>> == bidirectional_range<R>);
        STATIC_ASSERT(random_access_range<owning_view<R>> == random_access_range<R>);
        STATIC_ASSERT(contiguous_range<owning_view<R>> == contiguous_range<R>);

        { // constructors and assignment operators
            STATIC_ASSERT(constructible_from<owning_view<R>, R>);
            STATIC_ASSERT(!constructible_from<owning_view<R>, R&>);
            STATIC_ASSERT(!constructible_from<owning_view<R>, const R&>);
            STATIC_ASSERT(!constructible_from<owning_view<R>, const R>);

            STATIC_ASSERT(default_initializable<owning_view<R>> == default_initializable<R>);
            STATIC_ASSERT(is_nothrow_default_constructible_v<owning_view<R>> == is_nothrow_default_constructible_v<R>);
            if constexpr (default_initializable<R>) {
                [[maybe_unused]] owning_view<R> default_constructed;
            }

            owning_view<R> converted{R{input}};
            STATIC_ASSERT(is_nothrow_constructible_v<owning_view<R>, R> == is_nothrow_move_constructible_v<R>);

            owning_view<R> move_constructed = move(converted);
            if constexpr (forward_range<R>) {
                assert(move_constructed.begin().peek() == begin(input));
            }
            assert(move_constructed.end().peek() == end(input));

            if constexpr (movable<R>) {
                int other_data[3] = {4, 5, 6};
                owning_view<R> move_assigned{R{other_data}};
                move_assigned = move(move_constructed);
                if constexpr (forward_range<R>) {
                    assert(move_assigned.begin().peek() == begin(input));
                }
                assert(move_assigned.end().peek() == end(input));
            }
        }

        { // access
            owning_view<R> test_view{R{input}};
            same_as<R> auto& base = test_view.base();
            STATIC_ASSERT(noexcept(test_view.base()));
            assert(addressof(base) != nullptr);

            same_as<const R> auto& cbase = as_const(test_view).base();
            STATIC_ASSERT(noexcept(as_const(test_view).base()));
            assert(addressof(cbase) == addressof(base));

            same_as<R> auto&& rbase = move(test_view).base();
            STATIC_ASSERT(noexcept(move(test_view).base()));
            assert(addressof(rbase) == addressof(base));

            same_as<const R> auto&& crbase = move(as_const(test_view)).base();
            STATIC_ASSERT(noexcept(move(as_const(test_view)).base()));
            assert(addressof(crbase) == addressof(base));
        }

        { // iterators
            {
                owning_view<R> test_view{R{input}};
                same_as<ranges::iterator_t<R>> auto first = test_view.begin();
                STATIC_ASSERT(noexcept(test_view.begin()) == noexcept(declval<R&>().begin()));
                assert(first.peek() == input);

                const same_as<ranges::sentinel_t<R>> auto last = test_view.end();
                STATIC_ASSERT(noexcept(test_view.end()) == noexcept(declval<R&>().end()));
                assert(last.peek() == end(input));
            }

            if constexpr (input_range<const R>) {
                const owning_view<R> ctest_view{R{input}};
                same_as<ranges::iterator_t<const R>> auto first = ctest_view.begin();
                STATIC_ASSERT(noexcept(ctest_view.begin()) == noexcept(declval<const R&>().begin()));
                assert(first.peek() == input);

                const same_as<ranges::sentinel_t<const R>> auto last = ctest_view.end();
                STATIC_ASSERT(noexcept(ctest_view.end()) == noexcept(declval<const R&>().end()));
                assert(last.peek() == end(input));
            }
        }

#if _HAS_CXX23
        { // const iterators
            {
                owning_view<R> test_view{R{input}};
                same_as<ranges::const_iterator_t<R>> auto first = test_view.cbegin();
                assert(first.base().peek() == input);

                same_as<ranges::const_sentinel_t<R>> auto last = test_view.cend();
                if constexpr (_Is_specialization_v<decltype(last), basic_const_iterator>) {
                    assert(last.base().peek() == end(input));
                } else {
                    assert(last.peek() == end(input));
                }
            }

            if constexpr (input_range<const R>) {
                const owning_view<R> ctest_view{R{input}};
                same_as<ranges::const_iterator_t<const R>> auto first = ctest_view.cbegin();
                assert(first.base().peek() == input);

                same_as<ranges::const_sentinel_t<const R>> auto last = ctest_view.cend();
                if constexpr (_Is_specialization_v<decltype(last), basic_const_iterator>) {
                    assert(last.base().peek() == end(input));
                } else {
                    assert(last.peek() == end(input));
                }
            }
        }
#endif // _HAS_CXX23

        { // empty
            STATIC_ASSERT(CanMemberEmpty<owning_view<R>> == CanEmpty<R>);
            if constexpr (CanEmpty<R>) {
                owning_view<R> test_view{R{input}};

                const same_as<bool> auto is_empty = test_view.empty();
                STATIC_ASSERT(noexcept(test_view.empty()) == noexcept(ranges::empty(declval<R>())));
                assert(!is_empty);

                owning_view<R> empty_view{R{span<int, 0>{}}};
                assert(empty_view.empty());

                // view_interface::operator bool
                assert(test_view);
                assert(!empty_view);
            }

            STATIC_ASSERT(CanMemberEmpty<const owning_view<R>> == CanEmpty<const R>);
            if constexpr (CanEmpty<const R>) {
                const owning_view<R> test_view{R{input}};

                const same_as<bool> auto is_empty = test_view.empty();
                STATIC_ASSERT(noexcept(test_view.empty()) == noexcept(ranges::empty(declval<const R>())));
                assert(!is_empty);

                owning_view<R> empty_view{R{span<int, 0>{}}};
                assert(empty_view.empty());

                // view_interface::operator bool
                assert(test_view);
                assert(!empty_view);
            }
        }

        { // size
            STATIC_ASSERT(CanMemberSize<owning_view<R>> == ranges::sized_range<R>);
            if constexpr (ranges::sized_range<R>) {
                owning_view<R> test_view{R{input}};

                const same_as<ranges::range_size_t<R>> auto ref_size = test_view.size();
                assert(_To_unsigned_like(ref_size) == ranges::size(input));

                STATIC_ASSERT(noexcept(test_view.size()) == noexcept(ranges::size(declval<R>())));
            }

            STATIC_ASSERT(CanMemberSize<const owning_view<R>> == ranges::sized_range<const R>);
            if constexpr (ranges::sized_range<const R>) {
                const owning_view<R> test_view{R{input}};

                const same_as<ranges::range_size_t<const R>> auto ref_size = test_view.size();
                assert(_To_unsigned_like(ref_size) == ranges::size(input));

                STATIC_ASSERT(noexcept(test_view.size()) == noexcept(ranges::size(declval<const R>())));
            }
        }

        { // data
            STATIC_ASSERT(CanMemberData<owning_view<R>> == contiguous_range<R>);
            if constexpr (contiguous_range<R>) {
                owning_view<R> test_view{R{input}};

                const same_as<int*> auto ptr = test_view.data();
                assert(ptr == input);

                STATIC_ASSERT(noexcept(test_view.data()) == noexcept(ranges::data(declval<R&>())));
            }

            STATIC_ASSERT(CanMemberData<const owning_view<R>> == contiguous_range<const R>);
            if constexpr (contiguous_range<const R>) {
                const owning_view<R> test_view{R{input}};

                const same_as<int*> auto ptr = test_view.data();
                assert(ptr == input);

                STATIC_ASSERT(noexcept(test_view.data()) == noexcept(ranges::data(declval<const R&>())));
            }
        }

        // CTAD
        STATIC_ASSERT(same_as<decltype(owning_view{R{input}}), owning_view<R>>);

        { // view_interface::back, ::front
            STATIC_ASSERT(CanMemberFront<owning_view<R>> == forward_range<R>);
            if constexpr (forward_range<R>) {
                owning_view<R> test_view{R{input}};
                assert(test_view.front() == input[0]);

                STATIC_ASSERT(CanMemberBack<owning_view<R>> == (bidirectional_range<R> && common_range<R>) );
                if constexpr (bidirectional_range<R> && common_range<R>) {
                    assert(test_view.back() == input[2]);
                }
            }

            STATIC_ASSERT(CanMemberFront<const owning_view<R>> == forward_range<const R>);
            if constexpr (forward_range<const R>) {
                const owning_view<R> test_view{R{input}};
                assert(test_view.front() == input[0]);

                STATIC_ASSERT(
                    CanMemberBack<const owning_view<R>> == (bidirectional_range<const R> && common_range<const R>) );
                if constexpr (bidirectional_range<const R> && common_range<const R>) {
                    assert(test_view.back() == input[2]);
                }
            }
        }

        { // view_interface::operator[]
            STATIC_ASSERT(CanIndex<owning_view<R>> == random_access_range<R>);
            if constexpr (random_access_range<R>) {
                owning_view<R> test_view{R{input}};
                assert(test_view[0] == input[0]);
                assert(test_view[1] == input[1]);
                assert(test_view[2] == input[2]);
            }

            STATIC_ASSERT(CanIndex<const owning_view<R>> == random_access_range<const R>);
            if constexpr (random_access_range<const R>) {
                const owning_view<R> test_view{R{input}};
                assert(test_view[0] == input[0]);
                assert(test_view[1] == input[1]);
                assert(test_view[2] == input[2]);
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, int>(), true));
    test_in<instantiator, int>();
}
