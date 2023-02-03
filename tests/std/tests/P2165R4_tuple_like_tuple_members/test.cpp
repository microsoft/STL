// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <memory_resource>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

template <template <class...> class Template, class T, size_t N, class Indices = make_index_sequence<N>>
struct repeat_type_injection_impl;

template <template <class...> class Template, class T, size_t N, size_t... Indices>
struct repeat_type_injection_impl<Template, T, N, index_sequence<Indices...>> {
    template <size_t>
    using repeat_type = T;

    using type = Template<repeat_type<Indices>...>;
};

template <template <class...> class Template, class T, size_t N>
using repeat_type_injection = typename repeat_type_injection_impl<Template, T, N>::type;

static_assert(same_as<repeat_type_injection<tuple, int, 0>, tuple<>>);
static_assert(same_as<repeat_type_injection<pair, int, 2>, pair<int, int>>);
static_assert(same_as<repeat_type_injection<tuple, int, 4>, tuple<int, int, int, int>>);

struct TmpChar {
    constexpr TmpChar() : TmpChar('\0') {}
    constexpr TmpChar(char c) : val{c} {}

    constexpr TmpChar(const TmpChar&) = default;
    constexpr TmpChar(TmpChar&& other) : val{exchange(other.val, '\x7F')} {}

    constexpr TmpChar& operator=(const TmpChar&) = default;
    constexpr TmpChar& operator=(TmpChar&& other) {
        if (this != &other) {
            val = exchange(other.val, '\x7F');
        }
        return *this;
    }

    char val;
    constexpr bool operator==(const TmpChar&) const = default;
};

struct EvilComma {
    constexpr EvilComma& operator=(size_t) {
        return *this;
    }

    constexpr const EvilComma& operator=(size_t) const {
        return *this;
    }

    constexpr void operator,(auto&&) const {
        assert(false);
    }
};

template <size_t N, template <class...> class TplLike>
struct tester {
public:
    template <class T>
    using Tuple = repeat_type_injection<tuple, T, N>;

    template <class T>
    using TupleLike = repeat_type_injection<TplLike, T, N>;

    static constexpr bool run() {
        using Seq = make_index_sequence<N>;
        static_assert(test_tuple_like_constructor(Seq{}));
        static_assert(test_tuple_like_allocator_constructor(Seq{}));
        static_assert(test_tuple_like_assignment(Seq{}));
        static_assert(test_pair_like_const_assignment(Seq{}));
        return true;
    }

private:
    template <size_t... Indices>
    static constexpr bool test_tuple_like_constructor(index_sequence<Indices...>) {
        // Test tuple(tuple-like) constructor with TupleLike<int>&
        static_assert(constructible_from<Tuple<int>, TupleLike<int>&>);
        static_assert(constructible_from<Tuple<int&>, TupleLike<int>&>);
        static_assert(constructible_from<Tuple<const int&>, TupleLike<int>&>);
        static_assert(!constructible_from<Tuple<int&&>, TupleLike<int>&> || N == 0);
        static_assert(!constructible_from<Tuple<const int&&>, TupleLike<int>&> || N == 0);

        // Test tuple(tuple-like) constructor with const TupleLike<int>&
        static_assert(constructible_from<Tuple<int>, const TupleLike<int>&>);
        static_assert(!constructible_from<Tuple<int&>, const TupleLike<int>&> || N == 0);
        static_assert(constructible_from<Tuple<const int&>, const TupleLike<int>&>);
        static_assert(!constructible_from<Tuple<int&&>, const TupleLike<int>&> || N == 0);
        static_assert(!constructible_from<Tuple<const int&&>, const TupleLike<int>&> || N == 0);

        // Test tuple(tuple-like) constructor with TupleLike<int>
        static_assert(constructible_from<Tuple<int>, TupleLike<int>>);
        static_assert(!constructible_from<Tuple<int&>, TupleLike<int>> || N == 0);
        static_assert(constructible_from<Tuple<const int&>, TupleLike<int>>);
        static_assert(constructible_from<Tuple<int&&>, TupleLike<int>>);
        static_assert(constructible_from<Tuple<const int&&>, TupleLike<int>>);

        // Test tuple(tuple-like) constructor with const TupleLike<int>
        static_assert(constructible_from<Tuple<int>, const TupleLike<int>>);
        static_assert(!constructible_from<Tuple<int&>, const TupleLike<int>> || N == 0);
        static_assert(constructible_from<Tuple<const int&>, const TupleLike<int>>);
        static_assert(!constructible_from<Tuple<int&&>, const TupleLike<int>> || N == 0);
        static_assert(constructible_from<Tuple<const int&&>, const TupleLike<int>>);

        auto get_letter      = [](size_t Idx) { return static_cast<char>('A' + Idx); };
        TupleLike<TmpChar> a = {get_letter(Indices)...}; // 'A', 'B', 'C', ...

        Tuple<TmpChar> t1(a);
        assert((get<Indices>(t1) == get_letter(Indices) && get<Indices>(a) == get_letter(Indices)) && ...);

        Tuple<TmpChar&> t2(a);
        assert((&get<Indices>(t2) == &get<Indices>(a)) && ...);

        Tuple<const TmpChar&> t3(a);
        assert((&get<Indices>(t3) == &get<Indices>(a)) && ...);

        Tuple<TmpChar> t4(as_const(a));
        assert((get<Indices>(t4) == get_letter(Indices) && get<Indices>(a) == get_letter(Indices)) && ...);

        Tuple<const TmpChar&> t5(as_const(a));
        assert((&get<Indices>(t5) == &get<Indices>(a)) && ...);

        Tuple<TmpChar> t6(std::move(a));
        assert((get<Indices>(t6) == get_letter(Indices) && get<Indices>(a) == '\x7F') && ...);
        ((get<Indices>(a) = std::move(get<Indices>(t6))), ...);

        Tuple<const TmpChar&> t7(std::move(a));
        assert((&get<Indices>(t7) == &get<Indices>(a)) && ...);

        Tuple<TmpChar&&> t8(std::move(a));
        assert((&get<Indices>(t8) == &get<Indices>(a)) && ...);

        Tuple<const TmpChar&&> t9(std::move(a));
        assert((&get<Indices>(t9) == &get<Indices>(a)) && ...);

        Tuple<TmpChar> t10(std::move(as_const(a)));
        assert((get<Indices>(t10) == get_letter(Indices) && get<Indices>(a) == get_letter(Indices)) && ...);

        Tuple<const TmpChar&> t11(std::move(as_const(a)));
        assert((get<Indices>(t11) == get_letter(Indices) && get<Indices>(a) == get_letter(Indices)) && ...);

        Tuple<const TmpChar&&> t12(std::move(as_const(a)));
        assert((get<Indices>(t12) == get_letter(Indices) && get<Indices>(a) == get_letter(Indices)) && ...);

        // Check explicit specifier
        static_assert(!convertible_to<string_view, string>);
        static_assert(convertible_to<TupleLike<int>, Tuple<long>>);
        static_assert(!convertible_to<TupleLike<string_view>, Tuple<string>> || N == 0);
        static_assert(constructible_from<Tuple<string>, TupleLike<string_view>>);

        return true;
    }

    template <size_t... Indices>
    static constexpr bool test_tuple_like_allocator_constructor(index_sequence<Indices...>) {
        // Test tuple(allocator_arg_t, Alloc, tuple-like)
        static_assert(constructible_from<Tuple<string>, allocator_arg_t, allocator<char>, TupleLike<string>&>);
        static_assert(constructible_from<Tuple<string>, allocator_arg_t, allocator<char>, const TupleLike<string>&>);
        static_assert(constructible_from<Tuple<string>, allocator_arg_t, allocator<char>, TupleLike<string>>);
        static_assert(constructible_from<Tuple<string>, allocator_arg_t, allocator<char>, const TupleLike<string>>);

        if (!is_constant_evaluated()) {
            using Alloc = pmr::polymorphic_allocator<char>;

            array<byte, 128> buffer;
            pmr::monotonic_buffer_resource resource{buffer.data(), buffer.size()};
            auto get_str = [&](size_t Idx) { return pmr::string({static_cast<char>('A' + Idx)}, Alloc{&resource}); };

            TupleLike<pmr::string> a = {get_str(Indices)...}; // "A"s, "B"s, "C"s, ...

            Tuple<pmr::string> t1(allocator_arg, Alloc{&resource}, a);
            assert((get<Indices>(t1).get_allocator() == get<Indices>(a).get_allocator()) && ...);
            assert((ranges::equal(get<Indices>(t1), get_str(Indices)) //
                       && ranges::equal(get<Indices>(a), get_str(Indices)))
                   && ...);

            array<byte, 128> extra_buffer;
            pmr::monotonic_buffer_resource extra_resource{extra_buffer.data(), extra_buffer.size()};

            Tuple<const pmr::string> t2(allocator_arg, Alloc{&extra_resource}, a);
            assert((get<Indices>(t2).get_allocator() != get<Indices>(a).get_allocator()) && ...);
            assert((ranges::equal(get<Indices>(t2), get_str(Indices)) //
                       && ranges::equal(get<Indices>(a), get_str(Indices)))
                   && ...);
        }

        return true;
    }

    template <size_t... Indices>
    static constexpr bool test_tuple_like_assignment(index_sequence<Indices...>) {
        // Test operator=(tuple-like)
        static_assert(is_assignable_v<Tuple<int>&, TupleLike<int>&>);
        static_assert(is_assignable_v<Tuple<int>&, const TupleLike<int>&>);
        static_assert(is_assignable_v<Tuple<int>&, TupleLike<int>>);
        static_assert(is_assignable_v<Tuple<int>&, const TupleLike<int>>);

        auto get_letter      = [](size_t Idx) { return static_cast<char>('A' + Idx); };
        TupleLike<TmpChar> a = {get_letter(Indices)...};

        Tuple<TmpChar> t1;
        t1 = a;
        assert((get<Indices>(t1) == get_letter(Indices) && get<Indices>(a) == get_letter(Indices)) && ...);

        Tuple<TmpChar> t2;
        t2 = as_const(a);
        assert((get<Indices>(t2) == get_letter(Indices) && get<Indices>(a) == get_letter(Indices)) && ...);

        Tuple<TmpChar> t3;
        t3 = std::move(a);
        assert((get<Indices>(t3) == get_letter(Indices) && get<Indices>(a) == '\x7F') && ...);
        ((get<Indices>(a) = std::move(get<Indices>(t3))), ...);

        Tuple<TmpChar> t4;
        t4 = std::move(as_const(a));
        assert((get<Indices>(t4) == get_letter(Indices) && get<Indices>(a) == get_letter(Indices)) && ...);

        // Check comma hijacking
        Tuple<EvilComma> t5;
        t5 = TupleLike<size_t>{Indices...};

        return true;
    }

    template <size_t... Indices>
    static constexpr bool test_pair_like_const_assignment(index_sequence<Indices...>) {
        using Ref = vector<bool>::reference;

        // Test operator=(tuple-like) const
        static_assert(is_assignable_v<const Tuple<Ref>&, TupleLike<bool>&>);
        static_assert(is_assignable_v<const Tuple<Ref>&, const TupleLike<bool>&>);
        static_assert(is_assignable_v<const Tuple<Ref>&, TupleLike<bool>>);
        static_assert(is_assignable_v<const Tuple<Ref>&, const TupleLike<bool>>);

        auto get_false_true = [](size_t Idx) { return static_cast<bool>(Idx % 2); };
        auto get_true_false = [](size_t Idx) { return static_cast<bool>((Idx + 1) % 2); };

        vector<bool> booleans = {get_false_true(Indices)...}; // false, true, false, true, ...
        TupleLike<bool> a     = {get_true_false(Indices)...}; // true, false, true, false, ...

        const Tuple<Ref> t1{booleans[Indices]...};
        t1 = a;
        assert((get<Indices>(t1) == get_true_false(Indices)) && ...);
        booleans = {get_false_true(Indices)...};

        const Tuple<Ref> t2{booleans[Indices]...};
        t2 = as_const(a);
        assert((get<Indices>(t2) == get_true_false(Indices)) && ...);
        booleans = {get_false_true(Indices)...};

        const Tuple<Ref> t3{booleans[Indices]...};
        t3 = std::move(a);
        assert((get<Indices>(t3) == get_true_false(Indices)) && ...);
        booleans = {get_false_true(Indices)...};

        const Tuple<Ref> t4{booleans[Indices]...};
        t4 = std::move(as_const(a));
        assert((get<Indices>(t4) == get_true_false(Indices)) && ...);

        // Check comma hijacking
        const Tuple<EvilComma> t5;
        t5 = TupleLike<size_t>{Indices...};

        return true;
    }
};

template <class...>
struct TupleLikeArrayImpl;

template <>
struct TupleLikeArrayImpl<> {
    using type = array<int, 0>;
};

template <class Head, class... Rest>
struct TupleLikeArrayImpl<Head, Rest...> {
    using type = array<Head, 1 + sizeof...(Rest)>;
};

template <class... Ts>
using TupleLikeArray = typename TupleLikeArrayImpl<Ts...>::type;

int main() {
    static_assert(tester<0, TupleLikeArray>::run());
    static_assert(tester<1, TupleLikeArray>::run());
    static_assert(tester<2, TupleLikeArray>::run());
    static_assert(tester<2, pair>::run());
    static_assert(tester<3, TupleLikeArray>::run());
}
