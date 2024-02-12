// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <format>
#include <locale>
#include <memory>
#include <memory_resource>
#include <string>
#include <type_traits>
#include <vector>

#include <test_format_support.hpp>

using namespace std;

#define STR(Str) TYPED_LITERAL(CharT, Str)

template <class T>
struct alternative_allocator {
    using value_type = T;

    alternative_allocator() = default;
    template <class U>
    constexpr alternative_allocator(const alternative_allocator<U>&) noexcept {}

    T* allocate(size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) {
        allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    bool operator==(const alternative_allocator<U>&) const noexcept {
        return true;
    }
};

#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
template <class CharT>
struct yes_no_punct : numpunct<CharT> {
    basic_string<CharT> do_truename() const override {
        return STR("yes");
    }

    basic_string<CharT> do_falsename() const override {
        return STR("no");
    }
};
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING

template <class CharT, class Alloc>
void test_formatting_vector_bool_reference() {
    vector<bool, Alloc> vb{false, true};

    assert(format(STR("{}, {}"), vb[0], vb[1]) == format(STR("{}, {}"), false, true));
    assert(format(STR("{:}, {:}"), vb[0], vb[1]) == format(STR("{:}, {:}"), false, true));
    assert(format(STR("{:d}, {:x}"), vb[0], vb[1]) == format(STR("{:d}, {:x}"), false, true));

#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    locale loc{locale::classic(), new yes_no_punct<CharT>};

    assert(format(loc, STR("{:L}"), vb[1]) == STR("yes"));
    assert(format(loc, STR("{:L}"), vb[0]) == STR("no"));
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
}

int main() {
    test_formatting_vector_bool_reference<char, allocator<bool>>();
    test_formatting_vector_bool_reference<char, pmr::polymorphic_allocator<bool>>();
    test_formatting_vector_bool_reference<char, alternative_allocator<bool>>();

    test_formatting_vector_bool_reference<wchar_t, allocator<bool>>();
    test_formatting_vector_bool_reference<wchar_t, pmr::polymorphic_allocator<bool>>();
    test_formatting_vector_bool_reference<wchar_t, alternative_allocator<bool>>();
}
