// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CORE_HEADERS_ONLY
#define _ENFORCE_ONLY_CORE_HEADERS // Also test GH-311 "Make it easy to limit the STL to core features"

#include <__msvc_all_public_headers.hpp>

// Also test GH-3103 "<xatomic.h>: Investigate making this a core header" and other internal core headers
#include <__msvc_int128.hpp>
#include <__msvc_system_error_abi.hpp>
#include <__msvc_xlocinfo_types.hpp>
#include <xatomic.h>
#include <xbit_ops.h>
#include <xerrc.h>

#if _HAS_CXX17
#include <xcharconv_tables.h>
#include <xfilesystem_abi.h>
#endif // _HAS_CXX17

#if _HAS_CXX23
#include <__msvc_print.hpp>
#endif // _HAS_CXX23

// <__msvc_bit_utils.hpp> is included by <bit> and <limits>
// <__msvc_iter_core.hpp> is included by <tuple>
// <xkeycheck.h> should not be included outside of <yvals_core.h>
// <xtr1common> is included by <cstddef>
// <yvals_core.h> is included by every public core header

// Also test GH-3692 "Including <isa_availability.h> emits a non-reserved name"
#include <isa_availability.h>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(std::_Stl_isa_available_sse42 == __ISA_AVAILABLE_SSE42);
STATIC_ASSERT(std::_Stl_isa_available_avx2 == __ISA_AVAILABLE_AVX2);

#ifdef _YVALS
#error Core headers should not include <yvals.h>.
#endif

void test_gh_2699() {
    // GH-2699 Vectorized swap emits error LNK2019:
    // unresolved external symbol ___std_swap_ranges_trivially_swappable_noalias
    int cats[]{10, 20, 30};
    int dogs[]{40, 50, 60};

    std::swap(cats, dogs);
    assert(cats[1] == 50);
    assert(dogs[1] == 20);

#ifdef __cpp_lib_ranges
    std::ranges::swap(cats, dogs);
    assert(cats[1] == 20);
    assert(dogs[1] == 50);
#endif // __cpp_lib_ranges
}

int main() {
    test_gh_2699();
}
