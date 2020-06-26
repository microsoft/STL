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
    using std::assignable_from, std::conditional_t, std::convertible_to, std::copy_constructible, std::derived_from,
        std::exchange, std::ptrdiff_t, std::span;

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

    enum class CanDifference : bool { no, yes };
    enum class CanCompare : bool { no, yes };
    enum class ProxyRef : bool { no, yes };
    enum class IsWrapped : bool { no, yes };

    template <class T>
    [[nodiscard]] constexpr bool to_bool(T const t) noexcept {
        STATIC_ASSERT(std::is_enum_v<T> && std::same_as<std::underlying_type_t<T>, bool>);
        return static_cast<bool>(t);
    }

    template <class Element, IsWrapped Wrapped = IsWrapped::yes>
    class sentinel {
        Element* ptr_ = nullptr;

    public:
        sentinel() = default;
        constexpr explicit sentinel(Element* ptr) noexcept : ptr_{ptr} {}

        [[nodiscard]] constexpr Element* base() const noexcept {
            return ptr_;
        }

        using _Prevent_inheriting_unwrap = sentinel;

        using unwrap = sentinel<Element, IsWrapped::no>;

        [[nodiscard]] constexpr auto _Unwrapped() const noexcept requires(to_bool(Wrapped)) {
            return unwrap{ptr_};
        }

        static constexpr bool _Unwrap_when_unverified = true;

        constexpr void _Seek_to(unwrap const& s) noexcept requires(to_bool(Wrapped)) {
            ptr_ = s.base();
        }
    };

    // clang-format off
    template <class T, class U>
    concept CanEq = requires(T const& t, U const& u) {
        { t == u } -> convertible_to<bool>;
    };

    template <class T, class U>
    concept CanNEq = requires(T const& t, U const& u) {
        { t != u } -> convertible_to<bool>;
    };

    template <class T, class U>
    concept CanLt = requires(T const& t, U const& u) {
        { t < u } -> convertible_to<bool>;
    };

    template <class T, class U>
    concept CanLtE = requires(T const& t, U const& u) {
        { t <= u } -> convertible_to<bool>;
    };

    template <class T, class U>
    concept CanGt = requires(T const& t, U const& u) {
        { t > u } -> convertible_to<bool>;
    };

    template <class T, class U>
    concept CanGtE = requires(T const& t, U const& u) {
        { t >= u } -> convertible_to<bool>;
    };
    // clang-format on

    template <class Category, class Element>
    class proxy_reference {
        Element& ref_;

        using Value = std::remove_cv_t<Element>;

    public:
        constexpr explicit proxy_reference(Element& ref) : ref_{ref} {}
        proxy_reference(proxy_reference const&) = default;

        constexpr proxy_reference const& operator=(proxy_reference const& that) const
            requires assignable_from<Element&, Element&> {
            ref_ = that.ref_;
            return *this;
        }

        // clang-format off
        constexpr operator Value() const requires derived_from<Category, input> && copy_constructible<Value> {
            return ref_;
        }
        // clang-format on

        constexpr void operator=(Value const& val) const requires assignable_from<Element&, Value const&> {
            ref_ = val;
        }

        template <class Cat, class Elem>
        constexpr boolish operator==(proxy_reference<Cat, Elem> that) const requires CanEq<Element, Elem> {
            return {ref_ == that.peek()};
        }
        template <class Cat, class Elem>
        constexpr boolish operator!=(proxy_reference<Cat, Elem> that) const requires CanNEq<Element, Elem> {
            return {ref_ != that.peek()};
        }
        template <class Cat, class Elem>
        constexpr boolish operator<(proxy_reference<Cat, Elem> that) const requires CanLt<Element, Elem> {
            return {ref_ < that.peek()};
        }
        template <class Cat, class Elem>
        constexpr boolish operator>(proxy_reference<Cat, Elem> that) const requires CanGt<Element, Elem> {
            return {ref_ > that.peek()};
        }
        template <class Cat, class Elem>
        constexpr boolish operator<=(proxy_reference<Cat, Elem> that) const requires CanLtE<Element, Elem> {
            return {ref_ <= that.peek()};
        }
        template <class Cat, class Elem>
        constexpr boolish operator>=(proxy_reference<Cat, Elem> that) const requires CanGtE<Element, Elem> {
            return {ref_ >= that.peek()};
        }

        // clang-format off
        friend constexpr boolish operator==(proxy_reference ref, Value const& val) requires CanEq<Element, Value> {
            return {ref.ref_ == val};
        }
        friend constexpr boolish operator==(Value const& val, proxy_reference ref) requires CanEq<Element, Value> {
            return {ref.ref_ == val};
        }
        friend constexpr boolish operator!=(proxy_reference ref, Value const& val) requires CanNEq<Element, Value> {
            return {ref.ref_ != val};
        }
        friend constexpr boolish operator!=(Value const& val, proxy_reference ref) requires CanNEq<Element, Value> {
            return {ref.ref_ != val};
        }
        friend constexpr boolish operator<(Value const& val, proxy_reference ref) requires CanLt<Value, Element> {
            return {val < ref.ref_};
        }
        friend constexpr boolish operator<(proxy_reference ref, Value const& val) requires CanLt<Element, Value> {
            return {ref.ref_ < val};
        }
        friend constexpr boolish operator>(Value const& val, proxy_reference ref) requires CanGt<Value, Element> {
            return {val > ref.ref_};
        }
        friend constexpr boolish operator>(proxy_reference ref, Value const& val) requires CanGt<Element, Value> {
            return {ref.ref_ > val};
        }
        friend constexpr boolish operator<=(Value const& val, proxy_reference ref) requires CanLtE<Value, Element> {
            return {val <= ref.ref_};
        }
        friend constexpr boolish operator<=(proxy_reference ref, Value const& val) requires CanLtE<Element, Value> {
            return {ref.ref_ <= val};
        }
        friend constexpr boolish operator>=(Value const& val, proxy_reference ref) requires CanGtE<Value, Element> {
            return {val >= ref.ref_};
        }
        friend constexpr boolish operator>=(proxy_reference ref, Value const& val) requires CanGtE<Element, Value> {
            return {ref.ref_ >= val};
        }
        // clang-format on

        constexpr Element& peek() const noexcept {
            return ref_;
        }
    };

    // clang-format off
    template <class Category, class Element,
        // Model sized_sentinel_for along with sentinel?
        CanDifference Diff = CanDifference{derived_from<Category, random>},
        // Model sentinel_for with self (and sized_sentinel_for if Diff; implies copyable)?
        CanCompare Eq = CanCompare{derived_from<Category, fwd>},
        // Use a ProxyRef reference type (instead of Element&)?
        ProxyRef Proxy = ProxyRef{!derived_from<Category, contiguous>},
        // Interact with the STL's iterator unwrapping machinery?
        IsWrapped Wrapped = IsWrapped::yes>
        requires (to_bool(Eq) || !derived_from<Category, fwd>)
            && (!to_bool(Proxy) || !derived_from<Category, contiguous>)
    class iterator {
        Element* ptr_;

        template <class T>
        static constexpr bool at_least = derived_from<Category, T>;

        using ReferenceType = conditional_t<to_bool(Proxy), proxy_reference<Category, Element>, Element&>;

    public:
        // output iterator operations
        iterator() = default;

        constexpr explicit iterator(Element* ptr) noexcept : ptr_{ptr} {}

        constexpr iterator(iterator&& that) noexcept : ptr_{exchange(that.ptr_, nullptr)} {}
        constexpr iterator& operator=(iterator&& that) noexcept {
            ptr_ = exchange(that.ptr_, nullptr);
            return *this;
        }

        [[nodiscard]] constexpr Element* base() const& noexcept requires (to_bool(Eq)) {
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

        [[nodiscard]] constexpr friend std::remove_cv_t<Element> iter_move(iterator const& i)
            requires at_least<input> && std::constructible_from<std::remove_cv_t<Element>, Element> {
            return std::move(*i.ptr_);
        }

        constexpr friend void iter_swap(iterator const& x, iterator const& y) requires at_least<input> {
            ranges::iter_swap(x.ptr_, y.ptr_);
        }

        // sentinel operations (implied by forward iterator):
        iterator(iterator const&) requires (to_bool(Eq)) = default;
        iterator& operator=(iterator const&) requires (to_bool(Eq)) = default;
        [[nodiscard]] constexpr boolish operator==(iterator const& that) const noexcept requires (to_bool(Eq)) {
            return {ptr_ == that.ptr_};
        }
        [[nodiscard]] constexpr boolish operator!=(iterator const& that) const noexcept requires (to_bool(Eq)) {
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
        [[nodiscard]] constexpr boolish operator<(iterator const& that) const noexcept requires at_least<random> {
            return {ptr_ < that.ptr_};
        }
        [[nodiscard]] constexpr boolish operator>(iterator const& that) const noexcept requires at_least<random> {
            return that < *this;
        }
        [[nodiscard]] constexpr boolish operator<=(iterator const& that) const noexcept requires at_least<random> {
            return !(that < *this);
        }
        [[nodiscard]] constexpr boolish operator>=(iterator const& that) const noexcept requires at_least<random> {
            return !(*this < that);
        }
        [[nodiscard]] constexpr ReferenceType operator[](ptrdiff_t const n) const& noexcept requires at_least<random> {
            return ReferenceType{ptr_[n]};
        }
        constexpr iterator& operator+=(ptrdiff_t const n) & noexcept requires at_least<random> {
            ptr_ += n;
            return *this;
        }
        constexpr iterator& operator-=(ptrdiff_t const n) & noexcept requires at_least<random> {
            ptr_ -= n;
            return *this;
        }
        [[nodiscard]] constexpr iterator operator+(ptrdiff_t const n) const noexcept requires at_least<random> {
            return iterator{ptr_ + n};
        }
        [[nodiscard]] friend constexpr iterator operator+(ptrdiff_t const n, iterator const& i) noexcept
            requires at_least<random> {
            return i + n;
        }
        [[nodiscard]] constexpr iterator operator-(ptrdiff_t const n) const noexcept requires at_least<random> {
            return iterator{ptr_ - n};
        }

        // contiguous iterator operations:
        [[nodiscard]] constexpr Element* operator->() const noexcept requires at_least<contiguous> {
            return ptr_;
        }

        // sized_sentinel_for operations:
        [[nodiscard]] constexpr ptrdiff_t operator-(iterator const& that) const noexcept
            requires (to_bool(Diff) && to_bool(Eq)) || at_least<random> {
            return ptr_ - that.ptr_;
        }
        [[nodiscard]] constexpr ptrdiff_t operator-(sentinel<Element, Wrapped> const& s) const noexcept
            requires (to_bool(Diff)) {
            return ptr_ - s.base();
        }
        [[nodiscard]] friend constexpr ptrdiff_t operator-(
            sentinel<Element, Wrapped> const& s, iterator const& i) noexcept requires (to_bool(Diff)) {
            return -(i - s);
        }

        // iterator unwrapping operations:
        using _Prevent_inheriting_unwrap = iterator;

        using unwrap = iterator<Category, Element, Diff, Eq, Proxy, IsWrapped::no>;

        [[nodiscard]] constexpr auto _Unwrapped() const& noexcept requires (to_bool(Wrapped) && to_bool(Eq)) {
            return unwrap{ptr_};
        }

        [[nodiscard]] constexpr auto _Unwrapped() && noexcept requires (to_bool(Wrapped)) {
            return unwrap{exchange(ptr_, nullptr)};
        }

        static constexpr bool _Unwrap_when_unverified = true;

        constexpr void _Seek_to(unwrap const& i) noexcept requires (to_bool(Wrapped) && to_bool(Eq)) {
            ptr_ = i.base();
        }

        constexpr void _Seek_to(unwrap&& i) noexcept requires (to_bool(Wrapped)) {
            ptr_ = std::move(i).base();
        }
    };
    // clang-format on
} // namespace test

template <class Category, class Element, ::test::CanDifference Diff, ::test::CanCompare Eq, ::test::ProxyRef Proxy,
    ::test::IsWrapped Wrapped>
struct std::iterator_traits<::test::iterator<Category, Element, Diff, Eq, Proxy, Wrapped>> {
    using iterator_concept  = Category;
    using iterator_category = conditional_t<derived_from<Category, forward_iterator_tag>, //
        conditional_t<static_cast<bool>(Proxy), input_iterator_tag, Category>, //
        conditional_t<static_cast<bool>(Eq), Category, void>>; // TRANSITION, LWG-3289
    using value_type        = remove_cv_t<Element>;
    using difference_type   = ptrdiff_t;
    using pointer           = conditional_t<derived_from<Category, contiguous_iterator_tag>, Element*, void>;
    using reference         = iter_reference_t<::test::iterator<Category, Element, Diff, Eq, Proxy, Wrapped>>;
};

template <class Element, ::test::CanDifference Diff, ::test::IsWrapped Wrapped>
struct std::pointer_traits<::test::iterator<std::contiguous_iterator_tag, Element, Diff, ::test::CanCompare::yes,
    ::test::ProxyRef::no, Wrapped>> {
    using pointer         = ::test::iterator<contiguous_iterator_tag, Element, Diff, ::test::CanCompare::yes,
        ::test::ProxyRef::no, Wrapped>;
    using element_type    = Element;
    using difference_type = ptrdiff_t;

    [[nodiscard]] static constexpr element_type* to_address(pointer const& x) noexcept {
        return x.base();
    }
};

namespace test {
    enum class Sized : bool { no, yes };
    enum class Common : bool { no, yes };

    // clang-format off
    template <class Category, class Element,
        // Implement member size? (NB: Not equivalent to "Is this a sized_range?")
        Sized IsSized = Sized::no,
        // iterator and sentinel model sized_sentinel_for (also iterator and iterator if Eq)
        CanDifference Diff = CanDifference{derived_from<Category, random>},
        // Model common_range?
        Common IsCommon = Common::no,
        // Iterator models sentinel_for with self
        CanCompare Eq = CanCompare{derived_from<Category, fwd>},
        // Use a ProxyRef reference type?
        ProxyRef Proxy = ProxyRef{!derived_from<Category, contiguous>}>
        requires (!to_bool(IsCommon) || to_bool(Eq))
            && (to_bool(Eq) || !derived_from<Category, fwd>)
            && (!to_bool(Proxy) || !derived_from<Category, contiguous>)
    class range {
        span<Element> elements_;
        mutable bool begin_called_ = false;

    public:
        using I = iterator<Category, Element, Diff, Eq, Proxy, IsWrapped::yes>;
        using S = conditional_t<to_bool(IsCommon), I, sentinel<Element, IsWrapped::yes>>;

        range() = default;
        constexpr explicit range(span<Element> elements) noexcept : elements_{elements} {}

        range(range const&) = delete;
        range& operator=(range const&) = delete;

        [[nodiscard]] constexpr I begin() const noexcept {
            if constexpr (!derived_from<Category, fwd>) {
                assert(!exchange(begin_called_, true));
            }
            return I{elements_.data()};
        }

        [[nodiscard]] constexpr S end() const noexcept {
            return S{elements_.data() + elements_.size()};
        }

        [[nodiscard]] constexpr ptrdiff_t size() const noexcept requires (to_bool(IsSized)) {
            if constexpr (!derived_from<Category, fwd>) {
                assert(!begin_called_);
            }
            return static_cast<ptrdiff_t>(elements_.size());
        }

        [[nodiscard]] constexpr Element* data() const noexcept requires derived_from<Category, contiguous> {
            return elements_.data();
        }

        using UI = iterator<Category, Element, Diff, Eq, Proxy, IsWrapped::no>;
        using US = conditional_t<to_bool(IsCommon), I, sentinel<Element, IsWrapped::no>>;

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
class basic_borrowed_range : public test::range<test::input, T, test::Sized::no, test::CanDifference::no,
                                 test::Common::no, test::CanCompare::no, test::ProxyRef::no> {
    using test::range<test::input, T, test::Sized::no, test::CanDifference::no, test::Common::no, test::CanCompare::no,
        test::ProxyRef::no>::range;
};

template <ranges::contiguous_range R>
basic_borrowed_range(R&) -> basic_borrowed_range<std::remove_reference_t<ranges::range_reference_t<R>>>;

template <class T>
inline constexpr bool ranges::enable_borrowed_range<::basic_borrowed_range<T>> = true;

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
        using test::iterator;

        // Diff and Eq are not significant for "lone" single-pass iterators, so we can ignore them here.
        Continuation::template call<Args...,
            iterator<output, Element, CanDifference::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<output, Element, CanDifference::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Args...,
            iterator<input, Element, CanDifference::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<input, Element, CanDifference::no, CanCompare::no, ProxyRef::yes>>();
        // For forward and bidi, Eq is necessarily true but Diff and Proxy may vary.
        Continuation::template call<Args...,
            iterator<fwd, Element, CanDifference::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<fwd, Element, CanDifference::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            iterator<fwd, Element, CanDifference::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<fwd, Element, CanDifference::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Args...,
            iterator<bidi, Element, CanDifference::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<bidi, Element, CanDifference::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            iterator<bidi, Element, CanDifference::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<bidi, Element, CanDifference::yes, CanCompare::yes, ProxyRef::yes>>();
        // Random iterators are Diff and Eq - only Proxy varies.
        Continuation::template call<Args...,
            iterator<random, Element, CanDifference::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<random, Element, CanDifference::yes, CanCompare::yes, ProxyRef::yes>>();
        // Contiguous iterators are totally locked down.
        Continuation::template call<Args..., iterator<contiguous, Element>>();
    }
};

template <class Continuation, class Element = int>
struct with_contiguous_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // Ditto always Eq; !IsSized && SizedSentinel is uninteresting (ranges::size still works), as is
        // !IsSized && IsCommon. contiguous also implies !Proxy.
        Continuation::template call<Args...,
            range<contiguous, Element, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<contiguous, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<contiguous, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<contiguous, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<contiguous, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();
    }
};

template <class Continuation, class Element = int>
struct with_random_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // Ditto always Eq; !IsSized && SizedSentinel is uninteresting (ranges::size works either way), as is
        // !IsSized && IsCommon.
        Continuation::template call<Args...,
            range<random, Element, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<random, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        with_contiguous_ranges<Continuation, Element>::template call<Args...>();
    }
};

template <class Continuation, class Element = int>
struct with_bidirectional_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // Ditto always Eq; !IsSized && Diff is uninteresting (ranges::size still works).
        Continuation::template call<Args...,
            range<bidi, Element, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<bidi, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        with_random_ranges<Continuation, Element>::template call<Args...>();
    }
};

template <class Continuation, class Element = int>
struct with_forward_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // forward always has Eq; !IsSized && Diff is uninteresting (sized_range is sized_range).
        Continuation::template call<Args...,
            range<fwd, Element, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<fwd, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        with_bidirectional_ranges<Continuation, Element>::template call<Args...>();
    }
};

template <class Continuation, class Element = int>
struct with_input_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // For all ranges, IsCommon implies Eq.
        // For single-pass ranges, Eq is uninteresting without IsCommon (there's only one valid iterator
        // value at a time, and no reason to compare it with itself for equality).
        Continuation::template call<Args...,
            range<input, Element, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Args...,
            range<input, Element, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Args...,
            range<input, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::yes, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Args...,
            range<input, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::yes, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, Element, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        with_forward_ranges<Continuation, Element>::template call<Args...>();
    }
};

template <class Continuation, class Element = int>
struct with_input_iterators {
    template <class... Args>
    static constexpr void call() {
        using namespace test;

        // IsSized and Eq are not significant for "lone" single-pass iterators, so we can ignore them here.
        Continuation::template call<Args...,
            iterator<input, Element, CanDifference::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<input, Element, CanDifference::no, CanCompare::no, ProxyRef::yes>>();
        // For forward and bidi, Eq is necessarily true but IsSized and Proxy may vary.
        Continuation::template call<Args...,
            iterator<fwd, Element, CanDifference::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<fwd, Element, CanDifference::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            iterator<fwd, Element, CanDifference::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<fwd, Element, CanDifference::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Args...,
            iterator<bidi, Element, CanDifference::no, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<bidi, Element, CanDifference::no, CanCompare::yes, ProxyRef::yes>>();
        Continuation::template call<Args...,
            iterator<bidi, Element, CanDifference::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<bidi, Element, CanDifference::yes, CanCompare::yes, ProxyRef::yes>>();
        // Random iterators are IsSized and Eq - only Proxy varies.
        Continuation::template call<Args...,
            iterator<random, Element, CanDifference::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            iterator<random, Element, CanDifference::yes, CanCompare::yes, ProxyRef::yes>>();
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

template <class Instantiator, class Element = int>
constexpr void test_bidi() {
    with_bidirectional_ranges<Instantiator, Element>::call();
}

template <class Instantiator, class Element = int>
constexpr void test_random() {
    with_random_ranges<Instantiator, Element>::call();
}

template <class Instantiator, class Element = int>
constexpr void test_contiguous() {
    with_contiguous_ranges<Instantiator, Element>::call();
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
