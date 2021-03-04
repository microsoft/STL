// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma warning(disable : 4389) // signed/unsigned mismatch in arithmetic

#ifdef __clang__
#pragma clang diagnostic ignored "-Wsign-compare"
#endif // __clang__

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

using namespace std;

constexpr auto literal_input     = "Hello fluffy kittens";
constexpr auto literal_input_u16 = u"Hello fluffy kittens";
constexpr auto literal_input_u32 = U"Hello fluffy kittens";
constexpr auto literal_input_w   = L"Hello fluffy kittens";

constexpr auto view_input     = "Hello fluffy kittens"sv;
constexpr auto view_input_u16 = u"Hello fluffy kittens"sv;
constexpr auto view_input_u32 = U"Hello fluffy kittens"sv;
constexpr auto view_input_w   = L"Hello fluffy kittens"sv;

#ifdef __cpp_char8_t
constexpr auto literal_input_u8 = u8"Hello fluffy kittens";
constexpr auto view_input_u8    = u8"Hello fluffy kittens"sv;
#endif // __cpp_char8_t

template <class CharType>
constexpr auto get_literal_input() {
    if constexpr (is_same_v<CharType, char>) {
        return literal_input;
#ifdef __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char8_t>) {
        return literal_input_u8;
#endif // __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char16_t>) {
        return literal_input_u16;
    } else if constexpr (is_same_v<CharType, char32_t>) {
        return literal_input_u32;
    } else {
        return literal_input_w;
    }
}

template <class CharType>
constexpr auto get_view_input() {
    if constexpr (is_same_v<CharType, char>) {
        return view_input;
#ifdef __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char8_t>) {
        return view_input_u8;
#endif // __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char16_t>) {
        return view_input_u16;
    } else if constexpr (is_same_v<CharType, char32_t>) {
        return view_input_u32;
    } else {
        return view_input_w;
    }
}

template <class CharType>
constexpr auto get_cat() {
    if constexpr (is_same_v<CharType, char>) {
        return "kitten";
#ifdef __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char8_t>) {
        return u8"kitten";
#endif // __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char16_t>) {
        return u"kitten";
    } else if constexpr (is_same_v<CharType, char32_t>) {
        return U"kitten";
    } else {
        return L"kitten";
    }
}

template <class CharType>
constexpr auto get_dog() {
    if constexpr (is_same_v<CharType, char>) {
        return "dog";
#ifdef __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char8_t>) {
        return u8"dog";
#endif // __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char16_t>) {
        return u"dog";
    } else if constexpr (is_same_v<CharType, char32_t>) {
        return U"dog";
    } else {
        return L"dog";
    }
}

template <class CharType>
constexpr auto get_no_needle() {
    if constexpr (is_same_v<CharType, char>) {
        return "vxz";
#ifdef __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char8_t>) {
        return u8"vxz";
#endif // __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char16_t>) {
        return u"vxz";
    } else if constexpr (is_same_v<CharType, char32_t>) {
        return U"vxz";
    } else {
        return L"vxz";
    }
}

template <class CharType>
constexpr auto get_cute_and_scratchy() {
    if constexpr (is_same_v<CharType, char>) {
        return "cute and scratchy ";
#ifdef __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char8_t>) {
        return u8"cute and scratchy ";
#endif // __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char16_t>) {
        return u"cute and scratchy ";
    } else if constexpr (is_same_v<CharType, char32_t>) {
        return U"cute and scratchy ";
    } else {
        return L"cute and scratchy ";
    }
}

template <class CharType>
struct string_view_convertible {
    _CONSTEXPR20_CONTAINER operator basic_string_view<CharType>() const {
        if constexpr (is_same_v<CharType, char>) {
            return view_input;
#ifdef __cpp_char8_t
        } else if constexpr (is_same_v<CharType, char8_t>) {
            return view_input_u8;
#endif // __cpp_char8_t
        } else if constexpr (is_same_v<CharType, char16_t>) {
            return view_input_u16;
        } else if constexpr (is_same_v<CharType, char32_t>) {
            return view_input_u32;
        } else {
            return view_input_w;
        }
    }
};

// TRANSITION, EDG concepts support
template <class Range1, class Range2>
constexpr bool equalRanges(const Range1& range1, const Range2& range2) noexcept {
#ifdef __cpp_lib_concepts
    return ranges::equal(range1, range2);
#else // ^^^ __cpp_lib_concepts ^^^ / vvv !__cpp_lib_concepts vvv
    return equal(begin(range1), end(range1), begin(range2), end(range2));
#endif // !__cpp_lib_concepts
}

template <class CharType>
_CONSTEXPR20_CONTAINER bool test_interface() {
#ifndef __EDG__ // TRANSITION, VSO-1273296
    using str = basic_string<CharType>;

    { // constructors
        // range constructors
        str literal_constructed{get_literal_input<CharType>()};
        assert(equalRanges(literal_constructed, get_view_input<CharType>()));

        str view_constructed(get_view_input<CharType>());
        assert(equalRanges(view_constructed, literal_constructed));

        str initializer_list_constructed({CharType{'m'}, CharType{'e'}, CharType{'o'}, CharType{'w'}});
        assert(equalRanges(initializer_list_constructed, "meow"sv));

        // special member functions
        str default_constructed;
        assert(default_constructed.empty());

        str copy_constructed(literal_constructed);
        assert(equalRanges(copy_constructed, literal_constructed));

        str move_constructed(move(copy_constructed));
        assert(equalRanges(move_constructed, literal_constructed));
        assert(copy_constructed.empty());

        str copy_assigned(get_dog<CharType>());
        copy_assigned = literal_constructed;
        assert(equalRanges(copy_assigned, literal_constructed));

        str move_assigned(get_dog<CharType>());
        move_assigned = move(copy_assigned);
        assert(equalRanges(move_assigned, literal_constructed));
        assert(copy_assigned.empty());

        // Other constructors
        str size_value_constructed(5, CharType{'a'});
        assert(equalRanges(size_value_constructed, "aaaaa"sv));

        str copy_start_constructed(literal_constructed, 2);
        assert(equalRanges(copy_start_constructed, "llo fluffy kittens"sv));

        str copy_start_length_constructed(literal_constructed, 2, 3);
        assert(equalRanges(copy_start_length_constructed, "llo"sv));

        str ptr_size_constructed(get_literal_input<CharType>(), 2);
        assert(equalRanges(ptr_size_constructed, "He"sv));
#if defined(MSVC_INTERNAL_TESTING) || defined(__EDG__) // TRANSITION, VSO-1270433
        str iterator_constructed(literal_constructed.begin(), literal_constructed.end());
        assert(equalRanges(iterator_constructed, literal_constructed));
#endif // defined(MSVC_INTERNAL_TESTING) || defined(__EDG__)
        const string_view_convertible<CharType> convertible;
        str conversion_constructed(convertible);
        assert(equalRanges(conversion_constructed, literal_constructed));

        str conversion_start_length_constructed(convertible, 2, 3);
        assert(equalRanges(conversion_start_length_constructed, "llo"sv));
    }

    { // allocator constructors
        allocator<CharType> alloc;

        // range constructors
        str literal_constructed{get_literal_input<CharType>(), alloc};
        assert(equalRanges(literal_constructed, get_view_input<CharType>()));

        str view_constructed{get_view_input<CharType>(), alloc};
        assert(equalRanges(view_constructed, literal_constructed));

        str initializer_list_constructed({CharType{'m'}, CharType{'e'}, CharType{'o'}, CharType{'w'}}, alloc);
        assert(equalRanges(initializer_list_constructed, "meow"sv));

        // special member functions
        str default_constructed{alloc};
        assert(default_constructed.empty());

        str copy_constructed{literal_constructed, alloc};
        assert(equalRanges(copy_constructed, literal_constructed));

        str move_constructed{move(copy_constructed), alloc};
        assert(equalRanges(move_constructed, literal_constructed));
        assert(copy_constructed.empty());

        // Other constructors
        str size_value_constructed(5, CharType{'a'}, alloc);
        assert(equalRanges(size_value_constructed, "aaaaa"sv));

        str copy_start_constructed(literal_constructed, 2, alloc);
        assert(equalRanges(copy_start_constructed, "llo fluffy kittens"sv));

        str copy_start_length_constructed(literal_constructed, 2, 3, alloc);
        assert(equalRanges(copy_start_length_constructed, "llo"sv));

        str ptr_size_constructed(get_literal_input<CharType>(), 2, alloc);
        assert(equalRanges(ptr_size_constructed, "He"sv));
#if defined(MSVC_INTERNAL_TESTING) || defined(__EDG__) // TRANSITION, VSO-1270433
        str iterator_constructed(literal_constructed.begin(), literal_constructed.end(), alloc);
        assert(equalRanges(iterator_constructed, literal_constructed));
#endif // defined(MSVC_INTERNAL_TESTING) || defined(__EDG__)
        const string_view_convertible<CharType> convertible;
        str conversion_constructed(convertible, alloc);
        assert(equalRanges(conversion_constructed, literal_constructed));

        str conversion_start_length_constructed(convertible, 2, 3, alloc);
        assert(equalRanges(conversion_start_length_constructed, "llo"sv));
    }

    { // assignment operator
        str literal_constructed = get_literal_input<CharType>();

        str copy_assigned;
        copy_assigned = literal_constructed;
        assert(equalRanges(copy_assigned, literal_constructed));

        str move_assigned;
        move_assigned = move(copy_assigned);
        assert(equalRanges(move_assigned, literal_constructed));
        assert(copy_assigned.empty());

        str literal_assigned;
        literal_assigned = get_literal_input<CharType>();
        assert(equalRanges(literal_assigned, literal_constructed));

        str char_assigned;
        char_assigned = CharType{'!'};
        assert(equalRanges(char_assigned, "!"sv));

        str initializer_list_assigned;
        initializer_list_assigned = {CharType{'m'}, CharType{'e'}, CharType{'o'}, CharType{'w'}};
        assert(equalRanges(initializer_list_assigned, "meow"sv));

        const string_view_convertible<CharType> convertible;
        str conversion_assigned;
        conversion_assigned = convertible;
        assert(equalRanges(conversion_assigned, literal_constructed));
    }

    { // assign
        str literal_constructed = get_literal_input<CharType>();

        str assign_size_char;
        assign_size_char.assign(5, CharType{'a'});
        assert(equalRanges(assign_size_char, "aaaaa"sv));

        str assign_str;
        assign_str.assign(literal_constructed);
        assert(equalRanges(assign_str, literal_constructed));

        str assign_str_pos;
        assign_str_pos.assign(literal_constructed, 2);
        assert(equalRanges(assign_str_pos, "llo fluffy kittens"sv));

        str assign_str_pos_len;
        assign_str_pos_len.assign(literal_constructed, 2, 3);
        assert(equalRanges(assign_str_pos_len, "llo"sv));

        str assign_moved_str;
        assign_moved_str.assign(move(assign_str_pos_len));
        assert(equalRanges(assign_moved_str, "llo"sv));
        assert(assign_str_pos_len.empty());

        str assign_literal;
        assign_literal.assign(get_literal_input<CharType>());
        assert(equalRanges(assign_literal, literal_constructed));

        str assign_literal_count;
        assign_literal_count.assign(get_literal_input<CharType>(), 2);
        assert(equalRanges(assign_literal_count, "He"sv));

        str assign_iterator;
        assign_iterator.assign(begin(get_view_input<CharType>()), end(get_view_input<CharType>()));
        assert(equalRanges(assign_iterator, get_view_input<CharType>()));

        str assign_initializer_list;
        assign_initializer_list.assign({CharType{'m'}, CharType{'e'}, CharType{'o'}, CharType{'w'}});
        assert(equalRanges(assign_initializer_list, "meow"sv));

        const string_view_convertible<CharType> convertible;
        str assign_conversion;
        assign_conversion.assign(convertible);
        assert(equalRanges(assign_conversion, literal_constructed));

        str assign_conversion_start_length;
        assign_conversion_start_length.assign(convertible, 2, 3);
        assert(equalRanges(assign_conversion_start_length, "llo"sv));
    }

    { // allocator
        str default_constructed;
        [[maybe_unused]] const auto alloc = default_constructed.get_allocator();
        static_assert(is_same_v<remove_const_t<decltype(alloc)>, allocator<CharType>>);
    }

    { // access
        str literal_constructed             = get_literal_input<CharType>();
        const str const_literal_constructed = get_literal_input<CharType>();

        const auto at = literal_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(at)>, CharType>);
        assert(at == CharType{'l'});

        literal_constructed.at(2) = CharType{'v'};
        const auto at2            = literal_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(at2)>, CharType>);
        assert(at2 == CharType{'v'});

        const auto cat = const_literal_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(cat)>, CharType>);
        assert(cat == CharType{'l'});

        const auto op = literal_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(op)>, CharType>);
        assert(op == CharType{'l'});

        literal_constructed[3] = CharType{'u'};
        const auto op2         = literal_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(op2)>, CharType>);
        assert(op2 == CharType{'u'});

        const auto cop = const_literal_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(cop)>, CharType>);
        assert(cop == CharType{'l'});

        const auto f = literal_constructed.front();
        static_assert(is_same_v<remove_const_t<decltype(f)>, CharType>);
        assert(f == CharType{'H'});

        const auto cf = const_literal_constructed.front();
        static_assert(is_same_v<remove_const_t<decltype(cf)>, CharType>);
        assert(cf == CharType{'H'});

        const auto b = literal_constructed.back();
        static_assert(is_same_v<remove_const_t<decltype(b)>, CharType>);
        assert(b == CharType{'s'});

        const auto cb = const_literal_constructed.back();
        static_assert(is_same_v<remove_const_t<decltype(cb)>, CharType>);
        assert(cb == CharType{'s'});

        const auto d = literal_constructed.data();
        static_assert(is_same_v<decltype(d), CharType* const>);
        assert(*d == CharType{'H'});

        const auto cd = const_literal_constructed.data();
        static_assert(is_same_v<decltype(cd), const CharType* const>);
        assert(*cd == CharType{'H'});

        const auto cs = literal_constructed.c_str();
        static_assert(is_same_v<decltype(cs), const CharType* const>);
        assert(cs == literal_constructed.data());
        assert(char_traits<CharType>::length(cs) == literal_constructed.size());
    }

    { // iterators
        str literal_constructed             = get_literal_input<CharType>();
        const str const_literal_constructed = get_literal_input<CharType>();

        const auto b = literal_constructed.begin();
        static_assert(is_same_v<remove_const_t<decltype(b)>, typename str::iterator>);
        assert(*b == CharType{'H'});

        const auto cb = literal_constructed.cbegin();
        static_assert(is_same_v<remove_const_t<decltype(cb)>, typename str::const_iterator>);
        assert(*cb == CharType{'H'});

        const auto cb2 = const_literal_constructed.begin();
        static_assert(is_same_v<remove_const_t<decltype(cb2)>, typename str::const_iterator>);
        assert(*cb2 == CharType{'H'});

        const auto e = literal_constructed.end();
        static_assert(is_same_v<remove_const_t<decltype(e)>, typename str::iterator>);
        assert(*prev(e) == CharType{'s'});

        const auto ce = literal_constructed.cend();
        static_assert(is_same_v<remove_const_t<decltype(ce)>, typename str::const_iterator>);
        assert(*prev(ce) == CharType{'s'});

        const auto ce2 = const_literal_constructed.end();
        static_assert(is_same_v<remove_const_t<decltype(ce2)>, typename str::const_iterator>);
        assert(*prev(ce2) == CharType{'s'});

        const auto rb = literal_constructed.rbegin();
        static_assert(is_same_v<remove_const_t<decltype(rb)>, reverse_iterator<typename str::iterator>>);
        assert(*rb == CharType{'s'});

        const auto crb = literal_constructed.crbegin();
        static_assert(is_same_v<remove_const_t<decltype(crb)>, reverse_iterator<typename str::const_iterator>>);
        assert(*crb == CharType{'s'});

        const auto crb2 = const_literal_constructed.rbegin();
        static_assert(is_same_v<remove_const_t<decltype(crb2)>, reverse_iterator<typename str::const_iterator>>);
        assert(*crb2 == CharType{'s'});

        const auto re = literal_constructed.rend();
        static_assert(is_same_v<remove_const_t<decltype(re)>, reverse_iterator<typename str::iterator>>);
        assert(*prev(re) == CharType{'H'});

        const auto cre = literal_constructed.crend();
        static_assert(is_same_v<remove_const_t<decltype(cre)>, reverse_iterator<typename str::const_iterator>>);
        assert(*prev(cre) == CharType{'H'});

        const auto cre2 = const_literal_constructed.rend();
        static_assert(is_same_v<remove_const_t<decltype(cre2)>, reverse_iterator<typename str::const_iterator>>);
        assert(*prev(cre2) == CharType{'H'});
    }

    { // capacity
        str literal_constructed = get_literal_input<CharType>();

        const auto e = literal_constructed.empty();
        static_assert(is_same_v<remove_const_t<decltype(e)>, bool>);
        assert(!e);

        const auto s = literal_constructed.size();
        static_assert(is_same_v<remove_const_t<decltype(s)>, size_t>);
        assert(s == size(get_view_input<CharType>()));

        const auto l = literal_constructed.length();
        static_assert(is_same_v<remove_const_t<decltype(l)>, size_t>);
        assert(l == s);

        const auto ms = literal_constructed.max_size();
        static_assert(is_same_v<remove_const_t<decltype(ms)>, size_t>);
        if constexpr (is_same_v<CharType, char16_t> || is_same_v<CharType, char32_t> || is_same_v<CharType, wchar_t>) {
            assert(ms == static_cast<size_t>(-1) / sizeof(CharType) - 1);
        } else {
            assert(ms == static_cast<size_t>(-1) / 2);
        }

        literal_constructed.reserve(20);

        const auto c = literal_constructed.capacity();
        static_assert(is_same_v<remove_const_t<decltype(c)>, size_t>);
        if constexpr (is_same_v<CharType, char16_t> || is_same_v<CharType, char32_t> || is_same_v<CharType, wchar_t>) {
            assert(c == 23);
        } else {
            assert(c == 31);
        }

        // make reserve actually do work
        literal_constructed.reserve(35);
        const auto c2 = literal_constructed.capacity();
        if constexpr (is_same_v<CharType, char16_t> || is_same_v<CharType, wchar_t>) {
            assert(c2 == 39);
        } else if constexpr (is_same_v<CharType, char32_t>) {
            assert(c2 == 35);
        } else {
            assert(c2 == 47);
        }

        // shrink back to previous size
        literal_constructed.shrink_to_fit();

        const auto c3 = literal_constructed.capacity();
        if constexpr (is_same_v<CharType, char16_t> || is_same_v<CharType, char32_t> || is_same_v<CharType, wchar_t>) {
            assert(c3 == 23);
        } else {
            assert(c3 == 31);
        }

        literal_constructed.erase(3);
        literal_constructed.shrink_to_fit();

        const auto c4 = literal_constructed.capacity();
        if (is_constant_evaluated()) { // check minimum allocation of _BUF_SIZE when constant evaluated
            assert(c4 == 16 / sizeof(CharType));
        } else {
            if constexpr (is_same_v<CharType, char16_t> || is_same_v<CharType, wchar_t>) {
                assert(c4 == 7);
            } else if constexpr (is_same_v<CharType, char32_t>) {
                assert(c4 == 3);
            } else {
                assert(c4 == 15);
            }
        }
    }

    { // clear
        str cleared = get_literal_input<CharType>();
        cleared.clear();
        assert(cleared.empty());
        assert(cleared.capacity() == str{get_literal_input<CharType>()}.capacity());
    }

    { // insert
        str insert_char               = get_literal_input<CharType>();
        const CharType to_be_inserted = CharType{','};
        insert_char.insert(insert_char.begin() + 5, to_be_inserted);
        assert(equalRanges(insert_char, "Hello, fluffy kittens"sv));

        str insert_const_char = get_literal_input<CharType>();
        insert_const_char.insert(insert_const_char.cbegin() + 5, to_be_inserted);
        assert(equalRanges(insert_const_char, "Hello, fluffy kittens"sv));

        str insert_char_rvalue = get_literal_input<CharType>();
        insert_char_rvalue.insert(insert_char_rvalue.begin() + 5, CharType{','});
        assert(equalRanges(insert_char_rvalue, "Hello, fluffy kittens"sv));

        str insert_const_char_rvalue = get_literal_input<CharType>();
        insert_const_char_rvalue.insert(insert_const_char_rvalue.cbegin() + 5, CharType{','});
        assert(equalRanges(insert_const_char_rvalue, "Hello, fluffy kittens"sv));

        str insert_range(2, CharType{'b'});
        const auto it = insert_range.insert(
            insert_range.begin() + 1, begin(get_view_input<CharType>()), end(get_view_input<CharType>()));
        assert(it == insert_range.begin() + 1);
        assert(equalRanges(insert_range, "bHello fluffy kittensb"sv));

        str insert_const_range(2, CharType{'b'});
        const auto cit = insert_const_range.insert(
            insert_const_range.cbegin() + 1, begin(get_view_input<CharType>()), end(get_view_input<CharType>()));
        assert(cit == insert_const_range.cbegin() + 1);
        assert(equalRanges(insert_const_range, "bHello fluffy kittensb"sv));

        str insert_initializer_list = get_literal_input<CharType>();
        const auto it_ilist         = insert_initializer_list.insert(insert_initializer_list.begin() + 6,
            {CharType{'c'}, CharType{'u'}, CharType{'t'}, CharType{'e'}, CharType{' '}});
        assert(it_ilist == insert_initializer_list.begin() + 6);
        assert(equalRanges(insert_initializer_list, "Hello cute fluffy kittens"sv));

        str insert_const_initializer_list = get_literal_input<CharType>();
        const auto cit_ilist = insert_const_initializer_list.insert(insert_const_initializer_list.cbegin() + 6,
            {CharType{'c'}, CharType{'u'}, CharType{'t'}, CharType{'e'}, CharType{' '}});
        assert(cit_ilist == insert_const_initializer_list.cbegin() + 6);
        assert(equalRanges(insert_const_initializer_list, "Hello cute fluffy kittens"sv));

        str insert_pos_str  = get_literal_input<CharType>();
        const str to_insert = get_cute_and_scratchy<CharType>();
        insert_pos_str.insert(6, to_insert);
        assert(equalRanges(insert_pos_str, "Hello cute and scratchy fluffy kittens"sv));

        str insert_pos_substr = get_literal_input<CharType>();
        insert_pos_substr.insert(6, to_insert, 0, 5);
        assert(equalRanges(insert_pos_substr, "Hello cute fluffy kittens"sv));

        const string_view_convertible<CharType> convertible;
        str insert_pos_conversion = get_literal_input<CharType>();
        insert_pos_conversion.insert(6, convertible);
        assert(equalRanges(insert_pos_conversion, "Hello Hello fluffy kittensfluffy kittens"sv));

        str insert_pos_conversion_substr = get_literal_input<CharType>();
        insert_pos_conversion_substr.insert(6, convertible, 6, 7);
        assert(equalRanges(insert_pos_conversion_substr, "Hello fluffy fluffy kittens"sv));

        str insert_pos_literal = get_literal_input<CharType>();
        insert_pos_literal.insert(6, get_literal_input<CharType>());
        assert(equalRanges(insert_pos_literal, "Hello Hello fluffy kittensfluffy kittens"sv));

        str insert_pos_literal_substr = get_literal_input<CharType>();
        insert_pos_literal_substr.insert(6, get_literal_input<CharType>(), 6);
        assert(equalRanges(insert_pos_literal_substr, "Hello Hello fluffy kittens"sv));

        str insert_pos_count_char = get_literal_input<CharType>();
        insert_pos_count_char.insert(6, 3, CharType{'b'});
        assert(equalRanges(insert_pos_count_char, "Hello bbbfluffy kittens"sv));

        str insert_iter_count_char = get_literal_input<CharType>();
        insert_iter_count_char.insert(begin(insert_iter_count_char) + 5, 4, CharType{'o'});
        assert(equalRanges(insert_iter_count_char, "Hellooooo fluffy kittens"sv));
    }

    { // erase
        str erase_pos_count = get_literal_input<CharType>();
        erase_pos_count.erase(0, 6);
        assert(equalRanges(erase_pos_count, "fluffy kittens"sv));

        str erase_iter = get_literal_input<CharType>();
        erase_iter.erase(erase_iter.begin());
        assert(equalRanges(erase_iter, "ello fluffy kittens"sv));

        str erase_const_iter = get_literal_input<CharType>();
        erase_const_iter.erase(erase_const_iter.cbegin());
        assert(equalRanges(erase_const_iter, "ello fluffy kittens"sv));

        str erase_iter_iter = get_literal_input<CharType>();
        erase_iter_iter.erase(erase_iter_iter.begin(), erase_iter_iter.begin() + 6);
        assert(equalRanges(erase_iter_iter, "fluffy kittens"sv));

        str erase_const_iter_iter = get_literal_input<CharType>();
        erase_const_iter_iter.erase(erase_const_iter_iter.cbegin(), erase_const_iter_iter.cbegin() + 6);
        assert(equalRanges(erase_const_iter_iter, "fluffy kittens"sv));

        str erased_free = get_literal_input<CharType>();
        erase(erased_free, CharType{'l'});
        assert(equalRanges(erased_free, "Heo fuffy kittens"sv));

        str erased_free_if = get_literal_input<CharType>();
        erase_if(erased_free_if, [](const CharType val) { return val == CharType{'t'}; });
        assert(equalRanges(erased_free_if, "Hello fluffy kiens"sv));
    }

    { // push_back / pop_back
        str pushed;
        pushed.push_back(CharType{'y'});
        assert(pushed.size() == 1);
        assert(pushed.back() == CharType{'y'});

        const CharType to_be_pushed = CharType{'z'};
        pushed.push_back(to_be_pushed);
        assert(pushed.size() == 2);
        assert(pushed.back() == CharType{'z'});

        pushed.pop_back();
        assert(pushed.size() == 1);
        assert(pushed.back() == CharType{'y'});
    }

    { // append
        const str literal_constructed = get_literal_input<CharType>();

        str append_size_char(2, CharType{'b'});
        append_size_char.append(5, CharType{'a'});
        assert(equalRanges(append_size_char, "bbaaaaa"sv));

        str append_str(2, CharType{'b'});
        append_str.append(literal_constructed);
        assert(equalRanges(append_str, "bbHello fluffy kittens"sv));

        str append_str_pos(2, CharType{'b'});
        append_str_pos.append(literal_constructed, 2);
        assert(equalRanges(append_str_pos, "bbllo fluffy kittens"sv));

        str append_str_pos_len(2, CharType{'b'});
        append_str_pos_len.append(literal_constructed, 2, 3);
        assert(equalRanges(append_str_pos_len, "bbllo"sv));

        str append_literal(2, CharType{'b'});
        append_literal.append(get_literal_input<CharType>());
        assert(equalRanges(append_literal, "bbHello fluffy kittens"sv));

        str append_literal_count(2, CharType{'b'});
        append_literal_count.append(get_literal_input<CharType>(), 2);
        assert(equalRanges(append_literal_count, "bbHe"sv));

        str append_iterator(2, CharType{'b'});
        append_iterator.append(begin(get_view_input<CharType>()), end(get_view_input<CharType>()));
        assert(equalRanges(append_iterator, "bbHello fluffy kittens"sv));

        str append_initializer_list(2, CharType{'b'});
        append_initializer_list.append({CharType{'m'}, CharType{'e'}, CharType{'o'}, CharType{'w'}});
        assert(equalRanges(append_initializer_list, "bbmeow"sv));

        const string_view_convertible<CharType> convertible;
        str append_conversion(2, CharType{'b'});
        append_conversion.append(convertible);
        assert(equalRanges(append_conversion, "bbHello fluffy kittens"sv));

        str append_conversion_start_length(2, CharType{'b'});
        append_conversion_start_length.append(convertible, 2, 3);
        assert(equalRanges(append_conversion_start_length, "bbllo"sv));
    }

    { // operator+=
        str literal_constructed = get_literal_input<CharType>();

        str plus_string(2, CharType{'b'});
        plus_string += literal_constructed;
        assert(equalRanges(plus_string, "bbHello fluffy kittens"sv));

        str plus_character(2, CharType{'b'});
        plus_character += CharType{'a'};
        assert(equalRanges(plus_character, "bba"sv));

        str plus_literal(2, CharType{'b'});
        plus_literal += get_literal_input<CharType>();
        assert(equalRanges(plus_literal, "bbHello fluffy kittens"sv));

        str plus_initializer_list(2, CharType{'b'});
        plus_initializer_list += {CharType{'m'}, CharType{'e'}, CharType{'o'}, CharType{'w'}};
        assert(equalRanges(plus_initializer_list, "bbmeow"sv));

        const string_view_convertible<CharType> convertible;
        str plus_conversion(2, CharType{'b'});
        plus_conversion += convertible;
        assert(equalRanges(plus_conversion, "bbHello fluffy kittens"sv));
    }

    { // compare
        const str first  = get_literal_input<CharType>();
        const str second = get_cat<CharType>();

        const int comp_str_eq = first.compare(first);
        assert(comp_str_eq == 0);

        const int comp_str_less = first.compare(second);
        assert(comp_str_less == -1);

        const int comp_str_greater = second.compare(first);
        assert(comp_str_greater == 1);

        const int comp_pos_count_str_eq = first.compare(3, 7, first.substr(3, 7));
        assert(comp_pos_count_str_eq == 0);

        const int comp_pos_count_str_less = first.compare(0, 2, second);
        assert(comp_pos_count_str_less == -1);

        const int comp_pos_count_str_greater = second.compare(0, 2, first);
        assert(comp_pos_count_str_greater == 1);

        const int comp_pos_count_str_pos_eq = first.compare(3, 20, first, 3);
        assert(comp_pos_count_str_pos_eq == 0);

        const int comp_pos_count_str_pos_less = first.compare(0, 2, second, 2);
        assert(comp_pos_count_str_pos_less == -1);

        const int comp_pos_count_str_pos_greater = second.compare(0, 2, first, 6);
        assert(comp_pos_count_str_pos_greater == 1);

        const int comp_pos_count_str_pos_count_eq = first.compare(3, 5, first, 3, 5);
        assert(comp_pos_count_str_pos_count_eq == 0);

        const int comp_pos_count_str_pos_count_less = first.compare(0, 2, second, 2, 3);
        assert(comp_pos_count_str_pos_count_less == -1);

        const int comp_pos_count_str_pos_count_greater = second.compare(0, 2, first, 6, 4);
        assert(comp_pos_count_str_pos_count_greater == 1);

        const int comp_literal_eq = first.compare(get_literal_input<CharType>());
        assert(comp_literal_eq == 0);

        const int comp_literal_less = first.compare(get_cat<CharType>());
        assert(comp_literal_less == -1);

        const int comp_literal_greater = second.compare(get_literal_input<CharType>());
        assert(comp_literal_greater == 1);

        const int comp_pos_count_literal_eq = first.compare(13, 6, get_cat<CharType>());
        assert(comp_pos_count_literal_eq == 0);

        const int comp_pos_count_literal_less = first.compare(0, 2, get_cat<CharType>());
        assert(comp_pos_count_literal_less == -1);

        const int comp_pos_count_literal_greater = second.compare(0, 2, get_literal_input<CharType>());
        assert(comp_pos_count_literal_greater == 1);

        const int comp_pos_count_literal_count_eq = first.compare(13, 5, get_cat<CharType>(), 5);
        assert(comp_pos_count_literal_count_eq == 0);

        const int comp_pos_count_literal_count_less = first.compare(0, 2, get_cat<CharType>(), 2);
        assert(comp_pos_count_literal_count_less == -1);

        const int comp_pos_count_literal_count_greater = second.compare(0, 2, get_literal_input<CharType>(), 6);
        assert(comp_pos_count_literal_count_greater == 1);

        const int comp_pos_count_literal_pos_count_eq = first.compare(3, 5, get_literal_input<CharType>(), 3, 5);
        assert(comp_pos_count_literal_pos_count_eq == 0);

        const int comp_pos_count_literal_pos_count_less = first.compare(0, 2, get_cat<CharType>(), 2, 3);
        assert(comp_pos_count_literal_pos_count_less == -1);

        const int comp_pos_count_literal_pos_count_greater = second.compare(0, 2, get_literal_input<CharType>(), 6, 4);
        assert(comp_pos_count_literal_pos_count_greater == 1);

        const string_view_convertible<CharType> convertible;
        const int comp_conversion_eq = first.compare(convertible);
        assert(comp_conversion_eq == 0);

        const int comp_conversion_less = first.compare(second);
        assert(comp_conversion_less == -1);

        const int comp_conversion_greater = second.compare(convertible);
        assert(comp_conversion_greater == 1);

        const int comp_pos_count_conversion_eq = first.compare(0, 20, convertible);
        assert(comp_pos_count_conversion_eq == 0);

        const int comp_pos_count_conversion_less = first.compare(5, 4, convertible);
        assert(comp_pos_count_conversion_less == -1);

        const int comp_pos_count_conversion_greater = second.compare(0, 2, convertible);
        assert(comp_pos_count_conversion_greater == 1);

        const int comp_pos_count_conversion_pos_eq = first.compare(3, 20, convertible, 3);
        assert(comp_pos_count_conversion_pos_eq == 0);

        const int comp_pos_count_conversion_pos_less = first.compare(0, 2, second, 2);
        assert(comp_pos_count_conversion_pos_less == -1);

        const int comp_pos_count_conversion_pos_greater = second.compare(0, 2, convertible, 6);
        assert(comp_pos_count_conversion_pos_greater == 1);

        const int comp_pos_count_conversion_pos_count_eq = first.compare(3, 5, convertible, 3, 5);
        assert(comp_pos_count_conversion_pos_count_eq == 0);

        const int comp_pos_count_conversion_pos_count_less = first.compare(0, 2, second, 2, 3);
        assert(comp_pos_count_conversion_pos_count_less == -1);

        const int comp_pos_count_conversion_pos_count_greater = second.compare(0, 2, convertible, 6, 4);
        assert(comp_pos_count_conversion_pos_count_greater == 1);
    }

    { // starts_with
        const str starts            = get_literal_input<CharType>();
        const str input_string_true = starts.substr(0, 5);
        assert(starts.starts_with(input_string_true));

        const str input_string_false = get_cat<CharType>();
        assert(!starts.starts_with(input_string_false));

        assert(starts.starts_with(CharType{'H'}));
        assert(!starts.starts_with(CharType{'h'}));

        assert(starts.starts_with(get_literal_input<CharType>()));
        assert(!input_string_false.starts_with(get_literal_input<CharType>()));
    }

    { // ends_with
        const str ends              = get_literal_input<CharType>();
        const str input_string_true = ends.substr(5);
        assert(ends.ends_with(input_string_true));

        const str input_string_false = get_cat<CharType>();
        assert(!ends.ends_with(input_string_false));

        assert(ends.ends_with(CharType{'s'}));
        assert(!ends.ends_with(CharType{'S'}));

        assert(ends.ends_with(get_literal_input<CharType>()));
        assert(!input_string_false.ends_with(get_literal_input<CharType>()));
    }

    { // replace
        const str input = get_dog<CharType>();

        str replaced_pos_count_str = get_literal_input<CharType>();
        replaced_pos_count_str.replace(13, 7, input);
        assert(equalRanges(replaced_pos_count_str, "Hello fluffy dog"sv));

        str replaced_pos_count_str_shift = get_literal_input<CharType>();
        replaced_pos_count_str_shift.replace(13, 2, input);
        assert(equalRanges(replaced_pos_count_str_shift, "Hello fluffy dogttens"sv));

        str replaced_iter_str = get_literal_input<CharType>();
        replaced_iter_str.replace(replaced_iter_str.cbegin() + 13, replaced_iter_str.cend(), input);
        assert(equalRanges(replaced_iter_str, "Hello fluffy dog"sv));

        str replaced_iter_str_shift = get_literal_input<CharType>();
        replaced_iter_str_shift.replace(
            replaced_iter_str_shift.cbegin() + 13, replaced_iter_str_shift.cbegin() + 15, input);
        assert(equalRanges(replaced_iter_str_shift, "Hello fluffy dogttens"sv));

        str replaced_pos_count_str_pos_count = get_literal_input<CharType>();
        replaced_pos_count_str_pos_count.replace(13, 7, input, 1);
        assert(equalRanges(replaced_pos_count_str_pos_count, "Hello fluffy og"sv));

        str replaced_pos_count_str_pos_count_less = get_literal_input<CharType>();
        replaced_pos_count_str_pos_count_less.replace(13, 2, input, 1, 2);
        assert(equalRanges(replaced_pos_count_str_pos_count_less, "Hello fluffy ogttens"sv));

        str replaced_iter_iter = get_literal_input<CharType>();
        replaced_iter_iter.replace(
            replaced_iter_iter.cbegin() + 13, replaced_iter_iter.cend(), input.begin(), input.end());
        assert(equalRanges(replaced_iter_iter, "Hello fluffy dog"sv));

        str replaced_iter_iter_less = get_literal_input<CharType>();
        replaced_iter_iter_less.replace(replaced_iter_iter_less.cbegin() + 13, replaced_iter_iter_less.cbegin() + 15,
            input.begin() + 1, input.end());
        assert(equalRanges(replaced_iter_iter_less, "Hello fluffy ogttens"sv));

        str replaced_pos_count_literal = get_literal_input<CharType>();
        replaced_pos_count_literal.replace(13, 2, get_dog<CharType>());
        assert(equalRanges(replaced_pos_count_literal, "Hello fluffy dogttens"sv));

        str replaced_pos_count_literal_count = get_literal_input<CharType>();
        replaced_pos_count_literal_count.replace(13, 2, get_dog<CharType>(), 2);
        assert(equalRanges(replaced_pos_count_literal_count, "Hello fluffy dottens"sv));

        str replaced_iter_literal = get_literal_input<CharType>();
        replaced_iter_literal.replace(
            replaced_iter_literal.cbegin() + 13, replaced_iter_literal.cbegin() + 15, get_dog<CharType>());
        assert(equalRanges(replaced_iter_literal, "Hello fluffy dogttens"sv));

        str replaced_iter_literal_count = get_literal_input<CharType>();
        replaced_iter_literal_count.replace(replaced_iter_literal_count.cbegin() + 13,
            replaced_iter_literal_count.cbegin() + 15, get_dog<CharType>(), 2);
        assert(equalRanges(replaced_iter_literal_count, "Hello fluffy dottens"sv));

        str replaced_pos_count_chars = get_literal_input<CharType>();
        replaced_pos_count_chars.replace(13, 2, 5, CharType{'a'});
        assert(equalRanges(replaced_pos_count_chars, "Hello fluffy aaaaattens"sv));

        str replaced_iter_chars = get_literal_input<CharType>();
        replaced_iter_chars.replace(
            replaced_iter_chars.cbegin() + 13, replaced_iter_chars.cbegin() + 15, 5, CharType{'a'});
        assert(equalRanges(replaced_iter_chars, "Hello fluffy aaaaattens"sv));

        str replaced_iter_init = get_literal_input<CharType>();
        replaced_iter_init.replace(replaced_iter_init.cbegin() + 13, replaced_iter_init.cbegin() + 15,
            {CharType{'c'}, CharType{'u'}, CharType{'t'}, CharType{'e'}, CharType{' '}});
        assert(equalRanges(replaced_iter_init, "Hello fluffy cute ttens"sv));

        const string_view_convertible<CharType> convertible;
        str replaced_pos_count_conversion = get_dog<CharType>();
        replaced_pos_count_conversion.replace(1, 5, convertible);
        assert(equalRanges(replaced_pos_count_conversion, "dHello fluffy kittens"sv));

        str replaced_iter_conversion = get_dog<CharType>();
        replaced_iter_conversion.replace(
            replaced_iter_conversion.cbegin() + 1, replaced_iter_conversion.cbegin() + 2, convertible);
        assert(equalRanges(replaced_iter_conversion, "dHello fluffy kittensg"sv));

        str replaced_pos_count_conversion_pos = get_dog<CharType>();
        replaced_pos_count_conversion_pos.replace(1, 5, convertible, 6);
        assert(equalRanges(replaced_pos_count_conversion_pos, "dfluffy kittens"sv));

        str replaced_pos_count_conversion_pos_count = get_dog<CharType>();
        replaced_pos_count_conversion_pos_count.replace(1, 5, convertible, 6, 6);
        assert(equalRanges(replaced_pos_count_conversion_pos_count, "dfluffy"sv));
    }

    { // substr
        const str input = get_literal_input<CharType>();

        const str substr_pos = input.substr(6);
        assert(equalRanges(substr_pos, "fluffy kittens"sv));

        const str substr_pos_count = input.substr(6, 6);
        assert(equalRanges(substr_pos_count, "fluffy"sv));
    }

    { // copy
        const str input = get_literal_input<CharType>();

        CharType copy_count[5];
        input.copy(copy_count, 5);
        assert(equalRanges(copy_count, "Hello"sv));

        CharType copy_count_pos[6];
        input.copy(copy_count_pos, 6, 6);
        assert(equalRanges(copy_count_pos, "fluffy"sv));
    }

    { // resize
        str resized = get_literal_input<CharType>();
        resized.resize(3);
        assert(equalRanges(resized, "Hel"sv));

        resized.resize(6, CharType{'a'});
        assert(equalRanges(resized, "Helaaa"sv));
    }

    { // swap
        constexpr basic_string_view<CharType> expected_first  = get_dog<CharType>();
        constexpr basic_string_view<CharType> expected_second = get_cat<CharType>();
        str first{get_cat<CharType>()};
        str second{get_dog<CharType>()};
        swap(first, second);

        assert(equalRanges(first, expected_first));
        assert(equalRanges(second, expected_second));

        first.swap(second);
        assert(equalRanges(second, expected_first));
        assert(equalRanges(first, expected_second));
    }

    { // find
        const str input     = get_literal_input<CharType>();
        const str needle    = get_cat<CharType>();
        const str no_needle = get_no_needle<CharType>();

        const auto find_str = input.find(needle);
        assert(find_str == 13u);

        const auto find_str_none = input.find(no_needle);
        assert(find_str_none == str::npos);

        const auto find_str_pos = input.find(needle, 6);
        assert(find_str_pos == 13u);

        const auto find_str_pos_none = input.find(needle, 14);
        assert(find_str_pos_none == str::npos);

        const auto find_str_overflow = input.find(needle, 50);
        assert(find_str_overflow == str::npos);

        const auto find_literal = input.find(get_cat<CharType>());
        assert(find_literal == 13u);

        const auto find_literal_none = input.find(get_dog<CharType>());
        assert(find_literal_none == str::npos);

        const auto find_literal_pos = input.find(get_cat<CharType>(), 6);
        assert(find_literal_pos == 13u);

        const auto find_literal_pos_none = input.find(get_cat<CharType>(), 14);
        assert(find_literal_pos_none == str::npos);

        const auto find_literal_overflow = input.find(get_cat<CharType>(), 50);
        assert(find_literal_overflow == str::npos);

        const auto find_literal_pos_count = input.find(get_cat<CharType>(), 6, 4);
        assert(find_literal_pos_count == 13u);

        const auto find_literal_pos_count_none = input.find(get_dog<CharType>(), 14, 4);
        assert(find_literal_pos_count_none == str::npos);

        const auto find_char = input.find(CharType{'e'});
        assert(find_char == 1u);

        const auto find_char_none = input.find(CharType{'x'});
        assert(find_char_none == str::npos);

        const auto find_char_pos = input.find(CharType{'e'}, 4);
        assert(find_char_pos == 17u);

        const string_view_convertible<CharType> convertible;
        const auto find_convertible = input.find(convertible);
        assert(find_convertible == 0);

        const auto find_convertible_pos = input.find(convertible, 2);
        assert(find_convertible_pos == str::npos);
    }

    { // rfind
        const str input     = get_literal_input<CharType>();
        const str needle    = get_cat<CharType>();
        const str no_needle = get_no_needle<CharType>();

        const auto rfind_str = input.rfind(needle);
        assert(rfind_str == 13u);

        const auto rfind_str_none = input.rfind(no_needle);
        assert(rfind_str_none == str::npos);

        const auto rfind_str_pos = input.rfind(needle, 15);
        assert(rfind_str_pos == 13u);

        const auto rfind_str_pos_none = input.rfind(needle, 6);
        assert(rfind_str_pos_none == str::npos);

        const auto rfind_str_overflow = input.rfind(needle, 50);
        assert(rfind_str_overflow == 13u);

        const auto rfind_literal = input.rfind(get_cat<CharType>());
        assert(rfind_literal == 13u);

        const auto rfind_literal_none = input.rfind(get_dog<CharType>());
        assert(rfind_literal_none == str::npos);

        const auto rfind_literal_pos = input.rfind(get_cat<CharType>(), 15);
        assert(rfind_literal_pos == 13u);

        const auto rfind_literal_pos_none = input.rfind(get_cat<CharType>(), 6);
        assert(rfind_literal_pos_none == str::npos);

        const auto rfind_literal_overflow = input.rfind(get_cat<CharType>(), 50);
        assert(rfind_literal_overflow == 13u);

        const auto rfind_literal_pos_count = input.rfind(get_cat<CharType>(), 15, 4);
        assert(rfind_literal_pos_count == 13u);

        const auto rfind_literal_pos_count_none = input.rfind(get_dog<CharType>(), 6, 4);
        assert(rfind_literal_pos_count_none == str::npos);

        const auto rfind_char = input.rfind(CharType{'e'});
        assert(rfind_char == 17u);

        const auto rfind_char_none = input.rfind(CharType{'x'});
        assert(rfind_char_none == str::npos);

        const auto rfind_char_pos = input.rfind(CharType{'e'}, 4);
        assert(rfind_char_pos == 1u);

        const string_view_convertible<CharType> convertible;
        const auto rfind_convertible = input.rfind(convertible);
        assert(rfind_convertible == 0);

        const auto rfind_convertible_pos = input.rfind(convertible, 5);
        assert(rfind_convertible_pos == 0);
    }

    { // find_first_of
        const str input     = get_literal_input<CharType>();
        const str needle    = get_cat<CharType>();
        const str no_needle = get_no_needle<CharType>();

        const auto find_first_of_str = input.find_first_of(needle);
        assert(find_first_of_str == 1u);

        const auto find_first_of_str_none = input.find_first_of(no_needle);
        assert(find_first_of_str_none == str::npos);

        const auto find_first_of_str_pos = input.find_first_of(needle, 6);
        assert(find_first_of_str_pos == 13u);

        const auto find_first_of_str_pos_none = input.find_first_of(no_needle, 14);
        assert(find_first_of_str_pos_none == str::npos);

        const auto find_first_of_str_overflow = input.find_first_of(needle, 50);
        assert(find_first_of_str_overflow == str::npos);

        const auto find_first_of_literal = input.find_first_of(get_cat<CharType>());
        assert(find_first_of_literal == 1u);

        const auto find_first_of_literal_none = input.find_first_of(get_no_needle<CharType>());
        assert(find_first_of_literal_none == str::npos);

        const auto find_first_of_literal_pos = input.find_first_of(get_cat<CharType>(), 6);
        assert(find_first_of_literal_pos == 13u);

        const auto find_first_of_literal_pos_none = input.find_first_of(get_no_needle<CharType>(), 14);
        assert(find_first_of_literal_pos_none == str::npos);

        const auto find_first_of_literal_overflow = input.find_first_of(get_cat<CharType>(), 50);
        assert(find_first_of_literal_overflow == str::npos);

        const auto find_first_of_literal_pos_count = input.find_first_of(get_cat<CharType>(), 6, 4);
        assert(find_first_of_literal_pos_count == 13u);

        const auto find_first_of_literal_pos_count_none = input.find_first_of(get_no_needle<CharType>(), 14, 4);
        assert(find_first_of_literal_pos_count_none == str::npos);

        const auto find_first_of_char = input.find_first_of(CharType{'e'});
        assert(find_first_of_char == 1u);

        const auto find_first_of_char_none = input.find_first_of(CharType{'x'});
        assert(find_first_of_char_none == str::npos);

        const auto find_first_of_char_pos = input.find_first_of(CharType{'e'}, 4);
        assert(find_first_of_char_pos == 17u);

        const string_view_convertible<CharType> convertible;
        const auto find_first_of_convertible = input.find_first_of(convertible);
        assert(find_first_of_convertible == 0);

        const auto find_first_of_convertible_pos = input.find_first_of(convertible, 2);
        assert(find_first_of_convertible_pos == 2u);
    }

    { // find_first_not_of
        const str input     = get_literal_input<CharType>();
        const str needle    = get_cat<CharType>();
        const str no_needle = get_no_needle<CharType>();

        const auto find_first_not_of_str = input.find_first_not_of(needle);
        assert(find_first_not_of_str == 0u);

        const auto find_first_not_of_str_none = input.find_first_not_of(input);
        assert(find_first_not_of_str_none == str::npos);

        const auto find_first_not_of_str_pos = input.find_first_not_of(needle, 6);
        assert(find_first_not_of_str_pos == 6u);

        const auto find_first_not_of_str_pos_none = input.find_first_not_of(input, 14);
        assert(find_first_not_of_str_pos_none == str::npos);

        const auto find_first_not_of_str_overflow = input.find_first_not_of(needle, 50);
        assert(find_first_not_of_str_overflow == str::npos);

        const auto find_first_not_of_literal = input.find_first_not_of(get_cat<CharType>());
        assert(find_first_not_of_literal == 0u);

        const auto find_first_not_of_literal_none = input.find_first_not_of(get_literal_input<CharType>());
        assert(find_first_not_of_literal_none == str::npos);

        const auto find_first_not_of_literal_pos = input.find_first_not_of(get_cat<CharType>(), 6);
        assert(find_first_not_of_literal_pos == 6u);

        const auto find_first_not_of_literal_pos_none = input.find_first_not_of(get_literal_input<CharType>(), 2);
        assert(find_first_not_of_literal_pos_none == str::npos);

        const auto find_first_not_of_literal_overflow = input.find_first_not_of(get_cat<CharType>(), 50);
        assert(find_first_not_of_literal_overflow == str::npos);

        const auto find_first_not_of_literal_pos_count = input.find_first_not_of(get_cat<CharType>(), 6, 4);
        assert(find_first_not_of_literal_pos_count == 6u);

        const auto find_first_not_of_literal_pos_count_none =
            input.find_first_not_of(get_literal_input<CharType>(), 14, 20);
        assert(find_first_not_of_literal_pos_count_none == str::npos);

        const auto find_first_not_of_char = input.find_first_not_of(CharType{'H'});
        assert(find_first_not_of_char == 1u);

        const auto find_first_not_of_char_pos = input.find_first_not_of(CharType{'e'}, 1);
        assert(find_first_not_of_char_pos == 2u);

        const string_view_convertible<CharType> convertible;
        const auto find_first_not_of_convertible = input.find_first_not_of(convertible);
        assert(find_first_not_of_convertible == str::npos);

        const auto find_first_not_of_convertible_pos = input.find_first_not_of(convertible, 2);
        assert(find_first_not_of_convertible_pos == str::npos);
    }

    { // find_last_of
        const str input     = get_literal_input<CharType>();
        const str needle    = get_cat<CharType>();
        const str no_needle = get_no_needle<CharType>();

        const auto find_last_of_str = input.find_last_of(needle);
        assert(find_last_of_str == 18u);

        const auto find_last_of_str_none = input.find_last_of(no_needle);
        assert(find_last_of_str_none == str::npos);

        const auto find_last_of_str_pos = input.find_last_of(needle, 6);
        assert(find_last_of_str_pos == 1u);

        const auto find_last_of_str_pos_none = input.find_last_of(no_needle, 14);
        assert(find_last_of_str_pos_none == str::npos);

        const auto find_last_of_str_overflow = input.find_last_of(needle, 50);
        assert(find_last_of_str_overflow == 18u);

        const auto find_last_of_literal = input.find_last_of(get_cat<CharType>());
        assert(find_last_of_literal == 18u);

        const auto find_last_of_literal_none = input.find_last_of(get_no_needle<CharType>());
        assert(find_last_of_literal_none == str::npos);

        const auto find_last_of_literal_pos = input.find_last_of(get_cat<CharType>(), 6);
        assert(find_last_of_literal_pos == 1u);

        const auto find_last_of_literal_pos_none = input.find_last_of(get_no_needle<CharType>(), 14);
        assert(find_last_of_literal_pos_none == str::npos);

        const auto find_last_of_literal_overflow = input.find_last_of(get_cat<CharType>(), 50);
        assert(find_last_of_literal_overflow == 18u);

        const auto find_last_of_literal_pos_count = input.find_last_of(get_cat<CharType>(), 6, 7);
        assert(find_last_of_literal_pos_count == 1u);

        const auto find_last_of_literal_pos_count_none = input.find_last_of(get_no_needle<CharType>(), 14, 4);
        assert(find_last_of_literal_pos_count_none == str::npos);

        const auto find_last_of_char = input.find_last_of(CharType{'e'});
        assert(find_last_of_char == 17u);

        const auto find_last_of_char_none = input.find_last_of(CharType{'x'});
        assert(find_last_of_char_none == str::npos);

        const auto find_last_of_char_pos = input.find_last_of(CharType{'e'}, 4);
        assert(find_last_of_char_pos == 1u);

        const string_view_convertible<CharType> convertible;
        const auto find_last_of_convertible = input.find_last_of(convertible);
        assert(find_last_of_convertible == 19u);

        const auto find_last_of_convertible_pos = input.find_last_of(convertible, 4);
        assert(find_last_of_convertible_pos == 4u);
    }

    { // find_last_not_of
        const str input     = get_literal_input<CharType>();
        const str needle    = get_cat<CharType>();
        const str no_needle = get_no_needle<CharType>();

        const auto find_last_not_of_str = input.find_last_not_of(needle);
        assert(find_last_not_of_str == 19u);

        const auto find_last_not_of_str_none = input.find_last_not_of(input);
        assert(find_last_not_of_str_none == str::npos);

        const auto find_last_not_of_str_pos = input.find_last_not_of(needle, 6);
        assert(find_last_not_of_str_pos == 6u);

        const auto find_last_not_of_str_pos_none = input.find_last_not_of(input, 14);
        assert(find_last_not_of_str_pos_none == str::npos);

        const auto find_last_not_of_str_overflow = input.find_last_not_of(needle, 50);
        assert(find_last_not_of_str_overflow == 19u);

        const auto find_last_not_of_literal = input.find_last_not_of(get_cat<CharType>());
        assert(find_last_not_of_literal == 19u);

        const auto find_last_not_of_literal_none = input.find_last_not_of(get_literal_input<CharType>());
        assert(find_last_not_of_literal_none == str::npos);

        const auto find_last_not_of_literal_pos = input.find_last_not_of(get_cat<CharType>(), 6);
        assert(find_last_not_of_literal_pos == 6u);

        const auto find_last_not_of_literal_pos_none = input.find_last_not_of(get_literal_input<CharType>(), 2);
        assert(find_last_not_of_literal_pos_none == str::npos);

        const auto find_last_not_of_literal_overflow = input.find_last_not_of(get_cat<CharType>(), 50);
        assert(find_last_not_of_literal_overflow == 19u);

        const auto find_last_not_of_literal_pos_count = input.find_last_not_of(get_cat<CharType>(), 6, 4);
        assert(find_last_not_of_literal_pos_count == 6u);

        const auto find_last_not_of_literal_pos_count_none =
            input.find_last_not_of(get_literal_input<CharType>(), 14, 20);
        assert(find_last_not_of_literal_pos_count_none == str::npos);

        const auto find_last_not_of_char = input.find_last_not_of(CharType{'H'});
        assert(find_last_not_of_char == 19u);

        const auto find_last_not_of_char_pos = input.find_last_not_of(CharType{'e'}, 2);
        assert(find_last_not_of_char_pos == 2u);

        const string_view_convertible<CharType> convertible;
        const auto find_last_not_of_convertible = input.find_last_not_of(convertible);
        assert(find_last_not_of_convertible == str::npos);

        const auto find_last_not_of_convertible_pos = input.find_last_not_of(convertible, 2);
        assert(find_last_not_of_convertible_pos == str::npos);
    }

    { // operator+
        const str first  = get_cat<CharType>();
        const str second = get_dog<CharType>();

        const str op_str_str = first + second;
        assert(equalRanges(op_str_str, "kittendog"sv));

        const str op_str_literal = first + get_dog<CharType>();
        assert(equalRanges(op_str_literal, "kittendog"sv));

        const str op_str_char = first + CharType{'!'};
        assert(equalRanges(op_str_char, "kitten!"sv));

        const str op_literal_str = get_cat<CharType>() + second;
        assert(equalRanges(op_literal_str, "kittendog"sv));

        const str op_char_str = CharType{'!'} + second;
        assert(equalRanges(op_char_str, "!dog"sv));

        const str op_rstr_rstr = str{get_cat<CharType>()} + str{get_dog<CharType>()};
        assert(equalRanges(op_rstr_rstr, "kittendog"sv));

        const str op_rstr_str = str{get_cat<CharType>()} + second;
        assert(equalRanges(op_rstr_str, "kittendog"sv));

        const str op_rstr_literal = str{get_cat<CharType>()} + get_dog<CharType>();
        assert(equalRanges(op_rstr_literal, "kittendog"sv));

        const str op_rstr_char = str{get_cat<CharType>()} + CharType{'!'};
        assert(equalRanges(op_rstr_char, "kitten!"sv));

        const str op_str_rstr = first + str{get_dog<CharType>()};
        assert(equalRanges(op_str_rstr, "kittendog"sv));

        const str op_literal_rstr = get_cat<CharType>() + str{get_dog<CharType>()};
        assert(equalRanges(op_literal_rstr, "kittendog"sv));

        const str op_char_rstr = CharType{'!'} + str{get_dog<CharType>()};
        assert(equalRanges(op_char_rstr, "!dog"sv));
    }

    { // comparison
        str first(get_view_input<CharType>());
        str second(get_view_input<CharType>());
        str third{get_cat<CharType>()};

        const bool eq_str_str = first == second;
        assert(eq_str_str);

        const bool ne_str_str = first != third;
        assert(ne_str_str);

        const bool less_str_str = first < third;
        assert(less_str_str);

        const bool less_eq_str_str = first <= third;
        assert(less_eq_str_str);

        const bool greater_str_str = first > third;
        assert(!greater_str_str);

        const bool greater_eq_str_str = first >= third;
        assert(!greater_eq_str_str);

        const bool eq_str_literal = first == get_view_input<CharType>();
        assert(eq_str_literal);

        const bool ne_str_literal = first != get_cat<CharType>();
        assert(ne_str_literal);

        const bool less_str_literal = first < get_cat<CharType>();
        assert(less_str_literal);

        const bool less_eq_str_literal = first <= get_cat<CharType>();
        assert(less_eq_str_literal);

        const bool greater_str_literal = first > get_cat<CharType>();
        assert(!greater_str_literal);

        const bool greater_eq_str_literal = first >= get_cat<CharType>();
        assert(!greater_eq_str_literal);

        const bool eq_literal_str = get_view_input<CharType>() == second;
        assert(eq_literal_str);

        const bool ne_literal_str = get_view_input<CharType>() != third;
        assert(ne_literal_str);

        const bool less_literal_str = get_view_input<CharType>() < third;
        assert(less_literal_str);

        const bool less_eq_literal_str = get_view_input<CharType>() <= third;
        assert(less_eq_literal_str);

        const bool greater_literal_str = get_view_input<CharType>() > third;
        assert(!greater_literal_str);

        const bool greater_eq_literal_str = get_view_input<CharType>() >= third;
        assert(!greater_eq_literal_str);

        const strong_ordering spaceship_str_str_eq = first <=> second;
        assert(spaceship_str_str_eq == strong_ordering::equal);

        const strong_ordering spaceship_str_str_less = first <=> third;
        assert(spaceship_str_str_less == strong_ordering::less);

        const strong_ordering spaceship_str_str_greater = third <=> first;
        assert(spaceship_str_str_greater == strong_ordering::greater);

        const strong_ordering spaceship_str_literal_eq = first <=> get_view_input<CharType>();
        assert(spaceship_str_literal_eq == strong_ordering::equal);

        const strong_ordering spaceship_str_literal_less = first <=> get_cat<CharType>();
        assert(spaceship_str_literal_less == strong_ordering::less);

        const strong_ordering spaceship_str_literal_greater = third <=> get_dog<CharType>();
        assert(spaceship_str_literal_greater == strong_ordering::greater);
    }

    { // basic_string_view conversion
        str s                          = get_literal_input<CharType>();
        basic_string_view<CharType> sv = s;
        assert(equalRanges(sv, "Hello fluffy kittens"sv));
    }
#endif // __EDG__
    return true;
}

_CONSTEXPR20_CONTAINER bool test_udls() {
#ifndef __EDG__ // TRANSITION, VSO-1273296
    assert(equalRanges("purr purr"s, "purr purr"sv));
#ifdef __cpp_char8_t
    assert(equalRanges(u8"purr purr"s, "purr purr"sv));
#endif // __cpp_char8_t
    assert(equalRanges(u"purr purr"s, "purr purr"sv));
    assert(equalRanges(U"purr purr"s, "purr purr"sv));
    assert(equalRanges(L"purr purr"s, "purr purr"sv));
#endif // __EDG__
    return true;
}

template <class CharType>
struct CharLikeType {
    constexpr CharLikeType() = default;
    constexpr CharLikeType(CharType cc) : c(cc) {}
    CharType c;
};

template <class CharType>
_CONSTEXPR20_CONTAINER bool test_iterators() {
#ifndef __EDG__ // TRANSITION, VSO-1273296
    using str               = basic_string<CharType>;
    str literal_constructed = get_literal_input<CharType>();

    { // assignment
        auto it   = literal_constructed.begin();
        auto it2  = literal_constructed.end();
        auto cit  = literal_constructed.cbegin();
        auto cit2 = literal_constructed.cend();

        it  = it2;
        cit = cit2;
    }

    { // op->
        basic_string<CharLikeType<CharType>> bs{CharType{'x'}};
        auto it = bs.begin();
        auto c  = it->c;
        assert(c == CharType{'x'});

        auto cit = bs.cbegin();
        auto cc  = cit->c;
        assert(cc == CharType{'x'});
    }

    { // increment
        auto it = literal_constructed.begin();
        assert(*++it == CharType{'e'});
        assert(*it++ == CharType{'e'});
        assert(*it == CharType{'l'});

        auto cit = literal_constructed.cbegin();
        assert(*++cit == CharType{'e'});
        assert(*cit++ == CharType{'e'});
        assert(*cit == CharType{'l'});
    }

    { // advance
        auto it = literal_constructed.begin() + 2;
        assert(*it == CharType{'l'});
        it += 2;
        assert(*it == CharType{'o'});
        it = 2 + it;
        assert(*it == CharType{'f'});

        auto cit = literal_constructed.cbegin() + 2;
        assert(*cit == CharType{'l'});
        cit += 2;
        assert(*cit == CharType{'o'});
        cit = 2 + cit;
        assert(*cit == CharType{'f'});
    }

    { // decrement
        auto it = literal_constructed.end();
        assert(*--it == CharType{'s'});
        assert(*it-- == CharType{'s'});
        assert(*it == CharType{'n'});

        auto cit = literal_constructed.cend();
        assert(*--cit == CharType{'s'});
        assert(*cit-- == CharType{'s'});
        assert(*cit == CharType{'n'});
    }

    { // advance back
        auto it = literal_constructed.end() - 2;
        assert(*it == CharType{'n'});
        it -= 2;
        assert(*it == CharType{'t'});

        auto cit = literal_constructed.cend() - 2;
        assert(*cit == CharType{'n'});
        cit -= 2;
        assert(*cit == CharType{'t'});
    }

    { // difference
        const auto it1 = literal_constructed.begin();
        const auto it2 = literal_constructed.end();
        assert(it2 - it1 == ssize(get_view_input<CharType>()));

        const auto cit1 = literal_constructed.cbegin();
        const auto cit2 = literal_constructed.cend();
        assert(cit2 - cit1 == ssize(get_view_input<CharType>()));

        assert(it2 - cit1 == ssize(get_view_input<CharType>()));
        assert(cit2 - it1 == ssize(get_view_input<CharType>()));
    }

    { // comparison
        const auto it1 = literal_constructed.begin();
        const auto it2 = literal_constructed.begin();
        const auto it3 = literal_constructed.end();

        assert(it1 == it2);
        assert(it1 != it3);
        assert(it1 < it3);
        assert(it1 <= it3);
        assert(it3 > it1);
        assert(it3 >= it1);

        assert((it1 <=> it2) == strong_ordering::equal);
        assert((it1 <=> it3) == strong_ordering::less);
        assert((it3 <=> it1) == strong_ordering::greater);
    }

    { // access
        const auto it = literal_constructed.begin() + 2;
        it[2]         = CharType{'l'};
        assert(literal_constructed[4] == CharType{'l'});

        const auto cit = literal_constructed.cbegin() + 2;
        assert(cit[2] == CharType{'l'});
    }
#endif // __EDG__
    return true;
}

template <class CharType>
_CONSTEXPR20_CONTAINER bool test_growth() {
    using str = basic_string<CharType>;
#ifndef __EDG__ // TRANSITION, VSO-1273296
    {
        str v(1007, CharType{'a'});

        assert(v.size() == 1007);
        assert(v.capacity() == 1007);

        v.resize(1008);

        assert(v.size() == 1008);
        assert(v.capacity() == 1510);
    }

    {
        str v(1007, CharType{'a'});

        assert(v.size() == 1007);
        assert(v.capacity() == 1007);

        v.resize(8007);

        assert(v.size() == 8007);
        if constexpr (is_same_v<CharType, char16_t> || is_same_v<CharType, char32_t> || is_same_v<CharType, wchar_t>) {
            assert(v.capacity() == 8007);
        } else {
            assert(v.capacity() == 8015);
        }
    }

    {
        str v(1007, CharType{'a'});

        assert(v.size() == 1007);
        assert(v.capacity() == 1007);

        v.push_back(CharType{'b'});

        assert(v.size() == 1008);
        assert(v.capacity() == 1510);
    }

    {
        str v(1007, CharType{'a'});

        assert(v.size() == 1007);
        assert(v.capacity() == 1007);

        str l(3, CharType{'b'});

        v.insert(v.end(), l.begin(), l.end());

        assert(v.size() == 1010);
        assert(v.capacity() == 1510);
    }

    {
        str v(1007, CharType{'a'});

        assert(v.size() == 1007);
        assert(v.capacity() == 1007);

        str l(7000, CharType{'b'});

        v.insert(v.end(), l.begin(), l.end());

        assert(v.size() == 8007);
        if constexpr (is_same_v<CharType, char16_t> || is_same_v<CharType, char32_t> || is_same_v<CharType, wchar_t>) {
            assert(v.capacity() == 8007);
        } else {
            assert(v.capacity() == 8015);
        }
    }

    {
        str v(1007, CharType{'a'});

        assert(v.size() == 1007);
        assert(v.capacity() == 1007);

        v.insert(v.end(), 3, CharType{'b'});

        assert(v.size() == 1010);
        assert(v.capacity() == 1510);
    }

    {
        str v(1007, CharType{'a'});

        assert(v.size() == 1007);
        assert(v.capacity() == 1007);

        v.insert(v.end(), 7000, CharType{'b'});

        assert(v.size() == 8007);
        if constexpr (is_same_v<CharType, char16_t> || is_same_v<CharType, char32_t> || is_same_v<CharType, wchar_t>) {
            assert(v.capacity() == 8007);
        } else {
            assert(v.capacity() == 8015);
        }
    }
#endif // __EDG__
    return true;
}

int main() {
    test_interface<char>();
#ifdef __cpp_char8_t
    test_interface<char8_t>();
#endif // __cpp_char8_t
    test_interface<char16_t>();
    test_interface<char32_t>();
    test_interface<wchar_t>();

    test_udls();

    test_iterators<char>();
#ifdef __cpp_char8_t
    test_iterators<char8_t>();
#endif // __cpp_char8_t
    test_iterators<char16_t>();
    test_iterators<char32_t>();
    test_iterators<wchar_t>();

    test_growth<char>();
#ifdef __cpp_char8_t
    test_growth<char8_t>();
#endif // __cpp_char8_t
    test_growth<char16_t>();
    test_growth<char32_t>();
    test_growth<wchar_t>();

#ifdef __cpp_lib_constexpr_string
    static_assert(test_interface<char>());
#ifdef __cpp_char8_t
    static_assert(test_interface<char8_t>());
#endif // __cpp_char8_t
    static_assert(test_interface<char16_t>());
    static_assert(test_interface<char32_t>());
    static_assert(test_interface<wchar_t>());

    static_assert(test_udls());

    static_assert(test_iterators<char>());
#ifdef __cpp_char8_t
    static_assert(test_iterators<char8_t>());
#endif // __cpp_char8_t
    static_assert(test_iterators<char16_t>());
    static_assert(test_iterators<char32_t>());
    static_assert(test_iterators<wchar_t>());

    static_assert(test_growth<char>());
#ifdef __cpp_char8_t
    static_assert(test_growth<char8_t>());
#endif // __cpp_char8_t
    static_assert(test_growth<char16_t>());
    static_assert(test_growth<char32_t>());
    static_assert(test_growth<wchar_t>());
#endif // __cpp_lib_constexpr_string
}
