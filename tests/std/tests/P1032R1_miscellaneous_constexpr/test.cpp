// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <string_view>
#include <tuple>
#include <utility>

using namespace std;

struct constexpr_container {
    using value_type = int;
    using iterator   = array<value_type, 6>::iterator;

    array<value_type, 6> buffer{};
    size_t selected = 0;

    // Test back_insert_iterator
    constexpr void push_back(const value_type i) {
        buffer[selected++] = i;
    }

    // Test front_insert_iterator
    constexpr void push_front(const value_type i) {
        buffer[selected++] = i;
    }

    // Test insert_iterator
    constexpr iterator insert(iterator where, const value_type i) {
        *where = i;
        return where;
    }

    // Fake begin to ensure that we insert at the correct position for insert_iterator
    constexpr iterator begin() {
        return next(buffer.begin());
    }
};

constexpr bool run_tests() {
    // test pair piecewise constructor
    {
        const tuple<int, int, double, double> t1{1, 2, 0.5, 1.5};
        const tuple<double, int> t2{2.5, 3};
        const pair<tuple<int, int, double, double>, tuple<double, int>> meow{piecewise_construct, t1, t2};
        assert(meow.first == t1);
        assert(meow.second == t2);
    }

    // test pair assignment operator
    {
        pair<int, int> input{1, 2};
        pair<int, int> copyAssignment{};
        copyAssignment = input;
        assert(copyAssignment.first == 1);
        assert(copyAssignment.second == 2);

        pair<int, int> moveAssignment{};
        moveAssignment = move(input);
        assert(moveAssignment.first == 1);
        assert(moveAssignment.second == 2);

        pair<double, double> copyAssignmentConvertible{};
        copyAssignmentConvertible = copyAssignment;
        assert(copyAssignmentConvertible.first == 1.0);
        assert(copyAssignmentConvertible.second == 2.0);

        pair<double, double> moveAssignmentConvertible{};
        moveAssignmentConvertible = move(moveAssignment);
        assert(moveAssignmentConvertible.first == 1.0);
        assert(moveAssignmentConvertible.second == 2.0);
    }

    // test pair swap
    {
        pair<int, int> pair1{1, 2};
        pair<int, int> pair2{3, 4};

        swap(pair1, pair2);
        assert(pair1.first == 3 && pair1.second == 4 && pair2.first == 1 && pair2.second == 2);

        pair1.swap(pair2);
        assert(pair1.first == 1 && pair1.second == 2 && pair2.first == 3 && pair2.second == 4);
    }

    // test empty tuple
    {
        allocator<int> alloc;
        tuple<> tuple_alloc{allocator_arg, alloc};
        tuple<> tuple_alloc_copy{allocator_arg, alloc, tuple_alloc};
        tuple<> tuple_alloc_move{allocator_arg, alloc, move(tuple_alloc)};

        tuple_alloc_copy = tuple_alloc;
        tuple_alloc_move = move(tuple_alloc);

        swap(tuple_alloc, tuple_alloc_copy);
        tuple_alloc.swap(tuple_alloc_copy);
    }

    // test tuple
    {
        allocator<int> alloc;
        tuple<short, int> conversionInput{static_cast<short>(1), 2};
        const tuple<short, int> constConversionInput{static_cast<short>(3), 4};
        pair<short, int> conversionInputPair{static_cast<short>(5), 6};
        const pair<short, int> constConversionInputPair{static_cast<short>(7), 8};

        tuple<int, double> tuple_alloc{allocator_arg, alloc};
        assert(get<0>(tuple_alloc) == 0);
        assert(get<1>(tuple_alloc) == 0.0);

        tuple<int, double> tuple_alloc_non_forwarding_value{allocator_arg, alloc, {}, {}};
        assert(get<0>(tuple_alloc_non_forwarding_value) == 0);
        assert(get<1>(tuple_alloc_non_forwarding_value) == 0.0);

        tuple<int, double> tuple_alloc_value{allocator_arg, alloc, 9, 9.5};
        assert(get<0>(tuple_alloc_value) == 9);
        assert(get<1>(tuple_alloc_value) == 9.5);

        tuple<int, double> tuple_alloc_copy{allocator_arg, alloc, tuple_alloc};
        assert(get<0>(tuple_alloc_copy) == 0);
        assert(get<1>(tuple_alloc_copy) == 0.0);

        tuple<int, double> tuple_alloc_move{allocator_arg, alloc, move(tuple_alloc)};
        assert(get<0>(tuple_alloc_move) == 0);
        assert(get<1>(tuple_alloc_move) == 0.0);

        tuple<int, double> tuple_alloc_conversion{allocator_arg, alloc, static_cast<short>(11), 22};
        assert(get<0>(tuple_alloc_conversion) == 11);
        assert(get<1>(tuple_alloc_conversion) == 22.0);

        tuple<int, double> tuple_alloc_conversion_tuple{allocator_arg, alloc, move(conversionInput)};
        assert(get<0>(tuple_alloc_conversion_tuple) == 1);
        assert(get<1>(tuple_alloc_conversion_tuple) == 2.0);

        tuple<int, double> tuple_alloc_conversion_const_tuple{allocator_arg, alloc, constConversionInput};
        assert(get<0>(tuple_alloc_conversion_const_tuple) == 3);
        assert(get<1>(tuple_alloc_conversion_const_tuple) == 4.0);

        tuple<int, double> tuple_alloc_conversion_pair{allocator_arg, alloc, move(conversionInputPair)};
        assert(get<0>(tuple_alloc_conversion_pair) == 5);
        assert(get<1>(tuple_alloc_conversion_pair) == 6.0);

        tuple<int, double> tuple_alloc_conversion_const_pair{allocator_arg, alloc, constConversionInputPair};
        assert(get<0>(tuple_alloc_conversion_const_pair) == 7);
        assert(get<1>(tuple_alloc_conversion_const_pair) == 8.0);

        tuple<int, double> tuple_assign{};

        tuple_assign = tuple_alloc_value;
        assert(get<0>(tuple_assign) == 9);
        assert(get<1>(tuple_assign) == 9.5);

        tuple_assign = move(tuple_alloc);
        assert(get<0>(tuple_assign) == 0);
        assert(get<1>(tuple_assign) == 0.0);

        tuple_assign = move(conversionInput);
        assert(get<0>(tuple_assign) == 1);
        assert(get<1>(tuple_assign) == 2.0);

        tuple_assign = constConversionInput;
        assert(get<0>(tuple_assign) == 3);
        assert(get<1>(tuple_assign) == 4.0);

        tuple_assign = move(conversionInputPair);
        assert(get<0>(tuple_assign) == 5);
        assert(get<1>(tuple_assign) == 6.0);

        tuple_assign = constConversionInputPair;
        assert(get<0>(tuple_assign) == 7);
        assert(get<1>(tuple_assign) == 8.0);

        tuple<int, double> x{10, 20.5};
        tuple<int, double> y{30, 40.5};

        swap(x, y);
        assert(get<0>(x) == 30 && get<1>(x) == 40.5 && get<0>(y) == 10 && get<1>(y) == 20.5);

        x.swap(y);
        assert(get<0>(x) == 10 && get<1>(x) == 20.5 && get<0>(y) == 30 && get<1>(y) == 40.5);
    }

    // test array swap
    {
        array<int, 2> array1{{1, 2}};
        array<int, 2> array2{{3, 4}};

        swap(array1, array2);
        assert(array1[0] == 3 && array1[1] == 4 && array2[0] == 1 && array2[1] == 2);

        array1.swap(array2);
        assert(array1[0] == 1 && array1[1] == 2 && array2[0] == 3 && array2[1] == 4);

        array<int, 0> array_empty1 = {};
        array<int, 0> array_empty2 = {};

        swap(array_empty1, array_empty2);
        array_empty1.swap(array_empty2);
    }

    // test array::fill
    {
        array<int, 2> meow = {};
        meow.fill(1);
        assert(meow[0] == 1 && meow[1] == 1);

        array<int, 0> empty = {};
        empty.fill(1);
    }

    // test back_inserter/back_insert_iterator
    {
        constexpr_container input;
        const int toBeCopied = 5;
        auto tested          = back_inserter(input);

        *tested++ = 42;
        *++tested = 1729;
        *tested++ = 1234;
        tested    = 4;
        tested    = toBeCopied;

        assert(input.buffer[0] == 42 && input.buffer[1] == 1729 && input.buffer[2] == 1234 && input.buffer[3] == 4
               && input.buffer[4] == 5 && input.buffer[5] == 0);
    }

    // test front_inserter/front_insert_iterator
    {
        constexpr_container input;
        const int toBeCopied = 5;
        auto tested          = front_inserter(input);

        *tested++ = 42;
        *++tested = 1729;
        *tested++ = 1234;
        tested    = 4;
        tested    = toBeCopied;

        assert(input.buffer[0] == 42 && input.buffer[1] == 1729 && input.buffer[2] == 1234 && input.buffer[3] == 4
               && input.buffer[4] == 5 && input.buffer[5] == 0);
    }

    // test inserter/insert_iterator
    {
        constexpr_container input;
        const int toBeCopied = 5;
        auto tested          = inserter(input, input.begin());

        *tested++ = 42;
        *++tested = 1729;
        *tested++ = 1234;
        tested    = 4;
        tested    = toBeCopied;

        assert(input.buffer[0] == 0 && input.buffer[1] == 42 && input.buffer[2] == 1729 && input.buffer[3] == 1234
               && input.buffer[4] == 4 && input.buffer[5] == 5);
    }

    // test default_searcher
    {
        string_view in     = "This is some string";
        string_view needle = "some";
        default_searcher meow{needle.begin(), needle.end()};
        auto [first, last] = meow(in.begin(), in.end());
        assert(first - in.begin() == 8);
        assert(last - first == static_cast<ptrdiff_t>(needle.size()));
    }

    // test char_traits assign/copy/_Copy_s/move and basic_string_view::copy/_Copy_s
    {
        using Elem   = char;
        using Traits = char_traits<Elem>;

        const Elem src[20]{"cute fluffy KITTENS"};
        Elem buf[20]{"hungry evil ZOMBIES"};
        assert(buf == "hungry evil ZOMBIES"sv);

        assert(Traits::copy(buf, src, 12) == buf);
        assert(buf == "cute fluffy ZOMBIES"sv);

        assert(Traits::assign(buf, 4, '1') == buf);
        assert(buf == "1111 fluffy ZOMBIES"sv);

        assert(Traits::move(buf, src, 19) == buf); // different arrays
        assert(buf == "cute fluffy KITTENS"sv);

        assert(Traits::move(buf, buf, 19) == buf); // self-assignment
        assert(buf == "cute fluffy KITTENS"sv);

        assert(Traits::move(buf + 5, buf + 12, 6) == buf + 5); // non-overlapping
        assert(buf == "cute KITTEN KITTENS"sv);

        const auto sv = "..........abc......"sv;
        assert(sv.copy(buf, 4, 10) == 4);
        assert(buf == "abc. KITTEN KITTENS"sv);

        assert(sv.copy(buf, 19) == 19);
        assert(buf == "..........abc......"sv);

        assert(Traits::move(buf + 8, buf + 10, 2) == buf + 8); // adjacent, dest before src
        assert(buf == "........ababc......"sv);

        assert(Traits::move(buf + 13, buf + 9, 4) == buf + 13); // adjacent, dest after src
        assert(buf == "........ababcbabc.."sv);

        assert(Traits::move(buf + 4, buf + 8, 10) == buf + 4); // overlapping, dest before src
        assert(buf == "....ababcbabc.abc.."sv);

        assert(Traits::move(buf + 5, buf + 3, 11) == buf + 5); // overlapping, dest after src
        assert(buf == "....a.ababcbabc.c.."sv);

        assert(Traits::_Copy_s(buf, 20, src, 18) == buf);
        assert(buf == "cute fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 4, 10) == 4);
        assert(buf == "abc. fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 11) == 11);
        assert(buf == "..........a KITTEN."sv);
    }

    {
        using Elem   = char16_t;
        using Traits = char_traits<Elem>;

        const Elem src[20]{u"cute fluffy KITTENS"};
        Elem buf[20]{u"hungry evil ZOMBIES"};
        assert(buf == u"hungry evil ZOMBIES"sv);

        assert(Traits::copy(buf, src, 12) == buf);
        assert(buf == u"cute fluffy ZOMBIES"sv);

        assert(Traits::assign(buf, 4, u'1') == buf);
        assert(buf == u"1111 fluffy ZOMBIES"sv);

        assert(Traits::move(buf, src, 19) == buf); // different arrays
        assert(buf == u"cute fluffy KITTENS"sv);

        assert(Traits::move(buf, buf, 19) == buf); // self-assignment
        assert(buf == u"cute fluffy KITTENS"sv);

        assert(Traits::move(buf + 5, buf + 12, 6) == buf + 5); // non-overlapping
        assert(buf == u"cute KITTEN KITTENS"sv);

        const auto sv = u"..........abc......"sv;
        assert(sv.copy(buf, 4, 10) == 4);
        assert(buf == u"abc. KITTEN KITTENS"sv);

        assert(sv.copy(buf, 19) == 19);
        assert(buf == u"..........abc......"sv);

        assert(Traits::move(buf + 8, buf + 10, 2) == buf + 8); // adjacent, dest before src
        assert(buf == u"........ababc......"sv);

        assert(Traits::move(buf + 13, buf + 9, 4) == buf + 13); // adjacent, dest after src
        assert(buf == u"........ababcbabc.."sv);

        assert(Traits::move(buf + 4, buf + 8, 10) == buf + 4); // overlapping, dest before src
        assert(buf == u"....ababcbabc.abc.."sv);

        assert(Traits::move(buf + 5, buf + 3, 11) == buf + 5); // overlapping, dest after src
        assert(buf == u"....a.ababcbabc.c.."sv);

        assert(Traits::_Copy_s(buf, 20, src, 18) == buf);
        assert(buf == u"cute fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 4, 10) == 4);
        assert(buf == u"abc. fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 11) == 11);
        assert(buf == u"..........a KITTEN."sv);
    }

    {
        using Elem   = char32_t;
        using Traits = char_traits<Elem>;

        const Elem src[20]{U"cute fluffy KITTENS"};
        Elem buf[20]{U"hungry evil ZOMBIES"};
        assert(buf == U"hungry evil ZOMBIES"sv);

        assert(Traits::copy(buf, src, 12) == buf);
        assert(buf == U"cute fluffy ZOMBIES"sv);

        assert(Traits::assign(buf, 4, U'1') == buf);
        assert(buf == U"1111 fluffy ZOMBIES"sv);

        assert(Traits::move(buf, src, 19) == buf); // different arrays
        assert(buf == U"cute fluffy KITTENS"sv);

        assert(Traits::move(buf, buf, 19) == buf); // self-assignment
        assert(buf == U"cute fluffy KITTENS"sv);

        assert(Traits::move(buf + 5, buf + 12, 6) == buf + 5); // non-overlapping
        assert(buf == U"cute KITTEN KITTENS"sv);

        const auto sv = U"..........abc......"sv;
        assert(sv.copy(buf, 4, 10) == 4);
        assert(buf == U"abc. KITTEN KITTENS"sv);

        assert(sv.copy(buf, 19) == 19);
        assert(buf == U"..........abc......"sv);

        assert(Traits::move(buf + 8, buf + 10, 2) == buf + 8); // adjacent, dest before src
        assert(buf == U"........ababc......"sv);

        assert(Traits::move(buf + 13, buf + 9, 4) == buf + 13); // adjacent, dest after src
        assert(buf == U"........ababcbabc.."sv);

        assert(Traits::move(buf + 4, buf + 8, 10) == buf + 4); // overlapping, dest before src
        assert(buf == U"....ababcbabc.abc.."sv);

        assert(Traits::move(buf + 5, buf + 3, 11) == buf + 5); // overlapping, dest after src
        assert(buf == U"....a.ababcbabc.c.."sv);

        assert(Traits::_Copy_s(buf, 20, src, 18) == buf);
        assert(buf == U"cute fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 4, 10) == 4);
        assert(buf == U"abc. fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 11) == 11);
        assert(buf == U"..........a KITTEN."sv);
    }

    {
        using Elem   = wchar_t;
        using Traits = char_traits<Elem>;

        const Elem src[20]{L"cute fluffy KITTENS"};
        Elem buf[20]{L"hungry evil ZOMBIES"};
        assert(buf == L"hungry evil ZOMBIES"sv);

        assert(Traits::copy(buf, src, 12) == buf);
        assert(buf == L"cute fluffy ZOMBIES"sv);

        assert(Traits::assign(buf, 4, L'1') == buf);
        assert(buf == L"1111 fluffy ZOMBIES"sv);

        assert(Traits::move(buf, src, 19) == buf); // different arrays
        assert(buf == L"cute fluffy KITTENS"sv);

        assert(Traits::move(buf, buf, 19) == buf); // self-assignment
        assert(buf == L"cute fluffy KITTENS"sv);

        assert(Traits::move(buf + 5, buf + 12, 6) == buf + 5); // non-overlapping
        assert(buf == L"cute KITTEN KITTENS"sv);

        const auto sv = L"..........abc......"sv;
        assert(sv.copy(buf, 4, 10) == 4);
        assert(buf == L"abc. KITTEN KITTENS"sv);

        assert(sv.copy(buf, 19) == 19);
        assert(buf == L"..........abc......"sv);

        assert(Traits::move(buf + 8, buf + 10, 2) == buf + 8); // adjacent, dest before src
        assert(buf == L"........ababc......"sv);

        assert(Traits::move(buf + 13, buf + 9, 4) == buf + 13); // adjacent, dest after src
        assert(buf == L"........ababcbabc.."sv);

        assert(Traits::move(buf + 4, buf + 8, 10) == buf + 4); // overlapping, dest before src
        assert(buf == L"....ababcbabc.abc.."sv);

        assert(Traits::move(buf + 5, buf + 3, 11) == buf + 5); // overlapping, dest after src
        assert(buf == L"....a.ababcbabc.c.."sv);

        assert(Traits::_Copy_s(buf, 20, src, 18) == buf);
        assert(buf == L"cute fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 4, 10) == 4);
        assert(buf == L"abc. fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 11) == 11);
        assert(buf == L"..........a KITTEN."sv);
    }

#ifdef __cpp_lib_char8_t
    {
        using Elem   = char8_t;
        using Traits = char_traits<Elem>;

        const Elem src[20]{u8"cute fluffy KITTENS"};
        Elem buf[20]{u8"hungry evil ZOMBIES"};
        assert(buf == u8"hungry evil ZOMBIES"sv);

        assert(Traits::copy(buf, src, 12) == buf);
        assert(buf == u8"cute fluffy ZOMBIES"sv);

        assert(Traits::assign(buf, 4, u8'1') == buf);
        assert(buf == u8"1111 fluffy ZOMBIES"sv);

        assert(Traits::move(buf, src, 19) == buf); // different arrays
        assert(buf == u8"cute fluffy KITTENS"sv);

        assert(Traits::move(buf, buf, 19) == buf); // self-assignment
        assert(buf == u8"cute fluffy KITTENS"sv);

        assert(Traits::move(buf + 5, buf + 12, 6) == buf + 5); // non-overlapping
        assert(buf == u8"cute KITTEN KITTENS"sv);

        const auto sv = u8"..........abc......"sv;
        assert(sv.copy(buf, 4, 10) == 4);
        assert(buf == u8"abc. KITTEN KITTENS"sv);

        assert(sv.copy(buf, 19) == 19);
        assert(buf == u8"..........abc......"sv);

        assert(Traits::move(buf + 8, buf + 10, 2) == buf + 8); // adjacent, dest before src
        assert(buf == u8"........ababc......"sv);

        assert(Traits::move(buf + 13, buf + 9, 4) == buf + 13); // adjacent, dest after src
        assert(buf == u8"........ababcbabc.."sv);

        assert(Traits::move(buf + 4, buf + 8, 10) == buf + 4); // overlapping, dest before src
        assert(buf == u8"....ababcbabc.abc.."sv);

        assert(Traits::move(buf + 5, buf + 3, 11) == buf + 5); // overlapping, dest after src
        assert(buf == u8"....a.ababcbabc.c.."sv);

        assert(Traits::_Copy_s(buf, 20, src, 18) == buf);
        assert(buf == u8"cute fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 4, 10) == 4);
        assert(buf == u8"abc. fluffy KITTEN."sv);

        assert(sv._Copy_s(buf, 20, 11) == 11);
        assert(buf == u8"..........a KITTEN."sv);
    }
#endif // __cpp_lib_char8_t

    // test char_traits::compare refactoring through basic_string_view (not part of P1032R1)
    {
        assert("cat"sv == "cat"sv);
        assert("cat"sv < "category"sv);
        assert("catastrophe"sv > "cat"sv);
        assert("catch"sv < "category"sv);
        assert("catenary"sv > "catapult"sv);

        assert(u"cat"sv == u"cat"sv);
        assert(u"cat"sv < u"category"sv);
        assert(u"catastrophe"sv > u"cat"sv);
        assert(u"catch"sv < u"category"sv);
        assert(u"catenary"sv > u"catapult"sv);

        assert(U"cat"sv == U"cat"sv);
        assert(U"cat"sv < U"category"sv);
        assert(U"catastrophe"sv > U"cat"sv);
        assert(U"catch"sv < U"category"sv);
        assert(U"catenary"sv > U"catapult"sv);

        assert(L"cat"sv == L"cat"sv);
        assert(L"cat"sv < L"category"sv);
        assert(L"catastrophe"sv > L"cat"sv);
        assert(L"catch"sv < L"category"sv);
        assert(L"catenary"sv > L"catapult"sv);

        assert(u8"cat"sv == u8"cat"sv);
        assert(u8"cat"sv < u8"category"sv);
        assert(u8"catastrophe"sv > u8"cat"sv);
        assert(u8"catch"sv < u8"category"sv);
        assert(u8"catenary"sv > u8"catapult"sv);
    }

    return true;
}

int main() {
    static_assert(run_tests());
    assert(run_tests());
}
