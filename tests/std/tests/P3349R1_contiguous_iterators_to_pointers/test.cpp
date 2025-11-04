// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

struct safe_iter_out_of_bounds_err {};

enum class range_type { range_large = 5, range_medium = 4, range_small = 3 };

bool safe_iter_nothrow_OOB_sign = false;

template <class T>
constexpr T* unwrap(T* ptr) {
    return ptr;
}

template <class T, bool Nothrow>
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
        assert(current_ptr >= range_first_ptr);
        assert(current_ptr <= range_last_ptr);
    }
    constexpr safe_iter() noexcept = default;

    constexpr reference operator*() const noexcept(Nothrow) {
        if (current_ptr < range_first_ptr || current_ptr >= range_last_ptr) {
            if constexpr (Nothrow) {
                safe_iter_nothrow_OOB_sign = true;
            } else {
                throw safe_iter_out_of_bounds_err{};
            }
        }
        return *current_ptr;
    }
    constexpr pointer operator->() const noexcept { // must always be noexcept, see N5014 [pointer.conversion]/3
        if constexpr (Nothrow) {
            if (current_ptr < range_first_ptr || current_ptr > range_last_ptr) {
                safe_iter_nothrow_OOB_sign = true;
            }
        }
        return current_ptr;
    }
    constexpr reference operator[](difference_type n) const noexcept(Nothrow) {
        return *(*this + n);
    }

    // When `Nothrow` is true, disables bounds checking during iterator increment/decrement
    // (to verify `to_address` calls), logging out-of-bounds accesses instead of throwing.
    // When `Nothrow` is false, throws immediately on bounds violation.
    constexpr safe_iter& operator+=(difference_type offset) noexcept(Nothrow) {
        current_ptr += offset;
        if constexpr (!Nothrow) {
            if (current_ptr < range_first_ptr || current_ptr > range_last_ptr) {
                throw safe_iter_out_of_bounds_err{};
            }
        }
        return *this;
    }
    constexpr safe_iter operator+(difference_type offset) const noexcept(Nothrow) {
        auto tmp = *this;
        tmp += offset;
        return tmp;
    }
    friend constexpr safe_iter operator+(difference_type offset, const safe_iter& right) noexcept(Nothrow) {
        return right + offset;
    }
    constexpr safe_iter& operator-=(difference_type offset) noexcept(Nothrow) {
        return *this += -offset;
    }
    constexpr safe_iter operator-(difference_type offset) const noexcept(Nothrow) {
        return *this + (-offset);
    }
    constexpr difference_type operator-(const safe_iter& right) const noexcept {
        return current_ptr - right.current_ptr;
    }

    constexpr safe_iter& operator++() noexcept(Nothrow) {
        return *this += 1;
    }
    constexpr safe_iter& operator--() noexcept(Nothrow) {
        return *this -= 1;
    }
    constexpr safe_iter operator++(int) noexcept(Nothrow) {
        auto temp = *this;
        ++*this;
        return temp;
    }
    constexpr safe_iter operator--(int) noexcept(Nothrow) {
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
        const auto last_ptr   = to_address(ranges::begin(rng) + range_dist * (n - 1) / n);

        return {safe_iter{first_ptr, first_ptr, last_ptr}, safe_iter{last_ptr, first_ptr, last_ptr}};
    }

    friend constexpr T* unwrap(const safe_iter& iter) {
        return iter.current_ptr;
    }

private:
    pointer current_ptr     = nullptr;
    pointer range_first_ptr = nullptr;
    pointer range_last_ptr  = nullptr;
};

template <bool Nothrow, class Func>
void single_test_pass(Func&& func) noexcept {
    func();
    if constexpr (Nothrow) {
        assert(!safe_iter_nothrow_OOB_sign);
    }
}
template <bool Nothrow, class Func>
void single_test_fail(Func&& func) noexcept {
    if constexpr (Nothrow) {
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

#define PASS(expr) single_test_pass<Nothrow>([&]() { (void) (expr); })
#define FAIL(expr) single_test_fail<Nothrow>([&]() { (void) (expr); })

#define EXPR_WRAP_IS(expr)       ([&](auto&& i, auto&& s) { (void) (expr); })
#define EXPR_WRAP_IN(expr)       ([&](auto&& i, auto&& n) { (void) (expr); })
#define EXPR_WRAP_ISD(expr)      ([&](auto&& i, auto&& s, auto&& dst) { (void) (expr); })
#define EXPR_WRAP_IS_DI_DS(expr) ([&](auto&& i, auto&& s, auto&& dst_i, auto&& dst_s) { (void) (expr); })

template <class T, size_t RangeSize, bool Nothrow>
void test() {
    using iter       = safe_iter<T, Nothrow>;
    using const_iter = safe_iter<const T, Nothrow>;
    static_assert(contiguous_iterator<iter>);
    static_assert(contiguous_iterator<const_iter>);

    // Allow unsafe sentinels
    static_assert(sized_sentinel_for<typename iter::pointer, iter>);
    static_assert(sized_sentinel_for<typename const_iter::pointer, const_iter>);

    // Allows trivial container reuse for uninitialized memory algorithms
    static_assert(is_trivially_destructible_v<T>);

    vector<T> container_read(RangeSize);
    iota(container_read.begin(), container_read.end(), static_cast<T>(0));

    vector<T> container_read_unique(RangeSize);
    iota(container_read_unique.begin(), container_read_unique.end(), static_cast<T>(RangeSize));
    assert(ranges::find_first_of(container_read, container_read_unique) == ranges::end(container_read));

    auto container_write   = container_read;
    auto container_write_2 = container_read;

#define GEN_RANGE_ITER(prefix, suffix, type, range, iter_type)                      \
    [[maybe_unused]] const auto [prefix##_first_##suffix, prefix##_last_##suffix] = \
        iter_type::get_iters(range, range_type::range_##type);                      \
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

    // l m s  s m l
    // [ ( <  > ) ]
    assert(r_first_l < r_first_m);
    assert(r_first_m < r_first_s);
    assert(r_last_s < r_last_m);
    assert(r_last_m < r_last_l);

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

        // Tests ranges:: and std:: versions (when they exist)

        // Ensures full range traversal (worst-case)
        TESTR(adjacent_find(i, s));
        TESTR(count(i, s, container_read_unique[0]));
        TESTR(equal(i, s, unwrap(i), unwrap(s)));
        TESTR(equal(unwrap(i), unwrap(s), i, s));
        TESTR(find(i, s, container_read_unique[0]));
        TESTR(find_end(i, s, unwrap(i), unwrap(s)));
        TESTR(find_end(r_first_l, r_last_l, i, s));
        TESTR(find_first_of(i, s, r2_first_s, r2_last_s));
        TESTR(find_first_of(r2_first_l, r2_last_l, i, s));
        TESTR(includes(i, s, r2_first_s, r2_last_s));
        TESTR(includes(r_first_l, r_last_l, i, s));
        TESTR(is_sorted(i, s));
        TESTR(is_sorted_until(i, s));
        TESTR(lexicographical_compare(i, s, unwrap(i), unwrap(s)));
        TESTR(lexicographical_compare(unwrap(i), unwrap(s), i, s));
        TESTR(max(ranges::subrange{i, s}));
        TESTR(max_element(i, s));
        TESTR(min(ranges::subrange{i, s}));
        TESTR(min_element(i, s));
        TESTR(minmax(ranges::subrange{i, s}));
        TESTR(minmax_element(i, s));
        TESTR(mismatch(i, s, unwrap(i), unwrap(s)));
        TESTR(mismatch(unwrap(i), unwrap(s), i, s));
        TESTR(search(i, s, unwrap(i), unwrap(s)));
        TESTR(search(r_first_l, r_last_l, i, s));

        TESTW(fill(i, s, container_read[0]));
        TESTW(nth_element(i, i + (s - i) / 2, s));
        TESTW(remove(i, s, container_read[0]));
        TESTW(replace(i, s, container_read[0], container_read[1]));
        TESTW(reverse(i, s));
        TESTW(sort(i, s));
        TESTW(stable_sort(i, s));
        TESTW(uninitialized_fill(i, s, container_read[0]));
        TESTW(unique(i, s));

#if _HAS_CXX23
        TESTR(contains(i, s, container_read_unique[0]));
        TESTR(contains_subrange(i, s, unwrap(i), unwrap(s)));
        TESTR(contains_subrange(r_first_l, r_last_l, i, s));
        TESTR(ends_with(i, s, unwrap(i), unwrap(s)));
        TESTR(ends_with(unwrap(i), unwrap(s), i, s));
        TESTR(starts_with(i, s, unwrap(i), unwrap(s)));
        TESTR(starts_with(unwrap(i), unwrap(s), i, s));

        TESTW(iota(i, s, container_read[0]));
        TESTW(shift_left(i, s, (s - i) / 2));
        TESTW(shift_right(i, s, (s - i) / 2));
#endif // _HAS_CXX23

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

            tests_base(EXPR_WRAP_IS(adjacent_difference(i, s, w_first_m, minus<T>{})), r_first_m,
                r_last_m); // no ranges:: version
            FAIL(adjacent_difference(r_first_m, r_last_m, w_first_s, minus<T>{}));
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

template <bool Nothrow>
void test_matrix() {
    // Ensures vectorization (and iterator-to-pointer conversion)
    test<int8_t, 63, Nothrow>();
    test<int16_t, 256, Nothrow>();
    test<int32_t, 256, Nothrow>();
    test<int64_t, 256, Nothrow>();

    // remove_copy and unique_copy
    {
        using iter       = safe_iter<int, Nothrow>;
        using const_iter = safe_iter<const int, Nothrow>;

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

    // search_n
    {
        using const_iter = safe_iter<const int, Nothrow>;

        const auto rng             = {1, 1, 2, 3, 3, 3};
        const auto valid_first_ptr = rng.begin() + 1;
        const auto valid_last_ptr  = rng.end() - 1;

        const const_iter rng_first{valid_first_ptr, valid_first_ptr, valid_last_ptr};
        const const_iter rng_last{valid_last_ptr, valid_first_ptr, valid_last_ptr};
        const auto rng_last_ptr = to_address(rng_last);

        PASS(search_n(rng_first, rng_last, 2, 3));
        FAIL(search_n(rng_first, rng_last + 1, 3, 3));
        FAIL(search_n(rng_first - 1, rng_last, 2, 1));

        PASS(ranges::search_n(rng_first, rng_last_ptr, 2, 3));
        FAIL(ranges::search_n(rng_first, rng_last_ptr + 1, 3, 3));
        FAIL(ranges::search_n(rng_first - 1, rng_last_ptr, 2, 1));
    }

#if _HAS_CXX23
    // format {:s} and {:?s}
    {
        const vector chars = ranges::to<vector<char>>(R"("Hello, world!")");

        const auto [chars_first, chars_last] =
            safe_iter<const char, Nothrow>::get_iters(chars, range_type::range_medium);
        const auto last_ptr = to_address(chars_last);

        const ranges::subrange good_range{chars_first, last_ptr};
        const ranges::subrange bad_range{chars_first, last_ptr + 1};
        const ranges::subrange bad_range_2 = [&] {
            if constexpr (Nothrow) {
                return ranges::subrange{chars_first - 1, last_ptr};
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
#endif // _HAS_CXX23
}

int main() {
    test_matrix<true>();
    test_matrix<false>();

    return 0;
}
