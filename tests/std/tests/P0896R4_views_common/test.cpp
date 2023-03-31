// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

template <class Rng>
concept CanViewCommon = requires(Rng&& r) { views::common(static_cast<Rng&&>(r)); };

template <class Rng>
concept CanViewAll = requires(Rng&& r) { views::all(static_cast<Rng&&>(r)); };

// Test a silly precomposed range adaptor pipeline
constexpr auto pipeline = views::all | views::common;

// Due to language limitations we cannot declare variables of non-literal type in a branch that is guarded by
// `!is_constant_evaluated()`. But we can call a non-constexpr function that declares those variables.
template <ranges::view V, ranges::input_range R, ranges::input_range E>
void non_literal_parts(R& r, E& expected) {
    using ranges::iterator_t, ranges::begin, ranges::bidirectional_range, ranges::end, ranges::prev;

    const bool is_empty = ranges::empty(expected);

    const same_as<iterator_t<R>> auto first = r.begin();
    if (!is_empty) {
        assert(*first == *begin(expected));
    }

    if constexpr (copyable<V>) {
        auto r2                                  = r;
        const same_as<iterator_t<R>> auto first2 = r2.begin();
        if (!is_empty) {
            assert(*first2 == *first);
        }
    }

    if constexpr (CanBegin<const R&>) {
        const same_as<iterator_t<const R>> auto first3 = as_const(r).begin();
        if (!is_empty) {
            assert(*first3 == *first);
        }
    }

    const same_as<iterator_t<R>> auto last = r.end();
    if constexpr (bidirectional_range<R>) {
        if (!is_empty) {
            assert(*prev(last) == *prev(end(expected)));
        }
    }

    if constexpr (CanEnd<const R&>) {
        const same_as<iterator_t<const R>> auto last2 = as_const(r).end();
        if constexpr (bidirectional_range<const R>) {
            if (!is_empty) {
                assert(*prev(last2) == *prev(end(expected)));
            }
        }
    }

#if _HAS_CXX23
    using ranges::const_iterator_t;

    const same_as<const_iterator_t<R>> auto cfirst = r.cbegin();
    if (!is_empty) {
        assert(*cfirst == *begin(expected));
    }

    if constexpr (copyable<V>) {
        auto r2                                         = r;
        const same_as<const_iterator_t<R>> auto cfirst2 = r2.cbegin();
        if (!is_empty) {
            assert(*cfirst2 == *cfirst);
        }
    }

    if constexpr (CanCBegin<const R&>) {
        const same_as<const_iterator_t<const R>> auto cfirst3 = as_const(r).cbegin();
        if (!is_empty) {
            assert(*cfirst3 == *cfirst);
        }
    }

    const same_as<const_iterator_t<R>> auto clast = r.cend();
    if constexpr (bidirectional_range<R>) {
        if (!is_empty) {
            assert(*prev(clast) == *prev(end(expected)));
        }
    }

    if constexpr (CanCEnd<const R&>) {
        const same_as<const_iterator_t<const R>> auto clast2 = as_const(r).cend();
        if constexpr (bidirectional_range<const R>) {
            if (!is_empty) {
                assert(*prev(clast2) == *prev(end(expected)));
            }
        }
    }
#endif // _HAS_CXX23
}

template <class Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::common_view, ranges::bidirectional_range, ranges::common_range, ranges::contiguous_range,
        ranges::enable_borrowed_range, ranges::forward_range, ranges::input_range, ranges::iterator_t, ranges::prev,
        ranges::random_access_range, ranges::range, ranges::range_reference_t, ranges::size, ranges::sized_range,
        ranges::range_size_t, ranges::borrowed_range;

    constexpr bool is_view   = ranges::view<remove_cvref_t<Rng>>;
    using V                  = views::all_t<Rng>;
    constexpr bool is_common = common_range<V>;

    // Validate range adaptor object
    if constexpr (!is_common) { // range adaptor results in common_view
        using R = common_view<V>;
        static_assert(ranges::view<R>);

        // ...with lvalue argument
        static_assert(CanViewCommon<Rng&> == (!is_view && copyable<V>) );
        if constexpr (CanViewCommon<Rng&>) {
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::common(rng)), R>);
            static_assert(noexcept(views::common(rng)) == is_noexcept);

            static_assert(same_as<decltype(rng | views::common), R>);
            static_assert(noexcept(rng | views::common) == is_noexcept);
        }

        // ... with const lvalue argument
        static_assert(CanViewCommon<const remove_reference_t<Rng>&> == (!is_view || copyable<V>) );
        if constexpr (is_view && copyable<V>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::common(as_const(rng))), R>);
            static_assert(noexcept(views::common(as_const(rng))) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::common), R>);
            static_assert(noexcept(as_const(rng) | views::common) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::common | views::common | views::common), V>);
            static_assert(noexcept(as_const(rng) | views::common | views::common | views::common) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | pipeline), V>);
            static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
        } else if constexpr (!is_view) {
            using RC                   = common_view<views::all_t<const remove_reference_t<Rng>&>>;
            constexpr bool is_noexcept = is_nothrow_constructible_v<RC, const remove_reference_t<Rng>&>;

            static_assert(same_as<decltype(views::common(as_const(rng))), RC>);
            static_assert(noexcept(views::common(as_const(rng))) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::common), RC>);
            static_assert(noexcept(as_const(rng) | views::common) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::common | views::common | views::common), RC>);
            static_assert(noexcept(as_const(rng) | views::common | views::common | views::common) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | pipeline), RC>);
            static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
        }

        // ... with rvalue argument
        static_assert(CanViewCommon<remove_reference_t<Rng>> == is_view || enable_borrowed_range<remove_cvref_t<Rng>>);
        if constexpr (is_view) {
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
            static_assert(same_as<decltype(views::common(move(rng))), R>);
            static_assert(noexcept(views::common(move(rng))) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::common), R>);
            static_assert(noexcept(move(rng) | views::common) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::common | views::common | views::common), R>);
            static_assert(noexcept(move(rng) | views::common | views::common | views::common) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | pipeline), R>);
            static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
        } else if constexpr (enable_borrowed_range<remove_cvref_t<Rng>>) {
            using S                    = decltype(ranges::subrange{move(rng)});
            using RS                   = common_view<S>;
            constexpr bool is_noexcept = noexcept(S{move(rng)});

            static_assert(same_as<decltype(views::common(move(rng))), RS>);
            static_assert(noexcept(views::common(move(rng))) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::common), RS>);
            static_assert(noexcept(move(rng) | views::common) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::common | views::common | views::common), RS>);
            static_assert(noexcept(move(rng) | views::common | views::common | views::common) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | pipeline), RS>);
            static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
        }

        // ... with const rvalue argument
        static_assert(CanViewCommon<const remove_reference_t<Rng>> == (is_view && copyable<V>)
                      || (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) );
        if constexpr (is_view && copyable<V>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::common(move(as_const(rng)))), R>);
            static_assert(noexcept(views::common(move(as_const(rng)))) == is_nothrow_copy_constructible_v<R>);

            static_assert(same_as<decltype(move(as_const(rng)) | views::common), R>);
            static_assert(noexcept(move(as_const(rng)) | views::common) == is_nothrow_copy_constructible_v<R>);

            static_assert(same_as<decltype(move(as_const(rng)) | views::common | views::common | views::common), R>);
            static_assert(noexcept(move(as_const(rng)) | views::common | views::common | views::common) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | pipeline), R>);
            static_assert(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
        } else if constexpr (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) {
            using S                    = decltype(ranges::subrange{move(as_const(rng))});
            using RS                   = common_view<S>;
            constexpr bool is_noexcept = noexcept(S{move(as_const(rng))});

            static_assert(same_as<decltype(views::common(move(as_const(rng)))), RS>);
            static_assert(noexcept(views::common(move(as_const(rng)))) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | views::common), RS>);
            static_assert(noexcept(move(as_const(rng)) | views::common) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | views::common | views::common | views::common), RS>);
            static_assert(noexcept(move(as_const(rng)) | views::common | views::common | views::common) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | pipeline), RS>);
            static_assert(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
        }
    } else { // range adaptor results in views::all_t
        // ...with lvalue argument
        {
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::common(rng)), V>);
            static_assert(noexcept(views::common(rng)) == is_noexcept);

            static_assert(same_as<decltype(rng | views::common), V>);
            static_assert(noexcept(rng | views::common) == is_noexcept);
        }

        // ... with const lvalue argument
        if constexpr (is_view && copyable<V>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::common(as_const(rng))), V>);
            static_assert(noexcept(views::common(as_const(rng))) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::common), V>);
            static_assert(noexcept(as_const(rng) | views::common) == is_noexcept);
        } else if constexpr (!is_view) {
            using RC                   = views::all_t<const remove_reference_t<Rng>&>;
            constexpr bool is_noexcept = is_nothrow_constructible_v<RC, const remove_reference_t<Rng>&>;

            static_assert(same_as<decltype(views::common(as_const(rng))), RC>);
            static_assert(noexcept(views::common(as_const(rng))) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::common), RC>);
            static_assert(noexcept(as_const(rng) | views::common) == is_noexcept);
        }
        // ... with rvalue argument
        static_assert(CanViewCommon<remove_reference_t<Rng>> == is_view || enable_borrowed_range<remove_cvref_t<Rng>>);
        if constexpr (is_view) {
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
            static_assert(same_as<decltype(views::common(move(rng))), V>);
            static_assert(noexcept(views::common(move(rng))) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::common), V>);
            static_assert(noexcept(move(rng) | views::common) == is_noexcept);
        } else if constexpr (enable_borrowed_range<remove_cvref_t<Rng>>) {
            using S                    = decltype(ranges::subrange{move(rng)});
            constexpr bool is_noexcept = noexcept(S{move(rng)});

            static_assert(same_as<decltype(views::common(move(rng))), V>);
            static_assert(noexcept(views::common(move(rng))) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::common), V>);
            static_assert(noexcept(move(rng) | views::common) == is_noexcept);
        }

        // ... with const rvalue argument
        static_assert(CanViewCommon<const remove_reference_t<Rng>> == (is_view && copyable<V>)
                      || (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) );
        if constexpr (is_view && copyable<V>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::common(move(as_const(rng)))), V>);
            static_assert(noexcept(views::common(move(as_const(rng)))) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | views::common), V>);
            static_assert(noexcept(move(as_const(rng)) | views::common) == is_noexcept);
        } else if constexpr (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) {
            using S                    = decltype(ranges::subrange{move(as_const(rng))});
            constexpr bool is_noexcept = noexcept(S{move(as_const(rng))});

            static_assert(same_as<decltype(views::common(move(as_const(rng)))), V>);
            static_assert(noexcept(views::common(move(as_const(rng)))) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | views::common), V>);
            static_assert(noexcept(move(as_const(rng)) | views::common) == is_noexcept);
        }
    }

    if constexpr (!is_common) {
        // Validate deduction guide
        using R           = common_view<V>;
        same_as<R> auto r = common_view{forward<Rng>(rng)};
        if (!is_constant_evaluated()) {
            assert(ranges::equal(r, expected));
        }

        // Validate common_view::size
        static_assert(CanMemberSize<R> == sized_range<Rng>);
        if constexpr (sized_range<Rng>) {
            assert(r.size() == static_cast<range_size_t<R>>(size(expected)));
            static_assert(noexcept(r.size()) == noexcept(size(rng)));
        }

        static_assert(CanMemberSize<const R> == sized_range<const Rng>);
        if constexpr (sized_range<const Rng>) {
            assert(as_const(r).size() == static_cast<range_size_t<R>>(size(expected)));
            static_assert(noexcept(as_const(r).size()) == noexcept(size(as_const(rng))));
        }

        // Validate view_interface::empty and operator bool
        const bool is_empty = ranges::empty(expected);
        if (!is_constant_evaluated()) {
            if constexpr (CanMemberEmpty<R>) {
                assert(r.empty() == is_empty);
                assert(static_cast<bool>(r) == !is_empty);
            }

            if constexpr (CanMemberEmpty<const R>) {
                assert(as_const(r).empty() == is_empty);
                assert(static_cast<bool>(as_const(r)) == !is_empty);
            }
        }

        // Validate common_view::begin and common_view::end
        STATIC_ASSERT(CanMemberBegin<R>);
        STATIC_ASSERT(CanBegin<const R&> == range<const V>);
        STATIC_ASSERT(CanMemberEnd<R>);
        STATIC_ASSERT(CanEnd<const R&> == range<const V>);
        if (!is_constant_evaluated()) {
            non_literal_parts<V>(r, expected);
        }

        // Validate view_interface::data
        static_assert(CanMemberData<R> == (contiguous_range<V> && sized_range<V>) );
        static_assert(contiguous_range<R> == CanMemberData<R>);
        if constexpr (CanMemberData<R>) {
            assert(r.data() == &*r.begin());
        }
        static_assert(CanMemberData<const R> == (contiguous_range<const V> && sized_range<const V>) );
        static_assert(contiguous_range<const R> == CanMemberData<const R>);
        if constexpr (CanMemberData<const R>) {
            assert(as_const(r).data() == &*as_const(r).begin());
        }

        if (!is_constant_evaluated() && !is_empty) {
            // Validate view_interface::operator[]
            if constexpr (CanIndex<R>) {
                assert(r[0] == *begin(expected));
            }

            if constexpr (CanIndex<const R>) {
                assert(as_const(r)[0] == *begin(expected));
            }

            // Validate view_interface::front and back
            if constexpr (CanMemberFront<R>) {
                assert(r.front() == *begin(expected));
            }

            if constexpr (CanMemberFront<const R>) {
                assert(as_const(r).front() == *begin(expected));
            }

            if constexpr (CanMemberBack<R>) {
                assert(r.back() == *prev(end(expected)));
            }

            if constexpr (CanMemberBack<const R>) {
                assert(as_const(r).back() == *prev(end(expected)));
            }
        }

        // Validate common_view::base() const&
        static_assert(CanMemberBase<const R&> == copy_constructible<V>);
        if constexpr (copy_constructible<V>) {
            same_as<V> auto b1 = as_const(r).base();
            static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
            if (!is_empty) {
                assert(*b1.begin() == *begin(expected));
            }
        }

        // Validate common_view::base() && (NB: do this last since it leaves r moved-from)
        same_as<V> auto b2 = move(r).base();
        static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if (!is_empty) {
            assert(*b2.begin() == *begin(expected));
        }

        // Validate borrowed_range
        static_assert(borrowed_range<R> == borrowed_range<V>);
    }

    return true;
}

struct instantiator {
    static constexpr int some_ints[] = {0, 1, 2};

    template <ranges::input_range R>
    static constexpr void call() {
        if constexpr (copyable<ranges::iterator_t<R>>) {
            R r{some_ints};
            test_one(r, span<const int, 3>{some_ints});
        }
    }
};

template <class T>
struct difference_type_only_iterator {
    static_assert(is_object_v<T>);

    friend constexpr bool operator==(difference_type_only_iterator, difference_type_only_iterator)  = default;
    friend constexpr auto operator<=>(difference_type_only_iterator, difference_type_only_iterator) = default;

    using iterator_concept = contiguous_iterator_tag;
    using value_type       = remove_cvref_t<T>;

    constexpr T& operator*() const noexcept {
        return *ptr_;
    }

    constexpr T* operator->() const noexcept {
        return ptr_;
    }

    constexpr difference_type_only_iterator& operator++() noexcept {
        ++ptr_;
        return *this;
    }

    constexpr difference_type_only_iterator operator++(int) noexcept {
        auto result = *this;
        ++*this;
        return result;
    }

    constexpr difference_type_only_iterator& operator--() noexcept {
        --ptr_;
        return *this;
    }

    constexpr difference_type_only_iterator operator--(int) noexcept {
        auto result = *this;
        --*this;
        return result;
    }

    constexpr difference_type_only_iterator& operator+=(same_as<ptrdiff_t> auto n) noexcept {
        ptr_ += n;
        return *this;
    }

    constexpr difference_type_only_iterator& operator-=(same_as<ptrdiff_t> auto n) noexcept {
        ptr_ -= n;
        return *this;
    }

    friend constexpr difference_type_only_iterator operator+(
        difference_type_only_iterator i, same_as<ptrdiff_t> auto n) noexcept {
        i += n;
        return i;
    }

    friend constexpr difference_type_only_iterator operator+(
        same_as<ptrdiff_t> auto n, difference_type_only_iterator i) noexcept {
        i += n;
        return i;
    }

    friend constexpr difference_type_only_iterator operator-(
        difference_type_only_iterator i, same_as<ptrdiff_t> auto n) noexcept {
        i -= n;
        return i;
    }

    friend constexpr ptrdiff_t operator-(difference_type_only_iterator i, difference_type_only_iterator j) noexcept {
        return i.ptr_ - j.ptr_;
    }

    constexpr T& operator[](same_as<ptrdiff_t> auto n) const noexcept {
        return ptr_[n];
    }

    T* ptr_;
};

template <class T>
struct difference_type_only_sentinel {
    static_assert(is_object_v<T>);

    friend constexpr bool operator==(difference_type_only_iterator<T> i, difference_type_only_sentinel s) noexcept {
        return i.ptr_ == s.ptr_end_;
    }

    friend constexpr ptrdiff_t operator-(difference_type_only_iterator<T> i, difference_type_only_sentinel s) noexcept {
        return i.ptr_ - s.ptr_end_;
    }

    friend constexpr ptrdiff_t operator-(difference_type_only_sentinel s, difference_type_only_iterator<T> i) noexcept {
        return s.ptr_end_ - i.ptr_;
    }

    T* ptr_end_;
};

template <class T>
constexpr bool test_lwg3717() {
    remove_cv_t<T> x{};

    auto cmv_sr = ranges::subrange(difference_type_only_iterator<T>{&x}, difference_type_only_sentinel<T>{&x + 1})
                | views::common;

    static_assert(ranges::contiguous_range<decltype(cmv_sr)>);
    static_assert(ranges::contiguous_range<const decltype(cmv_sr)>);

    assert(ranges::end(cmv_sr) == ranges::begin(cmv_sr) + ptrdiff_t{1});
    assert(ranges::end(as_const(cmv_sr)) == ranges::begin(as_const(cmv_sr)) + ptrdiff_t{1});

    return true;
}

int main() {
    // Get full instantiation coverage
    static_assert((test_in<instantiator, const int>(), true));
    test_in<instantiator, const int>();

    static_assert(test_lwg3717<int>());
    static_assert(test_lwg3717<const int>());

    assert(test_lwg3717<int>());
    assert(test_lwg3717<const int>());
}
