// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma warning(disable : 5215) // volatile function arguments are deprecated in C++20

#ifdef __clang__
#pragma clang diagnostic ignored "-Wdeprecated-volatile" // volatile function arguments are deprecated in C++20
#endif // __clang__

#include <cassert>
#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

template <class Iter>
concept CanDifference = requires(Iter it) {
                            { it - it };
                        };

template <class Iter>
concept HasProxy = (!is_reference_v<iter_reference_t<Iter>>);

template <class Iter>
concept CanArrow = requires(const Iter& i) {
                       { i.operator->() };
                   };

struct instantiator {
    template <input_or_output_iterator Iter>
    static constexpr void call() {
        if constexpr (copyable<Iter>) {
            using ConstIter = typename Iter::Consterator;
            using Sen       = test::sentinel<iter_value_t<Iter>>;
            using OSen      = test::sentinel<const iter_value_t<Iter>>;
            using Cit       = common_iterator<Iter, Sen>;
            using OCit      = common_iterator<ConstIter, OSen>;
            P input[3]      = {{0, 1}, {0, 2}, {0, 3}};

            // [common.iter.types]
            {
                using iconcept = typename iterator_traits<Cit>::iterator_concept;
                if constexpr (forward_iterator<Iter>) {
                    STATIC_ASSERT(same_as<iconcept, forward_iterator_tag>);
                } else {
                    STATIC_ASSERT(same_as<typename iterator_traits<Cit>::iterator_concept, input_iterator_tag>);
                }

                using icat = typename iterator_traits<Cit>::iterator_category;
                if constexpr (derived_from<icat, forward_iterator_tag>) {
                    STATIC_ASSERT(same_as<icat, forward_iterator_tag>);
                } else {
                    STATIC_ASSERT(same_as<icat, input_iterator_tag>);
                }

                using ipointer = typename iterator_traits<Cit>::pointer;
                if constexpr (CanArrow<Cit>) {
                    STATIC_ASSERT(same_as<ipointer, decltype(declval<const Cit&>().operator->())>);
                } else {
                    STATIC_ASSERT(same_as<ipointer, void>);
                }
            }

            { // [common.iter.const]
                Cit defaultConstructed{};
                Cit iterConstructed{Iter{input}};
                Cit sentinelConstructed(Sen{});
                Cit copyConstructed{defaultConstructed};
                copyConstructed = iterConstructed;

                OCit conversionConstructed{defaultConstructed};
                conversionConstructed = iterConstructed;

                OCit conversionConstructedSentinel{sentinelConstructed};
                conversionConstructed = iterConstructed;
            }

            { // [common.iter.access]
                Cit iter{Iter{input}};
                assert(*iter == P(0, 1));
                assert(iter->first == 0);
                assert(iter->second == 1);

                using ArrowRetType = decltype(iter.operator->());
                if constexpr (HasProxy<Iter>) {
                    // We return a proxy class here
                    static_assert(is_class_v<ArrowRetType>);
                } else {
                    // Either a pointer or the wrapped iterator
                    static_assert(is_pointer_v<ArrowRetType> || is_same_v<Iter, ArrowRetType>);
                }

                const Cit constIter{Iter{input}};
                assert(*constIter == P(0, 1));
                assert(constIter->first == 0);
                assert(constIter->second == 1);

                static_assert(is_same_v<decltype(constIter.operator->()), ArrowRetType>);
            }

            { // [common.iter.nav]
                Cit iter{Iter{input}};
                ++iter;
                assert(*iter == P(0, 2));

                assert(*iter++ == P(0, 2));
                assert(*iter == P(0, 3));
            }

            { // [common.iter.cmp]
                // Compare iterator / iterator
                assert(Cit{Iter{input}} == Cit{Iter{input}});
                assert(Cit{Iter{input}} != Cit{Iter{input + 1}});

                // Compare iterator / sentinel
                assert(Cit{Iter{input}} == Cit{Sen{input}});
                assert(Cit{Sen{input}} != Cit{Iter{input + 1}});

                // Compare sentinel / sentinel
                assert(Cit{Sen{input}} == Cit{Sen{input}});
                assert(Cit{Sen{input}} == Cit{Sen{input + 1}});

                if constexpr (CanDifference<Iter>) {
                    // Difference iterator / iterator
                    const same_as<iter_difference_t<Iter>> auto diff_it_it = Cit{Iter{input}} - Cit{Iter{input + 1}};
                    assert(diff_it_it == -1);

                    // Difference iterator / sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_it_sen = Cit{Iter{input}} - Cit{Sen{input + 1}};
                    const same_as<iter_difference_t<Iter>> auto diff_sen_it = Cit{Sen{input + 1}} - Cit{Iter{input}};
                    assert(diff_it_sen == -1);
                    assert(diff_sen_it == 1);

                    // Difference sentinel / sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_sen_sen = Cit{Sen{input}} - Cit{Sen{input + 1}};
                    assert(diff_sen_sen == 0);

                    // Difference iterator / other iterator
                    const same_as<iter_difference_t<Iter>> auto diff_it_oit = Cit{Iter{input}} - OCit{Iter{input + 1}};
                    assert(diff_it_oit == -1);

                    // Difference iterator / other sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_it_osen = Cit{Iter{input}} - OCit{OSen{input + 1}};
                    assert(diff_it_osen == -1);

                    // Difference other iterator / sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_sen_oit = Cit{Sen{input + 1}} - OCit{Iter{input}};
                    assert(diff_sen_oit == 1);

                    // Difference sentinel / other sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_sen_osen = Cit{Sen{input}} - OCit{OSen{input + 1}};
                    assert(diff_sen_osen == 0);
                }
            }

            { // [common.iter.cust]
                if constexpr (input_iterator<Iter>) { // iter_move
                    Cit iter1{Iter{input}};

                    const same_as<iter_value_t<Iter>> auto element1 = ranges::iter_move(iter1);
                    assert(element1 == P(0, 1));
                }

                if constexpr (indirectly_swappable<Iter>) { // iter_swap
                    Cit iter1{Iter{input}};
                    Cit iter2{Iter{input + 1}};

                    ranges::iter_swap(iter1, iter2);
                    assert(*iter1 == P(0, 2));
                    assert(*iter2 == P(0, 1));
                }
            }
        }
    }
};

constexpr bool test_operator_arrow() {
    P input[3] = {{0, 1}, {0, 2}, {0, 3}};

    using pointerTest = common_iterator<P*, void*>;
    pointerTest pointerIter{input};

    assert(*pointerIter == P(0, 1));
    assert(pointerIter->first == 0);
    assert(pointerIter->second == 1);
    static_assert(is_same_v<decltype(pointerIter.operator->()), P*>);

    using countedTest = common_iterator<counted_iterator<P*>, default_sentinel_t>;
    countedTest countedIter{counted_iterator{input, 3}};

    assert(*countedIter == P(0, 1));
    assert(countedIter->first == 0);
    assert(countedIter->second == 1);
    static_assert(is_same_v<decltype(countedIter.operator->()), counted_iterator<P*>>);

    return true;
}

// common_iterator supports "copyable but not equality_comparable" iterators, which combination test::iterator does not
// provide (I don't think this is a combination of properties that any real iterator will ever exhibit). Whip up
// something so we can test the iterator_category metaprogramming.
// clang-format off
template <class T>
concept no_iterator_traits = !requires { typename iterator_traits<T>::iterator_concept; }
    && !requires { typename iterator_traits<T>::iterator_category; }
    && !requires { typename iterator_traits<T>::value_type; }
    && !requires { typename iterator_traits<T>::difference_type; }
    && !requires { typename iterator_traits<T>::pointer; }
    && !requires { typename iterator_traits<T>::reference; };
// clang-format on

struct input_copy_but_no_eq {
    using value_type      = int;
    using difference_type = int;

    input_copy_but_no_eq() = delete;

    int operator*() const;
    input_copy_but_no_eq& operator++();
    void operator++(int);

    bool operator==(default_sentinel_t) const;
};
STATIC_ASSERT(input_iterator<input_copy_but_no_eq>);
STATIC_ASSERT(no_iterator_traits<input_copy_but_no_eq>);
STATIC_ASSERT(sentinel_for<default_sentinel_t, input_copy_but_no_eq>);
using ICID = iterator_traits<common_iterator<input_copy_but_no_eq, default_sentinel_t>>;
STATIC_ASSERT(same_as<typename ICID::iterator_category, input_iterator_tag>);

struct poor_sentinel {
    poor_sentinel() = default;
    constexpr poor_sentinel(const poor_sentinel&) {} // non-trivial copy constructor, to test _Variantish behavior
    poor_sentinel& operator=(const poor_sentinel&) = default;

    template <weakly_incrementable Winc>
    [[nodiscard]] constexpr bool operator==(const Winc&) const noexcept {
        return true;
    }

    template <weakly_incrementable Winc>
    [[nodiscard]] constexpr iter_difference_t<Winc> operator-(const Winc&) const noexcept {
        return 0;
    }

    template <weakly_incrementable Winc>
    [[nodiscard]] friend constexpr iter_difference_t<Winc> operator-(const Winc&, const poor_sentinel&) noexcept {
        return 0;
    }
};

constexpr bool test_gh_2065() { // Guard against regression of GH-2065, for which we previously stumbled over CWG-1699.
    {
        int x = 42;
        common_iterator<int*, unreachable_sentinel_t> it1{&x};
        common_iterator<const int*, unreachable_sentinel_t> it2{&x};
        assert(it1 == it2);
    }

    {
        int i = 1729;
        common_iterator<int*, poor_sentinel> it1{&i};
        common_iterator<const int*, poor_sentinel> it2{&i};
        assert(it1 - it2 == 0);
    }

    return true;
}

constexpr bool test_lwg_3574() {
    // LWG-3574: "common_iterator should be completely constexpr-able"
    int arr[]{11, 22, 33};

    {
        common_iterator<int*, const int*> x{arr};
        common_iterator<int*, const int*> y{arr + 2};
        assert(y - x == 2);
    }

    { // test that copy construction is constexpr, even when the sentinel isn't trivially copy constructible
        common_iterator<int*, poor_sentinel> a{arr};
        common_iterator<int*, poor_sentinel> b{a}; // copy-construct with a stored iterator
        common_iterator<int*, poor_sentinel> x{poor_sentinel{}};
        common_iterator<int*, poor_sentinel> y{x}; // copy-construct with a stored sentinel
        assert(b - a == 0);
    }

    common_iterator<int*, unreachable_sentinel_t> i{arr};
    common_iterator<const int*, unreachable_sentinel_t> ci{arr + 1};

    assert(*ci == 22);
    assert(*as_const(ci) == 22);
    assert(ci.operator->() == arr + 1);

    ci = i;
    assert(*ci == 11);
    assert(ci == i);

    assert(*++ci == 22);
    assert(ci != i);

    assert(*ci++ == 22);
    assert(*ci == 33);

    assert(iter_move(i) == 11);

    common_iterator<int*, unreachable_sentinel_t> k{arr + 2};
    iter_swap(i, k);
    assert(arr[0] == 33);
    assert(arr[2] == 11);

    return true;
}

// Validate that _Variantish works when fed with a non-trivially-destructible type
void test_non_trivially_destructible_type() { // COMPILE-ONLY
    struct non_trivially_destructible_input_iterator {
        using difference_type = int;
        using value_type      = int;

        ~non_trivially_destructible_input_iterator() {}

        non_trivially_destructible_input_iterator& operator++() {
            return *this;
        }
        void operator++(int) {}
        int operator*() const {
            return 0;
        }
        bool operator==(default_sentinel_t) const {
            return true;
        }
    };

    common_iterator<non_trivially_destructible_input_iterator, default_sentinel_t> it;
}

struct VolatileSentinel {
    VolatileSentinel()                                   = default;
    VolatileSentinel(const VolatileSentinel&)            = default;
    VolatileSentinel(VolatileSentinel&&)                 = default;
    VolatileSentinel& operator=(const VolatileSentinel&) = default;
    VolatileSentinel& operator=(VolatileSentinel&&)      = default;

    constexpr explicit VolatileSentinel(const char* p) noexcept : ptr_{p} {}

    template <class T = VolatileSentinel>
    constexpr VolatileSentinel(const volatile type_identity_t<T>& other) noexcept : ptr_{other.ptr_} {}
    template <class T = VolatileSentinel>
    constexpr VolatileSentinel(const volatile type_identity_t<T>&& other) noexcept : ptr_{other.ptr_} {}

    template <class T = VolatileSentinel>
    VolatileSentinel& operator=(volatile type_identity_t<T>& rhs) noexcept {
        ptr_ = rhs.ptr_;
        return *this;
    }
    template <class T = VolatileSentinel>
    VolatileSentinel& operator=(volatile type_identity_t<T>&& rhs) noexcept {
        ptr_ = rhs.ptr_;
        return *this;
    }
    template <class T = VolatileSentinel>
    VolatileSentinel& operator=(const volatile type_identity_t<T>& rhs) noexcept {
        ptr_ = rhs.ptr_;
        return *this;
    }
    template <class T = VolatileSentinel>
    VolatileSentinel& operator=(const volatile type_identity_t<T>&& rhs) noexcept {
        ptr_ = rhs.ptr_;
        return *this;
    }

    template <class T = VolatileSentinel>
    volatile VolatileSentinel& operator=(const volatile type_identity_t<T>& rhs) volatile noexcept {
        ptr_ = rhs.ptr_;
        return *this;
    }
    template <class T = VolatileSentinel>
    volatile VolatileSentinel& operator=(const volatile type_identity_t<T>&& rhs) volatile noexcept {
        ptr_ = rhs.ptr_;
        return *this;
    }

    friend constexpr bool operator==(const char* const lhs, VolatileSentinel rhs) noexcept {
        return lhs == rhs.ptr_;
    }

    friend constexpr auto operator-(const char* const lhs, VolatileSentinel rhs) noexcept {
        return lhs - rhs.ptr_;
    }

    friend constexpr auto operator-(VolatileSentinel lhs, const char* const rhs) noexcept {
        return lhs.ptr_ - rhs;
    }

    const char* ptr_ = nullptr;
};

// constexpr-incompatible
void test_volatile() {
    using std::swap;
    using CommonIt = common_iterator<const char*, volatile VolatileSentinel>;

    CommonIt it{static_cast<const char*>(nullptr)};
    CommonIt se{VolatileSentinel{static_cast<const char*>(nullptr)}};

    assert(it == se);
    assert(it - se == 0);
    assert(se - it == 0);

    swap(it, it);
    it = se;
    it = move(se);
}

int main() {
    with_writable_iterators<instantiator, P>::call();
    static_assert(with_writable_iterators<instantiator, P>::call());

    test_operator_arrow();
    static_assert(test_operator_arrow());

    test_gh_2065();
    static_assert(test_gh_2065());

    test_lwg_3574();
    static_assert(test_lwg_3574());

    test_volatile(); // constexpr-incompatible
}
