// __msvc_formatter.hpp internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// NOTE:
// The contents of this header are derived in part from libfmt under the following license:

// Copyright (c) 2012 - present, Victor Zverovich
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// --- Optional exception to the license ---
//
// As an exception, if, as a result of your compiling your source code, portions
// of this Software are embedded into a machine-executable object form of such
// source code, you may redistribute such embedded portions in such object form
// without including the above copyright and permission notices.

#ifndef __MSVC_FORMATTER_HPP
#define __MSVC_FORMATTER_HPP
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#if !_HAS_CXX20
#error The contents of <format> are only available with C++20. (Also, you should not include this internal header.)
#endif // !_HAS_CXX20

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
#if _HAS_CXX23
#define _FMT_P2286_BEGIN inline namespace __p2286 {
#define _FMT_P2286_END   }
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
#define _FMT_P2286_BEGIN
#define _FMT_P2286_END
#endif // ^^^ !_HAS_CXX23 ^^^

enum class _Fmt_align : uint8_t { _None, _Left, _Right, _Center };

enum class _Fmt_sign : uint8_t { _None, _Plus, _Minus, _Space };

enum class _Basic_format_arg_type : uint8_t {
    _None,
    _Int_type,
    _UInt_type,
    _Long_long_type,
    _ULong_long_type,
    _Bool_type,
    _Char_type,
    _Float_type,
    _Double_type,
    _Long_double_type,
    _Pointer_type,
    _CString_type,
    _String_type,
    _Custom_type,
};
static_assert(static_cast<int>(_Basic_format_arg_type::_Custom_type) < 16, "must fit in 4-bit bitfield");

#if _HAS_CXX23
_NODISCARD consteval bool _Is_debug_enabled_fmt_type(_Basic_format_arg_type _Ty) {
    return _Ty == _Basic_format_arg_type::_Char_type || _Ty == _Basic_format_arg_type::_CString_type
        || _Ty == _Basic_format_arg_type::_String_type;
}
#endif // _HAS_CXX23

template <class _CharT>
struct _Basic_format_specs {
    int _Width            = 0;
    int _Precision        = -1;
    char _Type            = '\0';
    _Fmt_align _Alignment = _Fmt_align::_None;
    _Fmt_sign _Sgn        = _Fmt_sign::_None;
    bool _Alt             = false;
    bool _Localized       = false;
    bool _Leading_zero    = false;
    uint8_t _Fill_length  = 1;
    // At most one codepoint (so one char32_t or four utf-8 char8_t).
    _CharT _Fill[4 / sizeof(_CharT)] = {_CharT{' '}};
};

// Adds width and precision references to _Basic_format_specs.
// This is required for std::formatter implementations because we must
// parse the format specs without having access to the format args (via a format context).
template <class _CharT>
struct _Dynamic_format_specs : _Basic_format_specs<_CharT> {
    int _Dynamic_width_index     = -1;
    int _Dynamic_precision_index = -1;
};

_EXPORT_STD template <class _CharT>
class basic_format_parse_context;

template <class _CharT>
concept _Format_supported_charT = _Is_any_of_v<_CharT, char, wchar_t>;

// Generic formatter definition, the deleted default constructor
// makes it "disabled" as per N4950 [format.formatter.spec]/5
_EXPORT_STD template <class _Ty, class _CharT = char>
struct formatter {
    formatter()                            = delete;
    formatter(const formatter&)            = delete;
    formatter& operator=(const formatter&) = delete;
};

_FMT_P2286_BEGIN
// TRANSITION, VSO-1236041: Avoid declaring and defining member functions in different headers.
template <_Basic_format_arg_type _ArgType, class _CharT, class _Pc>
constexpr _Pc::iterator _Formatter_base_parse(_Dynamic_format_specs<_CharT>& _Specs, _Pc& _ParseCtx);

template <class _Ty, class _CharT, class _FormatContext>
_FormatContext::iterator _Formatter_base_format(
    const _Dynamic_format_specs<_CharT>& _Specs, const _Ty& _Val, _FormatContext& _FormatCtx);

template <class _Ty, class _CharT, _Basic_format_arg_type _ArgType>
struct _Formatter_base {
public:
#if _HAS_CXX23
    constexpr void _Set_debug_format() noexcept
        requires (_Is_debug_enabled_fmt_type(_ArgType))
    {
        _Specs._Type = '?';
    }
#endif // _HAS_CXX23

    template <class _Pc = basic_format_parse_context<_CharT>>
    constexpr _Pc::iterator parse(type_identity_t<_Pc&> _ParseCtx) {
        return _Formatter_base_parse<_ArgType>(_Specs, _ParseCtx);
    }

    template <class _FormatContext>
    _FormatContext::iterator format(const _Ty& _Val, _FormatContext& _FormatCtx) const {
        return _Formatter_base_format(_Specs, _Val, _FormatCtx);
    }

private:
    _Dynamic_format_specs<_CharT> _Specs;
};
_FMT_P2286_END

#define _FORMAT_SPECIALIZE_FOR(_Type, _ArgType) \
    template <_Format_supported_charT _CharT>   \
    struct formatter<_Type, _CharT> : _Formatter_base<_Type, _CharT, _ArgType> {}

_FORMAT_SPECIALIZE_FOR(int, _Basic_format_arg_type::_Int_type);
_FORMAT_SPECIALIZE_FOR(unsigned int, _Basic_format_arg_type::_UInt_type);
_FORMAT_SPECIALIZE_FOR(long long, _Basic_format_arg_type::_Long_long_type);
_FORMAT_SPECIALIZE_FOR(unsigned long long, _Basic_format_arg_type::_ULong_long_type);
_FORMAT_SPECIALIZE_FOR(bool, _Basic_format_arg_type::_Bool_type);
_FORMAT_SPECIALIZE_FOR(float, _Basic_format_arg_type::_Float_type);
_FORMAT_SPECIALIZE_FOR(double, _Basic_format_arg_type::_Double_type);
_FORMAT_SPECIALIZE_FOR(long double, _Basic_format_arg_type::_Long_double_type);
_FORMAT_SPECIALIZE_FOR(nullptr_t, _Basic_format_arg_type::_Pointer_type);
_FORMAT_SPECIALIZE_FOR(void*, _Basic_format_arg_type::_Pointer_type);
_FORMAT_SPECIALIZE_FOR(const void*, _Basic_format_arg_type::_Pointer_type);
_FORMAT_SPECIALIZE_FOR(short, _Basic_format_arg_type::_Int_type);
_FORMAT_SPECIALIZE_FOR(unsigned short, _Basic_format_arg_type::_UInt_type);
_FORMAT_SPECIALIZE_FOR(long, _Basic_format_arg_type::_Int_type);
_FORMAT_SPECIALIZE_FOR(unsigned long, _Basic_format_arg_type::_UInt_type);
_FORMAT_SPECIALIZE_FOR(signed char, _Basic_format_arg_type::_Int_type);
_FORMAT_SPECIALIZE_FOR(unsigned char, _Basic_format_arg_type::_UInt_type);

#undef _FORMAT_SPECIALIZE_FOR

// not using the macro because we'd like to add 'set_debug_format' member function in C++23 mode
template <_Format_supported_charT _CharT>
struct formatter<char, _CharT> : _Formatter_base<char, _CharT, _Basic_format_arg_type::_Char_type> {
#if _HAS_CXX23
    constexpr void set_debug_format() noexcept {
        this->_Set_debug_format();
    }
#endif // _HAS_CXX23
};

// not using the macro because we'd like to avoid the formatter<wchar_t, char> specialization
template <>
struct formatter<wchar_t, wchar_t> : _Formatter_base<wchar_t, wchar_t, _Basic_format_arg_type::_Char_type> {
#if _HAS_CXX23
    constexpr void set_debug_format() noexcept {
        _Set_debug_format();
    }
#endif // _HAS_CXX23
};

// We could use the macro for these specializations, but it's confusing to refer to symbols that are defined
// inside the macro in the macro's "call".
template <_Format_supported_charT _CharT>
struct formatter<_CharT*, _CharT> : _Formatter_base<_CharT*, _CharT, _Basic_format_arg_type::_CString_type> {
#if _HAS_CXX23
    constexpr void set_debug_format() noexcept {
        this->_Set_debug_format();
    }
#endif // _HAS_CXX23
};

template <_Format_supported_charT _CharT>
struct formatter<const _CharT*, _CharT>
    : _Formatter_base<const _CharT*, _CharT, _Basic_format_arg_type::_CString_type> {
#if _HAS_CXX23
    constexpr void set_debug_format() noexcept {
        this->_Set_debug_format();
    }
#endif // _HAS_CXX23
};

template <_Format_supported_charT _CharT, size_t _Nx>
struct formatter<_CharT[_Nx], _CharT> : _Formatter_base<_CharT[_Nx], _CharT, _Basic_format_arg_type::_CString_type> {
#if _HAS_CXX23
    constexpr void set_debug_format() noexcept {
        this->_Set_debug_format();
    }
#endif // _HAS_CXX23
};

_EXPORT_STD template <class _Elem, class _Traits, class _Alloc>
class basic_string;

_EXPORT_STD template <class _Elem, class _Traits>
class basic_string_view;

template <_Format_supported_charT _CharT, class _Traits, class _Allocator>
struct formatter<basic_string<_CharT, _Traits, _Allocator>, _CharT>
    : _Formatter_base<basic_string<_CharT, _Traits, _Allocator>, _CharT, _Basic_format_arg_type::_String_type> {
#if _HAS_CXX23
    constexpr void set_debug_format() noexcept {
        this->_Set_debug_format();
    }
#endif // _HAS_CXX23
};

template <_Format_supported_charT _CharT, class _Traits>
struct formatter<basic_string_view<_CharT, _Traits>, _CharT>
    : _Formatter_base<basic_string_view<_CharT, _Traits>, _CharT, _Basic_format_arg_type::_String_type> {
#if _HAS_CXX23
    constexpr void set_debug_format() noexcept {
        this->_Set_debug_format();
    }
#endif // _HAS_CXX23
};

#if _HAS_CXX23
_EXPORT_STD template <class, class>
struct pair;

_EXPORT_STD template <class...>
class tuple;

// Specializations for pairs and tuples are forward-declared to avoid any risk of using the disabled primary template.

// Per LWG-3997, `_CharT` in library-provided `formatter` specializations is
// constrained to character types supported by `format`.

template <_Format_supported_charT _CharT, class _Ty1, class _Ty2>
struct formatter<pair<_Ty1, _Ty2>, _CharT>;

template <_Format_supported_charT _CharT, class... _Types>
struct formatter<tuple<_Types...>, _CharT>;
#endif // _HAS_CXX23
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_FORMATTER_HPP
