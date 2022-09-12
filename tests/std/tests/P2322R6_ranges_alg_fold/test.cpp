// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <ranges>
#include <type_traits>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    static constexpr double some_doubles[] = {0.1, 0.2, 0.3};

    static constexpr double left_sum  = 0.1 + 0.2 + 0.3;
    static constexpr double right_sum = 0.1 + (0.2 + 0.3);
    static_assert(left_sum != right_sum);

    template <ranges::input_range Rng>
    static constexpr void call() {
        using ranges::fold_left, ranges::fold_left_first, ranges::fold_left_with_iter,
            ranges::fold_left_first_with_iter, ranges::fold_right, ranges::fold_right_last,
            ranges::fold_left_with_iter_result, ranges::fold_left_first_with_iter_result, ranges::begin, ranges::end;
        auto vec_of_doubles = some_doubles | ranges::to<vector>();

        { // Validate fold_left iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<double> auto sum1 = fold_left(begin(wrapped), end(wrapped), 0.0, plus{});
            assert(sum1 == left_sum);

            const same_as<double> auto sum2 = fold_left(begin(vec_of_doubles), end(vec_of_doubles), 0.0, plus{});
            assert(sum2 == left_sum);

            const double single_value       = 3.14;
            const auto e                    = views::empty<const double>;
            const same_as<double> auto sum3 = fold_left(begin(e), end(e), single_value, plus{});
            assert(sum3 == single_value);
        }

        { // Validate fold_left range overload
            const same_as<double> auto sum1 = fold_left(Rng{some_doubles}, 0.0, plus{});
            assert(sum1 == left_sum);

            const same_as<double> auto sum2 = fold_left(vec_of_doubles, 0.0, plus{});
            assert(sum2 == left_sum);

            const double single_value       = 3.14;
            const same_as<double> auto sum3 = fold_left(views::empty<const double>, single_value, plus{});
            assert(sum3 == single_value);
        }

        { // Validate fold_left_first iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<optional<double>> auto sum1 = fold_left_first(begin(wrapped), end(wrapped), plus{});
            assert(sum1 == left_sum);

            const same_as<optional<double>> auto sum2 =
                fold_left_first(begin(vec_of_doubles), end(vec_of_doubles), plus{});
            assert(sum2 == left_sum);

            const auto e                              = views::empty<const double>;
            const same_as<optional<double>> auto sum3 = fold_left_first(begin(e), end(e), plus{});
            assert(sum3 == nullopt);
        }

        { // Validate fold_left_first range overload
            const same_as<optional<double>> auto sum1 = fold_left_first(Rng{some_doubles}, plus{});
            assert(sum1 == left_sum);

            const same_as<optional<double>> auto sum2 = fold_left_first(vec_of_doubles, plus{});
            assert(sum2 == left_sum);

            const same_as<optional<double>> auto sum3 = fold_left_first(views::empty<const double>, plus{});
            assert(sum3 == nullopt);
        }

        { // Validate fold_left_with_iter iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_with_iter_result<ranges::iterator_t<Rng>, double>> auto sum1 =
                fold_left_with_iter(begin(wrapped), end(wrapped), 0.0, plus{});
            assert(sum1.in == end(wrapped));
            assert(sum1.value == left_sum);

            const same_as<fold_left_with_iter_result<vector<double>::iterator, double>> auto sum2 =
                fold_left_with_iter(begin(vec_of_doubles), end(vec_of_doubles), 0.0, plus{});
            assert(sum2.in == end(vec_of_doubles));
            assert(sum2.value == left_sum);

            const double single_value = 3.14;
            const auto e              = views::empty<const double>;
            const same_as<fold_left_with_iter_result<const double*, double>> auto sum3 =
                fold_left_with_iter(begin(e), end(e), single_value, plus{});
            assert(sum3.in == nullptr);
            assert(sum3.value == single_value);
        }

        { // Validate fold_left_with_iter range overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_with_iter_result<ranges::iterator_t<Rng>, double>> auto sum1 =
                fold_left_with_iter(wrapped, 0.0, plus{});
            assert(sum1.in == end(wrapped));
            assert(sum1.value == left_sum);

            const same_as<fold_left_with_iter_result<vector<double>::iterator, double>> auto sum2 =
                fold_left_with_iter(vec_of_doubles, 0.0, plus{});
            assert(sum2.in == end(vec_of_doubles));
            assert(sum2.value == left_sum);

            const same_as<fold_left_with_iter_result<ranges::dangling, double>> auto sum3 =
                fold_left_with_iter(some_doubles | ranges::to<vector>(), 0.0, plus{});
            assert(sum3.value == left_sum);

            const double single_value = 3.14;
            const same_as<fold_left_with_iter_result<const double*, double>> auto sum4 =
                fold_left_with_iter(views::empty<const double>, single_value, plus{});
            assert(sum4.in == nullptr);
            assert(sum4.value == single_value);
        }

        { // Validate fold_left_first_with_iter iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_first_with_iter_result<ranges::iterator_t<Rng>, optional<double>>> auto sum1 =
                fold_left_first_with_iter(begin(wrapped), end(wrapped), plus{});
            assert(sum1.in == end(wrapped));
            assert(sum1.value == left_sum);

            const same_as<fold_left_first_with_iter_result<vector<double>::iterator, optional<double>>> auto sum2 =
                fold_left_first_with_iter(begin(vec_of_doubles), end(vec_of_doubles), plus{});
            assert(sum2.in == end(vec_of_doubles));
            assert(sum2.value == left_sum);

            const auto e = views::empty<const double>;
            const same_as<fold_left_first_with_iter_result<const double*, optional<double>>> auto sum3 =
                fold_left_first_with_iter(begin(e), end(e), plus{});
            assert(sum3.in == end(e));
            assert(sum3.value == nullopt);
        }

        { // Validate fold_left_first_with_iter range overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_first_with_iter_result<ranges::iterator_t<Rng>, optional<double>>> auto sum1 =
                fold_left_first_with_iter(wrapped, plus{});
            assert(sum1.in == end(wrapped));
            assert(sum1.value == left_sum);

            const same_as<fold_left_first_with_iter_result<vector<double>::iterator, optional<double>>> auto sum2 =
                fold_left_first_with_iter(vec_of_doubles, plus{});
            assert(sum2.in == end(vec_of_doubles));
            assert(sum2.value == left_sum);

            const auto e = views::empty<const double>;
            const same_as<fold_left_first_with_iter_result<const double*, optional<double>>> auto sum3 =
                fold_left_first_with_iter(e, plus{});
            assert(sum3.in == end(e));
            assert(sum3.value == nullopt);

            const same_as<fold_left_first_with_iter_result<ranges::dangling, optional<double>>> auto sum4 =
                fold_left_first_with_iter(some_doubles | ranges::to<vector>(), plus{});
            assert(sum4.value == left_sum);
        }

        if constexpr (ranges::bidirectional_range<Rng>) {
            { // Validate fold_right iterator+sentinel overload
                const Rng wrapped{some_doubles};
                const same_as<double> auto sum1 = fold_right(begin(wrapped), end(wrapped), 0.0, plus{});
                assert(sum1 == right_sum);

                const same_as<double> auto sum2 = fold_right(begin(vec_of_doubles), end(vec_of_doubles), 0.0, plus{});
                assert(sum2 == right_sum);

                const double single_value       = 3.14;
                const auto e                    = views::empty<const double>;
                const same_as<double> auto sum3 = fold_left(begin(e), end(e), single_value, plus{});
                assert(sum3 == single_value);
            }

            { // Validate fold_right range overload
                const same_as<double> auto sum1 = fold_right(Rng{some_doubles}, 0.0, plus{});
                assert(sum1 == right_sum);

                const same_as<double> auto sum2 = fold_right(vec_of_doubles, 0.0, plus{});
                assert(sum2 == right_sum);

                const double single_value       = 3.14;
                const same_as<double> auto sum3 = fold_left(views::empty<const double>, single_value, plus{});
                assert(sum3 == single_value);
            }

            { // Validate fold_right_last iterator+sentinel overload
                const Rng wrapped{some_doubles};
                const same_as<optional<double>> auto sum1 = fold_right_last(begin(wrapped), end(wrapped), plus{});
                assert(sum1 == right_sum);

                const same_as<optional<double>> auto sum2 =
                    fold_right_last(begin(vec_of_doubles), end(vec_of_doubles), plus{});
                assert(sum2 == right_sum);

                const auto e                              = views::empty<const double>;
                const same_as<optional<double>> auto sum3 = fold_right_last(begin(e), end(e), plus{});
                assert(sum3 == nullopt);
            }

            { // Validate fold_right_last range overload
                const same_as<optional<double>> auto sum1 = fold_right_last(Rng{some_doubles}, plus{});
                assert(sum1 == right_sum);

                const same_as<optional<double>> auto sum2 = fold_right_last(vec_of_doubles, plus{});
                assert(sum2 == right_sum);

                const same_as<optional<double>> auto sum3 = fold_right_last(views::empty<const double>, plus{});
                assert(sum3 == nullopt);
            }
        }
    }
};

constexpr bool test_in_value_result() {
    using ranges::in_value_result;
    STATIC_ASSERT(is_aggregate_v<in_value_result<int, int>>);
    STATIC_ASSERT(is_trivial_v<in_value_result<int, int>>);

    in_value_result<int, int> res = {5, 6};
    { // Validate binding
        auto [in, value] = res;
        assert(in == 5);
        assert(value == 6);
    }

    { // Validate conversion operator (const &)
        in_value_result<long, long> long_res = res;
        assert(long_res.in == 5L);
        assert(long_res.value == 6L);
    }

    { // Validate conversion operator (&&)
        in_value_result<long long, long long> long_long_res = std::move(res);
        assert(long_long_res.in == 5LL);
        assert(long_long_res.value == 6LL);
    }

    return true;
}

int main() {
    STATIC_ASSERT((test_in<instantiator, const double>(), true));
    STATIC_ASSERT((test_bidi<instantiator, const double>(), true));
    test_in<instantiator, const double>();
    test_bidi<instantiator, const double>();

    STATIC_ASSERT(test_in_value_result());
    assert(test_in_value_result());
}
