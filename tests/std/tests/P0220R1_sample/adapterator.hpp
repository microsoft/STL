// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

namespace impl {
    namespace meta {
        template <class T>
        using _t = typename T::type;

        template <class>
        struct first_ {};
        template <template <class...> class L, class First, class... Rest>
        struct first_<L<First, Rest...>> {
            using type = First;
        };
        template <class T>
        using first = _t<first_<T>>;

        template <class>
        struct second_ {};
        template <template <class...> class L, class First, class Second, class... Rest>
        struct second_<L<First, Second, Rest...>> {
            using type = Second;
        };
        template <class T>
        using second = _t<second_<T>>;
    } // namespace meta

    template <class I, class Cat>
    struct associated_types : std::iterator_traits<I> {
        using iterator_category = Cat;
        using pointer           = I;
    };
    template <class I>
    struct associated_types<I, std::output_iterator_tag> {
        using iterator_category = std::output_iterator_tag;
        using difference_type   = void;
        using value_type        = void;
        using pointer           = void;
        using reference         = void;
    };

    template <class Derived>
    class output : public associated_types<meta::first<Derived>, meta::second<Derived>> {
    protected:
        using I             = meta::first<Derived>;
        using category      = meta::second<Derived>;
        using base_category = typename std::iterator_traits<I>::iterator_category;
        I i_;

    public:
        output() = default;
        explicit output(I i) : i_{std::move(i)} {
            static_assert(std::is_base_of_v<output, Derived>, "Go read about \"CRTP\" on wikipedia.");
        }

        I base() const {
            return i_;
        }

        decltype(auto) operator*() const {
            return *i_;
        }
        Derived& operator++() & {
            ++i_;
            return static_cast<Derived&>(*this);
        }
        Derived operator++(int) & {
            auto tmp = static_cast<Derived&>(*this);
            ++*this;
            return tmp;
        }

        static_assert(std::is_base_of_v<std::forward_iterator_tag, base_category>,
            "The definition of postfix ++ effectively requires forward iterators.");
        static_assert(std::is_same_v<std::output_iterator_tag, category> || std::is_base_of_v<category, base_category>,
            "Base iterator does not model adapted category.");
    };

    template <class Derived>
    class input : public output<Derived> {
    protected:
        using output<Derived>::i_;

    public:
        using pointer = typename output<Derived>::pointer;

        using output<Derived>::output;

        pointer operator->() const {
            return i_;
        }
        friend bool operator==(input const& x, input const& y) {
            return x.i_ == y.i_;
        }
        friend bool operator!=(input const& x, input const& y) {
            return !(x == y);
        }
    };

    template <class Derived>
    class bidi : public input<Derived> {
    protected:
        using input<Derived>::i_;

    public:
        using input<Derived>::input;

        Derived& operator--() & {
            --i_;
            return static_cast<Derived&>(*this);
        }
        Derived operator--(int) & {
            auto tmp = static_cast<Derived&>(*this);
            --*this;
            return tmp;
        }
    };

    template <class Derived>
    class random : public bidi<Derived> {
    protected:
        using bidi<Derived>::i_;

    public:
        using difference_type = typename bidi<Derived>::difference_type;
        using reference       = typename bidi<Derived>::reference;

        using bidi<Derived>::bidi;

        Derived& operator+=(difference_type n) & {
            i_ += n;
            return static_cast<Derived&>(*this);
        }
        friend Derived operator+(random const& x, difference_type n) {
            auto tmp = static_cast<Derived const&>(x);
            tmp += n;
            return tmp;
        }
        friend Derived operator+(difference_type n, random const& x) {
            return x + n;
        }

        Derived& operator-=(difference_type n) & {
            return static_cast<Derived&>(*this += -n);
        }
        friend difference_type operator-(random const& x, random const& y) {
            return x.i_ - y.i_;
        }
        friend Derived operator-(random const& x, difference_type n) {
            return x + -n;
        }

        reference operator[](difference_type i) const {
            return i_[i];
        }

        friend bool operator<(random const& x, random const& y) {
            return x.i_ < y.i_;
        }
        friend bool operator>(random const& x, random const& y) {
            return y < x;
        }
        friend bool operator<=(random const& x, random const& y) {
            return !(y < x);
        }
        friend bool operator>=(random const& x, random const& y) {
            return !(x < y);
        }
    };
} // namespace impl

template <class I, class C>
struct adapterator : impl::output<adapterator<I, C>> {
    adapterator() = delete;
    using impl::output<adapterator>::output;
};

template <class I>
struct adapterator<I, std::input_iterator_tag> : impl::input<adapterator<I, std::input_iterator_tag>> {
    adapterator() = delete;
    using impl::input<adapterator>::input;
};

template <class I>
struct adapterator<I, std::forward_iterator_tag> : impl::input<adapterator<I, std::forward_iterator_tag>> {
    using impl::input<adapterator>::input;
};

template <class I>
struct adapterator<I, std::bidirectional_iterator_tag> : impl::bidi<adapterator<I, std::bidirectional_iterator_tag>> {
    using impl::bidi<adapterator>::bidi;
};

template <class I>
struct adapterator<I, std::random_access_iterator_tag> : impl::random<adapterator<I, std::random_access_iterator_tag>> {
    using impl::random<adapterator>::random;
};
