// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: asan, x64 || x86

#if 0 // TRANSITION, VSO-1586016: String annotations disabled temporarily.
#pragma warning(disable : 4389) // signed/unsigned mismatch in arithmetic
#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension
#pragma warning(disable : 6326) // Potential comparison of a constant with another constant.

#ifdef __clang__
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#endif // __clang__

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <new>
#include <sstream>
#include <string>
#if _HAS_CXX17
#include <string_view>
#endif // _HAS_CXX17
#include <type_traits>
#include <utility>

using namespace std;

#ifdef __SANITIZE_ADDRESS__
extern "C" int __sanitizer_verify_contiguous_container(const void* beg, const void* mid, const void* end) noexcept;
#endif // ASan instrumentation enabled

constexpr auto literal_input = "Hello fluffy kittens";
#ifdef __cpp_char8_t
constexpr auto literal_input_u8 = u8"Hello fluffy kittens";
#endif // __cpp_char8_t
constexpr auto literal_input_u16 = u"Hello fluffy kittens";
constexpr auto literal_input_u32 = U"Hello fluffy kittens";
constexpr auto literal_input_w   = L"Hello fluffy kittens";

template <class CharType>
constexpr auto get_large_input() {
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
constexpr auto get_sso_input() {
    if constexpr (is_same_v<CharType, char>) {
        return "cat";
#ifdef __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char8_t>) {
        return u8"cat";
#endif // __cpp_char8_t
    } else if constexpr (is_same_v<CharType, char16_t>) {
        return u"cat";
    } else if constexpr (is_same_v<CharType, char32_t>) {
        return U"cat";
    } else {
        return L"cat";
    }
}

#if _HAS_CXX17
template <class CharType>
constexpr auto get_large_input_view() {
    return basic_string_view<CharType>{get_large_input<CharType>()};
}

template <class CharType>
constexpr auto get_sso_input_view() {
    return basic_string_view<CharType>{get_sso_input<CharType>()};
}
#endif // _HAS_CXX17

#if _HAS_CXX17
template <class CharType>
struct string_view_convertible {
    constexpr operator basic_string_view<CharType>() const noexcept {
        return get_large_input_view<CharType>();
    }
};

template <class CharType>
struct string_view_convertible_sso {
    constexpr operator basic_string_view<CharType>() const noexcept {
        return get_sso_input_view<CharType>();
    }
};
#endif // _HAS_CXX17

template <class CharType>
struct throw_on_conversion {
    throw_on_conversion() = default;
    throw_on_conversion(CharType) {}
    operator const CharType() const {
        throw 42;
    }
};

template <class CharType, int N>
class input_iterator_tester {
private:
    CharType data[N] = {};

public:
    input_iterator_tester() noexcept {
        fill(data, data + N, CharType{'b'});
    }

    class iterator {
    private:
        CharType* curr;

    public:
        using iterator_category = input_iterator_tag;
        using value_type        = CharType;
        using difference_type   = ptrdiff_t;
        using pointer           = void;
        using reference         = CharType&;

        explicit iterator(CharType* start) : curr(start) {}

        reference operator*() const {
            return *curr;
        }

        iterator& operator++() {
            ++curr;
            return *this;
        }

        iterator operator++(int) {
            auto tmp = *this;
            ++curr;
            return tmp;
        }

        bool operator==(const iterator& that) const {
            return curr == that.curr;
        }

        bool operator!=(const iterator& that) const {
            return !(*this == that);
        }
    };

    iterator begin() {
        return iterator(data);
    }

    iterator end() {
        return iterator(data + N);
    }
};

template <class CharType, class Alloc>
bool verify_string(basic_string<CharType, char_traits<CharType>, Alloc>& str) {
#ifdef __SANITIZE_ADDRESS__
    constexpr auto proxy_size = _Size_after_ebco_v<_Container_base>;
    if constexpr (proxy_size % _Asan_granularity != 0) { // If we have a misaligned SSO buffer we disable ASAN
        constexpr size_t max_sso_size = (16 / sizeof(CharType) < 1 ? 1 : 16 / sizeof(CharType)) - 1;
        if (str.capacity() == max_sso_size) {
            return true;
        }
    }

    size_t buffer_size  = (str.capacity() + 1) * sizeof(CharType);
    void* buffer        = const_cast<void*>(static_cast<const void*>(str.data()));
    void* aligned_start = align(8, 1, buffer, buffer_size);

    if (!aligned_start) {
        return true;
    }

    const void* end         = const_cast<void*>(static_cast<const void*>(str.data() + (str.capacity() + 1)));
    const void* mid         = const_cast<void*>(static_cast<const void*>(str.data() + str.size() + 1));
    const void* aligned_mid = mid > aligned_start ? mid : aligned_start;

    return __sanitizer_verify_contiguous_container(aligned_start, aligned_mid, end) != 0;
#else // ^^^ ASan instrumentation enabled ^^^ // vvv ASan instrumentation disabled vvv
    (void) str;
    return true;
#endif // ASan instrumentation disabled
}

// Note: This class does not satisfy all the allocator requirements but is sufficient for this test.
template <class CharType, class Pocma, class Stateless>
struct custom_test_allocator {
    using value_type                             = CharType;
    using propagate_on_container_move_assignment = Pocma;
    using is_always_equal                        = Stateless;
};

template <class T1, class T2, class Pocma, class Stateless>
constexpr bool operator==(
    const custom_test_allocator<T1, Pocma, Stateless>&, const custom_test_allocator<T2, Pocma, Stateless>&) noexcept {
    return Stateless::value;
}

template <class T1, class T2, class Pocma, class Stateless>
constexpr bool operator!=(
    const custom_test_allocator<T1, Pocma, Stateless>&, const custom_test_allocator<T2, Pocma, Stateless>&) noexcept {
    return !Stateless::value;
}

template <class CharType, class Pocma = true_type, class Stateless = true_type>
struct aligned_allocator : public custom_test_allocator<CharType, Pocma, Stateless> {
    static constexpr size_t _Minimum_allocation_alignment = 8;

    aligned_allocator() = default;
    template <class U>
    constexpr aligned_allocator(const aligned_allocator<U, Pocma, Stateless>&) noexcept {}

    CharType* allocate(size_t n) {
        return new CharType[n];
    }

    void deallocate(CharType* p, size_t) noexcept {
        delete[] p;
    }
};

template <class CharType, class Pocma = true_type, class Stateless = true_type>
struct explicit_allocator : public custom_test_allocator<CharType, Pocma, Stateless> {
    static constexpr size_t _Minimum_allocation_alignment = alignof(CharType);

    explicit_allocator() = default;
    template <class U>
    constexpr explicit_allocator(const explicit_allocator<U, Pocma, Stateless>&) noexcept {}

    CharType* allocate(size_t n) {
        CharType* mem = new CharType[n + 1];
        return mem + 1;
    }

    void deallocate(CharType* p, size_t) noexcept {
        delete[](p - 1);
    }
};

template <class CharType, class Pocma = true_type, class Stateless = true_type>
struct implicit_allocator : public custom_test_allocator<CharType, Pocma, Stateless> {
    implicit_allocator() = default;
    template <class U>
    constexpr implicit_allocator(const implicit_allocator<U, Pocma, Stateless>&) noexcept {}

    CharType* allocate(size_t n) {
        CharType* mem = new CharType[n + 1];
        return mem + 1;
    }

    void deallocate(CharType* p, size_t) noexcept {
        delete[](p - 1);
    }
};

template <class Alloc>
void test_construction() {
    using CharType = typename Alloc::value_type;
    using str      = basic_string<CharType, char_traits<CharType>, Alloc>;
    { // constructors
        // range constructors
        str literal_constructed{get_large_input<CharType>()};
        assert(verify_string(literal_constructed));

        str literal_constructed_sso{get_sso_input<CharType>()};
        assert(verify_string(literal_constructed_sso));

        str initializer_list_constructed({CharType{'H'}, CharType{'e'}, CharType{'l'}, CharType{'l'}, CharType{'o'},
            CharType{' '}, //
            CharType{'f'}, CharType{'l'}, CharType{'u'}, CharType{'f'}, CharType{'f'}, CharType{'y'}, CharType{' '},
            CharType{'k'}, CharType{'i'}, CharType{'t'}, CharType{'t'}, CharType{'e'}, CharType{'n'}, CharType{'s'}});
        assert(verify_string(initializer_list_constructed));

        str initializer_list_constructed_sso({CharType{'c'}, CharType{'a'}, CharType{'t'}});
        assert(verify_string(initializer_list_constructed_sso));

        // special member functions
        str default_constructed;
        assert(verify_string(default_constructed));

        str copy_constructed(literal_constructed);
        assert(verify_string(copy_constructed));

        str copy_constructed_sso(literal_constructed_sso);
        assert(verify_string(copy_constructed_sso));

        str move_constructed(move(copy_constructed));
        assert(verify_string(copy_constructed));
        assert(verify_string(move_constructed));

        str move_constructed_sso(move(copy_constructed_sso));
        assert(verify_string(copy_constructed_sso));
        assert(verify_string(move_constructed_sso));

        str copy_assigned_sso_to_sso(get_sso_input<CharType>());
        copy_assigned_sso_to_sso = literal_constructed_sso;
        assert(verify_string(literal_constructed_sso));
        assert(verify_string(copy_assigned_sso_to_sso));

        str copy_assigned_large_to_sso(get_sso_input<CharType>());
        copy_assigned_large_to_sso = literal_constructed;
        assert(verify_string(literal_constructed));
        assert(verify_string(copy_assigned_large_to_sso));

        str copy_assigned_sso_to_large(get_large_input<CharType>());
        copy_assigned_sso_to_large = literal_constructed_sso;
        assert(verify_string(literal_constructed_sso));
        assert(verify_string(copy_assigned_sso_to_large));

        str copy_assigned_large_to_large(get_large_input<CharType>());
        copy_assigned_large_to_large = literal_constructed;
        assert(verify_string(literal_constructed));
        assert(verify_string(copy_assigned_large_to_large));

        str move_assigned_sso_to_sso(get_sso_input<CharType>());
        move_assigned_sso_to_sso = move(copy_assigned_sso_to_sso);
        assert(verify_string(copy_assigned_sso_to_sso));
        assert(verify_string(move_assigned_sso_to_sso));

        str move_assigned_large_to_sso(get_sso_input<CharType>());
        move_assigned_large_to_sso = move(copy_assigned_large_to_sso);
        assert(verify_string(copy_assigned_large_to_sso));
        assert(verify_string(move_assigned_large_to_sso));

        str move_assigned_sso_to_large(get_large_input<CharType>());
        move_assigned_sso_to_large = move(copy_assigned_sso_to_large);
        assert(verify_string(copy_assigned_sso_to_large));
        assert(verify_string(move_assigned_sso_to_large));

        str move_assigned_large_to_large(get_large_input<CharType>());
        move_assigned_large_to_large = move(copy_assigned_large_to_large);
        assert(verify_string(copy_assigned_large_to_large));
        assert(verify_string(move_assigned_large_to_large));

        // Other constructors
        str size_value_constructed(20, CharType{'a'});
        assert(verify_string(size_value_constructed));

        str size_value_constructed_sso(2, CharType{'a'});
        assert(verify_string(size_value_constructed_sso));

        str ptr_size_constructed(get_large_input<CharType>(), 20);
        assert(verify_string(ptr_size_constructed));

        str ptr_size_constructed_sso(get_large_input<CharType>(), 2);
        assert(verify_string(ptr_size_constructed_sso));

        str iterator_constructed(literal_constructed.begin(), literal_constructed.end());
        assert(verify_string(iterator_constructed));
        assert(verify_string(literal_constructed));

        str iterator_constructed_sso(literal_constructed_sso.begin(), literal_constructed_sso.end());
        assert(verify_string(iterator_constructed_sso));
        assert(verify_string(literal_constructed_sso));

        input_iterator_tester<CharType, 20> input_iter_data;
        str input_iterator_constructed(input_iter_data.begin(), input_iter_data.end());
        assert(verify_string(input_iterator_constructed));

        input_iterator_tester<CharType, 3> input_iter_data_sso;
        str input_iterator_constructed_sso(input_iter_data_sso.begin(), input_iter_data_sso.end());
        assert(verify_string(input_iterator_constructed_sso));

#if _HAS_CXX17
        str copy_start_constructed_large_to_large(literal_constructed, 2);
        assert(verify_string(copy_start_constructed_large_to_large));
        assert(verify_string(literal_constructed));

        str copy_start_constructed_large_to_sso(literal_constructed, 20);
        assert(verify_string(copy_start_constructed_large_to_sso));
        assert(verify_string(literal_constructed));

        str copy_start_constructed_sso_to_sso(literal_constructed_sso, 1);
        assert(verify_string(copy_start_constructed_sso_to_sso));
        assert(verify_string(literal_constructed_sso));

        str copy_start_length_constructed_large_to_large(literal_constructed, 2, 18);
        assert(verify_string(copy_start_length_constructed_large_to_large));
        assert(verify_string(literal_constructed));

        str copy_start_length_constructed_large_to_sso(literal_constructed, 20, 2);
        assert(verify_string(copy_start_length_constructed_large_to_sso));
        assert(verify_string(literal_constructed));

        str copy_start_length_constructed_sso_to_sso(literal_constructed_sso, 1, 2);
        assert(verify_string(copy_start_length_constructed_sso_to_sso));
        assert(verify_string(literal_constructed_sso));

        str view_constructed(get_large_input_view<CharType>());
        assert(verify_string(view_constructed));

        str view_constructed_sso(get_sso_input_view<CharType>());
        assert(verify_string(view_constructed_sso));

        const string_view_convertible<CharType> convertible;
        str conversion_constructed(convertible);
        assert(verify_string(conversion_constructed));

        str conversion_start_length_constructed(convertible, 2, 18);
        assert(verify_string(conversion_start_length_constructed));

        str conversion_start_length_constructed_sso(convertible, 18, 2);
        assert(verify_string(conversion_start_length_constructed));

        const string_view_convertible_sso<CharType> convertible_sso;
        str conversion_constructed_sso(convertible_sso);
        assert(verify_string(conversion_constructed_sso));

        str conversion_start_length_constructed_sso_to_sso(convertible_sso, 1, 2);
        assert(verify_string(conversion_start_length_constructed_sso_to_sso));
#endif // _HAS_CXX17
    }

    { // allocator constructors
        Alloc alloc;

        // range constructors
        str literal_constructed{get_large_input<CharType>(), alloc};
        assert(verify_string(literal_constructed));

        str literal_constructed_sso{get_sso_input<CharType>(), alloc};
        assert(verify_string(literal_constructed_sso));

        str initializer_list_constructed(
            {CharType{'H'}, CharType{'e'}, CharType{'l'}, CharType{'l'}, CharType{'o'}, CharType{' '}, //
                CharType{'f'}, CharType{'l'}, CharType{'u'}, CharType{'f'}, CharType{'f'}, CharType{'y'}, CharType{' '},
                CharType{'k'}, CharType{'i'}, CharType{'t'}, CharType{'t'}, CharType{'e'}, CharType{'n'},
                CharType{'s'}},
            alloc);
        assert(verify_string(initializer_list_constructed));

        str initializer_list_constructed_sso({CharType{'c'}, CharType{'a'}, CharType{'t'}}, alloc);
        assert(verify_string(initializer_list_constructed_sso));

        // special member functions
        str default_constructed;
        assert(verify_string(default_constructed));

        str copy_constructed(literal_constructed);
        assert(verify_string(copy_constructed));

        str copy_constructed_sso(literal_constructed_sso);
        assert(verify_string(copy_constructed_sso));

        str move_constructed(move(copy_constructed));
        assert(verify_string(copy_constructed));
        assert(verify_string(move_constructed));

        str move_constructed_sso(move(copy_constructed_sso));
        assert(verify_string(copy_constructed_sso));
        assert(verify_string(move_constructed_sso));

        // Other constructors
        str size_value_constructed(20, CharType{'a'}, alloc);
        assert(verify_string(size_value_constructed));

        str size_value_constructed_sso(2, CharType{'a'}, alloc);
        assert(verify_string(size_value_constructed_sso));

        str ptr_size_constructed(get_large_input<CharType>(), 20, alloc);
        assert(verify_string(ptr_size_constructed));

        str ptr_size_constructed_sso(get_large_input<CharType>(), 2, alloc);
        assert(verify_string(ptr_size_constructed_sso));

        str iterator_constructed(literal_constructed.begin(), literal_constructed.end(), alloc);
        assert(verify_string(iterator_constructed));
        assert(verify_string(literal_constructed));

        str iterator_constructed_sso(literal_constructed_sso.begin(), literal_constructed_sso.end(), alloc);
        assert(verify_string(iterator_constructed_sso));
        assert(verify_string(literal_constructed_sso));

        input_iterator_tester<CharType, 20> input_iter_data;
        str input_iterator_constructed(input_iter_data.begin(), input_iter_data.end(), alloc);
        assert(verify_string(input_iterator_constructed));

        input_iterator_tester<CharType, 3> input_iter_data_sso;
        str input_iterator_constructed_sso(input_iter_data_sso.begin(), input_iter_data_sso.end(), alloc);
        assert(verify_string(input_iterator_constructed_sso));

#if _HAS_CXX17
        str view_constructed(get_large_input_view<CharType>(), alloc);
        assert(verify_string(view_constructed));

        str view_constructed_sso(get_sso_input_view<CharType>(), alloc);
        assert(verify_string(view_constructed_sso));

        const string_view_convertible<CharType> convertible;
        const string_view_convertible_sso<CharType> convertible_sso;
        str conversion_constructed(convertible, alloc);
        assert(verify_string(conversion_constructed));

        str conversion_constructed_sso(convertible_sso, alloc);
        assert(verify_string(conversion_constructed_sso));

        str conversion_start_length_constructed(convertible, 2, 18, alloc);
        assert(verify_string(conversion_start_length_constructed));

        str conversion_start_length_constructed_sso(convertible, 18, 2, alloc);
        assert(verify_string(conversion_start_length_constructed));

        str conversion_start_length_constructed_sso_to_sso(convertible_sso, 1, 2, alloc);
        assert(verify_string(conversion_start_length_constructed_sso_to_sso));
#endif // _HAS_CXX17
    }
}

template <class Alloc>
void test_append() {
    using CharType = typename Alloc::value_type;
    using str      = basic_string<CharType, char_traits<CharType>, Alloc>;

    constexpr size_t large_size   = 20;
    constexpr size_t sso_size     = 1;
    constexpr size_t max_sso_size = (16 / sizeof(CharType) < 1 ? 1 : 16 / sizeof(CharType)) - 1;

    const str input(large_size, CharType{'b'});
    const str input_sso(sso_size, CharType{'b'});
    const str input_sso_growing(max_sso_size, CharType{'b'});

    { // push_back
        str push_back{input};
        push_back.push_back(CharType{'y'});
        assert(verify_string(push_back));

        str push_back_sso{input_sso};
        push_back_sso.push_back(CharType{'y'});
        assert(verify_string(push_back_sso));

        str push_back_growing{input_sso_growing};
        push_back_growing.push_back(CharType{'y'});
        assert(verify_string(push_back_growing));
    }

    { // append
        str append_size_char{input};
        append_size_char.append(5, CharType{'a'});
        assert(verify_string(append_size_char));

        str append_size_char_sso{input_sso};
        append_size_char_sso.append(2, CharType{'a'});
        assert(verify_string(append_size_char_sso));

        str append_size_char_growing{input_sso_growing};
        append_size_char_growing.append(2, CharType{'a'});
        assert(verify_string(append_size_char_growing));

        str append_str{input};
        append_str.append(input_sso);
        assert(verify_string(append_str));

        str append_str_sso{input_sso};
        append_str_sso.append(input_sso);
        assert(verify_string(append_str_sso));

        str append_str_growing{input_sso_growing};
        append_str_growing.append(input_sso);
        assert(verify_string(append_str_growing));

        str append_str_pos{input};
        append_str_pos.append(input_sso, 1);
        assert(verify_string(append_str_pos));

        str append_str_pos_sso{input_sso};
        append_str_pos_sso.append(input_sso, 1);
        assert(verify_string(append_str_pos_sso));

        str append_str_pos_growing{input_sso_growing};
        append_str_pos_growing.append(input_sso, 1);
        assert(verify_string(append_str_pos_growing));

        str append_str_pos_len{input};
        append_str_pos_len.append(input_sso, 1, 2);
        assert(verify_string(append_str_pos_len));

        str append_str_pos_len_sso{input_sso};
        append_str_pos_len_sso.append(input_sso, 1, 2);
        assert(verify_string(append_str_pos_len_sso));

        str append_str_pos_len_growing{input_sso_growing};
        append_str_pos_len_growing.append(input_sso, 1, 2);
        assert(verify_string(append_str_pos_len_growing));

        str append_literal{input};
        append_literal.append(get_sso_input<CharType>());
        assert(verify_string(append_literal));

        str append_literal_sso{input_sso};
        append_literal_sso.append(get_sso_input<CharType>());
        assert(verify_string(append_literal_sso));

        str append_literal_growing{input_sso_growing};
        append_literal_growing.append(get_sso_input<CharType>());
        assert(verify_string(append_literal_growing));

        str append_literal_size{input};
        append_literal_size.append(get_sso_input<CharType>(), 2);
        assert(verify_string(append_literal_size));

        str append_literal_size_sso{input_sso};
        append_literal_size_sso.append(get_sso_input<CharType>(), 2);
        assert(verify_string(append_literal_size_sso));

        str append_literal_size_growing{input_sso_growing};
        append_literal_size_growing.append(get_sso_input<CharType>(), 2);
        assert(verify_string(append_literal_size_growing));

        str append_iterator{input};
        append_iterator.append(input_sso.begin(), input_sso.end());
        assert(verify_string(append_iterator));

        str append_iterator_sso{input_sso};
        append_iterator_sso.append(input_sso.begin(), input_sso.end());
        assert(verify_string(append_iterator_sso));

        str append_iterator_growing{input_sso_growing};
        append_iterator_growing.append(input_sso.begin(), input_sso.end());
        assert(verify_string(append_iterator_growing));

        input_iterator_tester<CharType, 3> input_iter_data;
        str append_input_iterator{input};
        append_input_iterator.append(input_iter_data.begin(), input_iter_data.end());
        assert(verify_string(append_input_iterator));

        str append_input_iterator_sso{input_sso};
        append_input_iterator_sso.append(input_iter_data.begin(), input_iter_data.end());
        assert(verify_string(append_input_iterator_sso));

        str append_input_iterator_growing{input_sso_growing};
        append_input_iterator_growing.append(input_iter_data.begin(), input_iter_data.end());
        assert(verify_string(append_input_iterator_growing));

        str append_initializer_list{input};
        append_initializer_list.append({CharType{'b'}, CharType{'b'}});
        assert(verify_string(append_initializer_list));

        str append_initializer_list_sso{input_sso};
        append_initializer_list_sso.append({CharType{'b'}, CharType{'b'}});
        assert(verify_string(append_initializer_list_sso));

        str append_initializer_list_growing{input_sso_growing};
        append_initializer_list_growing.append({CharType{'b'}, CharType{'b'}});
        assert(verify_string(append_initializer_list_growing));

#if _HAS_CXX17
        const string_view_convertible_sso<CharType> convertible;
        str append_conversion{input};
        append_conversion.append(convertible);
        assert(verify_string(append_conversion));

        str append_conversion_sso{input_sso};
        append_conversion_sso.append(convertible);
        assert(verify_string(append_conversion_sso));

        str append_conversion_growing{input_sso_growing};
        append_conversion_growing.append(convertible);
        assert(verify_string(append_conversion_growing));

        str append_conversion_start_length{input};
        append_conversion_start_length.append(convertible, 1, 2);
        assert(verify_string(append_conversion_start_length));

        str append_conversion_start_length_sso{input_sso};
        append_conversion_start_length_sso.append(convertible, 1, 2);
        assert(verify_string(append_conversion_start_length_sso));

        str append_conversion_start_length_growing{input_sso_growing};
        append_conversion_start_length_growing.append(convertible, 1, 2);
        assert(verify_string(append_conversion_start_length_growing));
#endif // _HAS_CXX17
    }

    { // operator+=
        str plus_string{input};
        plus_string += input_sso;
        assert(verify_string(plus_string));

        str plus_string_sso{input_sso};
        plus_string_sso += input_sso;
        assert(verify_string(plus_string_sso));

        str plus_string_growing{input_sso_growing};
        plus_string_growing += input_sso;
        assert(verify_string(plus_string_growing));

        str plus_character{input};
        plus_character += CharType{'a'};
        assert(verify_string(plus_character));

        str plus_character_sso{input_sso};
        plus_character_sso += CharType{'a'};
        assert(verify_string(plus_character_sso));

        str plus_character_growing{input_sso_growing};
        plus_character_growing += CharType{'a'};
        assert(verify_string(plus_character_growing));

        str plus_literal{input};
        plus_literal += get_sso_input<CharType>();
        assert(verify_string(plus_literal));

        str plus_literal_sso{input_sso};
        plus_literal_sso += get_sso_input<CharType>();
        assert(verify_string(plus_literal_sso));

        str plus_literal_growing{input_sso_growing};
        plus_literal_growing += get_sso_input<CharType>();
        assert(verify_string(plus_literal_growing));

        str plus_initializer_list{input};
        plus_initializer_list += {CharType{'c'}, CharType{'a'}, CharType{'t'}};
        assert(verify_string(plus_initializer_list));

        str plus_initializer_list_sso{input_sso};
        plus_initializer_list_sso += {CharType{'c'}, CharType{'a'}, CharType{'t'}};
        assert(verify_string(plus_initializer_list_sso));

        str plus_initializer_list_growing{input_sso_growing};
        plus_initializer_list_growing += {CharType{'c'}, CharType{'a'}, CharType{'t'}};
        assert(verify_string(plus_initializer_list_growing));

#if _HAS_CXX17
        const string_view_convertible_sso<CharType> convertible;
        str plus_conversion{input};
        plus_conversion += convertible;
        assert(verify_string(plus_conversion));

        str plus_conversion_sso{input_sso};
        plus_conversion_sso += convertible;
        assert(verify_string(plus_conversion_sso));

        str plus_conversion_growing{input_sso_growing};
        plus_conversion_growing += convertible;
        assert(verify_string(plus_conversion_growing));
#endif // _HAS_CXX17
    }

    { // operator+
        str op_str_str_large_large = input + input;
        assert(verify_string(op_str_str_large_large));

        str op_str_str_large_sso = input + input_sso;
        assert(verify_string(op_str_str_large_sso));

        str op_str_str_sso_large = input_sso + input;
        assert(verify_string(op_str_str_sso_large));

        str op_str_str_sso_sso = input_sso + input_sso;
        assert(verify_string(op_str_str_sso_sso));

        str op_str_literal_large_large = input + get_large_input<CharType>();
        assert(verify_string(op_str_literal_large_large));

        str op_str_literal_large_sso = input + get_sso_input<CharType>();
        assert(verify_string(op_str_literal_large_sso));

        str op_str_literal_sso_large = input_sso + get_large_input<CharType>();
        assert(verify_string(op_str_literal_sso_large));

        str op_str_literal_sso_sso = input_sso + get_sso_input<CharType>();
        assert(verify_string(op_str_literal_sso_sso));

        str op_literal_str_large_large = get_large_input<CharType>() + input;
        assert(verify_string(op_literal_str_large_large));

        str op_literal_str_large_sso = get_large_input<CharType>() + input;
        assert(verify_string(op_literal_str_large_sso));

        str op_literal_str_sso_large = get_sso_input<CharType>() + input_sso;
        assert(verify_string(op_literal_str_sso_large));

        str op_literal_str_sso_sso = get_sso_input<CharType>() + input_sso;
        assert(verify_string(op_literal_str_sso_sso));

        str op_str_char_large = input + CharType{'!'};
        assert(verify_string(op_str_char_large));

        str op_str_char_sso = input_sso + CharType{'!'};
        assert(verify_string(op_str_char_sso));

        str op_str_char_sso_growing = input_sso_growing + CharType{'!'};
        assert(verify_string(op_str_char_sso_growing));

        str op_char_str_large = CharType{'!'} + input;
        assert(verify_string(op_char_str_large));

        str op_char_str_sso = CharType{'!'} + input_sso;
        assert(verify_string(op_char_str_sso));

        str op_char_str_sso_growing = CharType{'!'} + input_sso_growing;
        assert(verify_string(op_char_str_sso_growing));

        // With rvalue input
        str op_rstr_rstr_large_large = str(large_size, CharType{'b'}) + str(large_size, CharType{'b'});
        assert(verify_string(op_rstr_rstr_large_large));

        str op_rstr_rstr_large_sso = str(large_size, CharType{'b'}) + str(sso_size, CharType{'b'});
        assert(verify_string(op_rstr_rstr_large_sso));

        str op_rstr_rstr_sso_large = str(sso_size, CharType{'b'}) + str(large_size, CharType{'b'});
        assert(verify_string(op_rstr_rstr_sso_large));

        str op_rstr_rstr_sso_sso = str(sso_size, CharType{'b'}) + str(sso_size, CharType{'b'});
        assert(verify_string(op_rstr_rstr_sso_sso));

        str op_rstr_literal_large_large = str(large_size, CharType{'b'}) + get_large_input<CharType>();
        assert(verify_string(op_rstr_literal_large_large));

        str op_rstr_literal_large_sso = str(large_size, CharType{'b'}) + get_sso_input<CharType>();
        assert(verify_string(op_rstr_literal_large_sso));

        str op_rstr_literal_sso_large = str(sso_size, CharType{'b'}) + get_large_input<CharType>();
        assert(verify_string(op_rstr_literal_sso_large));

        str op_rstr_literal_sso_sso = str(sso_size, CharType{'b'}) + get_sso_input<CharType>();
        assert(verify_string(op_rstr_literal_sso_sso));

        str op_literal_rstr_large_large = get_large_input<CharType>() + str(large_size, CharType{'b'});
        assert(verify_string(op_literal_rstr_large_large));

        str op_literal_rstr_large_sso = get_large_input<CharType>() + str(large_size, CharType{'b'});
        assert(verify_string(op_literal_rstr_large_sso));

        str op_literal_rstr_sso_large = get_sso_input<CharType>() + str(sso_size, CharType{'b'});
        assert(verify_string(op_literal_rstr_sso_large));

        str op_literal_rstr_sso_sso = get_sso_input<CharType>() + str(sso_size, CharType{'b'});
        assert(verify_string(op_literal_rstr_sso_sso));

        str op_rstr_char_large = str(large_size, CharType{'b'}) + CharType{'!'};
        assert(verify_string(op_rstr_char_large));

        str op_rstr_char_sso = str(sso_size, CharType{'b'}) + CharType{'!'};
        assert(verify_string(op_rstr_char_sso));

        str op_rstr_char_sso_growing = str(max_sso_size, CharType{'b'}) + CharType{'!'};
        assert(verify_string(op_rstr_char_sso_growing));

        str op_char_rstr_large = CharType{'!'} + str(large_size, CharType{'b'});
        assert(verify_string(op_char_rstr_large));

        str op_char_rstr_sso = CharType{'!'} + str(sso_size, CharType{'b'});
        assert(verify_string(op_char_rstr_sso));

        str op_char_rstr_sso_growing = CharType{'!'} + str(max_sso_size, CharType{'b'});
        assert(verify_string(op_char_rstr_sso_growing));
    }
}

template <class Alloc>
void test_assign() {
    using CharType = typename Alloc::value_type;
    using str      = basic_string<CharType, char_traits<CharType>, Alloc>;

    constexpr size_t large_size = 20;
    constexpr size_t sso_size   = 2;

    const str start(large_size - 1, CharType{'a'});
    const str start_sso(sso_size + 1, CharType{'a'});

    const str input(large_size, CharType{'b'});
    const str input_sso(sso_size, CharType{'b'});

    input_iterator_tester<CharType, 20> input_iter_data;
    input_iterator_tester<CharType, 3> input_iter_data_sso;

    { // assignment operator
        str copy_assigned_large_large{start};
        copy_assigned_large_large = input;
        assert(verify_string(copy_assigned_large_large));

        str copy_assigned_large_sso{start};
        copy_assigned_large_sso = input_sso;
        assert(verify_string(copy_assigned_large_sso));

        str copy_assigned_sso_large{start_sso};
        copy_assigned_sso_large = input;
        assert(verify_string(copy_assigned_sso_large));

        str copy_assigned_sso_sso{start_sso};
        copy_assigned_sso_sso = input_sso;
        assert(verify_string(copy_assigned_sso_sso));

        str move_assigned_large_large{start};
        move_assigned_large_large = move(copy_assigned_large_large);
        assert(verify_string(copy_assigned_large_large));
        assert(verify_string(move_assigned_large_large));

        str move_assigned_large_sso{start};
        move_assigned_large_sso = move(copy_assigned_large_sso);
        assert(verify_string(copy_assigned_large_sso));
        assert(verify_string(move_assigned_large_sso));

        str move_assigned_sso_large{start_sso};
        move_assigned_sso_large = move(move_assigned_large_large);
        assert(verify_string(move_assigned_large_large));
        assert(verify_string(move_assigned_sso_large));

        str move_assigned_sso_sso{start_sso};
        move_assigned_sso_sso = move(move_assigned_large_sso);
        assert(verify_string(move_assigned_large_sso));
        assert(verify_string(move_assigned_sso_sso));

        str literal_assigned_large_large{start};
        literal_assigned_large_large = get_large_input<CharType>();
        assert(verify_string(literal_assigned_large_large));

        str literal_assigned_large_sso{start};
        literal_assigned_large_sso = get_sso_input<CharType>();
        assert(verify_string(literal_assigned_large_sso));

        str literal_assigned_sso_large{start_sso};
        literal_assigned_sso_large = get_large_input<CharType>();
        assert(verify_string(literal_assigned_sso_large));

        str literal_assigned_sso_sso{start_sso};
        literal_assigned_sso_sso = get_sso_input<CharType>();
        assert(verify_string(literal_assigned_sso_sso));

        str char_assigned_large{start};
        char_assigned_large = CharType{'!'};
        assert(verify_string(char_assigned_large));

        str char_assigned_sso{start_sso};
        char_assigned_sso = CharType{'!'};
        assert(verify_string(char_assigned_sso));

        str initializer_list_assigned_large_large{start};
        initializer_list_assigned_large_large = {CharType{'H'}, CharType{'e'}, CharType{'l'}, CharType{'l'},
            CharType{'o'}, CharType{' '}, //
            CharType{'f'}, CharType{'l'}, CharType{'u'}, CharType{'f'}, CharType{'f'}, CharType{'y'}, CharType{' '},
            CharType{'k'}, CharType{'i'}, CharType{'t'}, CharType{'t'}, CharType{'e'}, CharType{'n'}, CharType{'s'}};
        assert(verify_string(initializer_list_assigned_large_large));

        str initializer_list_assigned_large_sso{start};
        initializer_list_assigned_large_sso = {CharType{'c'}, CharType{'a'}, CharType{'t'}};
        assert(verify_string(initializer_list_assigned_large_sso));

        str initializer_list_assigned_sso_large{start_sso};
        initializer_list_assigned_sso_large = {CharType{'H'}, CharType{'e'}, CharType{'l'}, CharType{'l'},
            CharType{'o'}, CharType{' '}, //
            CharType{'f'}, CharType{'l'}, CharType{'u'}, CharType{'f'}, CharType{'f'}, CharType{'y'}, CharType{' '},
            CharType{'k'}, CharType{'i'}, CharType{'t'}, CharType{'t'}, CharType{'e'}, CharType{'n'}, CharType{'s'}};
        assert(verify_string(initializer_list_assigned_sso_large));

        str initializer_list_assigned_sso_sso{start_sso};
        initializer_list_assigned_sso_sso = {CharType{'c'}, CharType{'a'}, CharType{'t'}};
        assert(verify_string(initializer_list_assigned_sso_sso));

#if _HAS_CXX17
        const string_view_convertible<CharType> convertible;
        const string_view_convertible_sso<CharType> convertible_sso;
        str conversion_assigned_large_large{start};
        conversion_assigned_large_large = convertible;
        assert(verify_string(conversion_assigned_large_large));

        str conversion_assigned_large_sso{start};
        conversion_assigned_large_sso = convertible_sso;
        assert(verify_string(conversion_assigned_large_sso));

        str conversion_assigned_sso_large{start_sso};
        conversion_assigned_sso_large = convertible;
        assert(verify_string(conversion_assigned_sso_large));

        str conversion_assigned_sso_sso{start_sso};
        conversion_assigned_sso_sso = convertible_sso;
        assert(verify_string(conversion_assigned_sso_sso));
#endif // _HAS_CXX17
    }

    { // assign
        str assign_str_large_large{start};
        assign_str_large_large.assign(input);
        assert(verify_string(assign_str_large_large));

        str assign_str_large_sso{start};
        assign_str_large_sso.assign(input_sso);
        assert(verify_string(assign_str_large_sso));

        str assign_str_sso_large{start_sso};
        assign_str_sso_large.assign(input);
        assert(verify_string(assign_str_sso_large));

        str assign_str_sso_sso{start_sso};
        assign_str_sso_sso.assign(input_sso);
        assert(verify_string(assign_str_sso_sso));

        str assign_str_size_large_large{start};
        assign_str_size_large_large.assign(input, 1);
        assert(verify_string(assign_str_size_large_large));

        str assign_str_size_large_sso{start};
        assign_str_size_large_sso.assign(input_sso, 1);
        assert(verify_string(assign_str_size_large_sso));

        str assign_str_size_sso_large{start_sso};
        assign_str_size_sso_large.assign(input, 1);
        assert(verify_string(assign_str_size_sso_large));

        str assign_str_size_sso_sso{start_sso};
        assign_str_size_sso_sso.assign(input_sso, 1);
        assert(verify_string(assign_str_size_sso_sso));

        str assign_str_len_size_large_large{start};
        assign_str_len_size_large_large.assign(input, 1, large_size - 1);
        assert(verify_string(assign_str_len_size_large_large));

        str assign_str_size_len_large_sso{start};
        assign_str_size_len_large_sso.assign(input_sso, 1, sso_size - 1);
        assert(verify_string(assign_str_size_len_large_sso));

        str assign_str_size_len_sso_large{start_sso};
        assign_str_size_len_sso_large.assign(input, 1, large_size - 1);
        assert(verify_string(assign_str_size_len_sso_large));

        str assign_str_size_len_sso_sso{start_sso};
        assign_str_size_len_sso_sso.assign(input_sso, 1, sso_size - 1);
        assert(verify_string(assign_str_size_len_sso_sso));

        str assign_rstr_large_large{start};
        assign_rstr_large_large.assign(move(assign_str_large_large));
        assert(verify_string(assign_str_large_large));
        assert(verify_string(assign_rstr_large_large));

        str assign_rstr_large_sso{start};
        assign_rstr_large_sso.assign(move(assign_str_large_sso));
        assert(verify_string(assign_str_large_sso));
        assert(verify_string(assign_rstr_large_sso));

        str assign_rstr_sso_large{start_sso};
        assign_rstr_sso_large.assign(move(assign_str_sso_large));
        assert(verify_string(assign_str_sso_large));
        assert(verify_string(assign_rstr_sso_large));

        str assign_rstr_sso_sso{start_sso};
        assign_rstr_sso_sso.assign(move(assign_str_sso_sso));
        assert(verify_string(assign_str_sso_sso));
        assert(verify_string(assign_rstr_sso_sso));

        str assign_literal_large_large{start};
        assign_literal_large_large.assign(get_large_input<CharType>());
        assert(verify_string(assign_literal_large_large));

        str assign_literal_large_sso{start};
        assign_literal_large_sso.assign(get_sso_input<CharType>());
        assert(verify_string(assign_literal_large_sso));

        str assign_literal_sso_large{start_sso};
        assign_literal_sso_large.assign(get_large_input<CharType>());
        assert(verify_string(assign_literal_sso_large));

        str assign_literal_sso_sso{start_sso};
        assign_literal_sso_sso.assign(get_sso_input<CharType>());
        assert(verify_string(assign_literal_sso_sso));

        str assign_literal_count_large_large{start};
        assign_literal_count_large_large.assign(get_large_input<CharType>(), 18);
        assert(verify_string(assign_literal_count_large_large));

        str assign_literal_count_large_sso{start};
        assign_literal_count_large_sso.assign(get_sso_input<CharType>(), 3);
        assert(verify_string(assign_literal_count_large_sso));

        str assign_literal_count_sso_large{start_sso};
        assign_literal_count_sso_large.assign(get_large_input<CharType>(), 18);
        assert(verify_string(assign_literal_count_sso_large));

        str assign_literal_count_sso_sso{start_sso};
        assign_literal_count_sso_sso.assign(get_sso_input<CharType>(), 3);
        assert(verify_string(assign_literal_count_sso_sso));

        str assign_char_count_large_large{start};
        assign_char_count_large_large.assign(18, CharType{'c'});
        assert(verify_string(assign_char_count_large_large));

        str assign_char_count_large_sso{start};
        assign_char_count_large_sso.assign(3, CharType{'c'});
        assert(verify_string(assign_char_count_large_sso));

        str assign_char_count_sso_large{start_sso};
        assign_char_count_sso_large.assign(18, CharType{'c'});
        assert(verify_string(assign_char_count_sso_large));

        str assign_char_count_sso_sso{start_sso};
        assign_char_count_sso_sso.assign(3, CharType{'c'});
        assert(verify_string(assign_char_count_sso_sso));

        str assign_iterator_large_large{start};
        assign_iterator_large_large.assign(input.begin(), input.end());
        assert(verify_string(assign_iterator_large_large));

        str assign_iterator_large_sso{start};
        assign_iterator_large_sso.assign(input_sso.begin(), input_sso.end());
        assert(verify_string(assign_iterator_large_sso));

        str assign_iterator_sso_large{start_sso};
        assign_iterator_sso_large.assign(input.begin(), input.end());
        assert(verify_string(assign_iterator_sso_large));

        str assign_iterator_sso_sso{start_sso};
        assign_iterator_sso_sso.assign(input_sso.begin(), input_sso.end());
        assert(verify_string(assign_iterator_sso_sso));

        str assign_input_iterator_large_large{start};
        assign_input_iterator_large_large.assign(input_iter_data.begin(), input_iter_data.end());
        assert(verify_string(assign_input_iterator_large_large));

        str assign_input_iterator_large_sso{start};
        assign_input_iterator_large_sso.assign(input_iter_data_sso.begin(), input_iter_data_sso.end());
        assert(verify_string(assign_input_iterator_large_sso));

        str assign_input_iterator_sso_large{start_sso};
        assign_input_iterator_sso_large.assign(input_iter_data.begin(), input_iter_data.end());
        assert(verify_string(assign_input_iterator_sso_large));

        str assign_input_iterator_sso_sso{start_sso};
        assign_input_iterator_sso_sso.assign(input_iter_data_sso.begin(), input_iter_data_sso.end());
        assert(verify_string(assign_input_iterator_sso_sso));

        str assign_initializer_list_large_large{start};
        assign_initializer_list_large_large.assign({CharType{'H'}, CharType{'e'}, CharType{'l'}, CharType{'l'},
            CharType{'o'}, CharType{' '}, //
            CharType{'f'}, CharType{'l'}, CharType{'u'}, CharType{'f'}, CharType{'f'}, CharType{'y'}, CharType{' '},
            CharType{'k'}, CharType{'i'}, CharType{'t'}, CharType{'t'}, CharType{'e'}, CharType{'n'}, CharType{'s'}});
        assert(verify_string(assign_initializer_list_large_large));

        str assign_initializer_list_large_sso{start};
        assign_initializer_list_large_sso.assign({CharType{'c'}, CharType{'a'}, CharType{'t'}});
        assert(verify_string(assign_initializer_list_large_large));

        str assign_initializer_list_sso_large{start_sso};
        assign_initializer_list_sso_large.assign({CharType{'H'}, CharType{'e'}, CharType{'l'}, CharType{'l'},
            CharType{'o'}, CharType{' '}, //
            CharType{'f'}, CharType{'l'}, CharType{'u'}, CharType{'f'}, CharType{'f'}, CharType{'y'}, CharType{' '},
            CharType{'k'}, CharType{'i'}, CharType{'t'}, CharType{'t'}, CharType{'e'}, CharType{'n'}, CharType{'s'}});
        assert(verify_string(assign_initializer_list_sso_large));

        str assign_initializer_list_sso_sso{start_sso};
        assign_initializer_list_sso_sso.assign({CharType{'c'}, CharType{'a'}, CharType{'t'}});
        assert(verify_string(assign_initializer_list_sso_sso));

#if _HAS_CXX17
        const string_view_convertible<CharType> convertible;
        const string_view_convertible_sso<CharType> convertible_sso;

        str assign_conversion_large_large{start};
        assign_conversion_large_large.assign(convertible);
        assert(verify_string(assign_conversion_large_large));

        str assign_conversion_large_sso{start};
        assign_conversion_large_sso.assign(convertible_sso);
        assert(verify_string(assign_conversion_large_sso));

        str assign_conversion_sso_large{start_sso};
        assign_conversion_sso_large.assign(convertible);
        assert(verify_string(assign_conversion_sso_large));

        str assign_conversion_sso_sso{start_sso};
        assign_conversion_sso_sso.assign(convertible_sso);
        assert(verify_string(assign_conversion_sso_sso));

        str assign_conversion_start_length_large_large{start};
        assign_conversion_start_length_large_large.assign(convertible, 2, 18);
        assert(verify_string(assign_conversion_start_length_large_large));

        str assign_conversion_start_length_large_sso{start};
        assign_conversion_start_length_large_sso.assign(convertible_sso, 1, 2);
        assert(verify_string(assign_conversion_start_length_large_sso));

        str assign_conversion_start_length_sso_large{start_sso};
        assign_conversion_start_length_sso_large.assign(convertible, 2, 18);
        assert(verify_string(assign_conversion_start_length_sso_large));

        str assign_conversion_start_length_sso_sso{start_sso};
        assign_conversion_start_length_sso_sso.assign(convertible_sso, 1, 2);
        assert(verify_string(assign_conversion_start_length_sso_sso));
#endif // _HAS_CXX17
    }
}

template <class Alloc>
void test_insertion() {
    using CharType = typename Alloc::value_type;
    using str      = basic_string<CharType, char_traits<CharType>, Alloc>;

    constexpr size_t large_size   = 20;
    constexpr size_t sso_size     = 1;
    constexpr size_t max_sso_size = (16 / sizeof(CharType) < 1 ? 1 : 16 / sizeof(CharType)) - 1;

    const str input(large_size, CharType{'b'});
    const str input_sso(sso_size, CharType{'b'});
    const str input_sso_growing(max_sso_size, CharType{'b'});

    input_iterator_tester<CharType, 3> input_iter_data_sso;

    { // insert
        const CharType to_be_inserted = CharType{','};
        str insert_char{input};
        insert_char.insert(insert_char.begin(), to_be_inserted);
        assert(verify_string(insert_char));

        str insert_char_sso{input_sso};
        insert_char_sso.insert(insert_char_sso.begin(), to_be_inserted);
        assert(verify_string(insert_char_sso));

        str insert_char_growing{input_sso_growing};
        insert_char_growing.insert(insert_char_growing.begin(), to_be_inserted);
        assert(verify_string(insert_char_growing));

        str insert_char_rvalue{input};
        insert_char_rvalue.insert(insert_char_rvalue.begin(), CharType{'a'});
        assert(verify_string(insert_char_rvalue));

        str insert_char_rvalue_sso{input_sso};
        insert_char_rvalue_sso.insert(insert_char_rvalue_sso.begin(), CharType{'a'});
        assert(verify_string(insert_char_rvalue_sso));

        str insert_char_rvalue_growing{input_sso_growing};
        insert_char_rvalue_growing.insert(insert_char_rvalue_growing.begin(), CharType{'a'});
        assert(verify_string(insert_char_rvalue_growing));

        str insert_iter_count_char{input};
        insert_iter_count_char.insert(insert_iter_count_char.begin(), 2, CharType{'a'});
        assert(verify_string(insert_iter_count_char));

        str insert_iter_count_char_sso{input_sso};
        insert_iter_count_char_sso.insert(insert_iter_count_char_sso.begin(), 2, CharType{'a'});
        assert(verify_string(insert_iter_count_char_sso));

        str insert_iter_count_char_growing{input_sso_growing};
        insert_iter_count_char_growing.insert(insert_iter_count_char_growing.begin(), 2, CharType{'a'});
        assert(verify_string(insert_iter_count_char_growing));

        str insert_iter{input};
        insert_iter.insert(insert_iter.begin(), input_sso.begin(), input_sso.end());
        assert(verify_string(insert_iter));

        str insert_iter_sso{input_sso};
        insert_iter_sso.insert(insert_iter_sso.begin(), input_sso.begin(), input_sso.end());
        assert(verify_string(insert_iter_sso));

        str insert_iter_growing{input_sso_growing};
        insert_iter_growing.insert(insert_iter_growing.begin(), input_sso.begin(), input_sso.end());
        assert(verify_string(insert_iter_growing));

        str insert_input_iter{input};
        insert_input_iter.insert(insert_input_iter.begin(), input_iter_data_sso.begin(), input_iter_data_sso.end());
        assert(verify_string(insert_input_iter));

        str insert_input_iter_sso{input_sso};
        insert_input_iter_sso.insert(
            insert_input_iter_sso.begin(), input_iter_data_sso.begin(), input_iter_data_sso.end());
        assert(verify_string(insert_input_iter_sso));

        str insert_input_iter_growing{input_sso_growing};
        insert_input_iter_growing.insert(
            insert_input_iter_growing.begin(), input_iter_data_sso.begin(), input_iter_data_sso.end());
        assert(verify_string(insert_input_iter_growing));

        str insert_initializer_list{input};
        insert_initializer_list.insert(insert_initializer_list.begin(), {CharType{'c'}, CharType{'a'}, CharType{'t'}});
        assert(verify_string(insert_initializer_list));

        str insert_initializer_list_sso{input_sso};
        insert_initializer_list_sso.insert(
            insert_initializer_list_sso.begin(), {CharType{'c'}, CharType{'a'}, CharType{'t'}});
        assert(verify_string(insert_initializer_list_sso));

        str insert_initializer_list_growing{input_sso_growing};
        insert_initializer_list_growing.insert(
            insert_initializer_list_growing.begin(), {CharType{'c'}, CharType{'a'}, CharType{'t'}});
        assert(verify_string(insert_initializer_list_growing));

        str insert_pos_str{input};
        insert_pos_str.insert(0, input_sso);
        assert(verify_string(insert_pos_str));

        str insert_pos_str_sso{input_sso};
        insert_pos_str_sso.insert(0, input_sso);
        assert(verify_string(insert_pos_str_sso));

        str insert_pos_str_growing{input_sso_growing};
        insert_pos_str_growing.insert(0, input_sso);
        assert(verify_string(insert_pos_str_growing));

        str insert_pos_substr{input};
        insert_pos_substr.insert(0, input_sso, 1, 2);
        assert(verify_string(insert_pos_substr));

        str insert_pos_substr_sso{input_sso};
        insert_pos_substr_sso.insert(0, input_sso, 1, 2);
        assert(verify_string(insert_pos_substr_sso));

        str insert_pos_substr_growing{input_sso_growing};
        insert_pos_substr_growing.insert(0, input_sso, 1, 2);
        assert(verify_string(insert_pos_substr_growing));

#if _HAS_CXX17
        const string_view_convertible_sso<CharType> convertible;
        str insert_pos_conversion{input};
        insert_pos_conversion.insert(0, convertible);
        assert(verify_string(insert_pos_conversion));

        str insert_pos_conversion_sso{input_sso};
        insert_pos_conversion_sso.insert(0, convertible);
        assert(verify_string(insert_pos_conversion_sso));

        str insert_pos_conversion_growing{input_sso_growing};
        insert_pos_conversion_growing.insert(0, convertible);
        assert(verify_string(insert_pos_conversion_growing));

        str insert_pos_conversion_substr{input};
        insert_pos_conversion.insert(0, convertible, 1, 2);
        assert(verify_string(insert_pos_conversion));

        str insert_pos_conversion_substr_sso{input_sso};
        insert_pos_conversion_sso.insert(0, convertible, 1, 2);
        assert(verify_string(insert_pos_conversion_sso));

        str insert_pos_conversion_substr_growing{input_sso_growing};
        insert_pos_conversion_growing.insert(0, convertible, 1, 2);
        assert(verify_string(insert_pos_conversion_growing));
#endif // _HAS_CXX17

        str insert_pos_literal{input};
        insert_pos_literal.insert(0, get_sso_input<CharType>());
        assert(verify_string(insert_pos_literal));

        str insert_pos_literal_sso{input_sso};
        insert_pos_literal_sso.insert(0, get_sso_input<CharType>());
        assert(verify_string(insert_pos_literal_sso));

        str insert_pos_literal_growing{input_sso_growing};
        insert_pos_literal_growing.insert(0, get_sso_input<CharType>());
        assert(verify_string(insert_pos_literal_growing));

        str insert_pos_literal_substr{input};
        insert_pos_literal_substr.insert(0, get_sso_input<CharType>(), 1);
        assert(verify_string(insert_pos_literal_substr));

        str insert_pos_literal_substr_sso{input_sso};
        insert_pos_literal_substr_sso.insert(0, get_sso_input<CharType>(), 1);
        assert(verify_string(insert_pos_literal_substr_sso));

        str insert_pos_literal_substr_growing{input_sso_growing};
        insert_pos_literal_substr_growing.insert(0, get_sso_input<CharType>(), 1);
        assert(verify_string(insert_pos_literal_substr_growing));

        str insert_pos_count_char{input};
        insert_pos_count_char.insert(0, 2, CharType{'a'});
        assert(verify_string(insert_pos_count_char));

        str insert_pos_count_char_sso{input_sso};
        insert_pos_count_char_sso.insert(0, 2, CharType{'a'});
        assert(verify_string(insert_pos_count_char_sso));

        str insert_pos_count_char_growing{input_sso_growing};
        insert_pos_count_char_growing.insert(0, 2, CharType{'a'});
        assert(verify_string(insert_pos_count_char_growing));
    }
}

template <class Alloc>
void test_removal() {
    using CharType = typename Alloc::value_type;
    using str      = basic_string<CharType, char_traits<CharType>, Alloc>;

    constexpr size_t large_size     = 20;
    constexpr size_t sso_size       = 2;
    constexpr size_t min_large_size = (16 / sizeof(CharType) < 1 ? 1 : 16 / sizeof(CharType));

    const str input(large_size, CharType{'b'});
    const str input_sso(sso_size, CharType{'b'});
    const str input_min_large(min_large_size, CharType{'b'});

    { // clear
        str cleared{input};
        cleared.clear();
        assert(verify_string(cleared));

        str cleared_sso{input_sso};
        cleared_sso.clear();
        assert(verify_string(cleared_sso));
    }

    { // erase
        str erase_pos_count{input};
        erase_pos_count.erase(0, 2);
        assert(verify_string(erase_pos_count));

        str erase_pos_count_sso{input_sso};
        erase_pos_count_sso.erase(0, 2);
        assert(verify_string(erase_pos_count_sso));

        str erase_pos_count_shrinking{input_min_large};
        erase_pos_count_shrinking.erase(0, 2);
        assert(verify_string(erase_pos_count_shrinking));

        str erase_iter{input};
        erase_iter.erase(erase_iter.begin());
        assert(verify_string(erase_iter));

        str erase_iter_sso{input_sso};
        erase_iter_sso.erase(erase_iter_sso.begin());
        assert(verify_string(erase_iter_sso));

        str erase_iter_shrinking{input_min_large};
        erase_iter_shrinking.erase(erase_iter_shrinking.begin());
        assert(verify_string(erase_iter_shrinking));

        str erase_const_iter{input};
        erase_const_iter.erase(erase_const_iter.cbegin());
        assert(verify_string(erase_const_iter));

        str erase_const_iter_sso{input_sso};
        erase_const_iter_sso.erase(erase_const_iter_sso.cbegin());
        assert(verify_string(erase_const_iter_sso));

        str erase_const_iter_shrinking{input_min_large};
        erase_const_iter_shrinking.erase(erase_const_iter_shrinking.cbegin());
        assert(verify_string(erase_const_iter_shrinking));

        str erase_iter_iter{input};
        erase_iter_iter.erase(erase_iter_iter.begin(), erase_iter_iter.begin() + 1);
        assert(verify_string(erase_iter_iter));

        str erase_iter_iter_sso{input_sso};
        erase_iter_iter_sso.erase(erase_iter_iter_sso.begin(), erase_iter_iter_sso.begin() + 1);
        assert(verify_string(erase_iter_iter_sso));

        str erase_iter_iter_shrinking{input_min_large};
        erase_iter_iter_shrinking.erase(erase_iter_iter_shrinking.begin(), erase_iter_iter_shrinking.begin() + 1);
        assert(verify_string(erase_iter_iter_shrinking));

        str erase_const_iter_iter{input};
        erase_const_iter_iter.erase(erase_const_iter_iter.begin(), erase_const_iter_iter.begin() + 1);
        assert(verify_string(erase_const_iter_iter));

        str erase_const_iter_iter_sso{input_sso};
        erase_const_iter_iter_sso.erase(erase_const_iter_iter_sso.begin(), erase_const_iter_iter_sso.begin() + 1);
        assert(verify_string(erase_const_iter_iter_sso));

        str erase_const_iter_iter_shrinking{input_min_large};
        erase_const_iter_iter_shrinking.erase(
            erase_const_iter_iter_shrinking.begin(), erase_const_iter_iter_shrinking.begin() + 1);
        assert(verify_string(erase_const_iter_iter_shrinking));

#if _HAS_CXX20
        str erased_free{get_large_input<CharType>()};
        erase(erased_free, CharType{'l'});
        assert(verify_string(erased_free));

        str erased_free_sso{get_sso_input<CharType>()};
        erase(erased_free_sso, CharType{'l'});
        assert(verify_string(erased_free_sso));

        str erased_free_if{get_large_input<CharType>()};
        erase_if(erased_free_if, [](const CharType val) { return val == CharType{'t'}; });
        assert(verify_string(erased_free_if));

        str erased_free_if_sso{get_sso_input<CharType>()};
        erase_if(erased_free_if_sso, [](const CharType val) { return val == CharType{'t'}; });
        assert(verify_string(erased_free_if_sso));
#endif // _HAS_CXX20
    }

    { //  pop_back
        str pop_back{input};
        pop_back.pop_back();
        assert(verify_string(pop_back));

        str pop_back_sso{input_sso};
        pop_back_sso.pop_back();
        assert(verify_string(pop_back_sso));

        str pop_back_shrinking{input_min_large};
        pop_back_shrinking.pop_back();
        assert(verify_string(pop_back_shrinking));
    }

    { //  shrink_to_fit
        str shrink_to_fit(32, CharType{'a'});
        shrink_to_fit.resize(min_large_size);
        shrink_to_fit.shrink_to_fit();
        assert(verify_string(shrink_to_fit));

        str shrink_to_fit_sso{input_sso};
        shrink_to_fit_sso.pop_back();
        shrink_to_fit_sso.shrink_to_fit();
        assert(verify_string(shrink_to_fit_sso));

        str shrink_to_fit_shrinking{input_min_large};
        shrink_to_fit_shrinking.pop_back();
        shrink_to_fit_shrinking.shrink_to_fit();
        assert(verify_string(shrink_to_fit_shrinking));
    }
}

template <class Alloc>
void test_misc() {
    using CharType = typename Alloc::value_type;
    using str      = basic_string<CharType, char_traits<CharType>, Alloc>;

    constexpr size_t large_size     = 20;
    constexpr size_t sso_size       = 2;
    constexpr size_t min_large_size = (16 / sizeof(CharType) < 1 ? 1 : 16 / sizeof(CharType));

    const str input(large_size, CharType{'b'});
    const str input_sso(sso_size, CharType{'b'});
    const str input_min_large(min_large_size, CharType{'b'});

    { // substr
        str substr_large_to_large = input.substr(2);
        assert(verify_string(substr_large_to_large));

        str substr_large_to_sso = input.substr(18);
        assert(verify_string(substr_large_to_sso));

        str substr_sso = input_sso.substr(1);
        assert(verify_string(substr_sso));

        str substr_count_large_to_large = input.substr(2, 18);
        assert(verify_string(substr_count_large_to_large));

        str substr_count_large_to_sso = input.substr(2, 2);
        assert(verify_string(substr_large_to_sso));

        str substr_count_sso = input_sso.substr(1, 2);
        assert(verify_string(substr_sso));
    }

    { // resize
        str resize_large_to_large{input};
        resize_large_to_large.resize(35);
        assert(verify_string(resize_large_to_large));

        str resize_large_to_sso{input};
        resize_large_to_sso.resize(3);
        assert(verify_string(resize_large_to_sso));

        str resize_sso_to_large{input_sso};
        resize_sso_to_large.resize(35);
        assert(verify_string(resize_sso_to_large));

        str resize_sso_to_sso{input_sso};
        resize_sso_to_sso.resize(3);
        assert(verify_string(resize_sso_to_sso));

        str resize_char_large_to_large{input};
        resize_char_large_to_large.resize(35, CharType{'c'});
        assert(verify_string(resize_char_large_to_large));

        str resize_char_large_to_sso{input};
        resize_char_large_to_sso.resize(3, CharType{'c'});
        assert(verify_string(resize_char_large_to_sso));

        str resize_char_sso_to_large{input_sso};
        resize_char_sso_to_large.resize(35, CharType{'c'});
        assert(verify_string(resize_char_sso_to_large));

        str resize_char_sso_to_sso{input_sso};
        resize_char_sso_to_sso.resize(3, CharType{'c'});
        assert(verify_string(resize_char_sso_to_sso));
    }

    if constexpr (allocator_traits<Alloc>::propagate_on_container_swap::value) { // swap
        str first_large{input};
        str second_large = input + str{CharType{'c'}, CharType{'a'}, CharType{'t'}};

        str first_sso{input_sso};
        str second_sso = input_sso + CharType{'s'};

        first_large.swap(second_large);
        assert(verify_string(first_large));
        assert(verify_string(second_large));

        first_sso.swap(second_sso);
        assert(verify_string(first_sso));
        assert(verify_string(second_sso));

        first_large.swap(first_sso);
        assert(verify_string(first_large));
        assert(verify_string(first_sso));

        second_sso.swap(second_large);
        assert(verify_string(second_sso));
        assert(verify_string(second_large));
    }
}

template <class Alloc>
void test_sstream() {
    using CharType = typename Alloc::value_type;
    using str      = basic_string<CharType, char_traits<CharType>, Alloc>;
    using stream   = basic_stringbuf<CharType, char_traits<CharType>, Alloc>;

    constexpr size_t large_size = 20;
    constexpr size_t sso_size   = 2;

    const str input(large_size, CharType{'b'});
    const str input_sso(sso_size, CharType{'b'});

    { // RValue construction
        stream constructed_from_empty{str{}};
        str buffered_empty = move(constructed_from_empty).str();
        assert(verify_string(buffered_empty));
        assert(buffered_empty.empty());

        str to_be_moved = input;
        stream constructed_from_large{move(to_be_moved)};
        str buffered = move(constructed_from_large).str();
        assert(verify_string(to_be_moved));
        assert(verify_string(buffered));

        str to_be_moved_sso = input_sso;
        stream constructed_from_sso{move(to_be_moved_sso)};
        str buffered_sso = move(constructed_from_sso).str();
        assert(verify_string(to_be_moved_sso));
        assert(verify_string(buffered_sso));
    }

    { // RValue str
        str to_be_moved = input;
        stream str_large;
        str_large.str(move(to_be_moved));
        str buffered = move(str_large).str();
        assert(verify_string(to_be_moved));
        assert(verify_string(buffered));

        str to_be_moved_sso = input_sso;
        stream str_sso;
        str_sso.str(move(to_be_moved_sso));
        str buffered_sso = move(str_sso).str();
        assert(verify_string(to_be_moved_sso));
        assert(verify_string(buffered_sso));
    }
}

template <class Alloc>
void test_exceptions() {
    using CharType = typename Alloc::value_type;
    using str      = basic_string<CharType, char_traits<CharType>, Alloc>;

    constexpr size_t large_size = 20;
    constexpr size_t sso_size   = 2;

    const str input(large_size, CharType{'b'});
    const str input_sso(sso_size, CharType{'b'});

    throw_on_conversion<CharType> iter_data[1] = {CharType{'b'}};
    input_iterator_tester<throw_on_conversion<CharType>, 1> input_iter_data;

    { // append
        str append_iterator{input};
        try {
            assert(verify_string(append_iterator));
            append_iterator.append(begin(iter_data), end(iter_data));
            assert(false);
        } catch (...) {
            assert(verify_string(append_iterator));
        }

#if 0 // TRANSITION, DevCom-1527920
        str append_iterator_sso{input_sso};
        try {
            assert(verify_string(append_iterator_sso));
            append_iterator_sso.append(begin(iter_data), end(iter_data));
            assert(false);
        } catch (...) {
            assert(verify_string(append_iterator_sso));
        }
#endif

        str append_input_iterator{input};
        try {
            assert(verify_string(append_input_iterator));
            append_input_iterator.append(input_iter_data.begin(), input_iter_data.end());
            assert(false);
        } catch (...) {
            assert(verify_string(append_input_iterator));
        }

#if 0 // TRANSITION, DevCom-1527920
        str append_input_iterator_sso{input_sso};
        try {
            append_input_iterator_sso.append(input_iter_data.begin(), input_iter_data.end());
        } catch (...) {
            assert(verify_string(append_input_iterator_sso));
        }
#endif
    }

    { // assign
        str assign_iterator{input};
        try {
            assert(verify_string(assign_iterator));
            assign_iterator.assign(begin(iter_data), end(iter_data));
            assert(false);
        } catch (...) {
            assert(verify_string(assign_iterator));
        }

#if 0 // TRANSITION, DevCom-1527920
        str assign_iterator_sso{input_sso};
        try {
            assert(verify_string(assign_iterator_sso));
            assign_iterator_sso.assign(begin(iter_data), end(iter_data));
        } catch (...) {
            assert(verify_string(assign_iterator_sso));
        }
#endif

        str assign_input_iterator{input};
        try {
            assert(verify_string(assign_input_iterator));
            assign_input_iterator.assign(input_iter_data.begin(), input_iter_data.end());
            assert(false);
        } catch (...) {
            assert(verify_string(assign_input_iterator));
        }

#if 0 // TRANSITION, DevCom-1527920
        str assign_input_iterator_sso{input_sso};
        try {
            assert(verify_string(assign_input_iterator_sso));
            assign_input_iterator_sso.assign(input_iter_data.begin(), input_iter_data.end());
            assert(false);
        } catch (...) {
            assert(verify_string(assign_input_iterator_sso));
        }
#endif
    }

    { // insert
        str insert_iterator{input};
        try {
            assert(verify_string(insert_iterator));
            insert_iterator.insert(insert_iterator.begin(), begin(iter_data), end(iter_data));
            assert(false);
        } catch (...) {
            assert(verify_string(insert_iterator));
        }

#if 0 // TRANSITION, DevCom-1527920
        str insert_iterator_sso{input_sso};
        try {
            assert(verify_string(insert_iterator_sso));
            insert_iterator_sso.insert(insert_iterator_sso.begin(), begin(iter_data), end(iter_data));
            assert(false);
        } catch (...) {
            assert(verify_string(insert_iterator_sso));
        }
#endif

        str insert_input_iterator{input};
        try {
            assert(verify_string(insert_input_iterator));
            insert_input_iterator.insert(insert_input_iterator.begin(), input_iter_data.begin(), input_iter_data.end());
            assert(false);
        } catch (...) {
            assert(verify_string(insert_input_iterator));
        }

#if 0 // TRANSITION, DevCom-1527920
        str insert_input_iterator_sso{input_sso};
        try {
            assert(verify_string(insert_input_iterator_sso));
            insert_input_iterator_sso.insert(
                insert_input_iterator_sso.begin(), input_iter_data.begin(), input_iter_data.end());
            assert(false);
        } catch (...) {
            assert(verify_string(insert_input_iterator_sso));
        }
#endif
    }
}

template <class Alloc>
void run_tests() {
    test_construction<Alloc>();
    test_append<Alloc>();
    test_assign<Alloc>();
    test_insertion<Alloc>();
    test_removal<Alloc>();
    test_misc<Alloc>();
    test_sstream<Alloc>();
#if !(defined(__clang__) && defined(_M_IX86)) // TRANSITION, LLVM-54804
    test_exceptions<Alloc>();
#endif // !(defined(__clang__) && defined(_M_IX86))
}

template <class CharType, template <class, class, class> class Alloc>
void run_custom_allocator_matrix() {
    run_tests<Alloc<CharType, true_type, true_type>>();
    run_tests<Alloc<CharType, true_type, false_type>>();
    run_tests<Alloc<CharType, false_type, true_type>>();
    run_tests<Alloc<CharType, false_type, false_type>>();
}

template <class CharType>
void run_allocator_matrix() {
    run_tests<allocator<CharType>>();
    run_custom_allocator_matrix<CharType, aligned_allocator>();
    run_custom_allocator_matrix<CharType, explicit_allocator>();
    run_custom_allocator_matrix<CharType, implicit_allocator>();
}

void test_DevCom_10116361() {
    // We failed to null-terminate copies of SSO strings with ASAN annotations active.
#ifdef _WIN64
    constexpr const char* text = "testtest";
    constexpr size_t n         = 8;
#else
    constexpr const char* text = "test";
    constexpr size_t n         = 4;
#endif

    string s0{text};
    assert(s0.c_str()[n] == '\0');

    alignas(string) unsigned char space[sizeof(string)];
    memset(space, 0xff, sizeof(space));

    string& s1 = *::new (&space) string{s0};
    assert(s1.c_str()[n] == '\0');

    s1.~string();
}

int main() {
    run_allocator_matrix<char>();
#ifdef __cpp_char8_t
    run_allocator_matrix<char8_t>();
#endif // __cpp_char8_t
    run_allocator_matrix<char16_t>();
    run_allocator_matrix<char32_t>();
    run_allocator_matrix<wchar_t>();

    test_DevCom_10116361();
}
#endif // TRANSITION, VSO-1586016

int main() {}
