// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <format>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

using namespace std;

struct safe_iter_out_of_bounds_err {};

enum range_type { range_large = 3, range_medium = 4, range_small = 5 };

bool safe_iter_nothrow_OOB_sign = false;

// When `nothrow` is true, disables bounds checking during iterator increment/decrement (to verify `to_address` calls),
// logging out-of-bounds accesses instead of throwing. When false, throws immediately on bounds violation.
template <class T, bool nothrow = false>
class safe_iter {
public:
    using iterator_concept  = contiguous_iterator_tag;
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    constexpr safe_iter(pointer ptr, pointer range_first, pointer range_last) noexcept
        : current_ptr{ptr}, range_first_ptr{range_first}, range_last_ptr{range_last} {
        assert(current_ptr >= range_first_ptr && current_ptr <= range_last_ptr);
    }
    constexpr safe_iter() noexcept = default;

    constexpr reference operator*() const noexcept(nothrow) {
        if (current_ptr < range_first_ptr || current_ptr >= range_last_ptr) {
            if constexpr (nothrow) {
                safe_iter_nothrow_OOB_sign = true;
            } else {
                throw safe_iter_out_of_bounds_err{};
            }
        }
        return *current_ptr;
    }
    constexpr pointer operator->() const noexcept {
        if constexpr (nothrow) {
            if (current_ptr < range_first_ptr || current_ptr > range_last_ptr) {
                safe_iter_nothrow_OOB_sign = true;
            }
        }
        return current_ptr;
    }
    constexpr reference operator[](difference_type n) const noexcept(nothrow) {
        return *(*this + n);
    }

    constexpr safe_iter& operator+=(difference_type offset) noexcept(nothrow) {
        current_ptr += offset;
        if constexpr (!nothrow) {
            if (current_ptr < range_first_ptr || current_ptr > range_last_ptr) {
                throw safe_iter_out_of_bounds_err{};
            }
        }
        return *this;
    }
    constexpr safe_iter operator+(difference_type offset) const noexcept(nothrow) {
        auto tmp = *this;
        tmp += offset;
        return tmp;
    }
    friend constexpr safe_iter operator+(difference_type offset, const safe_iter& right) noexcept(nothrow) {
        return right + offset;
    }
    constexpr safe_iter& operator-=(difference_type offset) noexcept(nothrow) {
        return *this += -offset;
    }
    constexpr safe_iter operator-(difference_type offset) const noexcept(nothrow) {
        return *this + (-offset);
    }
    constexpr difference_type operator-(const safe_iter& right) const noexcept {
        return current_ptr - right.current_ptr;
    }

    constexpr safe_iter& operator++() noexcept(nothrow) {
        return *this += 1;
    }
    constexpr safe_iter& operator--() noexcept(nothrow) {
        return *this -= 1;
    }
    constexpr safe_iter operator++(int) noexcept(nothrow) {
        auto temp = *this;
        ++*this;
        return temp;
    }
    constexpr safe_iter operator--(int) noexcept(nothrow) {
        auto temp = *this;
        --*this;
        return temp;
    }

    constexpr bool operator==(const safe_iter& right) const noexcept {
        return current_ptr == right.current_ptr;
    }
    constexpr auto operator<=>(const safe_iter& right) const noexcept {
        return current_ptr <=> right.current_ptr;
    }

    constexpr difference_type operator-(pointer right) const noexcept {
        return current_ptr - right;
    }
    friend constexpr difference_type operator-(pointer left, const safe_iter& right) noexcept {
        return left - right.current_ptr;
    }
    constexpr bool operator==(pointer right) const noexcept {
        return current_ptr == right;
    }
    friend constexpr bool operator==(pointer left, const safe_iter& right) noexcept {
        return left == right.current_ptr;
    }

    static constexpr safe_iter begin(ranges::contiguous_range auto&& rng) noexcept {
        return {to_address(ranges::begin(rng)), to_address(ranges::begin(rng)), to_address(ranges::end(rng))};
    }
    static constexpr safe_iter end(ranges::contiguous_range auto&& rng) noexcept {
        return {to_address(ranges::end(rng)), to_address(ranges::begin(rng)), to_address(ranges::end(rng))};
    }

    static constexpr pair<safe_iter, safe_iter> get_iters(ranges::contiguous_range auto&& rng, range_type type) {
        const auto n = static_cast<underlying_type_t<range_type>>(type);
        assert(n > 2);

        // Avoid real out-of-bounds access (UB)

        const auto range_dist = ranges::distance(rng);
        const auto first_ptr  = to_address(ranges::begin(rng) + range_dist / n);
        const auto last_ptr   = to_address(ranges::begin(rng) + range_dist * 2 / n);

        return {safe_iter{first_ptr, first_ptr, last_ptr}, safe_iter{last_ptr, first_ptr, last_ptr}};
    }

private:
    pointer current_ptr     = nullptr;
    pointer range_first_ptr = nullptr;
    pointer range_last_ptr  = nullptr;
};

template <bool nothrow, class Func>
void single_test_pass(Func&& func) noexcept {
    func();
    if constexpr (nothrow) {
        assert(!safe_iter_nothrow_OOB_sign);
    }
}
template <bool nothrow, class Func>
void single_test_fail(Func&& func) noexcept {
    if constexpr (nothrow) {
        func();
        assert(exchange(safe_iter_nothrow_OOB_sign, false));
    } else {
        try {
            func();
            assert(false && "should throw, but returned");
        } catch (const safe_iter_out_of_bounds_err&) {
        }
    }
}

#define PASS(expr) single_test_pass<nothrow>([&] { (void) (expr); })
#define FAIL(expr) single_test_fail<nothrow>([&] { (void) (expr); })

#define EXPR_WRAP_IS(expr)       ([&](auto&& i, auto&& s) { (void) (expr); })
#define EXPR_WRAP_IN(expr)       ([&](auto&& i, auto&& n) { (void) (expr); })
#define EXPR_WRAP_ISD(expr)      ([&](auto&& i, auto&& s, auto&& dst) { (void) (expr); })
#define EXPR_WRAP_IS_DI_DS(expr) ([&](auto&& i, auto&& s, auto&& dst_i, auto&& dst_s) { (void) (expr); })

template <ranges::contiguous_range ContainerT, ranges::range_size_t<ContainerT> range_size, bool nothrow>
void test() {
    using iter       = safe_iter<ranges::range_value_t<ContainerT>, nothrow>;
    using const_iter = safe_iter<const ranges::range_value_t<ContainerT>, nothrow>;
    static_assert(contiguous_iterator<iter>);
    static_assert(contiguous_iterator<const_iter>);

    // Allow unsafe sentinels
    static_assert(sized_sentinel_for<typename iter::pointer, iter>);
    static_assert(sized_sentinel_for<typename const_iter::pointer, const_iter>);

    // Allows trivial container reuse for uninitialized memory algorithms
    static_assert(is_trivially_destructible_v<iter_value_t<iter>>);

    const auto container_read =
        views::iota(static_cast<iter_value_t<iter>>(0), static_cast<iter_value_t<iter>>(range_size))
        | ranges::to<ContainerT>();
    assert(ranges::size(container_read) == range_size);

    const auto container_read_unique =
        views::iota(static_cast<iter_value_t<iter>>(range_size), static_cast<iter_value_t<iter>>(range_size) * 2)
        | ranges::to<ContainerT>();
    assert(ranges::size(container_read_unique) == range_size);
    assert(ranges::find_first_of(container_read, container_read_unique) == ranges::end(container_read));

    ContainerT container_write   = container_read;
    ContainerT container_write_2 = container_read;
    assert(ranges::size(container_write) == range_size);
    assert(ranges::size(container_write_2) == range_size);

#define GEN_RANGE_ITER(prefix, suffix, type, range, iter_type)                      \
    [[maybe_unused]] const auto [prefix##_first_##suffix, prefix##_last_##suffix] = \
        iter_type::get_iters(range, range_##type);                                  \
    [[maybe_unused]] const auto prefix##_dist_##suffix =                            \
        ranges::distance(prefix##_first_##suffix, prefix##_last_##suffix);
#define GEN_RANGE_ITERS(prefix, range, iter_type)       \
    GEN_RANGE_ITER(prefix, s, small, range, iter_type)  \
    GEN_RANGE_ITER(prefix, m, medium, range, iter_type) \
    GEN_RANGE_ITER(prefix, l, large, range, iter_type)

    // Iterators with the same suffix refer to ranges of the same size
    GEN_RANGE_ITERS(r, container_read, const_iter)
    GEN_RANGE_ITERS(r2, container_read_unique, const_iter)
    GEN_RANGE_ITERS(w, container_write, iter)
    GEN_RANGE_ITERS(w2, container_write_2, iter)

    assert(r_first_s < r_first_m && r_first_m < r_first_l && r_last_s < r_last_m && r_last_m < r_last_l);

#undef GEN_RANGE_ITERS
#undef GEN_RANGE_ITER

    static constexpr auto tests_base = [](auto&& func, auto&& first, auto&& last_or_dist) {
        PASS(func(first, last_or_dist));
        FAIL(func(first - 1, last_or_dist));
        FAIL(func(first, last_or_dist + 1));
    };

    // [i, s)
    {
        static constexpr auto tests_i_s = [](auto&& func, auto&& range_func, auto&& first, auto&& last) {
            tests_base(func, first, last);
            tests_base(range_func, first, last);
            tests_base(range_func, first, to_address(last));
        };

#define EXPR_WRAP_IS_SMART(expr)                     \
    [&](auto&& i, auto&& s) {                        \
        if constexpr (requires { (void) (expr); }) { \
            (void) (expr);                           \
        } else {                                     \
            (void) to_address(i);                    \
            (void) to_address(i + (s - i));          \
        }                                            \
    }
#define TESTR(expr) tests_i_s(EXPR_WRAP_IS_SMART(expr), EXPR_WRAP_IS(ranges::expr), r_first_m, r_last_m)
#define TESTW(expr) tests_i_s(EXPR_WRAP_IS_SMART(expr), EXPR_WRAP_IS(ranges::expr), w_first_m, w_last_m)

        // Tests ranges:: and std:: versions (when exist)

        // Ensures full range traversal (worst-case)
        TESTR(adjacent_find(i, s));
        TESTR(contains(i, s, container_read_unique[0]));
        TESTR(contains_subrange(i, s, r2_first_s, r2_last_s));
        TESTR(contains_subrange(r2_first_l, r2_last_l, i, s));
        TESTR(count(i, s, container_read_unique[0]));
        TESTR(ends_with(i, s, i, s));
        TESTR(equal(i, s, i, s));
        TESTR(find(i, s, container_read_unique[0]));
        TESTR(find_end(i, s, r2_first_s, r2_last_s));
        TESTR(find_end(r2_first_l, r2_last_l, i, s));
        TESTR(find_first_of(i, s, r2_first_s, r2_last_s));
        TESTR(find_first_of(r2_first_l, r2_last_l, i, s));
        TESTR(is_sorted(i, s));
        TESTR(is_sorted_until(i, s));
        TESTR(lexicographical_compare(i, s, i, s));
        TESTR(max(ranges::subrange{i, s}));
        TESTR(max_element(i, s));
        TESTR(min(ranges::subrange{i, s}));
        TESTR(min_element(i, s));
        TESTR(minmax(ranges::subrange{i, s}));
        TESTR(minmax_element(i, s));
        TESTR(mismatch(i, s, i, s));
        TESTR(search(i, s, r2_first_s, r2_last_s));
        TESTR(search(r2_first_l, r2_last_l, i, s));
        TESTR(search_n(i, s, (s - i) / 2, container_read_unique[0]));
        TESTR(starts_with(i, s, i, s));

        TESTW(fill(i, s, container_read[0]));
        TESTW(iota(i, s, container_read[0]));
        TESTW(nth_element(i, i + (s - i) / 2, s));
        TESTW(remove(i, s, container_read[0]));
        TESTW(replace(i, s, container_read[0], container_read[1]));
        TESTW(reverse(i, s));
        TESTW(shift_left(i, s, (s - i) / 2));
        TESTW(shift_right(i, s, (s - i) / 2));
        TESTW(sort(i, s));
        TESTW(stable_sort(i, s));
        TESTW(uninitialized_fill(i, s, container_read[0]));
        TESTW(unique(i, s));

        // [i, s) -> [dst, ...) / [..., dst)
        {
            static constexpr auto tests_i_s_dst = [](auto&& func, auto&& range_func, auto&& dst_small, auto&& dst_big,
                                                      auto&& src_first, auto&& src_last) {
                tests_i_s(
                    EXPR_WRAP_IS(func(i, s, dst_big)), EXPR_WRAP_IS(range_func(i, s, dst_big)), src_first, src_last);
                FAIL(func(src_first, src_last, dst_small));
                FAIL(range_func(src_first, src_last, dst_small));
                FAIL(range_func(src_first, to_address(src_last), dst_small));
            };
            static constexpr auto tests_i_s_di_ds = [](auto&& func, auto&& range_func, auto&& dst_small_first,
                                                        auto&& dst_small_last, auto&& dst_big_first, auto&& src_first,
                                                        auto&& src_last) {
                tests_i_s_dst(func, EXPR_WRAP_ISD(range_func(i, s, dst, unreachable_sentinel)), dst_small_first,
                    dst_big_first, src_first, src_last);

                const auto range_func_with_input_range = EXPR_WRAP_IS(range_func(src_first, src_last, i, s));
                const auto range_func_with_input_range_i_s =
                    EXPR_WRAP_IS(range_func(src_first, to_address(src_last), i, s));

                tests_base(range_func_with_input_range, dst_small_first, dst_small_last);
                tests_base(range_func_with_input_range, dst_small_first, to_address(dst_small_last));
                tests_base(range_func_with_input_range_i_s, dst_small_first, dst_small_last);
                tests_base(range_func_with_input_range_i_s, dst_small_first, to_address(dst_small_last));
            };

#define TEST2W(expr) \
    tests_i_s_dst(EXPR_WRAP_ISD(expr), EXPR_WRAP_ISD(ranges::expr), w_first_s, w_first_l, w2_first_m, w2_last_m)
#define TEST2WB(expr) \
    tests_i_s_dst(EXPR_WRAP_ISD(expr), EXPR_WRAP_ISD(ranges::expr), w_last_s, w_last_l, w2_first_m, w2_last_m)
#define TEST3W(expr, range_expr) \
    tests_i_s_di_ds(             \
        EXPR_WRAP_ISD(expr), EXPR_WRAP_IS_DI_DS(range_expr), w_first_s, w_last_s, w_first_l, w2_first_m, w2_last_m)

            // These algorithms always traverse the entire input range, so we always use the write container.

            TEST2W(copy(i, s, dst));
            TEST2WB(copy_backward(i, s, dst));
            TEST2W(move(i, s, dst));
            TEST2WB(move_backward(i, s, dst));
            TEST2W(replace_copy(i, s, dst, container_read[0], container_read[1]));
            TEST2W(reverse_copy(i, s, dst));
            TEST2W(rotate_copy(i, i + (s - i) / 2, s, dst));

            TEST3W(swap_ranges(i, s, dst), ranges::swap_ranges(i, s, dst_i, dst_s));
            TEST3W(uninitialized_copy(i, s, dst), ranges::uninitialized_copy(i, s, dst_i, dst_s));
            TEST3W(uninitialized_move(i, s, dst), ranges::uninitialized_move(i, s, dst_i, dst_s));

#undef TEST3W
#undef TEST2WB
#undef TEST2W

            tests_base(EXPR_WRAP_IS(adjacent_difference(i, s, w_first_m, minus<iter_value_t<iter>>{})), r_first_m,
                r_last_m); // no ranges:: version
            FAIL(adjacent_difference(r_first_m, r_last_m, w_first_s, minus<iter_value_t<iter>>{}));
        }

#undef TESTW
#undef TESTR
#undef EXPR_WRAP_IS_SMART
    }

    // i + [0, n)
    {
#define TESTR(expr) tests_base(EXPR_WRAP_IN(expr), r_first_m, r_dist_m)
#define TESTW(expr) tests_base(EXPR_WRAP_IN(expr), w_first_s, w_dist_s)

        TESTR(copy_n(i, n, w_first_l));
        TESTR(ranges::copy_n(i, n, w_first_l));

        TESTW(fill_n(i, n, container_read[0]));
        TESTW(ranges::fill_n(i, n, container_read[0]));

        TESTR(uninitialized_copy_n(i, n, w_first_l));
        TESTR(ranges::uninitialized_copy_n(i, n, w_first_l, unreachable_sentinel));
        tests_base(EXPR_WRAP_IS(ranges::uninitialized_copy_n(r_first_m, r_dist_m, i, s)), w_first_s, w_last_s);

        TESTW(uninitialized_move_n(i, n, w2_first_l));
        TESTW(ranges::uninitialized_move_n(i, n, w2_first_l, unreachable_sentinel));
        tests_base(EXPR_WRAP_IS(ranges::uninitialized_move_n(w_first_s, w_dist_s, i, s)), w2_first_s, w2_last_s);

        TESTW(uninitialized_fill_n(i, n, container_read[0]));
        TESTW(ranges::uninitialized_fill_n(i, n, container_read[0]));

#undef TESTW
#undef TESTR
    }
}

template <bool nothrow>
void test_matrix() {
    // Ensures vectorization (and iterator-to-pointer conversion)
    test<vector<int8_t>, 63, nothrow>();
    test<vector<int16_t>, 256, nothrow>();
    test<vector<int32_t>, 256, nothrow>();
    test<vector<int64_t>, 256, nothrow>();

    // remove_copy & unique_copy
    {
        using iter       = safe_iter<int, nothrow>;
        using const_iter = safe_iter<const int, nothrow>;

        const auto src = {1, 2, 3, 4, 4, 5, 5};

        const auto src_first    = const_iter::begin(src);
        const auto src_last     = const_iter::end(src);
        const auto src_last_ptr = to_address(src_last);

        array<int, 5 + 1> dst;
        const iter dst_first{
            to_address(ranges::begin(dst)), to_address(ranges::begin(dst)), to_address(ranges::begin(dst) + 5)};

        PASS(remove_copy(src_first, src_last, dst_first, 5));
        FAIL(remove_copy(src_first, src_last, dst_first, 1));
        PASS(ranges::remove_copy(src_first, src_last_ptr, dst_first, 5));
        FAIL(ranges::remove_copy(src_first, src_last_ptr, dst_first, 1));

        PASS(unique_copy(src_first, src_last, dst_first));
        FAIL(unique_copy(src_first, src_last, dst_first + 1));
        PASS(ranges::unique_copy(src_first, src_last_ptr, dst_first));
        FAIL(ranges::unique_copy(src_first, src_last_ptr, dst_first + 1));
    }

    // format {:s} & {:?s}
    {
        const vector chars = ranges::to<vector<char>>("\"Hello, world!\"");

        const auto [chars_first, chars_last] = safe_iter<const char, nothrow>::get_iters(chars, range_medium);

        const ranges::subrange good_range{chars_first, to_address(chars_last)};
        const ranges::subrange bad_range{chars_first, to_address(chars_last) + 1};
        const ranges::subrange bad_range_2 = [&] {
            if constexpr (nothrow) {
                return ranges::subrange{chars_first - 1, to_address(chars_last)};
            } else {
                return bad_range;
            }
        }();

        static_assert(ranges::contiguous_range<decltype(good_range)>);
        static_assert(ranges::contiguous_range<decltype(bad_range)>);
        static_assert(ranges::contiguous_range<decltype(bad_range_2)>);

        PASS(format("{:s}", good_range));
        FAIL(format("{:s}", bad_range));
        FAIL(format("{:s}", bad_range_2));

        PASS(format("{:?s}", good_range));
        FAIL(format("{:?s}", bad_range));
        FAIL(format("{:?s}", bad_range_2));
    }
}

int main() {
    test_matrix<true>();
    test_matrix<false>();

    return 0;
}
