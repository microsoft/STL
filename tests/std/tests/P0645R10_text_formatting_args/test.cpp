// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

using namespace std;

template <class CharType>
constexpr auto get_input_literal() {
    if constexpr (same_as<CharType, char>) {
        return "meow";
    } else {
        return L"meow";
    }
}

template <class CharType>
constexpr auto get_input_sv() {
    if constexpr (same_as<CharType, char>) {
        return "meow"sv;
    } else {
        return L"meow"sv;
    }
}

enum class Arg_type : uint8_t {
    none,
    int_type,
    unsigned_type,
    long_long_type,
    unsigned_long_long_type,
    bool_type,
    char_type,
    float_type,
    double_type,
    long_double_type,
    pointer_type,
    string_literal_type,
    string_type,
    handle_type,
};

template <class Context>
constexpr auto visitor = [](auto&& arg) {
    using T         = decay_t<decltype(arg)>;
    using char_type = typename Context::char_type;
    if constexpr (is_same_v<T, monostate>) {
        return Arg_type::none;
    } else if constexpr (is_same_v<T, int>) {
        return Arg_type::int_type;
    } else if constexpr (is_same_v<T, unsigned int>) {
        return Arg_type::unsigned_type;
    } else if constexpr (is_same_v<T, long long>) {
        return Arg_type::long_long_type;
    } else if constexpr (is_same_v<T, unsigned long long>) {
        return Arg_type::unsigned_long_long_type;
    } else if constexpr (is_same_v<T, char_type>) {
        return Arg_type::char_type;
    } else if constexpr (is_same_v<T, float>) {
        return Arg_type::float_type;
    } else if constexpr (is_same_v<T, double>) {
        return Arg_type::double_type;
    } else if constexpr (is_same_v<T, long double>) {
        return Arg_type::long_double_type;
    } else if constexpr (is_same_v<T, const void*>) {
        return Arg_type::pointer_type;
    } else if constexpr (is_same_v<T, const char_type*>) {
        return Arg_type::string_literal_type;
    } else if constexpr (is_same_v<T, basic_string_view<char_type>>) {
        return Arg_type::string_type;
    } else {
        return Arg_type::handle_type;
    }
};

template <class Context>
void test_basic_format_arg() {
    using char_type = typename Context::char_type;

    { // construction
        basic_format_arg<Context> default_constructed;
        assert(!default_constructed);

        // test internal _Make_from mechanism

        constexpr auto as_lvalue = []<class T>(T&& t) noexcept -> T& { return static_cast<T&>(t); };

        auto from_int = basic_format_arg<Context>::_Make_from(as_lvalue(5));
        assert(from_int);

        auto from_unsigned = basic_format_arg<Context>::_Make_from(as_lvalue(5u));
        assert(from_unsigned);

        auto from_long_long = basic_format_arg<Context>::_Make_from(as_lvalue(5ll));
        assert(from_long_long);

        auto from_unsigned_long_long = basic_format_arg<Context>::_Make_from(as_lvalue(5ull));
        assert(from_unsigned_long_long);

        auto from_float = basic_format_arg<Context>::_Make_from(as_lvalue(5.0f));
        assert(from_float);

        auto from_double = basic_format_arg<Context>::_Make_from(as_lvalue(5.0));
        assert(from_double);

        auto from_long_double = basic_format_arg<Context>::_Make_from(as_lvalue(5.0L));
        assert(from_long_double);

        auto from_nullptr = basic_format_arg<Context>::_Make_from(as_lvalue(nullptr));
        assert(from_nullptr);

        auto from_pointer = basic_format_arg<Context>::_Make_from(as_lvalue(static_cast<const void*>(nullptr)));
        assert(from_pointer);

        auto from_literal = basic_format_arg<Context>::_Make_from(as_lvalue(get_input_literal<char_type>()));
        assert(from_literal);

        auto from_string_view = basic_format_arg<Context>::_Make_from(as_lvalue(get_input_sv<char_type>()));
        assert(from_string_view);

        // the exposition-only constructor of basic_format_arg shouldn't be accessible
        static_assert(!is_constructible_v<basic_format_arg<Context>, int>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, unsigned int>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, long long>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, unsigned long long>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, float>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, double>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, long double>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, const char_type*>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, basic_string_view<char_type>>);
        static_assert(!is_constructible_v<basic_format_arg<Context>, const void*>);
    }
}
template <class Context>
void test_empty_format_arg() {
    const auto store = make_format_args<Context>();
    const basic_format_args<Context> args{store};
    const same_as<basic_format_arg<Context>> auto first_arg = args.get(0);
    assert(!first_arg);
}

template <class Context, class Type, Arg_type Result>
void test_single_format_arg(Type value) {
    const auto store = make_format_args<Context>(value);
    const basic_format_args<Context> args{store};
    const same_as<basic_format_arg<Context>> auto first_arg = args.get(0);
    assert(first_arg);
    assert(visit_format_arg(visitor<Context>, first_arg) == Result);
    const same_as<basic_format_arg<Context>> auto other_arg = args.get(1);
    assert(!other_arg);
}

template <class Context>
void test_format_arg_store() {
    using char_type = typename Context::char_type;

    test_empty_format_arg<Context>();

    test_single_format_arg<Context, char, Arg_type::char_type>(42);
    if constexpr (is_same_v<char_type, wchar_t>) {
        test_single_format_arg<Context, wchar_t, Arg_type::char_type>(42);
    }
    test_single_format_arg<Context, unsigned char, Arg_type::unsigned_type>(42);
    test_single_format_arg<Context, signed char, Arg_type::int_type>(42);
    test_single_format_arg<Context, int, Arg_type::int_type>(42);
    test_single_format_arg<Context, long, Arg_type::int_type>(42);
    test_single_format_arg<Context, int8_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int_fast8_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int_least8_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int16_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int_fast16_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int_least16_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int32_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int_fast32_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int_least32_t, Arg_type::int_type>(42);
    test_single_format_arg<Context, int64_t, Arg_type::long_long_type>(42);
    test_single_format_arg<Context, int_fast64_t, Arg_type::long_long_type>(42);
    test_single_format_arg<Context, int_least64_t, Arg_type::long_long_type>(42);
    if constexpr (sizeof(int) == sizeof(ptrdiff_t)) {
        test_single_format_arg<Context, ptrdiff_t, Arg_type::int_type>(42);
    } else {
        test_single_format_arg<Context, ptrdiff_t, Arg_type::long_long_type>(42);
    }

    test_single_format_arg<Context, unsigned int, Arg_type::unsigned_type>(42);
    test_single_format_arg<Context, unsigned long, Arg_type::unsigned_type>(42);
    test_single_format_arg<Context, uint8_t, Arg_type::unsigned_type>(42);
    test_single_format_arg<Context, uint_fast8_t, Arg_type::unsigned_type>(42);
    test_single_format_arg<Context, uint_least8_t, Arg_type::unsigned_type>(42);
    if constexpr (is_same_v<char_type, uint16_t>) {
        test_single_format_arg<Context, uint16_t, Arg_type::char_type>(42);
    } else {
        test_single_format_arg<Context, uint16_t, Arg_type::unsigned_type>(42);
    }
    test_single_format_arg<Context, uint_fast16_t, Arg_type::unsigned_type>(42);
    if constexpr (is_same_v<char_type, uint_least16_t>) {
        test_single_format_arg<Context, uint_least16_t, Arg_type::char_type>(42);
    } else {
        test_single_format_arg<Context, uint_least16_t, Arg_type::unsigned_type>(42);
    }
    test_single_format_arg<Context, uint32_t, Arg_type::unsigned_type>(42);
    test_single_format_arg<Context, uint_fast32_t, Arg_type::unsigned_type>(42);
    test_single_format_arg<Context, uint_least32_t, Arg_type::unsigned_type>(42);
    test_single_format_arg<Context, uint64_t, Arg_type::unsigned_long_long_type>(42);
    test_single_format_arg<Context, uint_fast64_t, Arg_type::unsigned_long_long_type>(42);
    test_single_format_arg<Context, uint_least64_t, Arg_type::unsigned_long_long_type>(42);
    if constexpr (sizeof(unsigned int) == sizeof(size_t)) {
        test_single_format_arg<Context, size_t, Arg_type::unsigned_type>(42);
    } else {
        test_single_format_arg<Context, size_t, Arg_type::unsigned_long_long_type>(42);
    }

    test_single_format_arg<Context, float, Arg_type::float_type>(42.f);
    test_single_format_arg<Context, double, Arg_type::double_type>(42.);
    test_single_format_arg<Context, long double, Arg_type::long_double_type>(42.);

    test_single_format_arg<Context, const void*, Arg_type::pointer_type>(nullptr);

    test_single_format_arg<Context, const char_type*, Arg_type::string_literal_type>(get_input_literal<char_type>());
    test_single_format_arg<Context, basic_string_view<char_type>, Arg_type::string_type>(get_input_sv<char_type>());
}

static_assert(sizeof(_Format_arg_index) == sizeof(size_t));
static_assert(is_same_v<_Format_arg_traits<format_context>::_Storage_type<void*>, const void*>);

static_assert(is_same_v<_Format_arg_traits<format_context>::_Storage_type<string>, string_view>);
static_assert(is_same_v<_Format_arg_traits<format_context>::_Storage_type<const string>, string_view>);
static_assert(is_same_v<_Format_arg_traits<format_context>::_Storage_type<char*>, const char*>);
static_assert(is_same_v<_Format_arg_traits<format_context>::_Storage_type<const char*>, const char*>);

// we rely on the _Storage_type<long> to be int
static_assert(is_same_v<_Format_arg_traits<format_context>::_Storage_type<long>, int>);
static_assert(is_same_v<_Format_arg_traits<format_context>::_Storage_type<unsigned long>, unsigned int>);

template <class Context>
void test_visit_monostate() {
    assert(visit_format_arg(visitor<Context>, basic_format_arg<Context>()) == Arg_type::none);
}

template <class Context>
void test_lwg3810() {
    [[maybe_unused]] auto args_store = make_format_args<Context>(1, 2, 3);
    static_assert(same_as<decltype(basic_format_args{args_store}), basic_format_args<Context>>);
}

struct lvalue_only_visitor {
    template <class T>
    void operator()(T&&) const = delete;
    template <class T>
    void operator()(T&) const noexcept {}
};

template <class Context>
void test_lvalue_only_visitation() {
    visit_format_arg(lvalue_only_visitor{}, basic_format_arg<Context>{});
}

int main() {
    test_basic_format_arg<format_context>();
    test_basic_format_arg<wformat_context>();
    test_format_arg_store<format_context>();
    test_format_arg_store<wformat_context>();
    test_visit_monostate<format_context>();
    test_visit_monostate<wformat_context>();

    test_lwg3810<format_context>();
    test_lwg3810<wformat_context>();

    test_lvalue_only_visitation<format_context>();
    test_lvalue_only_visitation<wformat_context>();
}
