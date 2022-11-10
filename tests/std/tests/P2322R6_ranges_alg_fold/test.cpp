// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    static constexpr double some_doubles[]  = {0.1, 0.2, 0.3};
    static constexpr double left_sum        = 0.1 + 0.2 + 0.3;
    static constexpr double left_difference = 0.1 - 0.2 - 0.3;
    static constexpr double right_product   = 0.1 * (0.2 * 0.3);

    static constexpr bool non_dependent() {
        using ranges::fold_left, ranges::fold_left_first, ranges::fold_left_with_iter,
            ranges::fold_left_first_with_iter, ranges::fold_right, ranges::fold_right_last,
            ranges::fold_left_with_iter_result, ranges::fold_left_first_with_iter_result, ranges::begin, ranges::end;

        constexpr auto empty_rng  = views::empty<const double>;
        const auto vec_of_doubles = some_doubles | ranges::to<vector>();

        { // Validate fold_left iterator+sentinel overload
            const same_as<double> auto sum2 = fold_left(begin(vec_of_doubles), end(vec_of_doubles), 0.0, plus{});
            assert(sum2 == left_sum);

            const double single_value       = 3.14;
            const same_as<double> auto sum3 = fold_left(begin(empty_rng), end(empty_rng), single_value, plus{});
            assert(sum3 == single_value);
        }

        { // Validate fold_left range overload
            const same_as<double> auto sum2 = fold_left(vec_of_doubles, 0.0, plus{});
            assert(sum2 == left_sum);

            const double single_value       = 3.14;
            const same_as<double> auto sum3 = fold_left(empty_rng, single_value, plus{});
            assert(sum3 == single_value);
        }

        { // Validate fold_left_first iterator+sentinel overload
            const same_as<optional<double>> auto diff2 =
                fold_left_first(begin(vec_of_doubles), end(vec_of_doubles), minus<double>{});
            assert(diff2 == left_difference);

            const same_as<optional<double>> auto diff3 =
                fold_left_first(begin(empty_rng), end(empty_rng), minus<double>{});
            assert(diff3 == nullopt);
        }

        { // Validate fold_left_first range overload
            const same_as<optional<double>> auto diff2 = fold_left_first(vec_of_doubles, minus<double>{});
            assert(diff2 == left_difference);

            const same_as<optional<double>> auto diff3 = fold_left_first(empty_rng, minus<double>{});
            assert(diff3 == nullopt);
        }

        { // Validate fold_left_with_iter iterator+sentinel overload
            const same_as<fold_left_with_iter_result<vector<double>::const_iterator, double>> auto sum2 =
                fold_left_with_iter(begin(vec_of_doubles), end(vec_of_doubles), 0.0, plus{});
            assert(sum2.in == end(vec_of_doubles));
            assert(sum2.value == left_sum);

            const double single_value = 3.14;
            const same_as<fold_left_with_iter_result<const double*, double>> auto sum3 =
                fold_left_with_iter(begin(empty_rng), end(empty_rng), single_value, plus{});
            assert(sum3.in == empty_rng.end());
            assert(sum3.value == single_value);
        }

        { // Validate fold_left_with_iter range overload
            const same_as<fold_left_with_iter_result<vector<double>::const_iterator, double>> auto sum2 =
                fold_left_with_iter(vec_of_doubles, 0.0, plus{});
            assert(sum2.in == end(vec_of_doubles));
            assert(sum2.value == left_sum);

            const same_as<fold_left_with_iter_result<ranges::dangling, double>> auto sum3 =
                fold_left_with_iter(some_doubles | ranges::to<vector>(), 0.0, plus{});
            assert(sum3.value == left_sum);

            const double single_value = 3.14;
            const same_as<fold_left_with_iter_result<const double*, double>> auto sum4 =
                fold_left_with_iter(empty_rng, single_value, plus{});
            assert(sum4.in == empty_rng.end());
            assert(sum4.value == single_value);
        }

        { // Validate fold_left_first_with_iter iterator+sentinel overload
            const same_as<fold_left_first_with_iter_result<vector<double>::const_iterator, optional<double>>> auto
                diff2 = fold_left_first_with_iter(begin(vec_of_doubles), end(vec_of_doubles), minus<double>{});
            assert(diff2.in == end(vec_of_doubles));
            assert(diff2.value == left_difference);

            const same_as<fold_left_first_with_iter_result<const double*, optional<double>>> auto diff3 =
                fold_left_first_with_iter(begin(empty_rng), end(empty_rng), minus<double>{});
            assert(diff3.in == end(empty_rng));
            assert(diff3.value == nullopt);
        }

        { // Validate fold_left_first_with_iter range overload
            const same_as<fold_left_first_with_iter_result<vector<double>::const_iterator, optional<double>>> auto
                diff2 = fold_left_first_with_iter(vec_of_doubles, minus<double>{});
            assert(diff2.in == end(vec_of_doubles));
            assert(diff2.value == left_difference);

            const same_as<fold_left_first_with_iter_result<const double*, optional<double>>> auto diff3 =
                fold_left_first_with_iter(empty_rng, minus<double>{});
            assert(diff3.in == end(empty_rng));
            assert(diff3.value == nullopt);

            const same_as<fold_left_first_with_iter_result<ranges::dangling, optional<double>>> auto diff4 =
                fold_left_first_with_iter(some_doubles | ranges::to<vector>(), minus<double>{});
            assert(diff4.value == left_difference);
        }

        { // Validate fold_right iterator+sentinel overload
            const same_as<double> auto prod2 =
                fold_right(begin(vec_of_doubles), end(vec_of_doubles), 1.0, multiplies{});
            assert(prod2 == right_product);

            const double single_value        = 3.14;
            const same_as<double> auto prod3 = fold_right(begin(empty_rng), end(empty_rng), single_value, multiplies{});
            assert(prod3 == single_value);
        }

        { // Validate fold_right range overload
            const same_as<double> auto prod2 = fold_right(vec_of_doubles, 1.0, multiplies{});
            assert(prod2 == right_product);

            const double single_value        = 3.14;
            const same_as<double> auto prod3 = fold_right(empty_rng, single_value, multiplies{});
            assert(prod3 == single_value);
        }

        { // Validate fold_right_last iterator+sentinel overload
            const same_as<optional<double>> auto prod2 =
                fold_right_last(begin(vec_of_doubles), end(vec_of_doubles), multiplies{});
            assert(prod2 == right_product);

            const same_as<optional<double>> auto prod3 =
                fold_right_last(begin(empty_rng), end(empty_rng), multiplies{});
            assert(prod3 == nullopt);
        }

        { // Validate fold_right_last range overload
            const same_as<optional<double>> auto prod2 = fold_right_last(vec_of_doubles, multiplies{});
            assert(prod2 == right_product);

            const same_as<optional<double>> auto prod3 = fold_right_last(empty_rng, multiplies{});
            assert(prod3 == nullopt);
        }

        return true;
    }

    template <ranges::input_range Rng>
    static constexpr void call() {
        using ranges::fold_left, ranges::fold_left_first, ranges::fold_left_with_iter,
            ranges::fold_left_first_with_iter, ranges::fold_right, ranges::fold_right_last,
            ranges::fold_left_with_iter_result, ranges::fold_left_first_with_iter_result, ranges::begin, ranges::end;

        { // Validate fold_left iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<double> auto sum1 = fold_left(begin(wrapped), end(wrapped), 0.0, plus{});
            assert(sum1 == left_sum);
        }

        { // Validate fold_left range overload
            const same_as<double> auto sum1 = fold_left(Rng{some_doubles}, 0.0, plus{});
            assert(sum1 == left_sum);
        }

        { // Validate fold_left_first iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<optional<double>> auto diff1 = fold_left_first(begin(wrapped), end(wrapped), minus<double>{});
            assert(diff1 == left_difference);
        }

        { // Validate fold_left_first range overload
            const same_as<optional<double>> auto diff1 = fold_left_first(Rng{some_doubles}, minus<double>{});
            assert(diff1 == left_difference);
        }

        { // Validate fold_left_with_iter iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_with_iter_result<ranges::iterator_t<Rng>, double>> auto sum1 =
                fold_left_with_iter(begin(wrapped), end(wrapped), 0.0, plus{});
            assert(sum1.in == end(wrapped));
            assert(sum1.value == left_sum);
        }

        { // Validate fold_left_with_iter range overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_with_iter_result<ranges::iterator_t<Rng>, double>> auto sum1 =
                fold_left_with_iter(wrapped, 0.0, plus{});
            assert(sum1.in == end(wrapped));
            assert(sum1.value == left_sum);
        }

        { // Validate fold_left_first_with_iter iterator+sentinel overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_first_with_iter_result<ranges::iterator_t<Rng>, optional<double>>> auto diff1 =
                fold_left_first_with_iter(begin(wrapped), end(wrapped), minus<double>{});
            assert(diff1.in == end(wrapped));
            assert(diff1.value == left_difference);
        }

        { // Validate fold_left_first_with_iter range overload
            const Rng wrapped{some_doubles};
            const same_as<fold_left_first_with_iter_result<ranges::iterator_t<Rng>, optional<double>>> auto diff1 =
                fold_left_first_with_iter(wrapped, minus<double>{});
            assert(diff1.in == end(wrapped));
            assert(diff1.value == left_difference);
        }

        if constexpr (ranges::bidirectional_range<Rng>) {
            { // Validate fold_right iterator+sentinel overload
                const Rng wrapped{some_doubles};
                const same_as<double> auto prod1 = fold_right(begin(wrapped), end(wrapped), 1.0, multiplies{});
                assert(prod1 == right_product);
            }

            { // Validate fold_right range overload
                const same_as<double> auto prod1 = fold_right(Rng{some_doubles}, 1.0, multiplies{});
                assert(prod1 == right_product);
            }

            { // Validate fold_right_last iterator+sentinel overload
                const Rng wrapped{some_doubles};
                const same_as<optional<double>> auto prod1 =
                    fold_right_last(begin(wrapped), end(wrapped), multiplies{});
                assert(prod1 == right_product);
            }

            { // Validate fold_right_last range overload
                const same_as<optional<double>> auto prod1 = fold_right_last(Rng{some_doubles}, multiplies{});
                assert(prod1 == right_product);
            }
        }
    }
};

constexpr bool test_in_value_result() {
    using ranges::in_value_result;
    STATIC_ASSERT(is_aggregate_v<in_value_result<int, int>>);
    STATIC_ASSERT(is_trivial_v<in_value_result<int, int>>);

    in_value_result<int*, int> res = {nullptr, 6};
    { // Validate binding
        auto [in, value] = res;
        assert(in == nullptr);
        assert(value == 6);
    }

    { // Validate conversion operator (const &)
        in_value_result<const int*, long> long_res = res;
        assert(long_res.in == nullptr);
        assert(long_res.value == 6L);
    }

    { // Validate conversion operator (&&)
        in_value_result<const int*, long long> long_long_res = std::move(res);
        assert(long_long_res.in == nullptr);
        assert(long_long_res.value == 6LL);
    }

    return true;
}

int main() {
    STATIC_ASSERT((test_in<instantiator, const double>(), true));
    test_in<instantiator, const double>();

    STATIC_ASSERT((test_bidi<instantiator, const double>(), true));
    test_bidi<instantiator, const double>();

    STATIC_ASSERT(instantiator::non_dependent());
    instantiator::non_dependent();

    STATIC_ASSERT(test_in_value_result());
    assert(test_in_value_result());
}
