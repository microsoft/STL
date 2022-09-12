// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    static constexpr double some_doubles[] = {0.1, 0.2, 0.3};

    static constexpr double left_folded_sum  = 0.1 + 0.2 + 0.3;
    static constexpr double right_folded_sum = 0.1 + (0.2 + 0.3);
    static_assert(left_folded_sum != right_folded_sum);

    template <ranges::input_range Rng>
    static constexpr void call() {
        using ranges::fold_left, ranges::fold_left_first, ranges::fold_left_with_iter,
            ranges::fold_left_first_with_iter, ranges::fold_right, ranges::fold_right_last,
            ranges::fold_left_with_iter_result, ranges::fold_left_first_with_iter_result, ranges::begin, ranges::end;
        auto vec_of_doubles = some_doubles | ranges::to<vector>();

        { // Validate fold_left iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<double> auto res = fold_left(begin(wrapped), end(wrapped), 0.0, plus{});
            assert(res == left_folded_sum);

            const same_as<double> auto res2 = fold_left(begin(vec_of_doubles), end(vec_of_doubles), 0.0, plus{});
            assert(res2 == left_folded_sum);
        }

        { // Validate fold_left range overload
            const same_as<double> auto res = fold_left(Rng{some_doubles}, 0.0, plus{});
            assert(res == left_folded_sum);

            const same_as<double> auto res2 = fold_left(vec_of_doubles, 0.0, plus{});
            assert(res2 == left_folded_sum);
        }

        { // Validate fold_left_first iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<optional<double>> auto res = fold_left_first(begin(wrapped), end(wrapped), plus{});
            assert(res == left_folded_sum);

            const same_as<optional<double>> auto res2 =
                fold_left_first(begin(vec_of_doubles), end(vec_of_doubles), plus{});
            assert(res2 == left_folded_sum);

            const auto e                              = views::empty<double>;
            const same_as<optional<double>> auto res3 = fold_left_first(begin(e), end(e), plus{});
            assert(res3 == nullopt);
        }

        { // Validate fold_left_first range overload
            const same_as<optional<double>> auto res = fold_left_first(Rng{some_doubles}, plus{});
            assert(res == left_folded_sum);

            const same_as<optional<double>> auto res2 = fold_left_first(vec_of_doubles, plus{});
            assert(res2 == left_folded_sum);

            const same_as<optional<double>> auto res3 = fold_left_first(views::empty<double>, plus{});
            assert(res3 == nullopt);
        }

        { // Validate fold_left_with_iter iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_with_iter_result<ranges::iterator_t<Rng>, double>> auto res =
                fold_left_with_iter(begin(wrapped), end(wrapped), 0.0, plus{});
            assert(res.in == end(wrapped));
            assert(res.value == left_folded_sum);

            const same_as<fold_left_with_iter_result<vector<double>::iterator, double>> auto res2 =
                fold_left_with_iter(begin(vec_of_doubles), end(vec_of_doubles), 0.0, plus{});
            assert(res2.in == end(vec_of_doubles));
            assert(res2.value == left_folded_sum);
        }

        { // Validate fold_left_with_iter range overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_with_iter_result<ranges::iterator_t<Rng>, double>> auto res =
                fold_left_with_iter(wrapped, 0.0, plus{});
            assert(res.in == end(wrapped));
            assert(res.value == left_folded_sum);

            const same_as<fold_left_with_iter_result<vector<double>::iterator, double>> auto res2 =
                fold_left_with_iter(vec_of_doubles, 0.0, plus{});
            assert(res2.in == end(vec_of_doubles));
            assert(res2.value == left_folded_sum);
        }

        { // Validate fold_left_first_with_iter iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_first_with_iter_result<ranges::iterator_t<Rng>, optional<double>>> auto res =
                fold_left_first_with_iter(begin(wrapped), end(wrapped), plus{});
            assert(res.in == end(wrapped));
            assert(res.value == left_folded_sum);

            const same_as<fold_left_first_with_iter_result<vector<double>::iterator, optional<double>>> auto res2 =
                fold_left_first_with_iter(begin(vec_of_doubles), end(vec_of_doubles), plus{});
            assert(res2.in == end(vec_of_doubles));
            assert(res2.value == left_folded_sum);

            const auto e = views::empty<const double>;
            const same_as<fold_left_first_with_iter_result<const double*, optional<double>>> auto res3 =
                fold_left_first_with_iter(begin(e), end(e), plus{});
            assert(res3.in == end(e));
            assert(res3.value == nullopt);
        }

        { // Validate fold_left_first_with_iter range overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_first_with_iter_result<ranges::iterator_t<Rng>, optional<double>>> auto res =
                fold_left_first_with_iter(wrapped, plus{});
            assert(res.in == end(wrapped));
            assert(res.value == left_folded_sum);

            const same_as<fold_left_first_with_iter_result<vector<double>::iterator, optional<double>>> auto res2 =
                fold_left_first_with_iter(vec_of_doubles, plus{});
            assert(res2.in == end(vec_of_doubles));
            assert(res2.value == left_folded_sum);

            const auto e = views::empty<const double>;
            const same_as<fold_left_first_with_iter_result<const double*, optional<double>>> auto res3 =
                fold_left_first_with_iter(e, plus{});
            assert(res3.in == end(e));
            assert(res3.value == nullopt);
        }

        if constexpr (ranges::bidirectional_range<Rng>) {
            { // Validate fold_right iterator+sentinel overload
                const Rng wrapped{some_doubles};
                const same_as<double> auto res = fold_right(begin(wrapped), end(wrapped), 0.0, plus{});
                assert(res == right_folded_sum);

                const same_as<double> auto res2 = fold_right(begin(vec_of_doubles), end(vec_of_doubles), 0.0, plus{});
                assert(res2 == right_folded_sum);
            }

            { // Validate fold_right range overload
                const same_as<double> auto res = fold_right(Rng{some_doubles}, 0.0, plus{});
                assert(res == right_folded_sum);

                const same_as<double> auto res2 = fold_right(vec_of_doubles, 0.0, plus{});
                assert(res2 == right_folded_sum);
            }

            { // Validate fold_right_last iterator+sentinel overload
                const Rng wrapped{some_doubles};
                const same_as<optional<double>> auto res = fold_right_last(begin(wrapped), end(wrapped), plus{});
                assert(res == right_folded_sum);

                const same_as<optional<double>> auto res2 =
                    fold_right_last(begin(vec_of_doubles), end(vec_of_doubles), plus{});
                assert(res2 == right_folded_sum);

                const auto e                              = views::empty<double>;
                const same_as<optional<double>> auto res3 = fold_right_last(begin(e), end(e), plus{});
                assert(res3 == nullopt);
            }

            { // Validate fold_right_last range overload
                const same_as<optional<double>> auto res = fold_right_last(Rng{some_doubles}, plus{});
                assert(res == right_folded_sum);

                const same_as<optional<double>> auto res2 = fold_right_last(vec_of_doubles, plus{});
                assert(res2 == right_folded_sum);

                const same_as<optional<double>> auto res3 = fold_right_last(views::empty<double>, plus{});
                assert(res3 == nullopt);
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, const double>(), true));
    STATIC_ASSERT((test_bidi<instantiator, const double>(), true));
    test_in<instantiator, const double>();
    test_bidi<instantiator, const double>();
}
