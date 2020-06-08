// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

namespace ranges = std::ranges;

template <class>
inline constexpr bool always_false = false;

namespace detail {
    static constexpr bool permissive() {
        return false;
    }

    template <class>
    struct DependentBase {
        static constexpr bool permissive() {
            return true;
        }
    };

    template <class T>
    struct Derived : DependentBase<T> {
        static constexpr bool test() {
            return permissive();
        }
    };
} // namespace detail
constexpr bool is_permissive = detail::Derived<int>::test();

template <bool>
struct borrowed { // borrowed<true> is a borrowed_range; borrowed<false> is not
    int* begin() const;
    int* end() const;
};

template <>
inline constexpr bool ranges::enable_borrowed_range<borrowed<true>> = true;

struct boolish {
    bool value_ = true;

    constexpr operator bool() const noexcept {
        return value_;
    }

    [[nodiscard]] constexpr boolish operator!() const noexcept {
        return {!value_};
    }
};

namespace test {
    using std::assignable_from, std::conditional_t, std::copy_constructible, std::derived_from, std::exchange,
        std::ptrdiff_t, std::span;

    using output     = std::output_iterator_tag;
    using input      = std::input_iterator_tag;
    using fwd        = std::forward_iterator_tag;
    using bidi       = std::bidirectional_iterator_tag;
    using random     = std::random_access_iterator_tag;
    using contiguous = std::contiguous_iterator_tag;

    template <class T>
    void operator&(T&&) {
        STATIC_ASSERT(always_false<T>);
    }

    template <class T, class U>
    void operator,(T&&, U&&) {
        STATIC_ASSERT(always_false<T>);
    }

    template <class Element, bool Wrapped = true>
    class sentinel {
        Element* ptr_ = nullptr;

    public:
        sentinel() = default;
        constexpr explicit sentinel(Element* ptr) noexcept : ptr_{ptr} {}

        [[nodiscard]] constexpr Element* base() const noexcept {
            return ptr_;
        }

        using _Prevent_inheriting_unwrap = sentinel;

        using unwrap = sentinel<Element, false>;

        [[nodiscard]] constexpr auto _Unwrapped() const noexcept requires Wrapped {
            return unwrap{ptr_};
        }

        static constexpr bool _Unwrap_when_unverified = true;

        constexpr void _Seek_to(unwrap const& s) noexcept requires Wrapped {
            ptr_ = s.base();
        }
    };

    template <class Category, class Element>
    class proxy_reference {
        Element& ref_;

        using ValueType = std::remove_cv_t<Element>;

    public:
        constexpr explicit proxy_reference(Element& ref) : ref_{ref} {}
        proxy_reference(proxy_reference const&) = default;

        constexpr proxy_reference const& operator=(proxy_reference const& that) const
            requires assignable_from<Element&, Element&> {
            ref_ = that.ref_;
            return *this;
        }

        // clang-format off
        constexpr operator ValueType() const requires derived_from<Category, input> && copy_constructible<ValueType> {
            return ref_;
        }
        // clang-format on

        constexpr void operator=(ValueType const& val) const requires assignable_from<Element&, ValueType const&> {
            ref_ = val;
        }

        constexpr Element& peek() const noexcept {
            return ref_;
        }
    };

    // clang-format off
    template <class Cat1, class Elem1, class Cat2, class Elem2>
    constexpr boolish operator==(proxy_reference<Cat1, Elem1> x, proxy_reference<Cat2, Elem2> y) requires requires {
        { x.peek() == y.peek() } -> std::convertible_to<bool>;
    } {
        return {x.peek() == y.peek()};
    }
    template <class Cat1, class Elem1, class Cat2, class Elem2>
    constexpr boolish operator!=(proxy_reference<Cat1, Elem1> x, proxy_reference<Cat2, Elem2> y) requires requires {
        { x.peek() == y.peek() } -> std::convertible_to<bool>;
    } {
        return !(x == y);
    }

    template <class Category, class Element,
        // Model sized_sentinel_for along with sentinel?
        bool Sized = derived_from<Category, random>,
        // Model sentinel_for with self (and sized_sentinel_for if Sized; implies copyable)?
        bool Common = derived_from<Category, fwd>,
        // Use a proxy reference type (instead of Element&)?
        bool Proxy = !derived_from<Category, contiguous>,
        // Interact with the STL's iterator unwrapping machinery?
        bool Wrapped = true>
        requires (Common || !derived_from<Category, fwd>)
            && (!Proxy || !derived_from<Category, contiguous>)
    class iterator {
        Element* ptr_;

        template <class T>
        static constexpr bool at_least = derived_from<Category, T>;

        using ReferenceType = conditional_t<Proxy, proxy_reference<Category, Element>, Element&>;

    public:
        // output iterator operations
        iterator() = default;

        constexpr explicit iterator(Element* ptr) noexcept : ptr_{ptr} {}

        constexpr iterator(iterator&& that) noexcept : ptr_{exchange(that.ptr_, nullptr)} {}
        constexpr iterator& operator=(iterator&& that) noexcept {
            ptr_ = exchange(that.ptr_, nullptr);
            return *this;
        }

        [[nodiscard]] constexpr Element* base() const& noexcept requires Common {
            return ptr_;
        }
        [[nodiscard]] constexpr Element* base() && noexcept {
            return exchange(ptr_, nullptr);
        }

        [[nodiscard]] constexpr ReferenceType operator*() const noexcept {
            return ReferenceType{*ptr_};
        }

        [[nodiscard]] constexpr boolish operator==(sentinel<Element, Wrapped> const& s) const noexcept {
            return boolish{ptr_ == s.base()};
        }
        [[nodiscard]] friend constexpr boolish operator==(
            sentinel<Element, Wrapped> const& s, iterator const& i) noexcept {
            return i == s;
        }
        [[nodiscard]] constexpr boolish operator!=(sentinel<Element, Wrapped> const& s) const noexcept {
            return !(*this == s);
        }
        [[nodiscard]] friend constexpr boolish operator!=(
            sentinel<Element, Wrapped> const& s, iterator const& i) noexcept {
            return !(i == s);
        }

        constexpr iterator& operator++() & noexcept {
            ++ptr_;
            return *this;
        }
        constexpr iterator operator++(int) & noexcept {
            auto tmp = *this;
            ++ptr_;
            return tmp;
        }

        auto operator--() & {
            STATIC_ASSERT(always_false<Category>);
        }
        auto operator--(int) & {
            STATIC_ASSERT(always_false<Category>);
        }

        friend void iter_swap(iterator const&, iterator const&) {
            STATIC_ASSERT(always_false<Category>);
        }

        void operator<(iterator const&) const {
            STATIC_ASSERT(always_false<Category>);
        }
        void operator>(iterator const&) const {
            STATIC_ASSERT(always_false<Category>);
        }
        void operator<=(iterator const&) const {
            STATIC_ASSERT(always_false<Category>);
        }
        void operator>=(iterator const&) const {
            STATIC_ASSERT(always_false<Category>);
        }

        // input iterator operations:
        constexpr void operator++(int) & noexcept requires std::is_same_v<Category, input> {
            ++ptr_;
        }

        [[nodiscard]] constexpr friend std::remove_cv_t<Element> iter_move(iterator const& i) requires at_least<input> {
            return ranges::iter_move(i.ptr_);
        }

        constexpr friend void iter_swap(iterator const& x, iterator const& y)
            requires at_least<input> {
            ranges::iter_swap(x.ptr_, y.ptr_);
        }

        // sentinel operations (implied by forward iterator):
        iterator(iterator const&) requires Common = default;
        iterator& operator=(iterator const&) requires Common = default;
        [[nodiscard]] constexpr boolish operator==(iterator const& that) const noexcept requires Common {
            return {ptr_ == that.ptr_};
        }
        [[nodiscard]] constexpr boolish operator!=(iterator const& that) const noexcept requires Common {
            return !(*this == that);
        }

        // bidi iterator operations:
        constexpr iterator& operator--() & noexcept requires at_least<bidi> {
            --ptr_;
            return *this;
        }
        constexpr iterator operator--(int) & noexcept requires at_least<bidi> {
            auto tmp = *this;
            --ptr_;
            return tmp;
        }

        // random-access iterator operations:
        [[nodiscard]] constexpr boolish operator<(iterator const& that) const noexcept
            requires at_least<random> {
            return {ptr_ < that.ptr_};
        }
        [[nodiscard]] constexpr boolish operator>(iterator const& that) const noexcept
            requires at_least<random> {
            return that < *this;
        }
        [[nodiscard]] constexpr boolish operator<=(iterator const& that) const noexcept
            requires at_least<random> {
            return !(that < *this);
        }
        [[nodiscard]] constexpr boolish operator>=(iterator const& that) const noexcept
            requires at_least<random> {
            return !(*this < that);
        }
        [[nodiscard]] constexpr ReferenceType operator[](ptrdiff_t const n) const& noexcept
            requires at_least<random> {
            return ReferenceType{ptr_[n]};
        }
        constexpr iterator& operator+=(ptrdiff_t const n) & noexcept
            requires at_least<random> {
            ptr_ += n;
            return *this;
        }
        constexpr iterator& operator-=(ptrdiff_t const n) & noexcept
            requires at_least<random> {
            ptr_ -= n;
            return *this;
        }
        [[nodiscard]] constexpr iterator operator+(ptrdiff_t const n) const noexcept
            requires at_least<random> {
            return iterator{ptr_ + n};
        }
        [[nodiscard]] friend constexpr iterator operator+(ptrdiff_t const n, iterator const& i) noexcept
            requires at_least<random> {
            return i + n;
        }
        [[nodiscard]] constexpr iterator operator-(ptrdiff_t const n) const noexcept
            requires at_least<random> {
            return iterator{ptr_ - n};
        }

        // contiguous iterator operations:
        [[nodiscard]] constexpr Element* operator->() const noexcept requires at_least<contiguous> {
            return ptr_;
        }

        // sized_sentinel_for operations:
        [[nodiscard]] constexpr ptrdiff_t operator-(iterator const& that) const noexcept
            requires (Sized && Common) || at_least<random> {
            return ptr_ - that.ptr_;
        }
        [[nodiscard]] constexpr ptrdiff_t operator-(sentinel<Element, Wrapped> const& s) const noexcept
            requires Sized {
            return ptr_ - s.base();
        }
        [[nodiscard]] friend constexpr ptrdiff_t operator-(
            sentinel<Element, Wrapped> const& s, iterator const& i) noexcept requires Sized {
            return -(i - s);
        }

        // iterator unwrapping operations:
        using _Prevent_inheriting_unwrap = iterator;

        using unwrap = iterator<Category, Element, Sized, Common, Proxy, false>;

        [[nodiscard]] constexpr auto _Unwrapped() const& noexcept requires Wrapped && Common {
            return unwrap{ptr_};
        }

        [[nodiscard]] constexpr auto _Unwrapped() && noexcept requires Wrapped {
            return unwrap{exchange(ptr_, nullptr)};
        }

        static constexpr bool _Unwrap_when_unverified = true;

        constexpr void _Seek_to(unwrap const& i) noexcept requires Wrapped && Common {
            ptr_ = i.base();
        }

        constexpr void _Seek_to(unwrap&& i) noexcept requires Wrapped {
            ptr_ = std::move(i).base();
        }
    };
    // clang-format on
} // namespace test

template <class Category, class Element, bool Sized, bool Common, bool Proxy, bool Wrapped>
struct std::iterator_traits<::test::iterator<Category, Element, Sized, Common, Proxy, Wrapped>> {
    using iterator_concept  = Category;
    using iterator_category = conditional_t<derived_from<Category, forward_iterator_tag>, //
        conditional_t<Proxy, input_iterator_tag, Category>, //
        conditional_t<Common, Category, void>>; // TRANSITION, LWG-3289
    using value_type        = remove_cv_t<Element>;
    using difference_type   = ptrdiff_t;
    using pointer           = conditional_t<derived_from<Category, contiguous_iterator_tag>, Element*, void>;
    using reference         = iter_reference_t<::test::iterator<Category, Element, Sized, Common, Proxy, Wrapped>>;
};

template <class Element, bool Sized, bool Wrapped>
struct std::pointer_traits<::test::iterator<std::contiguous_iterator_tag, Element, Sized, true, false, Wrapped>> {
    using pointer         = ::test::iterator<contiguous_iterator_tag, Element, Sized, true, false, Wrapped>;
    using element_type    = Element;
    using difference_type = ptrdiff_t;

    [[nodiscard]] static constexpr element_type* to_address(pointer const& x) noexcept {
        return x.base();
    }
};

namespace test {
    // clang-format off
    template <class Category, class Element,
        // Implement member size?
        bool Sized = false,
        // iterator and sentinel model sized_sentinel_for (also iterator and iterator if CommonIterators)
        bool SizedIterators = derived_from<Category, random>,
        // Model common_range?
        bool Common = false,
        // Iterator models sentinel_for with self
        bool CommonIterators = derived_from<Category, fwd>,
        // Use a proxy reference type?
        bool Proxy = !derived_from<Category, contiguous>>
        requires (!Common || CommonIterators)
            && (CommonIterators || !derived_from<Category, fwd>)
            && (!Proxy || !derived_from<Category, contiguous>)
    class range : ranges::view_base {
        span<Element> elements_;
        mutable bool begin_called_ = false;

    public:
        using I = iterator<Category, Element, SizedIterators, CommonIterators, Proxy, true>;
        using S = conditional_t<Common, I, sentinel<Element, true>>;

        range() = default;
        constexpr explicit range(span<Element> elements) noexcept : elements_{elements} {}

        range(const range&) requires derived_from<Category, fwd> = default;
        range& operator=(const range&) requires derived_from<Category, fwd> = default;

        constexpr range(range&& that) noexcept
            : elements_{exchange(that.elements_, {})}, begin_called_{that.begin_called_} {}

        constexpr range& operator=(range&& that) noexcept {
            elements_     = exchange(that.elements_, {});
            begin_called_ = that.begin_called_;
            return *this;
        }

        [[nodiscard]] constexpr I begin() const noexcept {
            if constexpr (!derived_from<Category, fwd>) {
                assert(!exchange(begin_called_, true));
            }
            return I{elements_.data()};
        }

        [[nodiscard]] constexpr S end() const noexcept {
            return S{elements_.data() + elements_.size()};
        }

        [[nodiscard]] constexpr ptrdiff_t size() const noexcept requires Sized {
            if constexpr (!derived_from<Category, fwd>) {
                assert(!begin_called_);
            }
            return static_cast<ptrdiff_t>(elements_.size());
        }

        [[nodiscard]] constexpr Element* data() const noexcept requires derived_from<Category, contiguous> {
            return elements_.data();
        }

        using UI = iterator<Category, Element, SizedIterators, CommonIterators, Proxy, false>;
        using US = conditional_t<Common, I, sentinel<Element, false>>;

        [[nodiscard]] constexpr UI _Unchecked_begin() const noexcept {
            return UI{elements_.data()};
        }
        [[nodiscard]] constexpr US _Unchecked_end() const noexcept {
            return US{elements_.data() + elements_.size()};
        }

        void operator&() const {
            STATIC_ASSERT(always_false<Category>);
        }
        template <class T>
        friend void operator,(range const&, T&&) {
            STATIC_ASSERT(always_false<Category>);
        }
    };
    // clang-format on
} // namespace test

template <class T>
class move_only_range : public test::range<test::input, T, false, false, false, false, false> {
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-1132704
    using test::range<test::input, T, false, false, false, false, false>::range;
#else // ^^^ no workaround / workaround vvv
public:
    constexpr move_only_range() = default;
    constexpr explicit move_only_range(std::span<T> elements) noexcept : move_only_range::range{elements} {}
    constexpr move_only_range(move_only_range&&) = default;
    constexpr move_only_range& operator=(move_only_range&&) = default;
#endif // TRANSITION, VSO-1132704
};

template <ranges::contiguous_range R>
move_only_range(R&) -> move_only_range<std::remove_reference_t<ranges::range_reference_t<R>>>;

template <class T>
inline constexpr bool ranges::enable_borrowed_range<::move_only_range<T>> = true;

template <int>
struct unique_tag {};

template <class I, int Tag = 0>
using ProjectionFor = unique_tag<Tag> (*)(std::iter_common_reference_t<I>);

template <class I>
using UnaryPredicateFor = boolish (*)(std::iter_common_reference_t<I>);

template <int Tag = 0>
using ProjectedUnaryPredicate = boolish (*)(unique_tag<Tag>);
template <class I2, int Tag1 = 0>
using HalfProjectedBinaryPredicateFor = boolish (*)(unique_tag<Tag1>, std::iter_common_reference_t<I2>);
template <int Tag1 = 0, int Tag2 = 0>
using ProjectedBinaryPredicate = boolish (*)(unique_tag<Tag1>, unique_tag<Tag2>);

template <class I1, class I2>
using BinaryPredicateFor = boolish (*)(std::iter_common_reference_t<I1>, std::iter_common_reference_t<I2>);

template <class Continuation, class Element = int>
struct with_writable_iterators {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // Sized and Common are not significant for "lone" single-pass iterators, so we can ignore them here.
        Continuation::template call<Args..., iterator<output, Element, false, false, false>>();
        Continuation::template call<Args..., iterator<output, Element, false, false, true>>();
        Continuation::template call<Args..., iterator<input, Element, false, false, false>>();
        Continuation::template call<Args..., iterator<input, Element, false, false, true>>();
        // For forward and bidi, Common is necessarily true but Sized and Proxy may vary.
        Continuation::template call<Args..., iterator<fwd, Element, false, true, false>>();
        Continuation::template call<Args..., iterator<fwd, Element, false, true, true>>();
        Continuation::template call<Args..., iterator<fwd, Element, true, true, false>>();
        Continuation::template call<Args..., iterator<fwd, Element, true, true, true>>();

        Continuation::template call<Args..., iterator<bidi, Element, false, true, false>>();
        Continuation::template call<Args..., iterator<bidi, Element, false, true, true>>();
        Continuation::template call<Args..., iterator<bidi, Element, true, true, false>>();
        Continuation::template call<Args..., iterator<bidi, Element, true, true, true>>();
        // Random iterators are Sized and Common - only Proxy varies.
        Continuation::template call<Args..., iterator<random, Element, true, true, false>>();
        Continuation::template call<Args..., iterator<random, Element, true, true, true>>();
        // Contiguous iterators are totally locked down.
        Continuation::template call<Args..., iterator<contiguous, Element>>();
    }
};

template <class Continuation, class Element = int>
struct with_input_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // For all ranges, Commmon implies CommonIterators.
        // For single-pass ranges, CommonIterators is uninteresting without Common (there's only one valid iterator
        // value at a time, and no reason to compare it with itself for equality).
        Continuation::template call<Args..., range<input, Element, false, false, false, false, false>>();
        Continuation::template call<Args..., range<input, Element, false, false, false, false, true>>();
        Continuation::template call<Args..., range<input, Element, false, false, true, true, false>>();
        Continuation::template call<Args..., range<input, Element, false, false, true, true, true>>();

        Continuation::template call<Args..., range<input, Element, false, true, false, false, false>>();
        Continuation::template call<Args..., range<input, Element, false, true, false, false, true>>();
        Continuation::template call<Args..., range<input, Element, false, true, true, true, false>>();
        Continuation::template call<Args..., range<input, Element, false, true, true, true, true>>();

        Continuation::template call<Args..., range<input, Element, true, false, false, false, false>>();
        Continuation::template call<Args..., range<input, Element, true, false, false, false, true>>();
        Continuation::template call<Args..., range<input, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<input, Element, true, false, true, true, true>>();

        Continuation::template call<Args..., range<input, Element, true, true, false, false, false>>();
        Continuation::template call<Args..., range<input, Element, true, true, false, false, true>>();
        Continuation::template call<Args..., range<input, Element, true, true, true, true, false>>();
        Continuation::template call<Args..., range<input, Element, true, true, true, true, true>>();

        // forward always has CommonIterators; !Sized && SizedSentinel is uninteresting (sized_range is sized_range).
        Continuation::template call<Args..., range<fwd, Element, false, false, false, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, false, false, false, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, false, false, true, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, false, false, true, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, true, false, false, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, true, false, false, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, true, false, true, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, true, true, false, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, true, true, false, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, true, true, true, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, true, true, true, true, true>>();

        // Ditto always CommonIterators; !Sized && SizedSentinel is uninteresting (ranges::size still works).
        Continuation::template call<Args..., range<bidi, Element, false, false, false, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, false, false, false, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, false, false, true, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, false, false, true, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, true, false, false, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, true, false, false, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, true, false, true, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, true, true, false, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, true, true, false, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, true, true, true, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, true, true, true, true, true>>();

        // Ditto always CommonIterators; !Sized && SizedSentinel is uninteresting (ranges::size still works), as is
        // !Sized && Common.
        Continuation::template call<Args..., range<random, Element, false, false, false, true, false>>();
        Continuation::template call<Args..., range<random, Element, false, false, false, true, true>>();
        Continuation::template call<Args..., range<random, Element, true, false, false, true, false>>();
        Continuation::template call<Args..., range<random, Element, true, false, false, true, true>>();
        Continuation::template call<Args..., range<random, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<random, Element, true, false, true, true, true>>();
        Continuation::template call<Args..., range<random, Element, true, true, false, true, false>>();
        Continuation::template call<Args..., range<random, Element, true, true, false, true, true>>();
        Continuation::template call<Args..., range<random, Element, true, true, true, true, false>>();
        Continuation::template call<Args..., range<random, Element, true, true, true, true, true>>();

        // Ditto always CommonIterators; !Sized && SizedSentinel is uninteresting (ranges::size still works), as is
        // !Sized && Common. contiguous also implies !Proxy.
        Continuation::template call<Args..., range<contiguous, Element, false, false, false, true, false>>();
        Continuation::template call<Args..., range<contiguous, Element, true, false, false, true, false>>();
        Continuation::template call<Args..., range<contiguous, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<contiguous, Element, true, true, false, true, false>>();
        Continuation::template call<Args..., range<contiguous, Element, true, true, true, true, false>>();
    }
};

template <class Continuation, class Element = int>
struct with_forward_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // forward always has CommonIterators; !Sized && SizedSentinel is uninteresting (sized_range is sized_range).
        Continuation::template call<Args..., range<fwd, Element, false, false, false, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, false, false, false, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, false, false, true, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, false, false, true, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, true, false, false, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, true, false, false, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, true, false, true, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, true, true, false, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, true, true, false, true, true>>();
        Continuation::template call<Args..., range<fwd, Element, true, true, true, true, false>>();
        Continuation::template call<Args..., range<fwd, Element, true, true, true, true, true>>();

        // Ditto always CommonIterators; !Sized && SizedSentinel is uninteresting (ranges::size still works).
        Continuation::template call<Args..., range<bidi, Element, false, false, false, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, false, false, false, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, false, false, true, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, false, false, true, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, true, false, false, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, true, false, false, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, true, false, true, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, true, true, false, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, true, true, false, true, true>>();
        Continuation::template call<Args..., range<bidi, Element, true, true, true, true, false>>();
        Continuation::template call<Args..., range<bidi, Element, true, true, true, true, true>>();

        // Ditto always CommonIterators; !Sized && SizedSentinel is uninteresting (ranges::size still works), as is
        // !Sized && Common.
        Continuation::template call<Args..., range<random, Element, false, false, false, true, false>>();
        Continuation::template call<Args..., range<random, Element, false, false, false, true, true>>();
        Continuation::template call<Args..., range<random, Element, true, false, false, true, false>>();
        Continuation::template call<Args..., range<random, Element, true, false, false, true, true>>();
        Continuation::template call<Args..., range<random, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<random, Element, true, false, true, true, true>>();
        Continuation::template call<Args..., range<random, Element, true, true, false, true, false>>();
        Continuation::template call<Args..., range<random, Element, true, true, false, true, true>>();
        Continuation::template call<Args..., range<random, Element, true, true, true, true, false>>();
        Continuation::template call<Args..., range<random, Element, true, true, true, true, true>>();

        // Ditto always CommonIterators; !Sized && SizedSentinel is uninteresting (ranges::size still works), as is
        // !Sized && Common. contiguous also implies !Proxy.
        Continuation::template call<Args..., range<contiguous, Element, false, false, false, true, false>>();
        Continuation::template call<Args..., range<contiguous, Element, true, false, false, true, false>>();
        Continuation::template call<Args..., range<contiguous, Element, true, false, true, true, false>>();
        Continuation::template call<Args..., range<contiguous, Element, true, true, false, true, false>>();
        Continuation::template call<Args..., range<contiguous, Element, true, true, true, true, false>>();
    }
};

template <class Continuation, class Element = int>
struct with_input_iterators {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // Sized and Common are not significant for "lone" single-pass iterators, so we can ignore them here.
        Continuation::template call<Args..., iterator<input, Element, false, false, false>>();
        Continuation::template call<Args..., iterator<input, Element, false, false, true>>();
        // For forward and bidi, Common is necessarily true but Sized and Proxy may vary.
        Continuation::template call<Args..., iterator<fwd, Element, false, true, false>>();
        Continuation::template call<Args..., iterator<fwd, Element, false, true, true>>();
        Continuation::template call<Args..., iterator<fwd, Element, true, true, false>>();
        Continuation::template call<Args..., iterator<fwd, Element, true, true, true>>();

        Continuation::template call<Args..., iterator<bidi, Element, false, true, false>>();
        Continuation::template call<Args..., iterator<bidi, Element, false, true, true>>();
        Continuation::template call<Args..., iterator<bidi, Element, true, true, false>>();
        Continuation::template call<Args..., iterator<bidi, Element, true, true, true>>();
        // Random iterators are Sized and Common - only Proxy varies.
        Continuation::template call<Args..., iterator<random, Element, true, true, false>>();
        Continuation::template call<Args..., iterator<random, Element, true, true, true>>();
        // Contiguous iterators are totally locked down.
        Continuation::template call<Args..., iterator<contiguous, Element>>();
    }
};

template <class Continuation>
struct with_difference {
    template <class Iterator>
    static constexpr void call() {
        Continuation::template call<Iterator, std::iter_difference_t<Iterator>>();
    }
};

template <class Instantiator, class Element = int>
constexpr void test_in() {
    with_input_ranges<Instantiator, Element>::call();
}

template <class Instantiator, class Element = int>
constexpr void test_fwd() {
    with_forward_ranges<Instantiator, Element>::call();
}

template <class Instantiator, class Element1 = int, class Element2 = int>
constexpr void test_in_in() {
    with_input_ranges<with_input_ranges<Instantiator, Element2>, Element1>::call();
}

template <class Instantiator, class Element1 = int, class Element2 = int>
constexpr void test_in_fwd() {
    with_input_ranges<with_forward_ranges<Instantiator, Element2>, Element1>::call();
}

template <class Instantiator, class Element1 = int, class Element2 = int>
constexpr void test_fwd_fwd() {
    with_forward_ranges<with_forward_ranges<Instantiator, Element2>, Element1>::call();
}

template <class Instantiator, class Element1 = int, class Element2 = int>
constexpr void test_in_write() {
    with_input_ranges<with_writable_iterators<Instantiator, Element2>, Element1>::call();
}

template <class Instantiator, class Element1 = int, class Element2 = int>
constexpr void test_counted_write() {
    with_input_iterators<with_difference<with_writable_iterators<Instantiator, Element2>>, Element1>::call();
}

template <size_t I>
struct get_nth_fn {
    template <class T>
    [[nodiscard]] constexpr auto&& operator()(T&& t) const noexcept requires requires {
        get<I>(std::forward<T>(t));
    }
    { return get<I>(std::forward<T>(t)); }

    template <class T, class Elem>
    [[nodiscard]] constexpr decltype(auto) operator()(test::proxy_reference<T, Elem> ref) const noexcept
        requires requires {
        (*this)(ref.peek());
    }
    { return (*this)(ref.peek()); }
};
inline constexpr get_nth_fn<0> get_first;
inline constexpr get_nth_fn<1> get_second;
