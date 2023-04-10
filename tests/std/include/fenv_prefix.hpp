// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifdef FP_CONFIG_PRESET
#if FP_CONFIG_PRESET == 3
#define FP_PRESET_FAST 1
#else // ^^^ FP_CONFIG_PRESET == 3 / FP_CONFIG_PRESET != 3 vvv
#define FP_PRESET_FAST 0
#endif // ^^^ FP_CONFIG_PRESET != 3 ^^^
#endif // defined(FP_CONFIG_PRESET)

#ifdef FP_CONTRACT_MODE
#ifdef __clang__

#if FP_CONTRACT_MODE == 0
#pragma STDC FP_CONTRACT OFF
#elif FP_CONTRACT_MODE == 1 // ^^^ no floating point contraction / standard floating point contraction vvv
#pragma STDC FP_CONTRACT ON
#elif FP_CONTRACT_MODE == 2 // ^^^ standard floating point contraction / fast floating point contraction vvv
#pragma STDC FP_CONTRACT ON
#else // ^^^ fast floating point contraction / invalid FP_CONTRACT_MODE vvv
#error invalid FP_CONTRACT_MODE
#endif // ^^^ invalid FP_CONTRACT_MODE ^^^

#else // ^^^ clang / MSVC vvv

#if FP_CONTRACT_MODE == 0
#pragma fp_contract(off)
#elif FP_CONTRACT_MODE == 1 // ^^^ no floating point contraction / standard floating point contraction vvv
#pragma fp_contract(on)
#elif FP_CONTRACT_MODE == 2 // ^^^ standard floating point contraction / fast floating point contraction vvv
#pragma fp_contract(on)
#else // ^^^ fast floating point contraction / invalid FP_CONTRACT_MODE vvv
#error invalid FP_CONTRACT_MODE
#endif // ^^^ invalid FP_CONTRACT_MODE ^^^

#endif // ^^^ MSVC ^^^
#endif // defined(FP_CONTRACT_MODE)

#include <cassert>
#include <cfloat>

struct fenv_initializer_t {
    fenv_initializer_t() {
#if WITH_FP_ABRUPT_UNDERFLOW
        {
            const errno_t result = _controlfp_s(nullptr, _DN_FLUSH, _MCW_DN);
            assert(result == 0);
        }
#endif // WITH_FP_ABRUPT_UNDERFLOW
    }

    ~fenv_initializer_t() = default;

    fenv_initializer_t(const fenv_initializer_t&)            = delete;
    fenv_initializer_t& operator=(const fenv_initializer_t&) = delete;
};

const fenv_initializer_t fenv_initializer{};
