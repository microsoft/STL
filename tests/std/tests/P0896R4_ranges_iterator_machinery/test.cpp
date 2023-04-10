// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <compare>
#include <concepts>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

template <class T>
using reference_to = T&;
template <class T>
concept can_reference = requires { typename reference_to<T>; };

template <class T, class U = T>
concept has_eq = requires(T const& t, U const& u) { t == u; };

template <class T, class U = T>
concept has_neq = requires(T const& t, U const& u) { t != u; };

template <class T, class U = T>
concept has_less = requires(T const& t, U const& u) { t < u; };

template <class T, class U = T>
concept has_greater = requires(T const& t, U const& u) { t > u; };

template <class T, class U = T>
concept has_less_eq = requires(T const& t, U const& u) { t <= u; };

template <class T, class U = T>
concept has_greater_eq = requires(T const& t, U const& u) { t >= u; };

template <class T, class U = T>
concept has_difference = requires(T const& t, U const& u) { t - u; };

struct no_such_type; // not defined

struct empty_type {
    bool operator==(empty_type const&) const = default;
};

struct move_only {
    move_only()                       = default;
    move_only(move_only&&)            = default;
    move_only& operator=(move_only&&) = default;
};

template <class... Types>
struct derives_from : Types... {};

template <class T>
struct dereferences_to {
    using value_type = std::remove_cvref_t<T>;
    T operator*() const;
};

struct my_iterator {
    using iterator_concept  = std::forward_iterator_tag;
    using iterator_category = std::input_iterator_tag;
    using value_type        = char;
    using difference_type   = long;
    using pointer           = void;
    using reference         = char;

    bool operator==(my_iterator const&) const = default;

    char operator*() const;
    my_iterator& operator++();
    my_iterator operator++(int);
};

template <class T>
concept has_member_iter_concept = requires { typename T::iterator_concept; };

template <class T>
concept has_member_iter_category = requires { typename T::iterator_category; };

template <class T>
concept has_member_value_type = requires { typename T::value_type; };
template <class T>
concept has_iter_value = requires { typename std::iter_value_t<T>; };

template <class T>
concept has_member_difference_type = requires { typename T::difference_type; };
template <class T>
concept has_iter_diff = requires { typename std::iter_difference_t<T>; };

template <class T>
struct arrow_base {
    T operator->() const;
    std::strong_ordering operator<=>(arrow_base const&) const = default;
};

struct sentinel_base {
    bool operator==(std::default_sentinel_t) const;
};

struct simple_input_iter {
    using value_type      = double;
    using difference_type = long;

    explicit simple_input_iter(int); // not default constructible

    value_type operator*() const;
    simple_input_iter& operator++();
    simple_input_iter operator++(int);

    bool operator==(simple_input_iter const&) const = default;
};

template <class Base = empty_type>
struct simple_forward_iter : Base {
    using value_type      = double;
    using difference_type = long;

    value_type const& operator*() const;
    simple_forward_iter& operator++();
    simple_forward_iter operator++(int);

    bool operator==(simple_forward_iter const&) const = default;
};

template <class Base = empty_type>
struct simple_bidi_iter : Base {
    using value_type      = double;
    using difference_type = long;
    using reference       = value_type const&;

    value_type& operator*() const;
    simple_bidi_iter& operator++();
    simple_bidi_iter operator++(int);

    bool operator==(simple_bidi_iter const&) const = default;

    simple_bidi_iter& operator--();
    simple_bidi_iter operator--(int);
};

template <class Base = empty_type>
struct simple_random_iter : Base {
    using value_type = double;
    using D          = long;

    value_type const& operator*() const;
    simple_random_iter& operator++();
    simple_random_iter operator++(int);

    simple_random_iter& operator--();
    simple_random_iter operator--(int);

    std::strong_ordering operator<=>(simple_random_iter const&) const;

    value_type const& operator[](D) const;
    simple_random_iter& operator-=(D);
    simple_random_iter operator-(D) const;
    D operator-(simple_random_iter const&) const;
    simple_random_iter& operator+=(D);
    simple_random_iter operator+(D) const;
    friend simple_random_iter operator+(D, const simple_random_iter&);
};

template <class Base = empty_type>
class simple_contiguous_iter : public Base {
private:
    using V = double;
    using D = long;

public:
    using element_type = V const;

    V const& operator*() const;
    simple_contiguous_iter& operator++();
    simple_contiguous_iter operator++(int);

    simple_contiguous_iter& operator--();
    simple_contiguous_iter operator--(int);

    std::strong_ordering operator<=>(simple_contiguous_iter const&) const;

    V const& operator[](D) const;
    simple_contiguous_iter& operator-=(D);
    simple_contiguous_iter operator-(D) const;
    D operator-(simple_contiguous_iter const&) const;
    simple_contiguous_iter& operator+=(D);
    simple_contiguous_iter operator+(D) const;
    friend simple_contiguous_iter operator+(D, const simple_contiguous_iter&);

    static simple_contiguous_iter pointer_to(element_type&) noexcept;
};

template <class Base>
struct std::indirectly_readable_traits<simple_contiguous_iter<Base>> {
    using value_type = double;
};

// Validate iterator_category of iterators whose reference types are rvalue references (LWG-3798).
struct xvalue_forward_iter {
    using value_type      = double;
    using difference_type = long;

    value_type&& operator*() const;
    xvalue_forward_iter& operator++();
    xvalue_forward_iter operator++(int);

    bool operator==(xvalue_forward_iter const&) const = default;
};

struct xvalue_bidi_iter {
    using value_type      = double;
    using difference_type = long;
    using reference       = value_type&&;

    value_type&& operator*() const;
    xvalue_bidi_iter& operator++();
    xvalue_bidi_iter operator++(int);

    bool operator==(xvalue_bidi_iter const&) const = default;

    xvalue_bidi_iter& operator--();
    xvalue_bidi_iter operator--(int);
};

struct xvalue_random_iter {
    using value_type = double;
    using D          = long;

    value_type&& operator*() const;
    xvalue_random_iter& operator++();
    xvalue_random_iter operator++(int);

    xvalue_random_iter& operator--();
    xvalue_random_iter operator--(int);

    bool operator==(xvalue_random_iter const&) const;
    std::strong_ordering operator<=>(xvalue_random_iter const&) const;

    value_type&& operator[](D) const;
    xvalue_random_iter& operator-=(D);
    xvalue_random_iter operator-(D) const;
    D operator-(xvalue_random_iter const&) const;
    xvalue_random_iter& operator+=(D);
    xvalue_random_iter operator+(D) const;
    friend xvalue_random_iter operator+(D, xvalue_random_iter const&);
};

template <int I>
struct proxy_iterator {
    using difference_type = int;
    using value_type      = int;

    struct reference {
        operator int() const;
    };

    proxy_iterator() = default;
    constexpr explicit proxy_iterator(int* count) : pcount_{count} {}

    constexpr reference operator*() const noexcept {
        return {};
    }
    constexpr reference operator[](int) const noexcept;

    bool operator==(proxy_iterator) const;
    std::strong_ordering operator<=>(proxy_iterator const&) const;

    proxy_iterator& operator++();
    proxy_iterator operator++(int);

    constexpr proxy_iterator& operator--() noexcept {
        return *this;
    }
    proxy_iterator operator--(int);

    constexpr proxy_iterator operator+(int) const {
        return *this;
    }
    friend proxy_iterator operator+(int, proxy_iterator const&) {
        return {};
    }
    proxy_iterator& operator+=(int) const;

    proxy_iterator operator-(int) const;
    int operator-(proxy_iterator const&) const;
    proxy_iterator& operator-=(int) const;

    friend constexpr int iter_move(proxy_iterator const& i) {
        ++*i.pcount_;
        return 42;
    }

    template <int J>
    friend constexpr void iter_swap(proxy_iterator const& x, proxy_iterator<J> const& y) {
        assert(x.pcount_ == y.pcount_);
        *x.pcount_ -= J;
    }

    int* pcount_ = nullptr;
};
STATIC_ASSERT(std::random_access_iterator<proxy_iterator<0>>);
STATIC_ASSERT(std::indirectly_swappable<proxy_iterator<0>, proxy_iterator<1>>);

#pragma warning(push)
#pragma warning(disable : 4624) // '%s': destructor was implicitly defined as deleted

// These "meow_archetype" class templates are used to test the corresponding concept "meow", which has
// "meow_archetype_max" requirements. The class template is specialized with a size_t non-type template argument "I".
// When I is less than meow_archetype_max, meow_archetype<I> implements all but the I-th requirement of concept meow.
// When I is at least meow_archetype_max, meow_archetype<I> implements all requirements of concept meow. If meow fails
// to enforce a requirement then the corresponding specialization of meow_archetype will incorrectly satisfy the concept
// resulting in test failure. Conversely, if meow enforces an unspecified requirement then (ideally)
// meow_archetype<meow_archetype_max> will incorrectly not satisfy the concept resulting in test failure.

template <std::size_t>
struct destructible_archetype {};
template <>
struct destructible_archetype<0> {
    ~destructible_archetype() = delete;
};
inline constexpr std::size_t destructible_archetype_max = 1;

// clang-format off
#define COPYABLE_OPS(prefix)                                                    \
    prefix##_archetype(prefix##_archetype const&) requires (I != 1);            \
    prefix##_archetype(prefix##_archetype&&) requires (I == 2) = delete;        \
                                                                                \
    prefix##_archetype& operator=(prefix##_archetype const&) requires (I != 3); \
    prefix##_archetype& operator=(prefix##_archetype&&) requires (I == 4) = delete
// clang-format on

template <std::size_t I>
struct semiregular_archetype : destructible_archetype<I> {
    semiregular_archetype()
        requires (I != 5);
    COPYABLE_OPS(semiregular);
};

inline constexpr std::size_t semiregular_archetype_max = 6;

template <std::size_t>
struct weakly_incrementable_archetype_dt {
    using difference_type = int;
};
template <>
struct weakly_incrementable_archetype_dt<5> {};
template <>
struct weakly_incrementable_archetype_dt<6> {
    using difference_type = void;
};
template <>
struct weakly_incrementable_archetype_dt<7> {
    using difference_type = unsigned int;
};

template <std::size_t I, class Derived, class Post = void>
struct increment_ops {
    // clang-format off
    void operator++() requires (I == 8);
    Derived operator++() requires (I == 9);
    Derived& operator++() requires (I < 8 || I >= 10);
    Post operator++(int) requires (I != 10);
    // clang-format on
};

template <std::size_t I>
struct weakly_incrementable_archetype : destructible_archetype<I>,
                                        weakly_incrementable_archetype_dt<I>,
                                        increment_ops<I, weakly_incrementable_archetype<I>, void> {
    // clang-format off
    weakly_incrementable_archetype(weakly_incrementable_archetype const&)                       = delete;
    weakly_incrementable_archetype(weakly_incrementable_archetype&&) requires (I < 1 || I >= 3) = default;

    weakly_incrementable_archetype& operator=(weakly_incrementable_archetype const&) = delete;
    weakly_incrementable_archetype& operator=(weakly_incrementable_archetype&&) requires (I < 3 || I >= 5) = default;
    // clang-format on
};

inline constexpr std::size_t weakly_incrementable_archetype_max = 11;

template <std::size_t I>
struct incrementable_archetype : weakly_incrementable_archetype<I>,
                                 increment_ops<I, incrementable_archetype<I>, incrementable_archetype<I>> {
    incrementable_archetype()
        requires (I != 11);
    COPYABLE_OPS(incrementable);
    using increment_ops<I, incrementable_archetype<I>, incrementable_archetype<I>>::operator++;

    // clang-format off
    bool operator==(incrementable_archetype const&) const requires (I != 12);
    bool operator!=(incrementable_archetype const&) const
        requires (I == 13) = delete;
    // clang-format on
};

inline constexpr std::size_t incrementable_archetype_max = 14;

template <std::size_t I>
struct iterator_archetype : weakly_incrementable_archetype<I> {
    COPYABLE_OPS(iterator);

    // clang-format off
    iterator_archetype& operator++() requires (I > 9);
    void operator++(int) requires (I != 10);

    void operator*() requires (I == 11);
    int operator*() requires (I != 11);
    // clang-format on
};

inline constexpr std::size_t iterator_archetype_max = 12;

template <std::size_t I>
struct sentinel_archetype : semiregular_archetype<I> {
    sentinel_archetype()
        requires (I != 5);
    COPYABLE_OPS(sentinel);

    // clang-format off
    template <std::size_t J>
        requires (I != 6)
    bool operator==(iterator_archetype<J> const&) const;
    // clang-format on
};

inline constexpr std::size_t sentinel_archetype_max = 7;

template <std::size_t I>
struct sized_sentinel_archetype : sentinel_archetype<I> {
    sized_sentinel_archetype()
        requires (I != 5);
    COPYABLE_OPS(sized_sentinel);
};

template <std::size_t I, std::size_t J>
    requires (I == 8)
double operator-(sized_sentinel_archetype<I> const&, iterator_archetype<J> const&);

template <std::size_t I, std::size_t J>
    requires (I < 7 || I >= 9)
std::iter_difference_t<iterator_archetype<J>> operator-(
    sized_sentinel_archetype<I> const&, iterator_archetype<J> const&);

template <std::size_t I, std::size_t J>
    requires (I == 9)
double operator-(iterator_archetype<J> const&, sized_sentinel_archetype<I> const&);

template <std::size_t I, std::size_t J>
    requires (I < 9 || I >= 11)
std::iter_difference_t<iterator_archetype<J>> operator-(
    iterator_archetype<J> const&, sized_sentinel_archetype<I> const&);

template <class I>
inline constexpr bool std::disable_sized_sentinel_for<sized_sentinel_archetype<11>, I> = true;

inline constexpr std::size_t sized_sentinel_archetype_max = 12;

template <std::size_t I>
struct output_iterator_archetype : iterator_archetype<I>,
                                   increment_ops<I, output_iterator_archetype<I>, output_iterator_archetype<I>&> {
    COPYABLE_OPS(output_iterator);
    using increment_ops<I, output_iterator_archetype<I>, output_iterator_archetype<I>&>::operator++;

    // clang-format off
    // dereference ops from iterator_archetype
    void operator*() requires (I == 11);
    output_iterator_archetype& operator*() requires (I != 11);

    // indirectly_writable requirements
    void operator=(int) requires (I != 12);
    // clang-format on
};

inline constexpr std::size_t output_iterator_archetype_max = 13;

template <std::size_t>
struct input_iterator_archetype_types {
    using value_type = int;
};
template <>
struct input_iterator_archetype_types<12> {};
template <>
struct input_iterator_archetype_types<13> {
    using value_type = void;
};
template <>
struct input_iterator_archetype_types<14> {
    using iterator_category = void;
    using value_type        = int;
};
template <>
struct input_iterator_archetype_types<15> {
    using iterator_concept = void;
    using value_type       = int;
};

template <std::size_t I>
struct input_iterator_archetype : iterator_archetype<I>,
                                  input_iterator_archetype_types<I>,
                                  increment_ops<I, input_iterator_archetype<I>, void> {
    COPYABLE_OPS(input_iterator);
    using increment_ops<I, input_iterator_archetype<I>, void>::operator++;

    // clang-format off
    // dereference ops from iterator_archetype
    void operator*() const requires (I == 11);
    int& operator*() const requires (I != 11);
    // clang-format on
};

inline constexpr std::size_t input_iterator_archetype_max = 16;

template <std::size_t I>
struct forward_iterator_archetype : input_iterator_archetype<I>,
                                    increment_ops<I, forward_iterator_archetype<I>, forward_iterator_archetype<I>> {
    forward_iterator_archetype()
        requires (I != 16);
    COPYABLE_OPS(forward_iterator);
    using increment_ops<I, forward_iterator_archetype<I>, forward_iterator_archetype<I>>::operator++;

    // clang-format off
    bool operator==(forward_iterator_archetype const&) const requires (I != 17);
    bool operator!=(forward_iterator_archetype const&) const requires (I == 18) = delete;
    // clang-format on
};

inline constexpr std::size_t forward_iterator_archetype_max = 19;

template <std::size_t I, class Derived>
struct decrement_ops {
    // clang-format off
    void operator--() requires (I == 19);
    Derived operator--() requires (I == 20);
    Derived& operator--() requires (I < 19 || I >= 21);
    Derived operator--(int) requires (I != 21);
    // clang-format on
};

template <std::size_t I>
struct bidi_iterator_archetype : forward_iterator_archetype<I>,
                                 increment_ops<I, bidi_iterator_archetype<I>, bidi_iterator_archetype<I>>,
                                 decrement_ops<I, bidi_iterator_archetype<I>> {
    bidi_iterator_archetype()
        requires (I != 16);
    COPYABLE_OPS(bidi_iterator);
    using increment_ops<I, bidi_iterator_archetype<I>, bidi_iterator_archetype<I>>::operator++;
};

inline constexpr std::size_t bidi_iterator_archetype_max = 22;

template <std::size_t I>
struct random_iterator_archetype : bidi_iterator_archetype<I>,
                                   increment_ops<I, random_iterator_archetype<I>, random_iterator_archetype<I>>,
                                   decrement_ops<I, random_iterator_archetype<I>> {
    random_iterator_archetype()
        requires (I != 16);
    COPYABLE_OPS(random_iterator);
    using increment_ops<I, random_iterator_archetype<I>, random_iterator_archetype<I>>::operator++;
    using decrement_ops<I, random_iterator_archetype<I>>::operator--;

    // clang-format off
    std::strong_ordering operator<=>(random_iterator_archetype const&) const requires (I != 22);

    int operator-(random_iterator_archetype const&) const requires (I != 5 && I != 23);

    random_iterator_archetype& operator+=(int) requires (I != 24);
    random_iterator_archetype operator+(int) const requires (I != 25);
    friend random_iterator_archetype operator+(int, random_iterator_archetype const&) requires (I != 26) {}

    random_iterator_archetype& operator-=(int) requires (I != 27);
    random_iterator_archetype operator-(int) const requires (I != 28);

    void operator[](int) const requires (I == 29);
    int operator[](int) const requires (I == 30);
    int& operator[](int) const requires (I < 29 || I >= 31);
    // clang-format on
};

inline constexpr std::size_t random_iterator_archetype_max = 31;

template <std::size_t I>
struct contig_iterator_archetype_types : random_iterator_archetype<I> {
    using iterator_concept  = std::contiguous_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
};
template <std::size_t I>
    requires (I == 14 || I == 15 || I == 31)
struct contig_iterator_archetype_types<I> : random_iterator_archetype<I> {};
template <>
struct contig_iterator_archetype_types<32> : random_iterator_archetype<32> {
    using iterator_category = std::random_access_iterator_tag;
};
template <>
struct contig_iterator_archetype_types<33> : random_iterator_archetype<33> {
    using iterator_concept  = std::input_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
};

template <std::size_t I>
struct contig_iterator_archetype : increment_ops<I, contig_iterator_archetype<I>, contig_iterator_archetype<I>>,
                                   decrement_ops<I, contig_iterator_archetype<I>>,
                                   contig_iterator_archetype_types<I> {
    contig_iterator_archetype()
        requires (I != 16);
    COPYABLE_OPS(contig_iterator);
    using increment_ops<I, contig_iterator_archetype<I>, contig_iterator_archetype<I>>::operator++;
    using decrement_ops<I, contig_iterator_archetype<I>>::operator--;

    // clang-format off
    int operator-(contig_iterator_archetype const&) const requires (I != 5 && I != 23);

    contig_iterator_archetype& operator+=(int) requires (I != 24);
    contig_iterator_archetype operator+(int) const requires (I != 25);
    friend contig_iterator_archetype operator+(int, contig_iterator_archetype const&) requires (I != 26) {}

    contig_iterator_archetype& operator-=(int) requires (I != 27);
    contig_iterator_archetype operator-(int) const requires (I != 28);
    // clang-format on
};

template <std::size_t I>
struct std::pointer_traits<contig_iterator_archetype<I>> {
    using pointer         = contig_iterator_archetype<I>;
    using element_type    = int;
    using difference_type = iter_difference_t<pointer>;

    static pointer pointer_to(element_type&);
    static element_type* to_address(const pointer&);
};

inline constexpr std::size_t contig_iterator_archetype_max = 34;

#pragma warning(pop)

struct iter_concept_example {
    // bidirectional_iterator and Cpp17InputIterator, but not Cpp17ForwardIterator (N4928 [iterator.concepts.general]/2)

    using value_type      = int;
    using difference_type = int;

    int operator*() const;

    iter_concept_example& operator++();
    iter_concept_example operator++(int);

    iter_concept_example& operator--();
    iter_concept_example operator--(int);

    bool operator==(iter_concept_example) const;
    bool operator!=(iter_concept_example) const;
};

namespace iterator_synopsis_test {
    using std::iter_reference_t, std::same_as;

    // Validate iter_reference_t
    template <class T>
    concept can_iter_ref = requires { typename iter_reference_t<T>; };

    STATIC_ASSERT(!can_iter_ref<int>);
    STATIC_ASSERT(!can_iter_ref<void>);
    STATIC_ASSERT(!can_iter_ref<empty_type>);

    STATIC_ASSERT(same_as<iter_reference_t<int*>, int&>);
    STATIC_ASSERT(same_as<iter_reference_t<int const*>, int const&>);
    STATIC_ASSERT(same_as<iter_reference_t<int volatile*>, int volatile&>);
    STATIC_ASSERT(same_as<iter_reference_t<int const volatile*>, int const volatile&>);
    STATIC_ASSERT(same_as<iter_reference_t<int[]>, int&>);
    STATIC_ASSERT(same_as<iter_reference_t<int[4]>, int&>);
    STATIC_ASSERT(same_as<iter_reference_t<int (*)[4]>, int (&)[4]>);
    STATIC_ASSERT(same_as<iter_reference_t<int (*)(int)>, int (&)(int)>);

    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int>>, int>);
    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int&>>, int&>);
    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int&&>>, int&&>);
    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int const&>>, int const&>);
    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int const&&>>, int const&&>);
    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int volatile&>>, int volatile&>);
    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int volatile&&>>, int volatile&&>);
    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int const volatile&>>, int const volatile&>);
    STATIC_ASSERT(same_as<iter_reference_t<dereferences_to<int const volatile&&>>, int const volatile&&>);

    struct with_bogus_typedefs : dereferences_to<int&> {
        using value_type = double;
        using reference  = double;
    };
    STATIC_ASSERT(same_as<iter_reference_t<with_bogus_typedefs>, int&>);
} // namespace iterator_synopsis_test

namespace incrementable_traits_test {
#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T, class D = no_such_type>
    constexpr bool test_difference() {
        using std::incrementable_traits, std::iter_difference_t, std::same_as;

        if constexpr (same_as<D, no_such_type>) {
            STATIC_ASSERT(!has_member_difference_type<incrementable_traits<T>>);
            STATIC_ASSERT(!has_member_difference_type<incrementable_traits<T const>>);
            STATIC_ASSERT(!has_iter_diff<T>);
            STATIC_ASSERT(!has_iter_diff<T const>);
            return false;
        } else {
            STATIC_ASSERT(same_as<typename incrementable_traits<T>::difference_type, D>);
            STATIC_ASSERT(same_as<typename incrementable_traits<T const>::difference_type, D>);
            STATIC_ASSERT(same_as<iter_difference_t<T>, D>);
            STATIC_ASSERT(same_as<iter_difference_t<T const>, D>);
            return true;
        }
    }
#pragma warning(pop)

    template <class T>
    struct with_difference_type {
        using difference_type = T;
    };

    STATIC_ASSERT(!test_difference<void>());
    STATIC_ASSERT(!test_difference<void*>());
    STATIC_ASSERT(!test_difference<int(int)>());
    STATIC_ASSERT(!test_difference<int(int) const>());
    STATIC_ASSERT(!test_difference<int (*)(int)>());

    STATIC_ASSERT(test_difference<int*, std::ptrdiff_t>());
    STATIC_ASSERT(test_difference<int const*, std::ptrdiff_t>());
    STATIC_ASSERT(test_difference<int[], std::ptrdiff_t>());
    STATIC_ASSERT(test_difference<int[4], std::ptrdiff_t>());
    STATIC_ASSERT(test_difference<int, int>());
    STATIC_ASSERT(test_difference<unsigned int, int>());
    STATIC_ASSERT(test_difference<with_difference_type<long>, long>());
    STATIC_ASSERT(test_difference<with_difference_type<void>, void>());

    STATIC_ASSERT(test_difference<my_iterator, long>());

    struct integral_difference {
        int operator-(integral_difference) const;
    };
    STATIC_ASSERT(test_difference<integral_difference, int>());

    struct non_integral_difference {
        void* operator-(non_integral_difference) const;
    };
    STATIC_ASSERT(!test_difference<non_integral_difference>());
} // namespace incrementable_traits_test

namespace indirectly_readable_traits_test {
#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T, class V = no_such_type>
    constexpr bool test_value() noexcept {
        using std::indirectly_readable_traits, std::iter_value_t, std::same_as;

        if constexpr (same_as<V, no_such_type>) {
            STATIC_ASSERT(!has_member_value_type<indirectly_readable_traits<T>>);
            STATIC_ASSERT(!has_member_value_type<indirectly_readable_traits<T const>>);
            STATIC_ASSERT(!has_iter_value<indirectly_readable_traits<T>>);
            STATIC_ASSERT(!has_iter_value<indirectly_readable_traits<T const>>);
            return false;
        } else {
            STATIC_ASSERT(same_as<typename indirectly_readable_traits<T>::value_type, V>);
            STATIC_ASSERT(same_as<typename indirectly_readable_traits<T const>::value_type, V>);
            STATIC_ASSERT(same_as<iter_value_t<T>, V>);
            STATIC_ASSERT(same_as<iter_value_t<T const>, V>);
            return true;
        }
    }
#pragma warning(pop)

    template <class T>
    struct with_element_type {
        using element_type = T;
    };

    struct A {
        using value_type = int;
        int& operator*() const;
    };
    struct B : A {
        using value_type = double;
    };

    STATIC_ASSERT(!test_value<void>());
    STATIC_ASSERT(!test_value<void*>());
    STATIC_ASSERT(!test_value<int(int)>());
    STATIC_ASSERT(!test_value<int(int) const>());
    STATIC_ASSERT(!test_value<int (*)(int)>());

    STATIC_ASSERT(test_value<int*, int>());
    STATIC_ASSERT(test_value<int const*, int>());
    STATIC_ASSERT(test_value<int[], int>());
    STATIC_ASSERT(test_value<int[4], int>());
    STATIC_ASSERT(test_value<A, int>());
    STATIC_ASSERT(test_value<B, double>());
    STATIC_ASSERT(test_value<with_element_type<int>, int>());
    STATIC_ASSERT(test_value<with_element_type<int const>, int>());

    STATIC_ASSERT(test_value<my_iterator, char>());
} // namespace indirectly_readable_traits_test

namespace iterator_traits_test {
    using std::incrementable_traits, std::indirectly_readable_traits, std::iterator_traits, std::iter_value_t,
        std::iter_difference_t, std::same_as, std::output_iterator_tag, std::input_iterator_tag,
        std::forward_iterator_tag, std::bidirectional_iterator_tag, std::random_access_iterator_tag,
        std::contiguous_iterator_tag;

    template <class T>
    struct with_iter_concept {
        using iterator_concept = T;

        auto operator<=>(with_iter_concept const&) const = default;
    };
    template <class T>
    struct with_iter_cat {
        using iterator_category = T;

        auto operator<=>(with_iter_cat const&) const = default;
    };
    template <class T>
    struct with_difference {
        using difference_type = T;

        auto operator<=>(with_difference const&) const = default;
    };
    template <class T>
    struct with_value {
        using value_type = T;

        auto operator<=>(with_value const&) const = default;
    };
    template <class T>
    struct with_reference {
        using reference = T;

        auto operator<=>(with_reference const&) const = default;
    };
    template <class T>
    struct with_pointer {
        using pointer = T;

        auto operator<=>(with_pointer const&) const = default;
    };

    // clang-format off
    template <class T>
    concept has_empty_traits = !(requires { typename iterator_traits<T>::iterator_concept; }
        || requires { typename iterator_traits<T>::iterator_category; }
        || requires { typename iterator_traits<T>::value_type; }
        || requires { typename iterator_traits<T>::difference_type; }
        || requires { typename iterator_traits<T>::pointer; }
        || requires { typename iterator_traits<T>::reference; });
    // clang-format on

    // Verify that iterator_traits pulls from nested member typedefs when the four key names are defined
    using four_members =
        derives_from<with_iter_cat<int>, with_difference<double>, with_value<char>, with_reference<long>>;
    using IT4 = iterator_traits<four_members>;
    STATIC_ASSERT(!has_member_iter_concept<IT4>);
    STATIC_ASSERT(same_as<typename IT4::iterator_category, int>);
    STATIC_ASSERT(same_as<typename IT4::value_type, char>);
    STATIC_ASSERT(same_as<iter_value_t<four_members>, char>);
    STATIC_ASSERT(same_as<typename IT4::difference_type, double>);
    STATIC_ASSERT(same_as<iter_difference_t<four_members>, double>);
    STATIC_ASSERT(same_as<typename IT4::pointer, void>);
    STATIC_ASSERT(same_as<typename IT4::reference, long>);

    // Verify that iterator_traits pulls from nested member typedefs when the four key names and pointer are defined
    using five_members = derives_from<with_iter_cat<int>, with_difference<double>, with_value<char>,
        with_reference<long>, with_pointer<bool>>;
    using IT5          = iterator_traits<five_members>;
    STATIC_ASSERT(!has_member_iter_concept<IT5>);
    STATIC_ASSERT(same_as<typename IT5::iterator_category, int>);
    STATIC_ASSERT(same_as<typename IT5::value_type, char>);
    STATIC_ASSERT(same_as<iter_value_t<five_members>, char>);
    STATIC_ASSERT(same_as<typename IT5::difference_type, double>);
    STATIC_ASSERT(same_as<iter_difference_t<five_members>, double>);
    STATIC_ASSERT(same_as<typename IT5::pointer, bool>);
    STATIC_ASSERT(same_as<typename IT5::reference, long>);

    // A type missing one of the four members has no iterator_traits (when it implements none of the operations needed
    // to satisfy the exposition-only Cpp17 concepts)
    STATIC_ASSERT(has_empty_traits<derives_from<with_difference<double>, with_value<char>, with_reference<long>>>);
    STATIC_ASSERT(has_empty_traits<derives_from<with_iter_cat<int>, with_value<char>, with_reference<long>>>);
    STATIC_ASSERT(has_empty_traits<derives_from<with_iter_cat<int>, with_difference<double>, with_reference<long>>>);
    STATIC_ASSERT(has_empty_traits<derives_from<with_iter_cat<int>, with_difference<double>, with_value<char>>>);

    // Validate the member typedefs of iterator_traits<T> against Concept, Category, etc.
    template <class T, class Concept, class Category, class Value, class Difference, class Pointer, class Reference>
    constexpr bool check() {
        if constexpr (same_as<Concept, no_such_type>) {
            STATIC_ASSERT(!has_member_iter_concept<iterator_traits<T>>);
        } else {
            STATIC_ASSERT(same_as<typename iterator_traits<T>::iterator_concept, Concept>);
        }
        STATIC_ASSERT(same_as<typename iterator_traits<T>::iterator_category, Category>);
        STATIC_ASSERT(same_as<typename iterator_traits<T>::value_type, Value>);
        if constexpr (!same_as<Value, void>) {
            STATIC_ASSERT(same_as<iter_value_t<T>, Value>);
            STATIC_ASSERT(same_as<typename indirectly_readable_traits<T>::value_type, Value>);
        }
        STATIC_ASSERT(same_as<typename iterator_traits<T>::difference_type, Difference>);
        if constexpr (!same_as<Difference, void>) {
            STATIC_ASSERT(same_as<iter_difference_t<T>, Difference>);
            STATIC_ASSERT(same_as<typename incrementable_traits<T>::difference_type, Difference>);
        }
        STATIC_ASSERT(same_as<typename iterator_traits<T>::pointer, Pointer>);
        STATIC_ASSERT(same_as<typename iterator_traits<T>::reference, Reference>);
        return true;
    }

    // N4928 [iterator.traits]/3.2: "Otherwise, if I satisfies the exposition-only concept cpp17-input-iterator..."

    // N4928 [iterator.traits]:
    // * 3.2.1: "... Otherwise, pointer names void."
    // * 3.2.2: "... Otherwise, reference names iter_reference_t<I>."
    // * 3.2.3.4 "... Otherwise, iterator_category names... input_iterator_tag."
    STATIC_ASSERT(check<simple_input_iter, no_such_type, input_iterator_tag, double, long, void, double>());

    // N4928 [iterator.traits]:
    // * 3.2.1: "... Otherwise, pointer names void."
    // * 3.2.2: "... Otherwise, reference names iter_reference_t<I>."
    // * 3.2.3.3 "... Otherwise, iterator_category names... forward_iterator_tag if I satisfies cpp17-forward-iterator."
    STATIC_ASSERT(
        check<simple_forward_iter<>, no_such_type, forward_iterator_tag, double, long, void, double const&>());
    STATIC_ASSERT(check<xvalue_forward_iter, no_such_type, forward_iterator_tag, double, long, void, double&&>());
    // N4928 [iterator.traits]/3.2.1: "... Otherwise, if decltype(declval<I&>().operator->()) is well-formed, then
    // pointer names that type."
    STATIC_ASSERT(check<simple_forward_iter<arrow_base<double const*>>, no_such_type, forward_iterator_tag, double,
        long, double const*, double const&>());
    // N4928 [iterator.traits]/3.2.3: "If the qualified-id I::iterator_category is valid and denotes a type,
    // iterator_category names that type."
    STATIC_ASSERT(check<simple_forward_iter<with_iter_cat<output_iterator_tag>>, no_such_type, output_iterator_tag,
        double, long, void, double const&>());
    STATIC_ASSERT(check<simple_forward_iter<with_iter_cat<input_iterator_tag>>, no_such_type, input_iterator_tag,
        double, long, void, double const&>());

    // N4928 [iterator.traits]:
    // * 3.2.1: "... Otherwise, pointer names void."
    // * 3.2.2: "If the qualified-id I::reference is valid and denotes a type, reference names that type."
    // * 3.2.3.2 "... Otherwise, iterator_category names... bidirectional_iterator_tag if I satisfies
    // cpp17-bidirectional-iterator."
    STATIC_ASSERT(
        check<simple_bidi_iter<>, no_such_type, bidirectional_iterator_tag, double, long, void, double const&>());
    STATIC_ASSERT(check<xvalue_bidi_iter, no_such_type, bidirectional_iterator_tag, double, long, void, double&&>());
    // N4928 [iterator.traits]/3.2.1: "... Otherwise, if decltype(declval<I&>().operator->()) is well-formed, then
    // pointer names that type."
    STATIC_ASSERT(check<simple_bidi_iter<arrow_base<double const*>>, no_such_type, bidirectional_iterator_tag, double,
        long, double const*, double const&>());
    // N4928 [iterator.traits]/3.2.3: "If the qualified-id I::iterator_category is valid and denotes a type,
    // iterator_category names that type."
    STATIC_ASSERT(check<simple_bidi_iter<with_iter_cat<output_iterator_tag>>, no_such_type, output_iterator_tag, double,
        long, void, double const&>());
    STATIC_ASSERT(check<simple_bidi_iter<with_iter_cat<input_iterator_tag>>, no_such_type, input_iterator_tag, double,
        long, void, double const&>());
    STATIC_ASSERT(check<simple_bidi_iter<with_iter_cat<forward_iterator_tag>>, no_such_type, forward_iterator_tag,
        double, long, void, double const&>());

    // N4928 [iterator.traits]:
    // * 3.2.1: "... Otherwise, pointer names void."
    // * 3.2.2: "... Otherwise, reference names iter_reference_t<I>."
    // * 3.2.3.1 "... Otherwise, iterator_category names... random_access_iterator_tag if I satisfies
    // cpp17-random-access-iterator."
    STATIC_ASSERT(
        check<simple_random_iter<>, no_such_type, random_access_iterator_tag, double, long, void, double const&>());
    STATIC_ASSERT(check<xvalue_random_iter, no_such_type, random_access_iterator_tag, double, long, void, double&&>());
    STATIC_ASSERT(
        check<simple_contiguous_iter<>, no_such_type, random_access_iterator_tag, double, long, void, double const&>());
    // N4928 [iterator.traits]/3.2.1: "... Otherwise, if decltype(declval<I&>().operator->()) is well-formed, then
    // pointer names that type."
    STATIC_ASSERT(check<simple_random_iter<arrow_base<double const*>>, no_such_type, random_access_iterator_tag, double,
        long, double const*, double const&>());
    STATIC_ASSERT(check<simple_contiguous_iter<arrow_base<double const*>>, no_such_type, random_access_iterator_tag,
        double, long, double const*, double const&>());
    // N4928 [iterator.traits]/3.2.3: "If the qualified-id I::iterator_category is valid and denotes a type,
    // iterator_category names that type."
    STATIC_ASSERT(check<simple_random_iter<with_iter_cat<output_iterator_tag>>, no_such_type, output_iterator_tag,
        double, long, void, double const&>());
    STATIC_ASSERT(check<simple_contiguous_iter<with_iter_cat<output_iterator_tag>>, no_such_type, output_iterator_tag,
        double, long, void, double const&>());
    STATIC_ASSERT(check<simple_random_iter<with_iter_cat<forward_iterator_tag>>, no_such_type, forward_iterator_tag,
        double, long, void, double const&>());
    STATIC_ASSERT(check<simple_contiguous_iter<with_iter_cat<forward_iterator_tag>>, no_such_type, forward_iterator_tag,
        double, long, void, double const&>());
    STATIC_ASSERT(check<simple_random_iter<with_iter_cat<bidirectional_iterator_tag>>, no_such_type,
        bidirectional_iterator_tag, double, long, void, double const&>());
    STATIC_ASSERT(check<simple_contiguous_iter<with_iter_cat<bidirectional_iterator_tag>>, no_such_type,
        bidirectional_iterator_tag, double, long, void, double const&>());
    STATIC_ASSERT(check<simple_random_iter<with_iter_cat<contiguous_iterator_tag>>, no_such_type,
        contiguous_iterator_tag, double, long, void, double const&>());
    STATIC_ASSERT(check<simple_contiguous_iter<with_iter_cat<contiguous_iterator_tag>>, no_such_type,
        contiguous_iterator_tag, double, long, void, double const&>());

    // N4928 [iterator.traits]/3.3: "Otherwise, if I satisfies the exposition-only concept cpp17-iterator..."
    template <class Base = empty_type>
    struct simple_output_iter : Base {
        simple_output_iter const& operator*() const;
        simple_output_iter& operator++();
        simple_output_iter& operator++(int);
    };
    // "... If the qualified-id incrementable_traits<I>::difference_type is valid and denotes a type, then
    // difference_type names that type; ..."
    STATIC_ASSERT(
        check<simple_output_iter<with_difference<long>>, no_such_type, output_iterator_tag, void, long, void, void>());
    // "... otherwise, it names void."
    STATIC_ASSERT(check<simple_output_iter<>, no_such_type, output_iterator_tag, void, void, void, void>());

    // N4928 [iterator.traits]/3.4: "Otherwise, iterator_traits<I> has no members by any of the above names."
    STATIC_ASSERT(has_empty_traits<int>);
    STATIC_ASSERT(has_empty_traits<void>);
    STATIC_ASSERT(has_empty_traits<int(int)>);
    STATIC_ASSERT(has_empty_traits<int(int) const>);

    // N4928 [iterator.traits]/5: "iterator_traits is specialized for pointers..."
    STATIC_ASSERT(check<int*, contiguous_iterator_tag, random_access_iterator_tag, int, std::ptrdiff_t, int*, int&>());
    STATIC_ASSERT(check<int const*, contiguous_iterator_tag, random_access_iterator_tag, int, std::ptrdiff_t,
        int const*, int const&>());
    STATIC_ASSERT(check<int (*)[4], contiguous_iterator_tag, random_access_iterator_tag, int[4], std::ptrdiff_t,
        int (*)[4], int (&)[4]>());
    // pointers to non-object types are not iterators
    STATIC_ASSERT(has_empty_traits<int (*)(int)>);
    STATIC_ASSERT(has_empty_traits<void*>);

    STATIC_ASSERT(check<iter_concept_example, no_such_type, input_iterator_tag, int, int, void, int>());
} // namespace iterator_traits_test

namespace iterator_cust_move_test {
    using std::iter_rvalue_reference_t, std::same_as;

    template <class T>
    concept can_iter_move = requires(T&& t) { ranges::iter_move(std::forward<T>(t)); };
    template <class T>
    concept can_iter_rvalue_ref = requires { typename iter_rvalue_reference_t<T>; };

    // N4928 [iterator.cust.move]/1.1 "iter_move(E), if [...] iter_move(E) is a well-formed expression when [...]
    // performing argument-dependent lookup only."
    struct friend_hook {
        friend constexpr double iter_move(friend_hook) noexcept {
            return 3.14;
        }
    };
    STATIC_ASSERT(same_as<decltype(ranges::iter_move(friend_hook{})), double>);
    STATIC_ASSERT(!can_iter_rvalue_ref<friend_hook>);
    STATIC_ASSERT(ranges::iter_move(friend_hook{}) == 3.14);
    STATIC_ASSERT(noexcept(ranges::iter_move(friend_hook{})));

    struct non_member_hook {};
    constexpr bool iter_move(non_member_hook) noexcept {
        return false;
    }
    STATIC_ASSERT(same_as<decltype(ranges::iter_move(non_member_hook{})), bool>);
    STATIC_ASSERT(!can_iter_rvalue_ref<non_member_hook>);
    STATIC_ASSERT(ranges::iter_move(non_member_hook{}) == false);
    STATIC_ASSERT(noexcept(ranges::iter_move(non_member_hook{})));

    enum class E1 { x };
    constexpr E1 iter_move(E1) noexcept {
        return E1::x;
    }
    STATIC_ASSERT(same_as<decltype(ranges::iter_move(E1::x)), E1>);
    STATIC_ASSERT(!can_iter_rvalue_ref<E1>);
    STATIC_ASSERT(static_cast<int>(ranges::iter_move(E1::x)) == 0);
    STATIC_ASSERT(noexcept(ranges::iter_move(E1::x)));

    // N4928 [iterator.cust.move]/1.2.1 "if *E is an lvalue, std::move(*E)"
    static constexpr int some_ints[] = {0, 1, 2, 3};
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<int*>, int&&>);
    STATIC_ASSERT(ranges::iter_move(&some_ints[1]) == 1);
    STATIC_ASSERT(noexcept(ranges::iter_move(&some_ints[1])));

    STATIC_ASSERT(same_as<iter_rvalue_reference_t<int const*>, int const&&>);
    STATIC_ASSERT(ranges::iter_move(static_cast<int const*>(&some_ints[2])) == 2);
    STATIC_ASSERT(noexcept(ranges::iter_move(static_cast<int const*>(&some_ints[2]))));

    STATIC_ASSERT(same_as<iter_rvalue_reference_t<int[]>, int&&>);
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-1008447
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<int(int)>, int (&)(int)>);
#else // ^^^ no workaround / workaround vvv
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<int(int)>, int (*)(int)>);
#endif // TRANSITION, VSO-1008447

    STATIC_ASSERT(same_as<iter_rvalue_reference_t<int[4]>, int&&>);
    STATIC_ASSERT(ranges::iter_move(some_ints) == 0);
    STATIC_ASSERT(noexcept(ranges::iter_move(some_ints)));

    constexpr int f(int i) noexcept {
        return i + 1;
    }
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-1008447
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<int (*)(int)>, int (&)(int)>);
#else // ^^^ no workaround / workaround vvv
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<int (*)(int)>, int (&&)(int)>);
#endif // TRANSITION, VSO-1008447
    STATIC_ASSERT(ranges::iter_move (&f)(42) == 43);
    STATIC_ASSERT(noexcept(ranges::iter_move(&f)));

    struct ref_is_lvalue {
        constexpr int const& operator*() const {
            return some_ints[1];
        }
    };
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<ref_is_lvalue>, int const&&>);
    STATIC_ASSERT(ranges::iter_move(ref_is_lvalue{}) == 1);

    struct with_bogus_typedefs : ref_is_lvalue {
        using value_type = void;
        using reference  = void;
    };
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<with_bogus_typedefs>, int const&&>); // oblivious to nested types
    STATIC_ASSERT(ranges::iter_move(with_bogus_typedefs{}) == 1);

    // N4928 [iterator.cust.move]/1.2.2 "otherwise, *E."
    struct ref_is_prvalue {
        int operator*() const {
            return 42;
        }
    };
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<ref_is_prvalue>, int>);

    struct ref_is_xvalue {
        int&& operator*() const;
    };
    STATIC_ASSERT(same_as<iter_rvalue_reference_t<ref_is_xvalue>, int&&>);
    STATIC_ASSERT(!noexcept(ranges::iter_move(ref_is_xvalue{})));

    // N4928 [iterator.cust.move]/1.3 "Otherwise, ranges::iter_move(E) is ill-formed."
    STATIC_ASSERT(!can_iter_move<int>);
    STATIC_ASSERT(!can_iter_move<void>);
    STATIC_ASSERT(!can_iter_move<int(int) const>);

    enum class E2 { x };
    STATIC_ASSERT(!can_iter_move<E2>);
    STATIC_ASSERT(!can_iter_rvalue_ref<E2>);
} // namespace iterator_cust_move_test

namespace iterator_cust_swap_test {
    using std::indirectly_movable_storable, std::iter_reference_t, std::indirectly_readable, std::remove_reference_t,
        std::same_as, std::swappable_with;

    template <class T, class U>
    concept can_iter_swap = requires(T&& t, U&& u) { ranges::iter_swap(std::forward<T>(t), std::forward<U>(u)); };

    // N4928 [iterator.cust.swap]/4.1: "(void)iter_swap(E1, E2), if [...] iter_swap(E1, E2) is a
    // well-formed expression with overload resolution performed in a context [...]"
    namespace adl_barrier {
        template <class T, class U>
        void iter_swap(T, U) = delete;

        template <class T, class U = T>
        concept bullet1 = requires(T&& t, U&& u) { iter_swap(std::forward<T>(t), std::forward<U>(u)); };
    } // namespace adl_barrier
    using adl_barrier::bullet1;

    struct friend_hook {
        friend constexpr int iter_swap(friend_hook, friend_hook) noexcept {
            return 42;
        }
    };
    STATIC_ASSERT(bullet1<friend_hook>);
    STATIC_ASSERT(same_as<decltype(ranges::iter_swap(friend_hook{}, friend_hook{})), void>);
    STATIC_ASSERT((ranges::iter_swap(friend_hook{}, friend_hook{}), true));
    STATIC_ASSERT(noexcept(ranges::iter_swap(friend_hook{}, friend_hook{})));

    struct non_member_hook {};
    constexpr char iter_swap(non_member_hook, non_member_hook) noexcept {
        return 'x';
    }
    STATIC_ASSERT(bullet1<non_member_hook>);
    STATIC_ASSERT(same_as<decltype(ranges::iter_swap(non_member_hook{}, non_member_hook{})), void>);
    STATIC_ASSERT((ranges::iter_swap(non_member_hook{}, non_member_hook{}), true));
    STATIC_ASSERT(noexcept(ranges::iter_swap(non_member_hook{}, non_member_hook{})));

    enum class E1 { x };
    constexpr void iter_swap(E1, E1) {}
    STATIC_ASSERT(bullet1<E1>);
    STATIC_ASSERT((ranges::iter_swap(E1::x, E1::x), true));

    // N4928 [iterator.cust.swap]/4.2: "Otherwise, if the types of E1 and E2 each model indirectly_readable,
    // and if the reference types of E1 and E2 model swappable_with, then ranges::swap(*E1, *E2)."
    // clang-format off
    template <class T, class U = T>
    concept bullet2 = !bullet1<T, U> && indirectly_readable<remove_reference_t<T>>
        && indirectly_readable<remove_reference_t<U>> && swappable_with<iter_reference_t<T>, iter_reference_t<U>>;
    // clang-format on

    constexpr bool test() {
        // This test notably executes both at runtime and at compiletime.
        STATIC_ASSERT(bullet2<int*>);

        int i0 = 42, i1 = 13;
        STATIC_ASSERT(same_as<decltype(ranges::iter_swap(&i0, &i1)), void>);
        ranges::iter_swap(&i0, &i1);
        assert(i0 == 13);
        assert(i1 == 42);
        STATIC_ASSERT(noexcept(ranges::iter_swap(&i0, &i1)));
        return true;
    }
    STATIC_ASSERT(test());

    template <int>
    struct swap_proxy_ref {
        constexpr operator int() const noexcept {
            return 42;
        }
        template <int X>
        friend constexpr void swap(swap_proxy_ref, swap_proxy_ref<X>) noexcept {}
    };
    template <int X>
    struct swap_proxy_readable {
        using value_type = int;

        constexpr swap_proxy_ref<X> operator*() const noexcept {
            return {};
        }
    };
} // namespace iterator_cust_swap_test

template <int X, int Y>
struct std::common_type<iterator_cust_swap_test::swap_proxy_ref<X>, iterator_cust_swap_test::swap_proxy_ref<Y>> {
    using type = int;
};

namespace iterator_cust_swap_test {
    STATIC_ASSERT(bullet2<swap_proxy_readable<0>, swap_proxy_readable<1>>);
    STATIC_ASSERT(same_as<decltype(ranges::iter_swap(swap_proxy_readable<0>{}, swap_proxy_readable<1>{})), void>);
    STATIC_ASSERT((ranges::iter_swap(swap_proxy_readable<0>{}, swap_proxy_readable<1>{}), true));
    STATIC_ASSERT(noexcept(ranges::iter_swap(swap_proxy_readable<0>{}, swap_proxy_readable<1>{})));

    // N4928 [iterator.cust.swap]/4.3: "Otherwise, if the types T1 and T2 of E1 and E2 model
    // indirectly_movable_storable<T1, T2> and indirectly_movable_storable<T2, T1>..."
    // clang-format off
    template <class T, class U = T>
    concept bullet3 = !bullet1<T, U> && !bullet2<T, U> && indirectly_movable_storable<T, U>
        && indirectly_movable_storable<U, T>;

    STATIC_ASSERT(bullet3<int*, long*> && can_iter_swap<int*, long*>);
    // clang-format on

    template <int>
    struct unswap_proxy_ref {
        operator int() const noexcept;
        unswap_proxy_ref const& operator=(int&&) const noexcept;
    };
    template <int X>
    struct unswap_proxy_readable {
        using value_type = int;

        unswap_proxy_ref<X> operator*() const noexcept;
        friend int&& iter_move(unswap_proxy_readable) noexcept;
    };
    STATIC_ASSERT(bullet3<unswap_proxy_readable<0>, unswap_proxy_readable<1>>);
    STATIC_ASSERT(same_as<decltype(ranges::iter_swap(unswap_proxy_readable<0>{}, unswap_proxy_readable<1>{})), void>);
    STATIC_ASSERT(noexcept(ranges::iter_swap(unswap_proxy_readable<0>{}, unswap_proxy_readable<1>{})));

    // N4928 [iterator.cust.swap]/4.4: "Otherwise, ranges::iter_swap(E1, E2) is ill-formed."
    template <class T, class U>
    concept bullet4 = (!can_iter_swap<T, U>);

    STATIC_ASSERT(bullet4<void, void>);
    STATIC_ASSERT(bullet4<int, int>);
    STATIC_ASSERT(bullet4<int, void>);
    STATIC_ASSERT(bullet4<void, int>);
    STATIC_ASSERT(bullet4<void*, void*>);
    STATIC_ASSERT(bullet4<int*, void*>);
    STATIC_ASSERT(bullet4<int(), int()>);
    STATIC_ASSERT(bullet4<int() const, int() volatile>);
    STATIC_ASSERT(bullet4<int (*)(), int (*)()>);
    STATIC_ASSERT(bullet4<int (&)(), int (&)()>);

    // verify that a hook found by ADL must be more specialized than the poison pill to be used
    namespace poison_pill {
        struct S {
            using value_type = int;
            int& i;

            constexpr int& operator*() const noexcept {
                return i;
            }
        };
        template <class T, class U>
        void iter_swap(T&, U&) { // this function is unordered with the poison pill
            STATIC_ASSERT(always_false<T>);
        }

        // clang-format off
        STATIC_ASSERT(bullet2<S&, S&> && can_iter_swap<S&, S&>);
        // clang-format on

        constexpr bool test() {
            {
                int i0 = 42;
                int i1 = 13;
                S s0{i0};
                S s1{i1};
                ranges::iter_swap(s0, s1);
                assert(i0 == 13);
                assert(i1 == 42);
            }

            {
                // Validate iter_swap bullet 3 to defend against regression of GH-1067 "ranges::iter_swap is broken"
                int i  = 42;
                long l = 13;
                ranges::iter_swap(&i, &l);
                assert(i == 13);
                assert(l == 42);
            }

            return true;
        }
        STATIC_ASSERT(test());
    } // namespace poison_pill
} // namespace iterator_cust_swap_test

namespace iterator_concept_readable_test {
    using std::indirectly_readable;

    STATIC_ASSERT(!indirectly_readable<void>);
    STATIC_ASSERT(!indirectly_readable<void*>);
    STATIC_ASSERT(indirectly_readable<int*>);
    STATIC_ASSERT(indirectly_readable<int const*>);
    STATIC_ASSERT(!indirectly_readable<int const empty_type::*>);

    STATIC_ASSERT(!indirectly_readable<dereferences_to<void>>);
    STATIC_ASSERT(indirectly_readable<dereferences_to<int>>);
    STATIC_ASSERT(indirectly_readable<dereferences_to<int&>>);
    STATIC_ASSERT(indirectly_readable<dereferences_to<int const&>>);
    STATIC_ASSERT(indirectly_readable<dereferences_to<move_only>>);
    STATIC_ASSERT(indirectly_readable<dereferences_to<int (&)[42]>>);
    STATIC_ASSERT(!indirectly_readable<dereferences_to<int (&)()>>);
    STATIC_ASSERT(!indirectly_readable<int (empty_type::*)()>);

    struct no_value_type {
        int operator*() const;
    };
    STATIC_ASSERT(!indirectly_readable<no_value_type>);

    struct not_dereferenceable {
        using value_type = int;
    };
    STATIC_ASSERT(!indirectly_readable<no_value_type>);

    struct simple_abstract {
        virtual void f() = 0;
    };
    STATIC_ASSERT(indirectly_readable<dereferences_to<simple_abstract&>>);
} // namespace iterator_concept_readable_test

namespace iterator_concept_writable_test {
    using std::indirectly_writable;

    template <class I, class T>
    constexpr bool test_writable() {
#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
        constexpr bool result = indirectly_writable<I, T>;
        STATIC_ASSERT(indirectly_writable<I, T const> == result);
        STATIC_ASSERT(indirectly_writable<I, T volatile> == result);
        STATIC_ASSERT(indirectly_writable<I, T const volatile> == result);

        if constexpr (can_reference<T>) {
            STATIC_ASSERT(indirectly_writable<I, T&> == result);
            STATIC_ASSERT(indirectly_writable<I, T const&> == result);
            STATIC_ASSERT(indirectly_writable<I, T volatile&> == result);
            STATIC_ASSERT(indirectly_writable<I, T const volatile&> == result);
        }
#pragma warning(pop)

        return result;
    }

    STATIC_ASSERT(!indirectly_writable<void, int>);
    STATIC_ASSERT(!indirectly_writable<void*, void>);
    STATIC_ASSERT(!test_writable<int*, void>());

    STATIC_ASSERT(test_writable<int*, int>());
    STATIC_ASSERT(test_writable<int*, short>());
    STATIC_ASSERT(test_writable<int*, long>());
    STATIC_ASSERT(test_writable<int*, double>());

    STATIC_ASSERT(!test_writable<int, int>());

    STATIC_ASSERT(test_writable<dereferences_to<int&>, int>());
    STATIC_ASSERT(test_writable<dereferences_to<int&>, short>());
    STATIC_ASSERT(test_writable<dereferences_to<int&>, long>());
    STATIC_ASSERT(test_writable<dereferences_to<int&>, double>());

    STATIC_ASSERT(!test_writable<int const*, int>());
    STATIC_ASSERT(!test_writable<int const*, short>());
    STATIC_ASSERT(!test_writable<int const*, long>());
    STATIC_ASSERT(!test_writable<int const*, double>());

    STATIC_ASSERT(!test_writable<dereferences_to<int const&>, int>());
    STATIC_ASSERT(!test_writable<dereferences_to<int const&>, short>());
    STATIC_ASSERT(!test_writable<dereferences_to<int const&>, long>());
    STATIC_ASSERT(!test_writable<dereferences_to<int const&>, double>());

    STATIC_ASSERT(!test_writable<int*, int()>());
    STATIC_ASSERT(!test_writable<int (*)(), int()>());
    STATIC_ASSERT(!test_writable<int (*)(), int (*)()>());
    STATIC_ASSERT(test_writable<int (**)(), int (*)()>());

    STATIC_ASSERT(indirectly_writable<move_only*, move_only>);
    STATIC_ASSERT(!test_writable<move_only*, move_only&>());

    // Verify the "indirectly_writable through a constified reference" requirements
    struct std_string {}; // slightly simplified
    STATIC_ASSERT(!test_writable<dereferences_to<std_string>, std_string>());

    template <bool HasConstAssign>
    struct proxy_writable {
        using value_type = int;

        struct reference {
            operator int() const;
            reference& operator=(int);

            reference const& operator=(int) const
                requires HasConstAssign;
        };

        reference operator*() const;
    };
    STATIC_ASSERT(!test_writable<proxy_writable<false>, int>());
    STATIC_ASSERT(!test_writable<proxy_writable<false>, short>());
    STATIC_ASSERT(test_writable<proxy_writable<true>, int>());
    STATIC_ASSERT(test_writable<proxy_writable<true>, short>());
} // namespace iterator_concept_writable_test

namespace iterator_concept_winc_test {
    using std::weakly_incrementable;

    STATIC_ASSERT(!weakly_incrementable<void>);
    STATIC_ASSERT(weakly_incrementable<int>);
    STATIC_ASSERT(weakly_incrementable<unsigned int>);
    STATIC_ASSERT(!weakly_incrementable<void*>);
    STATIC_ASSERT(weakly_incrementable<int*>);
    STATIC_ASSERT(weakly_incrementable<int const*>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(
            std::same_as<std::index_sequence<Is...>, std::make_index_sequence<weakly_incrementable_archetype_max>>);
#ifndef _M_CEE // TRANSITION, VSO-1665674
        STATIC_ASSERT((!weakly_incrementable<weakly_incrementable_archetype<Is>> && ...));
#endif // _M_CEE
        STATIC_ASSERT(weakly_incrementable<weakly_incrementable_archetype<weakly_incrementable_archetype_max>>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<weakly_incrementable_archetype_max>{}));
} // namespace iterator_concept_winc_test

namespace iterator_concept_inc_test {
    using std::incrementable;

    STATIC_ASSERT(!incrementable<void>);
    STATIC_ASSERT(incrementable<int>);
    STATIC_ASSERT(incrementable<unsigned int>);
    STATIC_ASSERT(!incrementable<void*>);
    STATIC_ASSERT(incrementable<int*>);
    STATIC_ASSERT(incrementable<int const*>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(std::same_as<std::index_sequence<Is...>, std::make_index_sequence<incrementable_archetype_max>>);
        STATIC_ASSERT((!incrementable<incrementable_archetype<Is>> && ...));
        STATIC_ASSERT(incrementable<incrementable_archetype<incrementable_archetype_max>>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<incrementable_archetype_max>{}));
} // namespace iterator_concept_inc_test

namespace iterator_concept_iterator_test {
    using std::input_or_output_iterator;

    STATIC_ASSERT(!input_or_output_iterator<void>);
    STATIC_ASSERT(!input_or_output_iterator<int>);
    STATIC_ASSERT(!input_or_output_iterator<void*>);
    STATIC_ASSERT(input_or_output_iterator<int*>);
    STATIC_ASSERT(input_or_output_iterator<int const*>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(std::same_as<std::index_sequence<Is...>, std::make_index_sequence<iterator_archetype_max>>);
#ifndef _M_CEE // TRANSITION, VSO-1665674
        STATIC_ASSERT((!input_or_output_iterator<iterator_archetype<Is>> && ...));
#endif // _M_CEE
        STATIC_ASSERT(input_or_output_iterator<iterator_archetype<iterator_archetype_max>>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<iterator_archetype_max>{}));
} // namespace iterator_concept_iterator_test

namespace iterator_concept_sentinel_test {
    using std::input_or_output_iterator, std::sentinel_for;

    STATIC_ASSERT(sentinel_for<int*, int*>);
    STATIC_ASSERT(sentinel_for<int const*, int const*>);
    STATIC_ASSERT(sentinel_for<int const*, int*>);
    STATIC_ASSERT(sentinel_for<int*, int const*>);
    STATIC_ASSERT(!sentinel_for<void*, void*>);

    struct A {
        using difference_type = int;

        A& operator++();
        A& operator++(int);
        A& operator*() const;

        bool operator==(A const&) const;
    };
    STATIC_ASSERT(input_or_output_iterator<A>);
    STATIC_ASSERT(sentinel_for<A, A>);

    template <std::size_t I, std::size_t J>
    constexpr bool test_one_pair() {
#ifndef _M_CEE // TRANSITION, VSO-1665674
        constexpr bool expected = I >= sentinel_archetype_max && J >= iterator_archetype_max;
        STATIC_ASSERT(sentinel_for<sentinel_archetype<I>, iterator_archetype<J>> == expected);
#endif // _M_CEE
        return true;
    }

    template <std::size_t I, std::size_t... Js>
    constexpr bool unpack_iterator(std::index_sequence<Js...>) {
        return (test_one_pair<I, Js>() && ...);
    }

    template <std::size_t... Is>
    constexpr bool unpack_sentinel(std::index_sequence<Is...>) {
        return (unpack_iterator<Is>(std::make_index_sequence<iterator_archetype_max + 1>{}) && ...);
    }
    STATIC_ASSERT(unpack_sentinel(std::make_index_sequence<sentinel_archetype_max + 1>{}));
} // namespace iterator_concept_sentinel_test

namespace iterator_concept_sizedsentinel_test {
    using std::sized_sentinel_for;

    STATIC_ASSERT(sized_sentinel_for<int*, int*>);
    STATIC_ASSERT(sized_sentinel_for<int const*, int const*>);
    STATIC_ASSERT(sized_sentinel_for<int const*, int*>);
    STATIC_ASSERT(sized_sentinel_for<int*, int const*>);
    STATIC_ASSERT(!sized_sentinel_for<void*, void*>);

    template <std::size_t I, std::size_t J>
    constexpr bool test_one_pair() {
#ifndef _M_CEE // TRANSITION, VSO-1665674
        constexpr bool expected = I >= sized_sentinel_archetype_max && J >= iterator_archetype_max;
        STATIC_ASSERT(sized_sentinel_for<sized_sentinel_archetype<I>, iterator_archetype<J>> == expected);
#endif // _M_CEE
        return true;
    }

    template <std::size_t I, std::size_t... Js>
    constexpr bool unpack_iterator(std::index_sequence<Js...>) {
        return (test_one_pair<I, Js>() && ...);
    }

    template <std::size_t... Is>
    constexpr bool unpack_sentinel(std::index_sequence<Is...>) {
        return (unpack_iterator<Is>(std::make_index_sequence<iterator_archetype_max + 1>{}) && ...);
    }
    STATIC_ASSERT(unpack_sentinel(std::make_index_sequence<sized_sentinel_archetype_max + 1>{}));
} // namespace iterator_concept_sizedsentinel_test

namespace iterator_concept_input_test {
    using std::input_iterator;

    STATIC_ASSERT(input_iterator<int*>);
    STATIC_ASSERT(input_iterator<int const*>);
    STATIC_ASSERT(input_iterator<iter_concept_example>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(std::same_as<std::index_sequence<Is...>, std::make_index_sequence<input_iterator_archetype_max>>);
#ifndef _M_CEE // TRANSITION, VSO-1665674
        STATIC_ASSERT((!input_iterator<input_iterator_archetype<Is>> && ...));
#endif // _M_CEE
        STATIC_ASSERT(input_iterator<input_iterator_archetype<input_iterator_archetype_max>>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<input_iterator_archetype_max>{}));
} // namespace iterator_concept_input_test

namespace iterator_concept_output_test {
    using std::output_iterator;

    STATIC_ASSERT(output_iterator<int*, int>);
    STATIC_ASSERT(output_iterator<int*, int const>);
    STATIC_ASSERT(output_iterator<int*, int&>);
    STATIC_ASSERT(output_iterator<int*, int const&>);
    STATIC_ASSERT(!output_iterator<int const*, int>);
    STATIC_ASSERT(!output_iterator<int const*, int const>);
    STATIC_ASSERT(!output_iterator<int const*, int&>);
    STATIC_ASSERT(!output_iterator<int const*, int const&>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(
            std::same_as<std::index_sequence<Is...>, std::make_index_sequence<output_iterator_archetype_max>>);
#ifndef _M_CEE // TRANSITION, VSO-1665674
        STATIC_ASSERT((!output_iterator<output_iterator_archetype<Is>, int> && ...));
        STATIC_ASSERT((!output_iterator<output_iterator_archetype<Is>, int const> && ...));
        STATIC_ASSERT((!output_iterator<output_iterator_archetype<Is>, int&> && ...));
        STATIC_ASSERT((!output_iterator<output_iterator_archetype<Is>, int const&> && ...));
#endif // _M_CEE
        STATIC_ASSERT(output_iterator<output_iterator_archetype<output_iterator_archetype_max>, int>);
        STATIC_ASSERT(output_iterator<output_iterator_archetype<output_iterator_archetype_max>, int const>);
        STATIC_ASSERT(output_iterator<output_iterator_archetype<output_iterator_archetype_max>, int&>);
        STATIC_ASSERT(output_iterator<output_iterator_archetype<output_iterator_archetype_max>, int const&>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<output_iterator_archetype_max>{}));
} // namespace iterator_concept_output_test

namespace iterator_concept_forward_test {
    using std::forward_iterator;

    STATIC_ASSERT(forward_iterator<int*>);
    STATIC_ASSERT(forward_iterator<int const*>);
    STATIC_ASSERT(forward_iterator<iter_concept_example>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(
            std::same_as<std::index_sequence<Is...>, std::make_index_sequence<forward_iterator_archetype_max>>);
        STATIC_ASSERT((!forward_iterator<forward_iterator_archetype<Is>> && ...));
        STATIC_ASSERT(forward_iterator<forward_iterator_archetype<forward_iterator_archetype_max>>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<forward_iterator_archetype_max>{}));
} // namespace iterator_concept_forward_test

namespace iterator_concept_bidir_test {
    using std::bidirectional_iterator;

    STATIC_ASSERT(bidirectional_iterator<int*>);
    STATIC_ASSERT(bidirectional_iterator<int const*>);
    STATIC_ASSERT(bidirectional_iterator<iter_concept_example>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(std::same_as<std::index_sequence<Is...>, std::make_index_sequence<bidi_iterator_archetype_max>>);
        STATIC_ASSERT((!bidirectional_iterator<bidi_iterator_archetype<Is>> && ...));
        STATIC_ASSERT(bidirectional_iterator<bidi_iterator_archetype<bidi_iterator_archetype_max>>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<bidi_iterator_archetype_max>{}));
} // namespace iterator_concept_bidir_test

namespace iterator_concept_random_access_test {
    using std::random_access_iterator;

    STATIC_ASSERT(random_access_iterator<int*>);
    STATIC_ASSERT(random_access_iterator<int const*>);
    STATIC_ASSERT(!random_access_iterator<iter_concept_example>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(
            std::same_as<std::index_sequence<Is...>, std::make_index_sequence<random_iterator_archetype_max>>);
        STATIC_ASSERT((!random_access_iterator<random_iterator_archetype<Is>> && ...));
        STATIC_ASSERT(random_access_iterator<random_iterator_archetype<random_iterator_archetype_max>>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<random_iterator_archetype_max>{}));
} // namespace iterator_concept_random_access_test

namespace iterator_concept_contiguous_test {
    using std::contiguous_iterator;

    STATIC_ASSERT(contiguous_iterator<int*>);
    STATIC_ASSERT(contiguous_iterator<int const*>);
    STATIC_ASSERT(!contiguous_iterator<iter_concept_example>);

    template <std::size_t... Is>
    constexpr bool test(std::index_sequence<Is...>) {
        STATIC_ASSERT(
            std::same_as<std::index_sequence<Is...>, std::make_index_sequence<contig_iterator_archetype_max>>);
        STATIC_ASSERT((!contiguous_iterator<contig_iterator_archetype<Is>> && ...));
        STATIC_ASSERT(contiguous_iterator<contig_iterator_archetype<contig_iterator_archetype_max>>);
        return true;
    }
    STATIC_ASSERT(test(std::make_index_sequence<contig_iterator_archetype_max>{}));
} // namespace iterator_concept_contiguous_test

namespace std_iterator_tags_test {
    using std::bidirectional_iterator_tag, std::contiguous_iterator_tag, std::forward_iterator_tag,
        std::input_iterator_tag, std::output_iterator_tag, std::random_access_iterator_tag;

    template <class T, bool derives_from_output, bool derives_from_input, bool derives_from_forward,
        bool derives_from_bidi, bool derives_from_random, bool derives_from_contiguous>
    constexpr bool test_tag() {
        using std::derived_from;

        STATIC_ASSERT(std::is_empty_v<T>);
        STATIC_ASSERT(std::semiregular<T>);
        (void) T{};

        STATIC_ASSERT(derived_from<T, output_iterator_tag> == derives_from_output);
        STATIC_ASSERT(derived_from<T, input_iterator_tag> == derives_from_input);
        STATIC_ASSERT(derived_from<T, forward_iterator_tag> == derives_from_forward);
        STATIC_ASSERT(derived_from<T, bidirectional_iterator_tag> == derives_from_bidi);
        STATIC_ASSERT(derived_from<T, random_access_iterator_tag> == derives_from_random);
        STATIC_ASSERT(derived_from<T, contiguous_iterator_tag> == derives_from_contiguous);
        return true;
    }

    STATIC_ASSERT(test_tag<output_iterator_tag, true, false, false, false, false, false>());
    STATIC_ASSERT(test_tag<input_iterator_tag, false, true, false, false, false, false>());
    STATIC_ASSERT(test_tag<forward_iterator_tag, false, true, true, false, false, false>());
    STATIC_ASSERT(test_tag<bidirectional_iterator_tag, false, true, true, true, false, false>());
    STATIC_ASSERT(test_tag<random_access_iterator_tag, false, true, true, true, true, false>());
    STATIC_ASSERT(test_tag<contiguous_iterator_tag, false, true, true, true, true, true>());
} // namespace std_iterator_tags_test

namespace incomplete_test {
    template <class T>
    struct do_not_instantiate {
        static_assert(always_false<T>);
    };

    using E = do_not_instantiate<void>;

    // Verify that the iterator trait aliases do not cause instantiation of pointee types
    using V = std::iter_value_t<E*>;
    using D = std::iter_difference_t<E*>;
    using R = std::iter_reference_t<E*>;
} // namespace incomplete_test

namespace default_sentinel_test {
    using std::default_sentinel, std::default_sentinel_t;

    STATIC_ASSERT(std::is_empty_v<default_sentinel_t>);
    STATIC_ASSERT(std::semiregular<default_sentinel_t>);
    STATIC_ASSERT(std::same_as<decltype((default_sentinel)), default_sentinel_t const&>);

    // All special member functions are implicitly noexcept
    STATIC_ASSERT(std::is_nothrow_default_constructible_v<default_sentinel_t>);
    STATIC_ASSERT(std::is_nothrow_copy_constructible_v<default_sentinel_t>);
    STATIC_ASSERT(std::is_nothrow_move_constructible_v<default_sentinel_t>);
    STATIC_ASSERT(std::is_nothrow_copy_assignable_v<default_sentinel_t>);
    STATIC_ASSERT(std::is_nothrow_move_assignable_v<default_sentinel_t>);

    constexpr bool test() {
        // Validate that default_sentinel_t's special member functions are all constexpr
        default_sentinel_t ds0{}; // default constructor
        default_sentinel_t ds1{default_sentinel}; // copy constructor
        [[maybe_unused]] default_sentinel_t ds2{std::move(ds0)}; // move constructor
        ds0 = default_sentinel; // copy assignment
        ds1 = std::move(ds0); // move assignment
        return true;
    }
    STATIC_ASSERT(test());
} // namespace default_sentinel_test

namespace unreachable_sentinel_test {
    using std::unreachable_sentinel, std::unreachable_sentinel_t;

    STATIC_ASSERT(std::is_empty_v<unreachable_sentinel_t>);
    STATIC_ASSERT(std::semiregular<unreachable_sentinel_t>);
    STATIC_ASSERT(std::same_as<decltype((unreachable_sentinel)), unreachable_sentinel_t const&>);

    // All special member functions are implicitly noexcept
    STATIC_ASSERT(std::is_nothrow_default_constructible_v<unreachable_sentinel_t>);
    STATIC_ASSERT(std::is_nothrow_copy_constructible_v<unreachable_sentinel_t>);
    STATIC_ASSERT(std::is_nothrow_move_constructible_v<unreachable_sentinel_t>);
    STATIC_ASSERT(std::is_nothrow_copy_assignable_v<unreachable_sentinel_t>);
    STATIC_ASSERT(std::is_nothrow_move_assignable_v<unreachable_sentinel_t>);

    // clang-format off
    template <class T>
    concept Comparable = requires(T const& t) {
        { t == unreachable_sentinel } -> std::same_as<bool>;
        { t != unreachable_sentinel } -> std::same_as<bool>;
        { unreachable_sentinel == t } -> std::same_as<bool>;
        { unreachable_sentinel != t } -> std::same_as<bool>;
    };
    // clang-format on

    STATIC_ASSERT(Comparable<int>);

    template <int... Is>
    constexpr bool test(std::integer_sequence<int, Is...>) {
        unreachable_sentinel_t us0{}; // default constructor is (implicitly) constexpr
        unreachable_sentinel_t us1{unreachable_sentinel}; // ditto copy constructor
        [[maybe_unused]] unreachable_sentinel_t us2{std::move(us0)}; // ditto move constructor
        us0 = unreachable_sentinel; // ditto copy assignment
        us1 = std::move(us0); // ditto move assignment

        // == and != are constexpr and noexcept:
        STATIC_ASSERT(!(unreachable_sentinel == 42));
        STATIC_ASSERT(noexcept(unreachable_sentinel == 42));

        STATIC_ASSERT(unreachable_sentinel != 42);
        STATIC_ASSERT(noexcept(unreachable_sentinel != 42));

        STATIC_ASSERT(!(42 == unreachable_sentinel));
        STATIC_ASSERT(noexcept(42 == unreachable_sentinel));

        STATIC_ASSERT(42 != unreachable_sentinel);
        STATIC_ASSERT(noexcept(42 != unreachable_sentinel));

#ifndef _M_CEE // TRANSITION, VSO-1665674
        STATIC_ASSERT((!Comparable<weakly_incrementable_archetype<Is>> && ...));
#endif // _M_CEE
        STATIC_ASSERT(Comparable<weakly_incrementable_archetype<weakly_incrementable_archetype_max>>);

        return true;
    }
    STATIC_ASSERT(test(std::make_integer_sequence<int, weakly_incrementable_archetype_max>{}));

    namespace regress_1029409 {
        // VSO-1029409 failed because std::unreachable_sentinel_t's hidden friend operator!= is not hidden in permissive
        // mode. Overload resolution for != expressions would therefore try to determine if evil models
        // std::weakly_incrementable, instantiating the operator- that must not be instantiated.

        template <class>
        struct evil {
            friend constexpr bool operator!=(evil const&, evil const&) {
                return true;
            }

            template <class T>
            auto operator-(T const&) const {
                static_assert(always_false<T>, "Don't instantiate me!");
            }
        };

        using std_type = std::default_sentinel_t;
        STATIC_ASSERT(evil<std_type>{} != evil<std_type>{});
    } // namespace regress_1029409
} // namespace unreachable_sentinel_test

namespace unwrap_move_only {
    // Validate the iterator unwrapping machinery works with move-only iterators, and that move-only iterators are not
    // C++17 iterators

    template <class T, bool IsWrapped>
    struct iter {
        using value_type      = T;
        using difference_type = std::ptrdiff_t;

        iter()       = default;
        iter(iter&&) = default;

        iter& operator=(iter&&) = default;

        T operator*() const noexcept;

        iter& operator++() noexcept;
        void operator++(int) noexcept;

        bool operator==(std::default_sentinel_t const&) const noexcept;

        static constexpr bool _Unwrap_when_unverified = true;
        iter<T, false> _Unwrapped() && noexcept
            requires IsWrapped;
        void _Seek_to(iter<T, false>) noexcept
            requires IsWrapped;

        void _Verify_offset(std::ptrdiff_t) const noexcept
            requires IsWrapped;
    };
    STATIC_ASSERT(std::input_iterator<iter<int, true>>);
    STATIC_ASSERT(!has_member_iter_category<std::iterator_traits<iter<int, true>>>);

    STATIC_ASSERT(!std::_Unwrappable_v<iter<int, true>&>);
    STATIC_ASSERT(!std::_Unwrappable_v<iter<int, true> const&>);
    STATIC_ASSERT(std::_Unwrappable_v<iter<int, true>>);
    STATIC_ASSERT(!std::_Unwrappable_v<iter<int, true> const>);
    STATIC_ASSERT(std::same_as<std::_Unwrapped_t<iter<int, true>>, iter<int, false>>);

    STATIC_ASSERT(!std::_Unwrappable_for_unverified_v<iter<int, true>&>);
    STATIC_ASSERT(!std::_Unwrappable_for_unverified_v<iter<int, true> const&>);
    STATIC_ASSERT(std::_Unwrappable_for_unverified_v<iter<int, true>>);
    STATIC_ASSERT(!std::_Unwrappable_for_unverified_v<iter<int, true> const>);
    STATIC_ASSERT(std::same_as<std::_Unwrapped_unverified_t<iter<int, true>>, iter<int, false>>);

    STATIC_ASSERT(!std::_Unwrappable_for_offset_v<iter<int, true>&>);
    STATIC_ASSERT(!std::_Unwrappable_for_offset_v<iter<int, true> const&>);
    STATIC_ASSERT(std::_Unwrappable_for_offset_v<iter<int, true>>);
    STATIC_ASSERT(!std::_Unwrappable_for_offset_v<iter<int, true> const>);

    STATIC_ASSERT(!std::_Wrapped_seekable_v<iter<int, true>, iter<int, false>&>);
    STATIC_ASSERT(!std::_Wrapped_seekable_v<iter<int, true>, iter<int, false> const&>);
    STATIC_ASSERT(std::_Wrapped_seekable_v<iter<int, true>, iter<int, false>>);
    STATIC_ASSERT(!std::_Wrapped_seekable_v<iter<int, true>, iter<int, false> const>);
} // namespace unwrap_move_only

namespace iter_ops {
    using std::default_sentinel, std::default_sentinel_t;
    using std::input_iterator_tag, std::forward_iterator_tag, std::bidirectional_iterator_tag,
        std::random_access_iterator_tag;

    struct trace {
        unsigned int compares_;
        unsigned int differences_;
        unsigned int increments_;
        unsigned int decrements_;
        unsigned int assignments_;
        unsigned int seeks_;
        unsigned int sizes_;
        unsigned int begins_;
        unsigned int ends_;

        bool operator==(trace const&) const = default;
    };

    enum class sized { no, yes };
    enum class assign { no, yes };
    enum class nothrow { no, yes };

    static constexpr int sentinel_position = 42;

    template <class Category, sized Sized = sized::no, assign Assign = assign::no, nothrow NoThrow = nothrow::no>
    struct trace_iterator {
        using value_type      = int;
        using difference_type = int;

        static constexpr bool is_forward = std::derived_from<Category, forward_iterator_tag>;
        static constexpr bool is_bidi    = std::derived_from<Category, bidirectional_iterator_tag>;
        static constexpr bool is_random  = std::derived_from<Category, random_access_iterator_tag>;
        static constexpr bool is_sized   = Sized == sized::yes;

        trace_iterator() = default;
        constexpr explicit trace_iterator(trace& t) noexcept(NoThrow == nothrow::yes) : trace_{&t} {}
        constexpr explicit trace_iterator(int const pos, trace& t) noexcept(NoThrow == nothrow::yes)
            : trace_{&t}, pos_{pos} {}

        // clang-format off
        trace_iterator(trace_iterator const&) requires is_forward = default;
        // clang-format on
        trace_iterator(trace_iterator&&) = default;

        constexpr trace_iterator& operator=(trace_iterator const& that) noexcept(NoThrow == nothrow::yes)
            requires is_forward
        {
            if (!trace_) {
                trace_ = that.trace_;
            }
            ++trace_->assignments_;
            pos_ = that.pos_;
            return *this;
        }
        constexpr trace_iterator& operator=(trace_iterator&& that) noexcept(NoThrow == nothrow::yes) {
            if (!trace_) {
                trace_ = that.trace_;
            }
            ++trace_->assignments_;
            pos_ = that.pos_;
            return *this;
        }

        // clang-format off
        constexpr trace_iterator& operator=(default_sentinel_t) noexcept(NoThrow == nothrow::yes)
            requires (Assign == assign::yes) {
            ++trace_->assignments_;
            pos_ = sentinel_position;
            return *this;
        }
        // clang-format on

        int operator*() const noexcept(NoThrow == nothrow::yes);

        constexpr trace_iterator& operator++() noexcept(NoThrow == nothrow::yes) {
            ++trace_->increments_;
            ++pos_;
            return *this;
        }
        trace_iterator operator++(int) noexcept(NoThrow == nothrow::yes);

        constexpr bool operator==(default_sentinel_t) const noexcept(NoThrow == nothrow::yes) {
            ++trace_->compares_;
            return pos_ == sentinel_position;
        }
        constexpr int operator-(default_sentinel_t) const noexcept(NoThrow == nothrow::yes)
            requires is_sized
        {
            ++trace_->differences_;
            return pos_ - sentinel_position;
        }
        friend constexpr int operator-(default_sentinel_t, trace_iterator const& i) noexcept(NoThrow == nothrow::yes)
            requires is_sized
        {
            return -(i - default_sentinel);
        }

        constexpr bool operator==(trace_iterator const& that) const noexcept(NoThrow == nothrow::yes)
            requires is_forward
        {
            ++trace_->compares_;
            return pos_ == that.pos_;
        }

        constexpr trace_iterator& operator--() noexcept(NoThrow == nothrow::yes)
            requires is_bidi
        {
            ++trace_->decrements_;
            --pos_;
            return *this;
        }
        trace_iterator operator--(int) noexcept(NoThrow == nothrow::yes)
            requires is_bidi;

        std::strong_ordering operator<=>(trace_iterator const&) const noexcept(NoThrow == nothrow::yes)
            requires is_random;

        constexpr trace_iterator& operator+=(int const n) noexcept(NoThrow == nothrow::yes)
            requires is_random
        {
            ++trace_->seeks_;
            pos_ += n;
            return *this;
        }
        trace_iterator operator+(int) const noexcept(NoThrow == nothrow::yes)
            requires is_random;
        friend trace_iterator operator+(int, trace_iterator const&) noexcept(NoThrow == nothrow::yes)
            requires is_random
        {}

        trace_iterator& operator-=(int) noexcept(NoThrow == nothrow::yes)
            requires is_random;
        trace_iterator operator-(int) const noexcept(NoThrow == nothrow::yes)
            requires is_random;

        constexpr int operator-(trace_iterator const& that) const noexcept(NoThrow == nothrow::yes)
            requires is_random || is_sized
        {
            ++trace_->differences_;
            return pos_ - that.pos_;
        }

        int operator[](int) const noexcept(NoThrow == nothrow::yes);

        trace* trace_ = nullptr;
        int pos_      = 0;
    };

    // To model Assignable<trace_iterator</* ... */, assign::yes>&, std::default_sentinel_t>, we must satisfy
    // common_reference_with<trace_iterator</* ... */, assign::yes> const&, const std::default_sentinel_t&>.
    // Specializing common_type is the simplest way to do so:
    struct commontype {
        template <class Category, sized Sized>
        commontype(trace_iterator<Category, Sized, assign::yes> const&);
        commontype(default_sentinel_t);
    };
} // namespace iter_ops

template <class Category, ::iter_ops::sized Sized>
struct std::common_type<::iter_ops::trace_iterator<Category, Sized, ::iter_ops::assign::yes>, std::default_sentinel_t> {
    using type = ::iter_ops::commontype;
};
template <class Category, ::iter_ops::sized Sized>
struct std::common_type<std::default_sentinel_t, ::iter_ops::trace_iterator<Category, Sized, ::iter_ops::assign::yes>> {
    using type = ::iter_ops::commontype;
};

namespace iter_ops {
    using ranges::advance, ranges::distance, ranges::next, ranges::prev;

    constexpr bool test_iter_forms() {
        {
            // Call next(i), validating that ++i is called once
            using R = trace_iterator<random_access_iterator_tag>;
            trace t{};
            R r = next(R{t});
            assert(r.pos_ == 1 && t == trace{.increments_ = 1});
        }
        {
            // Ditto, move-only iterator
            using I = trace_iterator<input_iterator_tag>;
            trace t{};
            I i = next(I{t});
            assert(i.pos_ == 1 && t == trace{.increments_ = 1});
        }
        {
            // Call prev(i), validating that --i is called once
            using R = trace_iterator<random_access_iterator_tag>;
            trace t{};
            R r = prev(R{t});
            assert(r.pos_ == -1 && t == trace{.decrements_ = 1});
        }

        return true;
    }
    STATIC_ASSERT(test_iter_forms());

    constexpr bool test_iter_count_forms() {
        // Call advance(i, n) / next(i, n) / prev(i, -n) with:
        {
            // random_access_iterator<I>, validating that i += n is called
            using I = trace_iterator<random_access_iterator_tag>;
            {
                trace t{};
                I i{t};
                advance(i, 42);
                assert(i.pos_ == 42 && t == trace{.seeks_ = 1});
            }
            {
                trace t{};
                I i = next(I{t}, 42);
                assert(i.pos_ == 42 && t == trace{.seeks_ = 1});
            }
            {
                trace t{};
                I i = next(I{t}, -42);
                assert(i.pos_ == -42 && t == trace{.seeks_ = 1});
            }
            {
                trace t{};
                I i = prev(I{t}, 42);
                assert(i.pos_ == -42 && t == trace{.seeks_ = 1});
            }
            {
                trace t{};
                I i = prev(I{t}, -42);
                assert(i.pos_ == 42 && t == trace{.seeks_ = 1});
            }
        }
        {
            // bidirectional_iterator<I> && n > 0, validating that ++i is called n times
            using I = trace_iterator<bidirectional_iterator_tag>;
            {
                trace t{};
                I i{t};
                advance(i, 42);
                assert(i.pos_ == 42 && t == trace{.increments_ = 42});
            }
            {
                trace t{};
                I i = next(I{t}, 42);
                assert(i.pos_ == 42 && t == trace{.increments_ = 42});
            }
            {
                trace t{};
                I i = prev(I{t}, -42);
                assert(i.pos_ == 42 && t == trace{.increments_ = 42});
            }
        }
        {
            // bidirectional_iterator<I> && n < 0, validating that --i is called -n times
            using I = trace_iterator<bidirectional_iterator_tag>;
            {
                trace t{};
                I i{t};
                advance(i, -42);
                assert(i.pos_ == -42 && t == trace{.decrements_ = 42});
            }
            {
                trace t{};
                I i = next(I{t}, -42);
                assert(i.pos_ == -42 && t == trace{.decrements_ = 42});
            }
            {
                trace t{};
                I i = prev(I{t}, 42);
                assert(i.pos_ == -42 && t == trace{.decrements_ = 42});
            }
        }
        {
            // input_iterator<I> && n > 0, validating that ++i is called n times
            using I = trace_iterator<input_iterator_tag>;
            {
                trace t{};
                I i{t};
                advance(i, 42);
                assert(i.pos_ == 42 && t == trace{.increments_ = 42});
            }
            {
                trace t{};
                I i = next(I{t}, 42);
                assert(i.pos_ == 42 && t == trace{.increments_ = 42});
            }
        }
        {
            // random_access_iterator<I> && n == 0, validating that i += 0 is called.
            using R = trace_iterator<random_access_iterator_tag>;
            {
                trace t{};
                R r{t};
                advance(r, 0);
                assert(r.pos_ == 0 && t == trace{.seeks_ = 1});
            }
            {
                trace t{};
                R r = next(R{t}, 0);
                assert(r.pos_ == 0 && t == trace{.seeks_ = 1});
            }
            {
                trace t{};
                R r = prev(R{t}, 0);
                assert(r.pos_ == 0 && t == trace{.seeks_ = 1});
            }
        }
        {
            // bidirectional_iterator<I> && n == 0, validating that there are no effects
            using B = trace_iterator<bidirectional_iterator_tag>;
            {
                trace t{};
                B b{t};
                advance(b, 0);
                assert(b.pos_ == 0 && t == trace{});
            }
            {
                trace t{};
                B b = next(B{t}, 0);
                assert(b.pos_ == 0 && t == trace{});
            }
            {
                trace t{};
                B b = prev(B{t}, 0);
                assert(b.pos_ == 0 && t == trace{});
            }
        }
        {
            // input_iterator<I> && n == 0, validating that there are no effects
            using I = trace_iterator<input_iterator_tag>;
            {
                trace t{};
                I i{t};
                advance(i, 0);
                assert(i.pos_ == 0 && t == trace{});
            }
            {
                trace t{};
                I i = next(I{t}, 0);
                assert(i.pos_ == 0 && t == trace{});
            }
        }

        return true;
    }
    STATIC_ASSERT(test_iter_count_forms());

    constexpr bool test_iter_sentinel_forms() {
        // Call advance(i, s) / next(i, s) with:
        {
            // assignable_from<I&, S>, validating that i = s is called
            using I = trace_iterator<input_iterator_tag, sized::no, assign::yes>;
            {
                trace t{};
                I i{t};
                advance(i, default_sentinel);
                assert(i.pos_ == sentinel_position && t == trace{.assignments_ = 1});
            }
            {
                trace t{};
                I i = next(I{t}, default_sentinel);
                assert(i.pos_ == sentinel_position && t == trace{.assignments_ = 1});
            }
        }
        {
            // sized_sentinel_for<S, I> && random_access_iterator<I>, validating that i += s - i is called
            using I = trace_iterator<random_access_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                advance(i, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I i = next(I{t}, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
        }
        {
            // sized_sentinel_for<S, I> && input_iterator<I>, validating that ++i is called s - i times
            using I = trace_iterator<input_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                advance(i, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .increments_ = 42}));
            }
            {
                trace t{};
                I i = next(I{t}, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .increments_ = 42}));
            }
        }
        {
            // input_iterator<I>, validating that ++i is called enough times to make i == s
            using I = trace_iterator<input_iterator_tag>;
            {
                trace t{};
                I i{t};
                advance(i, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.compares_ = 43, .increments_ = 42}));
            }
            {
                trace t{};
                I i = next(I{t}, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.compares_ = 43, .increments_ = 42}));
            }
        }

        return true;
    }
    STATIC_ASSERT(test_iter_sentinel_forms());

    constexpr bool test_iter_count_sentinel_forms() {
        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   sized_sentinel_for<S, I> && n < s - i && s - i < 0
        {
            // && bidirectional_iterator<I>, validating that i = s is called (NB: s and i must have the same type),
            // and the return value is n + (i - s)
            using I = trace_iterator<bidirectional_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -2 * sentinel_position, I{t});
                assert(result == -sentinel_position);
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = next(I{sentinel_position, t}, -2 * sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = prev(I{sentinel_position, t}, 2 * sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
        }
        {
            // && random_access_iterator<I>, validating that i = s is called (NB: s and i must have the same type),
            // and the return value is n + (i - s)
            using I = trace_iterator<random_access_iterator_tag>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -2 * sentinel_position, I{t});
                assert(result == -sentinel_position);
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = next(I{sentinel_position, t}, -2 * sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = prev(I{sentinel_position, t}, 2 * sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   sized_sentinel_for<S, I> && n == s - i && s - i < 0
        {
            // && bidirectional_iterator<I>, validating that i = s is called, and the return value is 0
            using I = trace_iterator<bidirectional_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -sentinel_position, I{t});
                assert(result == 0);
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = next(I{sentinel_position, t}, -sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = prev(I{sentinel_position, t}, sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
        }
        {
            // && random_access_iterator<I>, validating that i = s is called, and the return value is 0
            using I = trace_iterator<random_access_iterator_tag>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -sentinel_position, I{t});
                assert(result == 0);
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = next(I{sentinel_position, t}, -sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = prev(I{sentinel_position, t}, sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   sized_sentinel_for<S, I> && s - i < n && n < 0
        {
            // && bidirectional_iterator<I>, validating that --i is called -n times, and the return value is 0
            using I = trace_iterator<bidirectional_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -sentinel_position / 2, I{t});
                assert(result == 0);
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .decrements_ = sentinel_position / 2}));
            }
            {
                trace t{};
                I i = next(I{sentinel_position, t}, -sentinel_position / 2, I{t});
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .decrements_ = sentinel_position / 2}));
            }
            {
                trace t{};
                I i = prev(I{sentinel_position, t}, sentinel_position / 2, I{t});
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .decrements_ = sentinel_position / 2}));
            }
        }
        {
            // && random_access_iterator<I>, validating that i += n is called, and the return value is 0
            using I = trace_iterator<random_access_iterator_tag>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -sentinel_position / 2, I{t});
                assert(result == 0);
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I i = next(I{sentinel_position, t}, -sentinel_position / 2, I{t});
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I i = prev(I{sentinel_position, t}, sentinel_position / 2, I{t});
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .seeks_ = 1}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   sized_sentinel_for<S, I> && s - i < n && n == 0
        {
            // && bidirectional_iterator<I>, validating that there are no effects, and the return value is 0
            using I = trace_iterator<bidirectional_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, 0, I{t});
                assert(result == 0);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1}));
            }
            {
                trace t{};
                I i = next(I{sentinel_position, t}, 0, I{t});
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1}));
            }
            {
                trace t{};
                I i = prev(I{sentinel_position, t}, 0, I{t});
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1}));
            }
        }
        {
            // && random_access_iterator<I>, validating that i += 0 is called, and the return value is 0
            using I = trace_iterator<random_access_iterator_tag>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, 0, I{t});
                assert(result == 0);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I i = next(I{sentinel_position, t}, 0, I{t});
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I i = prev(I{sentinel_position, t}, 0, I{t});
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   sized_sentinel_for<S, I> && 0 < s - i && s - i < n
        {
            // && input_iterator<I> && assignable_from<I&, S>, validating that i = s is called, and the return value is
            // n + (i - s)
            using I = trace_iterator<input_iterator_tag, sized::yes, assign::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, 2 * sentinel_position, default_sentinel);
                assert(result == sentinel_position);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I i = next(I{t}, 2 * sentinel_position, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .assignments_ = 1}));
            }

            // Ditto, but bidirectional_iterator<I> for prev(i, n, i)
            {
                using B = trace_iterator<bidirectional_iterator_tag, sized::yes, assign::yes>;
                trace t{};
                B b = prev(B{t}, -2 * sentinel_position, B{sentinel_position, t});
                assert((b.pos_ == sentinel_position && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
        }
        {
            // && input_iterator<I>, validating that ++i is called s - i times, and the return value is n + (i - s)
            using I = trace_iterator<input_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, 2 * sentinel_position, default_sentinel);
                assert(result == sentinel_position);
                assert(i.pos_ == sentinel_position);
                assert((t == trace{.differences_ = 1, .increments_ = sentinel_position}));
            }
            {
                trace t{};
                I const i = next(I{t}, 2 * sentinel_position, default_sentinel);
                assert(i.pos_ == sentinel_position);
                assert((t == trace{.differences_ = 1, .increments_ = sentinel_position}));
            }
        }
        {
            // && random_access_iterator<I>, validating that i += s - i is called, and the return value is n + (i - s)
            using I = trace_iterator<random_access_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, 2 * sentinel_position, default_sentinel);
                assert(result == sentinel_position);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I const i = next(I{t}, 2 * sentinel_position, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   sized_sentinel_for<S, I> && 0 < s - i && s - i == n
        {
            // && input_iterator<I> && assignable_from<I&, S>, validating that i = s is called,
            // and the return value is 0
            using I = trace_iterator<input_iterator_tag, sized::yes, assign::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, sentinel_position, default_sentinel);
                assert(result == 0);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
            {
                trace t{};
                I const i = next(I{t}, sentinel_position, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .assignments_ = 1}));
            }

            // Ditto, but bidirectional_iterator<I> for prev(i, n, i)
            {
                using B = trace_iterator<bidirectional_iterator_tag, sized::yes, assign::yes>;
                trace t{};
                B b = prev(B{t}, -sentinel_position, B{sentinel_position, t});
                assert((b.pos_ == sentinel_position && t == trace{.differences_ = 1, .assignments_ = 1}));
            }
        }
        {
            // && input_iterator<I>, validating that ++i is called n times, and the return value is 0
            using I = trace_iterator<input_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, sentinel_position, default_sentinel);
                assert(result == 0);
                assert(i.pos_ == sentinel_position);
                assert((t == trace{.differences_ = 1, .increments_ = sentinel_position}));
            }
            {
                trace t{};
                I const i = next(I{t}, sentinel_position, default_sentinel);
                assert(i.pos_ == sentinel_position);
                assert((t == trace{.differences_ = 1, .increments_ = sentinel_position}));
            }
        }
        {
            // && random_access_iterator<I>, validating that i += n is called, and the return value is 0
            using I = trace_iterator<random_access_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, sentinel_position, default_sentinel);
                assert(result == 0);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I const i = next(I{t}, sentinel_position, default_sentinel);
                assert((i.pos_ == sentinel_position && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   sized_sentinel_for<S, I> && 0 < n && n < s - i
        {
            // && input_iterator<I>, validating that ++i is called n times, and the return value is 0
            using I = trace_iterator<input_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, sentinel_position / 2, default_sentinel);
                assert(result == 0);
                assert(i.pos_ == sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .increments_ = sentinel_position / 2}));
            }
            {
                trace t{};
                I const i = next(I{t}, sentinel_position / 2, default_sentinel);
                assert(i.pos_ == sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .increments_ = sentinel_position / 2}));
            }

            // Ditto, but bidirectional_iterator<I> for prev(i, n, i)
            {
                using B = trace_iterator<bidirectional_iterator_tag, sized::yes>;
                trace t{};
                B b = prev(B{t}, -sentinel_position / 2, B{sentinel_position, t});
                assert(b.pos_ == sentinel_position / 2);
                assert((t == trace{.differences_ = 1, .increments_ = sentinel_position / 2}));
            }
        }
        {
            // && random_access_iterator<I>, validating that i += n is called, and the return value is 0
            using I = trace_iterator<random_access_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, sentinel_position / 2, default_sentinel);
                assert(result == 0);
                assert((i.pos_ == sentinel_position / 2 && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I const i = next(I{t}, sentinel_position / 2, default_sentinel);
                assert((i.pos_ == sentinel_position / 2 && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I const i = prev(I{t}, -sentinel_position / 2, I{sentinel_position, t});
                assert((i.pos_ == sentinel_position / 2 && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   sized_sentinel_for<S, I> && 0 == n && n < s - i
        {
            // && input_iterator<I>, validating that there are no effects, and the return value is 0
            using I = trace_iterator<input_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, 0, default_sentinel);
                assert(result == 0);
                assert((i.pos_ == 0 && t == trace{.differences_ = 1}));
            }
            {
                trace t{};
                I const i = next(I{t}, 0, default_sentinel);
                assert((i.pos_ == 0 && t == trace{.differences_ = 1}));
            }

            // Ditto, but bidirectional_iterator<I> for prev(i, n, i)
            {
                using B = trace_iterator<bidirectional_iterator_tag, sized::yes>;
                trace t{};
                B b = prev(B{t}, 0, B{sentinel_position, t});
                assert((b.pos_ == 0 && t == trace{.differences_ = 1}));
            }
        }
        {
            // && random_access_iterator<I>, validating that i += 0 is called, and the return value is 0
            using I = trace_iterator<random_access_iterator_tag, sized::yes>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, 0, default_sentinel);
                assert(result == 0);
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I const i = next(I{t}, 0, default_sentinel);
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
            {
                trace t{};
                I const i = prev(I{t}, 0, I{sentinel_position, t});
                assert((i.pos_ == 0 && t == trace{.differences_ = 1, .seeks_ = 1}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //    n < s - i < 0 && bidirectional_iterator<I>
        {
            // validating that --i is called i - s times, and the return value is n + (i - s)
            using I = trace_iterator<bidirectional_iterator_tag>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -2 * sentinel_position, I{t});
                assert(result == -sentinel_position);
                assert(i.pos_ == 0);
                assert((t == trace{.compares_ = sentinel_position + 1, .decrements_ = sentinel_position}));
            }
            {
                trace t{};
                I const i = next(I{sentinel_position, t}, -2 * sentinel_position, I{t});
                assert(i.pos_ == 0);
                assert((t == trace{.compares_ = sentinel_position + 1, .decrements_ = sentinel_position}));
            }
            {
                trace t{};
                I const i = prev(I{sentinel_position, t}, 2 * sentinel_position, I{t});
                assert(i.pos_ == 0);
                assert((t == trace{.compares_ = sentinel_position + 1, .decrements_ = sentinel_position}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   n == s - i && s - i < 0 && bidirectional_iterator<I>
        {
            // validating that --i is called -n times, and the return value is 0
            using I = trace_iterator<bidirectional_iterator_tag>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -sentinel_position, I{t});
                assert(result == 0);
                assert((i.pos_ == 0 && t == trace{.compares_ = sentinel_position, .decrements_ = sentinel_position}));
            }
            {
                trace t{};
                I const i = next(I{sentinel_position, t}, -sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.compares_ = sentinel_position, .decrements_ = sentinel_position}));
            }
            {
                trace t{};
                I const i = prev(I{sentinel_position, t}, sentinel_position, I{t});
                assert((i.pos_ == 0 && t == trace{.compares_ = sentinel_position, .decrements_ = sentinel_position}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   s - i < n && n < 0 && bidirectional_iterator<I>
        {
            // validating that --i is called -n times, and the return value is 0
            using I = trace_iterator<bidirectional_iterator_tag>;
            {
                trace t{};
                I i{sentinel_position, t};
                int const result = advance(i, -sentinel_position / 2, I{t});
                assert(result == 0);
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.compares_ = sentinel_position / 2, .decrements_ = sentinel_position / 2}));
            }
            {
                trace t{};
                I const i = next(I{sentinel_position, t}, -sentinel_position / 2, I{t});
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.compares_ = sentinel_position / 2, .decrements_ = sentinel_position / 2}));
            }
            {
                trace t{};
                I const i = prev(I{sentinel_position, t}, sentinel_position / 2, I{t});
                assert(i.pos_ == sentinel_position - sentinel_position / 2);
                assert((t == trace{.compares_ = sentinel_position / 2, .decrements_ = sentinel_position / 2}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   0 < s - i && s - i < n && input_iterator<I>
        {
            // validating that ++i is called i - s times, and the return value is n + (i - s)
            using I = trace_iterator<input_iterator_tag>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, 2 * sentinel_position, default_sentinel);
                assert(result == sentinel_position);
                assert(i.pos_ == sentinel_position);
                assert((t == trace{.compares_ = sentinel_position + 1, .increments_ = sentinel_position}));
            }
            {
                trace t{};
                I const i = next(I{t}, 2 * sentinel_position, default_sentinel);
                assert(i.pos_ == sentinel_position);
                assert((t == trace{.compares_ = sentinel_position + 1, .increments_ = sentinel_position}));
            }

            // Ditto, bidirectional_iterator<I> for prev
            {
                using B = trace_iterator<bidirectional_iterator_tag>;
                trace t{};
                B const b = prev(B{t}, -2 * sentinel_position, B{sentinel_position, t});
                assert(b.pos_ == sentinel_position);
                assert((t == trace{.compares_ = sentinel_position + 1, .increments_ = sentinel_position}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   0 < n && n == s - i && input_iterator<I>
        {
            // validating that ++i is called n times, and the return value is 0
            using I = trace_iterator<input_iterator_tag>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, sentinel_position, default_sentinel);
                assert(result == 0);
                assert(i.pos_ == sentinel_position);
                assert((t == trace{.compares_ = sentinel_position, .increments_ = sentinel_position}));
            }
            {
                trace t{};
                I const i = next(I{t}, sentinel_position, default_sentinel);
                assert(i.pos_ == sentinel_position);
                assert((t == trace{.compares_ = sentinel_position, .increments_ = sentinel_position}));
            }

            // Ditto, bidirectional_iterator<I> for prev
            {
                using B = trace_iterator<bidirectional_iterator_tag>;
                trace t{};
                B const b = prev(B{t}, -sentinel_position, B{sentinel_position, t});
                assert(b.pos_ == sentinel_position);
                assert((t == trace{.compares_ = sentinel_position, .increments_ = sentinel_position}));
            }
        }

        // Call advance(i, n, s) / next(i, n, s) / prev(i, -n, s) with:
        //   input_iterator<I> && 0 < n && n < s - i
        {
            // validating that ++i is called n times, and the return value is 0
            using I = trace_iterator<input_iterator_tag>;
            {
                trace t{};
                I i{t};
                int const result = advance(i, sentinel_position / 2, default_sentinel);
                assert(result == 0);
                assert(i.pos_ == sentinel_position / 2);
                assert((t == trace{.compares_ = sentinel_position / 2, .increments_ = sentinel_position / 2}));
            }
            {
                trace t{};
                I const i = next(I{t}, sentinel_position / 2, default_sentinel);
                assert(i.pos_ == sentinel_position / 2);
                assert((t == trace{.compares_ = sentinel_position / 2, .increments_ = sentinel_position / 2}));
            }

            // Ditto, bidirectional_iterator<I> for prev
            {
                using B = trace_iterator<bidirectional_iterator_tag>;
                trace t{};
                B const b = prev(B{t}, -sentinel_position / 2, B{sentinel_position, t});
                assert(b.pos_ == sentinel_position / 2);
                assert((t == trace{.compares_ = sentinel_position / 2, .increments_ = sentinel_position / 2}));
            }
        }

        return true;
    }
    STATIC_ASSERT(test_iter_count_sentinel_forms());

    template <nothrow NoThrow = nothrow::no>
    struct sized_test_range {
        mutable trace t{};

        constexpr unsigned char size() const noexcept(NoThrow == nothrow::yes) {
            ++t.sizes_;
            return 42;
        }
        int* begin() const noexcept(NoThrow == nothrow::yes);
        int* end() const noexcept(NoThrow == nothrow::yes);
    };

    template <nothrow NoThrow = nothrow::no>
    struct unsized_test_range {
        mutable trace t{};

        constexpr trace_iterator<forward_iterator_tag> begin() const noexcept(NoThrow == nothrow::yes) {
            ++t.begins_;
            return trace_iterator<forward_iterator_tag>{t};
        }
        constexpr std::default_sentinel_t end() const noexcept(NoThrow == nothrow::yes) {
            ++t.ends_;
            return {};
        }
    };

    template <class Element>
    struct pointer_sentinel {
        Element* ptr = nullptr;

        pointer_sentinel() = default;
        constexpr explicit pointer_sentinel(Element* const p) noexcept : ptr{p} {}

        template <class T>
        [[nodiscard]] constexpr bool operator==(T* that) const noexcept {
            static_assert(std::same_as<T, Element>);
            return ptr == that;
        }

        template <class T>
        [[nodiscard]] friend constexpr std::ptrdiff_t operator-(T* x, const pointer_sentinel& y) noexcept {
            static_assert(std::same_as<T, Element>);
            return x - y.ptr;
        }

        template <class T>
        [[nodiscard]] friend constexpr std::ptrdiff_t operator-(const pointer_sentinel& y, T* x) noexcept {
            static_assert(std::same_as<T, Element>);
            return y.ptr - x;
        }
    };

    constexpr bool test_distance() {
        using ranges::distance, ranges::size;
        using std::iter_difference_t, std::same_as;

        {
            // Call distance(i, s) with: sized_sentinel_for<S, I> && input_iterator<I> && last - first > 0
            // Validate return is last - first
            using I = trace_iterator<input_iterator_tag, sized::yes, assign::no, nothrow::no>;
            trace t{};
            I first{t};
            same_as<iter_difference_t<I>> auto const result = distance(first, default_sentinel);
            STATIC_ASSERT(!noexcept(distance(first, default_sentinel)));
            assert(result == sentinel_position);
            assert((t == trace{.differences_ = 1}));
        }
        {
            // Call distance(i, s) with: sized_sentinel_for<S, I> && input_iterator<I> && last - first > 0
            // Validate return is last - first
            using I = trace_iterator<input_iterator_tag, sized::yes, assign::no, nothrow::yes>;
            trace t{};
            I first{t};
            same_as<iter_difference_t<I>> auto const result = distance(first, default_sentinel);
            STATIC_ASSERT(noexcept(distance(first, default_sentinel)));
            assert(result == sentinel_position);
            assert((t == trace{.differences_ = 1}));
        }

        {
            // Call distance(i, s) with:
            //   sized_sentinel_for<S, I> && forward_iterator<I> && same_as<S, I> && last - first < 0
            // Validate return is last - first
            using I = trace_iterator<forward_iterator_tag, sized::yes, assign::no, nothrow::no>;
            trace t{};
            I first{sentinel_position, t};
            I last{t};
            same_as<iter_difference_t<I>> auto const result = distance(first, last);
            STATIC_ASSERT(!noexcept(distance(first, last)));
            assert(result == -sentinel_position);
            assert((t == trace{.differences_ = 1}));
        }
        {
            // Call distance(i, s) with:
            //   sized_sentinel_for<S, I> && forward_iterator<I> && same_as<S, I> && last - first < 0
            // Validate return is last - first
            using I = trace_iterator<forward_iterator_tag, sized::yes, assign::no, nothrow::yes>;
            trace t{};
            I first{sentinel_position, t};
            I last{t};
            same_as<iter_difference_t<I>> auto const result = distance(first, last);
            STATIC_ASSERT(noexcept(distance(first, last)));
            assert(result == -sentinel_position);
            assert((t == trace{.differences_ = 1}));
        }

        {
            // Call distance(i, s) with: !sized_sentinel_for<S, I> && input_iterator<I> && last - first > 0
            // Validate return is last - first, and increment is called last - first times
            using I = trace_iterator<input_iterator_tag, sized::no, assign::no, nothrow::no>;
            trace t{};
            I first{t};
            same_as<iter_difference_t<I>> auto const result = distance(move(first), default_sentinel);
            STATIC_ASSERT(!noexcept(distance(move(first), default_sentinel)));
            assert(result == sentinel_position);
            assert((t == trace{.compares_ = sentinel_position + 1, .increments_ = sentinel_position}));
        }
        {
            // Call distance(i, s) with: !sized_sentinel_for<S, I> && input_iterator<I> && last - first > 0
            // Validate return is last - first, and increment is called last - first times
            using I = trace_iterator<input_iterator_tag, sized::no, assign::no, nothrow::yes>;
            trace t{};
            I first{t};
            same_as<iter_difference_t<I>> auto const result = distance(move(first), default_sentinel);
            STATIC_ASSERT(noexcept(distance(move(first), default_sentinel)));
            assert(result == sentinel_position);
            assert((t == trace{.compares_ = sentinel_position + 1, .increments_ = sentinel_position}));
        }

        {
            // Call distance(r) with: sized_range<R>, validate that begin and end are not called
            sized_test_range<nothrow::no> r{};
            same_as<ptrdiff_t> auto const result = distance(r);
            STATIC_ASSERT(!noexcept(distance(r)));
            assert(result == 42);
            assert((r.t == trace{.sizes_ = 1}));
        }
        {
            // Call distance(r) with: sized_range<R>, validate that begin and end are not called
            sized_test_range<nothrow::yes> r{};
            same_as<ptrdiff_t> auto const result = distance(r);
            STATIC_ASSERT(noexcept(distance(r)));
            assert(result == 42);
            assert((r.t == trace{.sizes_ = 1}));
        }

        {
            // Call distance(r) with: !sized_range<R>, validate that begin and end are called
            unsized_test_range<nothrow::no> r{};
            assert(distance(r) == sentinel_position);
            STATIC_ASSERT(!noexcept(distance(r)));
            trace const expected{
                .compares_ = sentinel_position + 1, .increments_ = sentinel_position, .begins_ = 1, .ends_ = 1};
            assert(r.t == expected);
        }
        {
            // Call distance(r) with: !sized_range<R>, validate that begin and end are called
            unsized_test_range<nothrow::yes> r{};
            assert(distance(r) == sentinel_position);
            STATIC_ASSERT(!noexcept(distance(r))); // No conditional noexcept
            trace const expected{
                .compares_ = sentinel_position + 1, .increments_ = sentinel_position, .begins_ = 1, .ends_ = 1};
            assert(r.t == expected);
        }

        {
            // Call distance(i, s) with arrays which must be decayed to pointers.
            // (This behavior was regressed by LWG-3392.)
            int some_ints[] = {1, 2, 3};
            assert(distance(some_ints, pointer_sentinel{some_ints + 1}) == 1);
            STATIC_ASSERT(noexcept(distance(some_ints, pointer_sentinel{some_ints + 1})));
            assert(distance(some_ints + 1, some_ints) == -1);
            STATIC_ASSERT(noexcept(distance(some_ints + 1, some_ints)));
            assert(distance(some_ints, some_ints) == 0);
            STATIC_ASSERT(noexcept(distance(some_ints, some_ints)));

            const auto& const_ints = some_ints;
            assert(distance(const_ints, pointer_sentinel{const_ints + 1}) == 1);
            STATIC_ASSERT(noexcept(distance(const_ints, pointer_sentinel{const_ints + 1})));
            assert(distance(const_ints + 1, const_ints) == -1);
            STATIC_ASSERT(noexcept(distance(const_ints + 1, const_ints)));
            assert(distance(const_ints, const_ints) == 0);
            STATIC_ASSERT(noexcept(distance(const_ints, const_ints)));
        }

        return true;
    }
    STATIC_ASSERT(test_distance());

    void test() {
        test_iter_forms();
        test_iter_count_forms();
        test_iter_sentinel_forms();
        test_iter_count_sentinel_forms();

        test_distance();
    }
} // namespace iter_ops

namespace insert_iterators {
    template <class Container>
    constexpr bool test() {
        using std::back_insert_iterator, std::front_insert_iterator, std::insert_iterator;
        using std::iter_difference_t, std::ptrdiff_t, std::same_as;

        STATIC_ASSERT(same_as<iter_difference_t<back_insert_iterator<Container>>, ptrdiff_t>);
        STATIC_ASSERT(same_as<iter_difference_t<front_insert_iterator<Container>>, ptrdiff_t>);
        STATIC_ASSERT(same_as<iter_difference_t<insert_iterator<Container>>, ptrdiff_t>);

        return true;
    }

    STATIC_ASSERT(test<std::list<double>>());
    STATIC_ASSERT(test<std::vector<int>>());

    struct container {
        using value_type = int;

        constexpr int* begin() {
            return &value;
        }
        constexpr int* end() {
            return &value + 1;
        }
        constexpr int* insert(int* ptr, int i) {
            assert(ptr == &value);
            value = i;
            return &value;
        }

        int value;
    };

    constexpr bool test_insert_relaxation() {
        // Verify that insert_iterator correctly does not require a nested `iterator` typename
        container c;
        std::insert_iterator i(c, c.begin());
        *i = 42;
        assert(c.value == 42);
        return true;
    }

    STATIC_ASSERT(test_insert_relaxation());
} // namespace insert_iterators

namespace reverse_iterator_test {
    using std::bidirectional_iterator_tag, std::random_access_iterator_tag, std::reverse_iterator, std::same_as,
        std::string, std::three_way_comparable, std::three_way_comparable_with;

    // Validate the iterator_concept/iterator_category metaprogramming
    STATIC_ASSERT(same_as<reverse_iterator<simple_contiguous_iter<>>::iterator_concept, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<simple_contiguous_iter<>>::iterator_category, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<simple_random_iter<>>::iterator_concept, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<simple_random_iter<>>::iterator_category, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<simple_bidi_iter<>>::iterator_concept, bidirectional_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<simple_bidi_iter<>>::iterator_category, bidirectional_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<xvalue_random_iter>::iterator_concept, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<xvalue_random_iter>::iterator_category, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<xvalue_bidi_iter>::iterator_concept, bidirectional_iterator_tag>);
    STATIC_ASSERT(same_as<reverse_iterator<xvalue_bidi_iter>::iterator_category, bidirectional_iterator_tag>);

    // Validate operator-> for a pointer, and for non-pointers with and without operator->()
    // clang-format off
    template <class I, class P>
    concept has_arrow = requires(I i) {
        { i.operator->() } -> same_as<P>;
    };

    template <class I>
    concept has_no_arrow = !requires(I i) {
        i.operator->();
    };
    // clang-format on

    STATIC_ASSERT(has_arrow<reverse_iterator<int*>, int*>);
    STATIC_ASSERT(same_as<reverse_iterator<int*>::pointer, int*>);

    using simple_arrow = simple_bidi_iter<arrow_base<double const*>>;
    STATIC_ASSERT(has_arrow<reverse_iterator<simple_arrow>, double const*>);
    STATIC_ASSERT(same_as<reverse_iterator<simple_arrow>::pointer, double const*>);

    using simple_no_arrow = simple_bidi_iter<>;
    STATIC_ASSERT(has_no_arrow<reverse_iterator<simple_no_arrow>>);
    STATIC_ASSERT(same_as<reverse_iterator<simple_no_arrow>::pointer, void>);

    // Validate comparison constraints
    STATIC_ASSERT(has_eq<reverse_iterator<simple_bidi_iter<>>>);
    STATIC_ASSERT(has_neq<reverse_iterator<simple_bidi_iter<>>>);
    STATIC_ASSERT(!has_less<reverse_iterator<simple_bidi_iter<>>>);
    STATIC_ASSERT(!has_greater<reverse_iterator<simple_bidi_iter<>>>);
    STATIC_ASSERT(!has_less_eq<reverse_iterator<simple_bidi_iter<>>>);
    STATIC_ASSERT(!has_greater_eq<reverse_iterator<simple_bidi_iter<>>>);
    STATIC_ASSERT(!three_way_comparable<reverse_iterator<simple_bidi_iter<>>>);

    STATIC_ASSERT(has_eq<reverse_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_neq<reverse_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_less<reverse_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_greater<reverse_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_less_eq<reverse_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_greater_eq<reverse_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(three_way_comparable<reverse_iterator<simple_random_iter<>>, std::strong_ordering>);

    STATIC_ASSERT(has_eq<reverse_iterator<int*>, reverse_iterator<int const*>>);
    STATIC_ASSERT(has_neq<reverse_iterator<int*>, reverse_iterator<int const*>>);
    STATIC_ASSERT(has_less<reverse_iterator<int*>, reverse_iterator<int const*>>);
    STATIC_ASSERT(has_greater<reverse_iterator<int*>, reverse_iterator<int const*>>);
    STATIC_ASSERT(has_less_eq<reverse_iterator<int*>, reverse_iterator<int const*>>);
    STATIC_ASSERT(has_greater_eq<reverse_iterator<int*>, reverse_iterator<int const*>>);
    STATIC_ASSERT(
        three_way_comparable_with<reverse_iterator<int*>, reverse_iterator<int const*>, std::strong_ordering>);

    STATIC_ASSERT(!has_eq<reverse_iterator<int*>, reverse_iterator<string*>>);
    STATIC_ASSERT(!has_neq<reverse_iterator<int*>, reverse_iterator<string*>>);
    STATIC_ASSERT(!has_less<reverse_iterator<int*>, reverse_iterator<string*>>);
    STATIC_ASSERT(!has_greater<reverse_iterator<int*>, reverse_iterator<string*>>);
    STATIC_ASSERT(!has_less_eq<reverse_iterator<int*>, reverse_iterator<string*>>);
    STATIC_ASSERT(!has_greater_eq<reverse_iterator<int*>, reverse_iterator<string*>>);
    STATIC_ASSERT(!three_way_comparable_with<reverse_iterator<int*>, reverse_iterator<string*>>);

    constexpr bool test() {
        // Validate iter_move
        int count = 0;
        reverse_iterator i{proxy_iterator<0>{&count}};
        assert(ranges::iter_move(i) == 42);
        assert(count == 1);

        // Validate iter_swap
        ranges::iter_swap(i, reverse_iterator{proxy_iterator<2>{&count}});
        assert(count == -1);

        // Validate <=>
        int some_ints[] = {3, 2, 1, 0};
        reverse_iterator<int*> ri{&some_ints[1]};
        reverse_iterator<int const*> ric{&some_ints[2]};
        assert((ri <=> ric) == std::strong_ordering::greater);

        return true;
    }
    STATIC_ASSERT(test());

    // Validate disable_sized_sentinel_for partial specialization for reverse_iterator
    struct weird_difference_base {
        template <class T>
        long operator-(T const&) const {
            return 42;
        }

        bool operator==(weird_difference_base const&) const = default;
    };
    using simple_no_difference = simple_bidi_iter<weird_difference_base>;
} // namespace reverse_iterator_test

template <>
inline constexpr bool std::disable_sized_sentinel_for<reverse_iterator_test::simple_no_difference,
    reverse_iterator_test::simple_no_difference> = true;

namespace reverse_iterator_test {
    STATIC_ASSERT(!std::sized_sentinel_for<simple_no_difference, simple_no_difference>);
    STATIC_ASSERT(
        !std::sized_sentinel_for<reverse_iterator<simple_no_difference>, reverse_iterator<simple_no_difference>>);
} // namespace reverse_iterator_test

namespace move_iterator_test {
    using std::bidirectional_iterator_tag, std::default_sentinel_t, std::forward_iterator_tag, std::input_iterator_tag,
        std::move_iterator, std::move_sentinel, std::random_access_iterator_tag, std::same_as, std::string,
        std::three_way_comparable, std::three_way_comparable_with;

    template <bool CanCopy>
    struct input_iter {
        using iterator_concept = input_iterator_tag;
        using value_type       = int;
        using difference_type  = int;
        using pointer          = void;

        struct reference {
            operator int() const;
            reference const& operator=(int) const;
        };
        struct rvalue_reference {
            constexpr operator int() const noexcept {
                return 42;
            }
        };

        input_iter() = default;
        // clang-format off
        input_iter(input_iter const&) requires CanCopy = default;
        input_iter(input_iter&&) = default;
        input_iter& operator=(input_iter const&) requires CanCopy = default;
        // clang-format on
        input_iter& operator=(input_iter&&) = default;

        reference operator*() const;
        input_iter& operator++();
        void operator++(int);

        friend constexpr rvalue_reference iter_move(input_iter const&) noexcept {
            return {};
        }
        friend void iter_swap(input_iter const&, input_iter const&) noexcept {}

        friend bool operator==(input_iter const&, std::default_sentinel_t) {
            return true;
        }
        friend int operator-(input_iter const&, std::default_sentinel_t) {
            return 0;
        }
        friend int operator-(std::default_sentinel_t, input_iter const&) {
            return 0;
        }
    };
    struct common {
        common(input_iter<false>::reference);
        common(input_iter<true>::reference);
        common(input_iter<false>::rvalue_reference);
        common(input_iter<true>::rvalue_reference);
    };
} // namespace move_iterator_test

template <>
struct std::common_type<move_iterator_test::input_iter<false>::reference,
    move_iterator_test::input_iter<false>::rvalue_reference> {
    using type = move_iterator_test::common;
};
template <>
struct std::common_type<move_iterator_test::input_iter<false>::rvalue_reference,
    move_iterator_test::input_iter<false>::reference> {
    using type = move_iterator_test::common;
};
template <>
struct std::common_type<move_iterator_test::input_iter<true>::reference,
    move_iterator_test::input_iter<true>::rvalue_reference> {
    using type = move_iterator_test::common;
};
template <>
struct std::common_type<move_iterator_test::input_iter<true>::rvalue_reference,
    move_iterator_test::input_iter<true>::reference> {
    using type = move_iterator_test::common;
};

namespace move_iterator_test {
    // Validate the iterator_concept/iterator_category metaprogramming
    STATIC_ASSERT(same_as<move_iterator<simple_contiguous_iter<>>::iterator_concept, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_contiguous_iter<>>::iterator_category, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_random_iter<>>::iterator_concept, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_random_iter<>>::iterator_category, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_bidi_iter<>>::iterator_concept, bidirectional_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_bidi_iter<>>::iterator_category, bidirectional_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_forward_iter<>>::iterator_concept, forward_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_forward_iter<>>::iterator_category, forward_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_input_iter>::iterator_concept, input_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<simple_input_iter>::iterator_category, input_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<input_iter<true>>::iterator_concept, input_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<xvalue_random_iter>::iterator_concept, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<xvalue_random_iter>::iterator_category, random_access_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<xvalue_bidi_iter>::iterator_concept, bidirectional_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<xvalue_bidi_iter>::iterator_category, bidirectional_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<xvalue_forward_iter>::iterator_concept, forward_iterator_tag>);
    STATIC_ASSERT(same_as<move_iterator<xvalue_forward_iter>::iterator_category, forward_iterator_tag>);
    STATIC_ASSERT(!has_member_iter_category<move_iterator<input_iter<true>>>);
    STATIC_ASSERT(same_as<move_iterator<input_iter<false>>::iterator_concept, input_iterator_tag>);
    STATIC_ASSERT(!has_member_iter_category<move_iterator<input_iter<false>>>);

    // Validate that move_iterator<some_proxy_iterator>::reference is iter_rvalue_reference_t<some_proxy_iterator>
    STATIC_ASSERT(same_as<move_iterator<input_iter<false>>::reference, input_iter<false>::rvalue_reference>);

    // Validate that postincrement returns void for single-pass adaptees
    STATIC_ASSERT(same_as<decltype(move_iterator<input_iter<false>> {} ++), void>);
    STATIC_ASSERT(same_as<decltype(move_iterator<simple_forward_iter<>> {} ++), move_iterator<simple_forward_iter<>>>);

    // Validate comparison constraints
    STATIC_ASSERT(!has_eq<move_iterator<input_iter<false>>>);
    STATIC_ASSERT(!has_neq<move_iterator<input_iter<false>>>);
    STATIC_ASSERT(!has_less<move_iterator<input_iter<false>>>);
    STATIC_ASSERT(!has_greater<move_iterator<input_iter<false>>>);
    STATIC_ASSERT(!has_less_eq<move_iterator<input_iter<false>>>);
    STATIC_ASSERT(!has_greater_eq<move_iterator<input_iter<false>>>);
    STATIC_ASSERT(!three_way_comparable<move_iterator<input_iter<false>>>);

    STATIC_ASSERT(has_eq<move_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(has_neq<move_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(!has_less<move_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(!has_greater<move_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(!has_less_eq<move_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(!has_greater_eq<move_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(!three_way_comparable<move_iterator<simple_forward_iter<>>>);

    STATIC_ASSERT(has_eq<move_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_neq<move_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_less<move_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_greater<move_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_less_eq<move_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_greater_eq<move_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(three_way_comparable<move_iterator<simple_random_iter<>>, std::strong_ordering>);

    STATIC_ASSERT(has_eq<move_iterator<int*>, move_iterator<int const*>>);
    STATIC_ASSERT(has_neq<move_iterator<int*>, move_iterator<int const*>>);
    STATIC_ASSERT(has_less<move_iterator<int*>, move_iterator<int const*>>);
    STATIC_ASSERT(has_greater<move_iterator<int*>, move_iterator<int const*>>);
    STATIC_ASSERT(has_less_eq<move_iterator<int*>, move_iterator<int const*>>);
    STATIC_ASSERT(has_greater_eq<move_iterator<int*>, move_iterator<int const*>>);
    STATIC_ASSERT(three_way_comparable_with<move_iterator<int*>, move_iterator<int const*>, std::strong_ordering>);

    STATIC_ASSERT(!has_eq<move_iterator<int*>, move_iterator<string*>>);
    STATIC_ASSERT(!has_neq<move_iterator<int*>, move_iterator<string*>>);
    STATIC_ASSERT(!has_less<move_iterator<int*>, move_iterator<string*>>);
    STATIC_ASSERT(!has_greater<move_iterator<int*>, move_iterator<string*>>);
    STATIC_ASSERT(!has_less_eq<move_iterator<int*>, move_iterator<string*>>);
    STATIC_ASSERT(!has_greater_eq<move_iterator<int*>, move_iterator<string*>>);
    STATIC_ASSERT(!three_way_comparable_with<move_iterator<int*>, move_iterator<string*>>);

    // Validate that move_sentinel requires a semiregular template argument, and models semiregular
    template <class T>
    concept CanMoveSentinel = requires { typename move_sentinel<T>; };
    struct moveonly {
        moveonly()                      = default;
        moveonly(moveonly&&)            = default;
        moveonly& operator=(moveonly&&) = default;
    };
    STATIC_ASSERT(!CanMoveSentinel<void>);
    STATIC_ASSERT(!CanMoveSentinel<moveonly>);
    STATIC_ASSERT(CanMoveSentinel<int>);
    STATIC_ASSERT(CanMoveSentinel<default_sentinel_t>);

    // Validate move_sentinel comparisons and difference
    STATIC_ASSERT(!has_eq<move_iterator<input_iter<false>>, move_sentinel<int>>);
    STATIC_ASSERT(!has_neq<move_iterator<input_iter<false>>, move_sentinel<int>>);
    STATIC_ASSERT(!has_less<move_iterator<input_iter<false>>, move_sentinel<int>>);
    STATIC_ASSERT(!has_greater<move_iterator<input_iter<false>>, move_sentinel<int>>);
    STATIC_ASSERT(!has_less_eq<move_iterator<input_iter<false>>, move_sentinel<int>>);
    STATIC_ASSERT(!has_greater_eq<move_iterator<input_iter<false>>, move_sentinel<int>>);
    STATIC_ASSERT(!three_way_comparable_with<move_iterator<input_iter<false>>, move_sentinel<int>>);
    STATIC_ASSERT(!has_difference<move_iterator<input_iter<false>>, move_sentinel<int>>);
    STATIC_ASSERT(!has_difference<move_sentinel<int>, move_iterator<input_iter<false>>>);

    STATIC_ASSERT(has_eq<move_iterator<input_iter<false>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(has_neq<move_iterator<input_iter<false>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(!has_less<move_iterator<input_iter<false>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(!has_greater<move_iterator<input_iter<false>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(!has_less_eq<move_iterator<input_iter<false>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(!has_greater_eq<move_iterator<input_iter<false>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(!three_way_comparable_with<move_iterator<input_iter<false>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(has_difference<move_iterator<input_iter<false>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(has_difference<move_sentinel<std::default_sentinel_t>, move_iterator<input_iter<false>>>);

    STATIC_ASSERT(has_eq<move_iterator<simple_random_iter<sentinel_base>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(has_neq<move_iterator<simple_random_iter<sentinel_base>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(!has_less<move_iterator<simple_random_iter<sentinel_base>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(
        !has_greater<move_iterator<simple_random_iter<sentinel_base>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(
        !has_less_eq<move_iterator<simple_random_iter<sentinel_base>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(
        !has_greater_eq<move_iterator<simple_random_iter<sentinel_base>>, move_sentinel<std::default_sentinel_t>>);
    STATIC_ASSERT(!three_way_comparable<move_iterator<simple_random_iter<sentinel_base>>,
                  move_sentinel<std::default_sentinel_t>>);

    // GH-3014 "<ranges>: list-initialization is misused"
    void test_gh_3014() { // COMPILE-ONLY
        using S = test::init_list_not_constructible_sentinel<int>;
        S s;
        [[maybe_unused]] move_sentinel<S> y{s}; // Check 'move_sentinel(S s)'

        move_sentinel<int*> s2;
        [[maybe_unused]] move_sentinel<S> z{s2}; // Check 'move_sentinel(const move_sentinel<S2>& s2)'
    }

    constexpr bool test() {
        // Validate iter_move
        int count = 0;
        move_iterator i{proxy_iterator<0>{&count}};
        assert(ranges::iter_move(i) == 42);
        assert(count == 1);

        // Validate that operator* and operator[] call iter_move
        assert(*i == 42);
        assert(count == 2);
        assert(i[42] == 42);
        assert(count == 3);

        // Validate iter_swap
        ranges::iter_swap(i, move_iterator{proxy_iterator<2>{&count}});
        assert(count == 1);

        // Validate <=>
        int some_ints[] = {3, 2, 1, 0};
        move_iterator<int*> mi{&some_ints[1]};
        move_iterator<int const*> mic{&some_ints[2]};
        assert((mi <=> mi) == std::strong_ordering::equal);
        assert((mi <=> mic) == std::strong_ordering::less);
        assert((mic <=> mi) == std::strong_ordering::greater);

        return true;
    }
    STATIC_ASSERT(test());

    // Validate disable_sized_sentinel_for partial specialization for move_iterator (LWG-3736)
    struct weird_difference_base {
        template <class T>
        long operator-(T const&) const {
            return 42;
        }

        bool operator==(weird_difference_base const&) const = default;
    };
    using simple_no_difference = simple_bidi_iter<weird_difference_base>;
} // namespace move_iterator_test

template <>
inline constexpr bool std::disable_sized_sentinel_for<move_iterator_test::simple_no_difference,
    move_iterator_test::simple_no_difference> = true;

namespace move_iterator_test {
    STATIC_ASSERT(!std::sized_sentinel_for<simple_no_difference, simple_no_difference>);
    STATIC_ASSERT(!std::sized_sentinel_for<move_iterator<simple_no_difference>, move_iterator<simple_no_difference>>);
} // namespace move_iterator_test

namespace counted_iterator_test {
    using std::bidirectional_iterator_tag, std::default_sentinel_t, std::forward_iterator_tag, std::input_iterator_tag,
        std::iterator_traits, std::counted_iterator, std::random_access_iterator_tag, std::same_as, std::string,
        std::three_way_comparable, std::three_way_comparable_with;

    // Validate the iterator_concept/iterator_category metaprogramming
    STATIC_ASSERT(same_as<iterator_traits<counted_iterator<simple_contiguous_iter<>>>::iterator_category,
        random_access_iterator_tag>);
    STATIC_ASSERT(same_as<iterator_traits<counted_iterator<simple_random_iter<>>>::iterator_category,
        random_access_iterator_tag>);
    STATIC_ASSERT(
        same_as<iterator_traits<counted_iterator<simple_bidi_iter<>>>::iterator_category, bidirectional_iterator_tag>);
    STATIC_ASSERT(
        same_as<iterator_traits<counted_iterator<simple_forward_iter<>>>::iterator_category, forward_iterator_tag>);
    STATIC_ASSERT(same_as<iterator_traits<counted_iterator<simple_input_iter>>::iterator_category, input_iterator_tag>);
    STATIC_ASSERT(
        same_as<iterator_traits<counted_iterator<xvalue_random_iter>>::iterator_category, random_access_iterator_tag>);
    STATIC_ASSERT(
        same_as<iterator_traits<counted_iterator<xvalue_bidi_iter>>::iterator_category, bidirectional_iterator_tag>);
    STATIC_ASSERT(
        same_as<iterator_traits<counted_iterator<xvalue_forward_iter>>::iterator_category, forward_iterator_tag>);

    // Validate postincrement
    STATIC_ASSERT(same_as<decltype(std::declval<counted_iterator<simple_input_iter>&>()++), simple_input_iter>);
    STATIC_ASSERT(
        same_as<decltype(counted_iterator<simple_forward_iter<>> {} ++), counted_iterator<simple_forward_iter<>>>);

    // Validate comparison constraints
    STATIC_ASSERT(has_eq<counted_iterator<simple_input_iter>>);
    STATIC_ASSERT(has_neq<counted_iterator<simple_input_iter>>);
    STATIC_ASSERT(has_less<counted_iterator<simple_input_iter>>);
    STATIC_ASSERT(has_greater<counted_iterator<simple_input_iter>>);
    STATIC_ASSERT(has_less_eq<counted_iterator<simple_input_iter>>);
    STATIC_ASSERT(has_greater_eq<counted_iterator<simple_input_iter>>);
    STATIC_ASSERT(three_way_comparable<counted_iterator<simple_input_iter>>);

    STATIC_ASSERT(has_eq<counted_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(has_neq<counted_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(has_less<counted_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(has_greater<counted_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(has_less_eq<counted_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(has_greater_eq<counted_iterator<simple_forward_iter<>>>);
    STATIC_ASSERT(three_way_comparable<counted_iterator<simple_forward_iter<>>>);

    STATIC_ASSERT(has_eq<counted_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_neq<counted_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_less<counted_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_greater<counted_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_less_eq<counted_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(has_greater_eq<counted_iterator<simple_random_iter<>>>);
    STATIC_ASSERT(three_way_comparable<counted_iterator<simple_random_iter<>>, std::strong_ordering>);

    STATIC_ASSERT(has_eq<counted_iterator<int*>, counted_iterator<int const*>>);
    STATIC_ASSERT(has_neq<counted_iterator<int*>, counted_iterator<int const*>>);
    STATIC_ASSERT(has_less<counted_iterator<int*>, counted_iterator<int const*>>);
    STATIC_ASSERT(has_greater<counted_iterator<int*>, counted_iterator<int const*>>);
    STATIC_ASSERT(has_less_eq<counted_iterator<int*>, counted_iterator<int const*>>);
    STATIC_ASSERT(has_greater_eq<counted_iterator<int*>, counted_iterator<int const*>>);
    STATIC_ASSERT(
        three_way_comparable_with<counted_iterator<int*>, counted_iterator<int const*>, std::strong_ordering>);

    STATIC_ASSERT(!has_eq<counted_iterator<int*>, counted_iterator<string*>>);
    STATIC_ASSERT(!has_neq<counted_iterator<int*>, counted_iterator<string*>>);
    STATIC_ASSERT(!has_less<counted_iterator<int*>, counted_iterator<string*>>);
    STATIC_ASSERT(!has_greater<counted_iterator<int*>, counted_iterator<string*>>);
    STATIC_ASSERT(!has_less_eq<counted_iterator<int*>, counted_iterator<string*>>);
    STATIC_ASSERT(!has_greater_eq<counted_iterator<int*>, counted_iterator<string*>>);
    STATIC_ASSERT(!three_way_comparable_with<counted_iterator<int*>, counted_iterator<string*>>);

    // Validate default_sentinel_t comparisons and difference
    STATIC_ASSERT(has_eq<counted_iterator<simple_input_iter>, default_sentinel_t>);
    STATIC_ASSERT(has_neq<counted_iterator<simple_input_iter>, default_sentinel_t>);
    STATIC_ASSERT(!has_less<counted_iterator<simple_input_iter>, default_sentinel_t>);
    STATIC_ASSERT(!has_greater<counted_iterator<simple_input_iter>, default_sentinel_t>);
    STATIC_ASSERT(!has_less_eq<counted_iterator<simple_input_iter>, default_sentinel_t>);
    STATIC_ASSERT(!has_greater_eq<counted_iterator<simple_input_iter>, default_sentinel_t>);
    STATIC_ASSERT(!three_way_comparable_with<counted_iterator<simple_input_iter>, default_sentinel_t>);
    STATIC_ASSERT(has_difference<counted_iterator<simple_input_iter>, default_sentinel_t>);
    STATIC_ASSERT(has_difference<default_sentinel_t, counted_iterator<simple_input_iter>>);
} // namespace counted_iterator_test

namespace lwg3420 {
    // Validate that we can ask for the iterator_traits of a type with no operator* for which checking copyability
    // results in constraint recursion.
    struct X {
        X() = default;
        template <std::copyable T>
        X(T const&);
    };
    STATIC_ASSERT(!has_member_iter_concept<std::iterator_traits<X>>);
    STATIC_ASSERT(!has_member_iter_category<std::iterator_traits<X>>);
    STATIC_ASSERT(!has_member_difference_type<std::iterator_traits<X>>);
    STATIC_ASSERT(!has_member_value_type<std::iterator_traits<X>>);
} // namespace lwg3420

namespace vso1121031 {
    // Validate that indirectly_readable_traits accepts type arguments with both value_type and element_type nested
    // types if they are consistent.
    using std::indirectly_readable_traits, std::same_as;

    template <class Element>
    struct iterish {
        using value_type   = int;
        using element_type = Element;
    };
    STATIC_ASSERT(same_as<indirectly_readable_traits<iterish<int>>::value_type, int>);
    STATIC_ASSERT(same_as<indirectly_readable_traits<iterish<int const>>::value_type, int>);
    STATIC_ASSERT(same_as<indirectly_readable_traits<iterish<int volatile>>::value_type, int>);
    STATIC_ASSERT(same_as<indirectly_readable_traits<iterish<int const volatile>>::value_type, int>);

    STATIC_ASSERT(!has_member_value_type<indirectly_readable_traits<iterish<float>>>);
    STATIC_ASSERT(!has_member_value_type<indirectly_readable_traits<iterish<float const>>>);
    STATIC_ASSERT(!has_member_value_type<indirectly_readable_traits<iterish<float volatile>>>);
    STATIC_ASSERT(!has_member_value_type<indirectly_readable_traits<iterish<float const volatile>>>);
} // namespace vso1121031

int main() {
    iterator_cust_swap_test::test();
    iter_ops::test();
    reverse_iterator_test::test();
    move_iterator_test::test();
}
