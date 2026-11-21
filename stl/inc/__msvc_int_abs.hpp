// __msvc_int_abs.hpp internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Under /Zc:cmath, <__msvc_math.hpp> and <__msvc_stdlib.hpp> replace <math.h> and <stdlib.h>, and both need these.
// N5032 [support.c.headers.other]/1 requires <math.h> to provide the abs() overloads that <cmath> declares,
// and the UCRT's <math.h> also declares labs() and llabs().
// Like <__msvc_stdlib.hpp>, this includes only UCRT headers, so <stdlib.h> doesn't start including <yvals_core.h>.

#ifndef __MSVC_INT_ABS_HPP
#define __MSVC_INT_ABS_HPP

#include <corecrt.h>

#pragma warning(push)
#pragma warning(disable : _UCRT_DISABLED_WARNINGS)
_UCRT_DISABLE_CLANG_WARNINGS

#if _HAS_CXX23 && defined(_MSVC_LIBC_MATH)
#define _CONSTEXPR_INT_ABS23 constexpr
#else // ^^^ constexpr in C++23 and later / inline when /Zc:cmath- opts out of constexpr, and in C++20 and earlier vvv
#define _CONSTEXPR_INT_ABS23 inline
#endif // ^^^ inline when /Zc:cmath- opts out of constexpr, and in C++20 and earlier ^^^

extern "C++" {
inline namespace _Msvc_stdlib {
    [[nodiscard]] _Check_return_ _CONSTEXPR_INT_ABS23 int __cdecl abs(_In_ int _Xx) noexcept /* strengthened */ {
        return _Xx >= 0 ? _Xx : -_Xx;
    }

    [[nodiscard]] _Check_return_ _CONSTEXPR_INT_ABS23 long __cdecl labs(_In_ long _Xx) noexcept /* strengthened */ {
        return _Xx >= 0 ? _Xx : -_Xx;
    }

    [[nodiscard]] _Check_return_ _CONSTEXPR_INT_ABS23 long long __cdecl llabs(_In_ long long _Xx) noexcept
    /* strengthened */ {
        return _Xx >= 0 ? _Xx : -_Xx;
    }

    [[nodiscard]] _CONSTEXPR_INT_ABS23 long abs(long const _Xx) noexcept /* strengthened */ {
        return _Xx >= 0 ? _Xx : -_Xx;
    }

    [[nodiscard]] _CONSTEXPR_INT_ABS23 long long abs(long long const _Xx) noexcept /* strengthened */ {
        return _Xx >= 0 ? _Xx : -_Xx;
    }
} // namespace _Msvc_stdlib
} // extern "C++"

#undef _CONSTEXPR_INT_ABS23

_UCRT_RESTORE_CLANG_WARNINGS
#pragma warning(pop) // _UCRT_DISABLED_WARNINGS
#endif // __MSVC_INT_ABS_HPP
