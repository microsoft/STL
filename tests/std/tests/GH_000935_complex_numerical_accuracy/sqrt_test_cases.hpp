// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <complex>

#include "floating_point_utils.hpp"
#include "test.hpp"

constexpr complex_unary_test_case<double> sqrt_double_cases[] = {
    // normal cases
    {{+0x3p-0, +0x4p-0}, {+0x2p-0, +0x1p-0}},
    {{+0x3p-0, -0x4p-0}, {+0x2p-0, -0x1p-0}},
    {{-0x3p-0, +0x4p-0}, {+0x1p-0, +0x2p-0}},
    {{-0x3p-0, -0x4p-0}, {+0x1p-0, -0x2p-0}},
    {{+0x3p-2, +0x4p-2}, {+0x2p-1, +0x1p-1}},
    {{+0x3p-2, -0x4p-2}, {+0x2p-1, -0x1p-1}},
    {{-0x3p-2, +0x4p-2}, {+0x1p-1, +0x2p-1}},
    {{-0x3p-2, -0x4p-2}, {+0x1p-1, -0x2p-1}},
    {{+0x3p-4, +0x4p-4}, {+0x2p-2, +0x1p-2}},
    {{+0x3p-4, -0x4p-4}, {+0x2p-2, -0x1p-2}},
    {{-0x3p-4, +0x4p-4}, {+0x1p-2, +0x2p-2}},
    {{-0x3p-4, -0x4p-4}, {+0x1p-2, -0x2p-2}},

    // special cases
    {{+0.0, +0.0}, {+0.0, +0.0}, {true, true}},
    {{+0.0, -0.0}, {+0.0, -0.0}, {true, true}},
    {{-0.0, +0.0}, {+0.0, +0.0}, {true, true}},
    {{-0.0, -0.0}, {+0.0, -0.0}, {true, true}},
    {{+1.0, +0.0}, {+1.0, +0.0}, {false, true}},
    {{+1.0, -0.0}, {+1.0, -0.0}, {false, true}},
    {{-1.0, +0.0}, {+0.0, +1.0}, {true, false}},
    {{-1.0, -0.0}, {+0.0, -1.0}, {true, false}},
    {{+0.0, +1.0}, {+0x1.6a09e667f3bcdp-1, +0x1.6a09e667f3bcdp-1}},
    {{+0.0, -1.0}, {+0x1.6a09e667f3bcdp-1, -0x1.6a09e667f3bcdp-1}},
    {{-0.0, +1.0}, {+0x1.6a09e667f3bcdp-1, +0x1.6a09e667f3bcdp-1}},
    {{-0.0, -1.0}, {+0x1.6a09e667f3bcdp-1, -0x1.6a09e667f3bcdp-1}},

#if !FP_PRESET_FAST
    {{+double_inf, +0.0}, {+double_inf, +0.0}, {true, true}},
    {{+double_inf, -0.0}, {+double_inf, -0.0}, {true, true}},
    {{-double_inf, +0.0}, {+0.0, +double_inf}, {true, true}},
    {{-double_inf, -0.0}, {+0.0, -double_inf}, {true, true}},
    {{+double_inf, +1.0}, {+double_inf, +0.0}, {true, true}},
    {{+double_inf, -1.0}, {+double_inf, -0.0}, {true, true}},
    {{-double_inf, +1.0}, {+0.0, +double_inf}, {true, true}},
    {{-double_inf, -1.0}, {+0.0, -double_inf}, {true, true}},
    {{+double_inf, +double_inf}, {+double_inf, +double_inf}, {true, true}},
    {{+double_inf, -double_inf}, {+double_inf, -double_inf}, {true, true}},
    {{-double_inf, +double_inf}, {+double_inf, +double_inf}, {true, true}},
    {{-double_inf, -double_inf}, {+double_inf, -double_inf}, {true, true}},
    {{+1.0, +double_inf}, {+double_inf, +double_inf}, {true, true}},
    {{+1.0, -double_inf}, {+double_inf, -double_inf}, {true, true}},
    {{-1.0, +double_inf}, {+double_inf, +double_inf}, {true, true}},
    {{-1.0, -double_inf}, {+double_inf, -double_inf}, {true, true}},
    {{+0.0, +double_inf}, {+double_inf, +double_inf}, {true, true}},
    {{+0.0, -double_inf}, {+double_inf, -double_inf}, {true, true}},
    {{-0.0, +double_inf}, {+double_inf, +double_inf}, {true, true}},
    {{-0.0, -double_inf}, {+double_inf, -double_inf}, {true, true}},
    {{+double_inf, +double_nan}, {+double_inf, +double_nan}, {true, true}},
    {{+double_inf, -double_nan}, {+double_inf, -double_nan}, {true, true}},
    {{-double_inf, +double_nan}, {+double_nan, +double_inf}, {true, true}},
    {{-double_inf, -double_nan}, {+double_nan, -double_inf}, {true, true}},
    {{+double_nan, +double_inf}, {+double_inf, +double_inf}, {true, true}},
    {{+double_nan, -double_inf}, {+double_inf, -double_inf}, {true, true}},
    {{+double_nan, +0.0}, {+double_nan, +double_nan}, {true, true}},
    {{+double_nan, -0.0}, {+double_nan, -double_nan}, {true, true}},
    {{+0.0, +double_nan}, {+double_nan, +double_nan}, {true, true}},
    {{+0.0, -double_nan}, {+double_nan, -double_nan}, {true, true}},
    {{+double_nan, +double_nan}, {+double_nan, +double_nan}, {true, true}},
    {{+double_nan, -double_nan}, {+double_nan, -double_nan}, {true, true}},
#endif // !FP_PRESET_FAST

    // abs(z) overflows
    {{+0x1.fffffffffffffp+1023, +0x1.fffffffffffffp+1023}, {+0x1.19435caffa9f8p+512, +0x1.d203138f6c828p+510}},
    {{-0x1.bb67ae8584caap+1023, +0x1.0000000000000p+1023}, {+0x1.0907dc1930691p+510, +0x1.ee8dd4748bf15p+511}},
    {{+0x1.fffffffffffffp+1023, -0x0.0000000000001p-1022}, {+0x1.fffffffffffffp+511, -0x0.0000000000000p-1022}},

    // norm(z) overflows
    {{-0x1.4e718d7d7625ap+664, -0x1.4e718d7d7625ap+665}, {+0x1.cc1033be914a7p+331, -0x1.7432f2f528ea0p+332}},
    {{+0x1.ca3d8e6d80cbbp+511, -0x1.57ae2ad22098cp+511}, {+0x1.00e0ed3ec75c3p+256, -0x1.56813c53b47afp+254}},

#if !WITH_FP_ABRUPT_UNDERFLOW
    // abs(z) underflows
    {{-0x0.0000000000001p-1022, +0x0.0000000000001p-1022}, {+0x1.d203138f6c828p-539, +0x1.19435caffa9f9p-537}},
    {{+0x0.0000000000001p-1022, +0x0.8000000000000p-1022}, {+0x1.0000000000001p-512, +0x1.ffffffffffffep-513}},
#endif // !WITH_FP_ABRUPT_UNDERFLOW

    // abs(z) close to underflow
    {{+0x1.4p-1022, +0x1p-1022}, {+0x1.31a33f3eb2fd9p-511, +0x1.acd8ff10ebe7ep-513}},

    // norm(z) underflows
    {{+0x1.87e92154ef7acp-664, -0x1.87e92154ef7acp-665}, {+0x1.45f5e3f782563p-332, -0x1.33cb9c4327c54p-334}},
    {{-0x1.9be34ac46b18fp-513, -0x1.1297872d9cbb5p-512}, {+0x1.09220ecd9c241p-257, -0x1.09220ecd9c241p-256}},

    // control flow edge cases
    {{+0x1p-2, +0x1.fffffffffffffp-1}, {+0x1.99b96593b936dp-1, +0x1.3fe72a921c6f4p-1}},
    {{+0x1p-2, +0x1.0000000000000p+0}, {+0x1.99b96593b936ep-1, +0x1.3fe72a921c6f4p-1}},
    {{+0x1p-2, +0x1.0000000000001p+0}, {+0x1.99b96593b936ep-1, +0x1.3fe72a921c6f5p-1}},
    {{+0x1p+0, +0x1p-54}, {+0x1.0000000000000p+0, +0x1.0000000000000p-55}},
    {{+0x1p+0, +0x1p-53}, {+0x1.0000000000000p+0, +0x1.0000000000000p-54}},
    {{+0x1p+0, +0x1p-52}, {+0x1.0000000000000p+0, +0x1.0000000000000p-53}},
    {{+0x1p+0, +0x1.ffffffffffffep-2}, {+0x1.077225f1da572p+0, +0x1.f18773c56f720p-3}},
    {{+0x1p+0, +0x1.fffffffffffffp-2}, {+0x1.077225f1da572p+0, +0x1.f18773c56f721p-3}},
    {{+0x1p+0, +0x1.0000000000000p-1}, {+0x1.077225f1da572p+0, +0x1.f18773c56f721p-3}},
    {{+0x1p+0, +0x1.0000000000001p-1}, {+0x1.077225f1da572p+0, +0x1.f18773c56f723p-3}},
    {{+0x1.ffffffffffffep-970, +0x1.fffffffffffffp-970}, {+0x1.8dc42193d5c02p-485, +0x1.49852f983efddp-486}},
    {{+0x1.fffffffffffffp-970, +0x1.0000000000000p-969}, {+0x1.8dc42193d5c02p-485, +0x1.49852f983efdep-486}},
    {{+0x1.0000000000000p-969, +0x1.0000000000001p-969}, {+0x1.8dc42193d5c03p-485, +0x1.49852f983efdep-486}},
    {{+0x1.ffffffffffffep-971, +0x1.fffffffffffffp-970}, {+0x1.45a3146a88455p-485, +0x1.92826ef258d1bp-486}},
    {{+0x1.fffffffffffffp-971, +0x1.0000000000000p-969}, {+0x1.45a3146a88456p-485, +0x1.92826ef258d1bp-486}},
    {{+0x1.0000000000000p-970, +0x1.0000000000001p-969}, {+0x1.45a3146a88456p-485, +0x1.92826ef258d1cp-486}},
    {{+0x1.fffffffffffffp-971, +0x1.fffffffffffffp-970}, {+0x1.45a3146a88456p-485, +0x1.92826ef258d1bp-486}},
    {{+0x1.0000000000000p-970, +0x1.0000000000000p-969}, {+0x1.45a3146a88456p-485, +0x1.92826ef258d1bp-486}},
    {{+0x1.0000000000001p-970, +0x1.0000000000001p-969}, {+0x1.45a3146a88457p-485, +0x1.92826ef258d1cp-486}},
    {{+0x1.0000000000000p-970, +0x1.fffffffffffffp-970}, {+0x1.45a3146a88456p-485, +0x1.92826ef258d1bp-486}},
    {{+0x1.0000000000001p-970, +0x1.0000000000000p-969}, {+0x1.45a3146a88456p-485, +0x1.92826ef258d1bp-486}},
    {{+0x1.0000000000002p-970, +0x1.0000000000001p-969}, {+0x1.45a3146a88457p-485, +0x1.92826ef258d1cp-486}},
    {{+0x1p-1022, +0x1.fffffffffffffp-970}, {+0x1.0000000000000p-485, +0x1.fffffffffffffp-486}},
    {{+0x1p-1022, +0x1.0000000000000p-969}, {+0x1.0000000000000p-485, +0x1.0000000000000p-485}},
    {{+0x1p-1022, +0x1.0000000000001p-969}, {+0x1.0000000000001p-485, +0x1.0000000000000p-485}},

#if !WITH_FP_ABRUPT_UNDERFLOW
    {{+0x0.0000000000001p-1022, +0x1.fffffffffffffp-970}, {+0x1.0000000000000p-485, +0x1.fffffffffffffp-486}},
    {{+0x0.0000000000001p-1022, +0x1.0000000000000p-969}, {+0x1.0000000000000p-485, +0x1.0000000000000p-485}},
    {{+0x0.0000000000001p-1022, +0x1.0000000000001p-969}, {+0x1.0000000000001p-485, +0x1.0000000000000p-485}},
#endif // !WITH_FP_ABRUPT_UNDERFLOW
};

constexpr complex_unary_test_case<float> sqrt_float_cases[] = {
    // normal cases
    {{+0x3p-0F, +0x4p-0F}, {+0x2p-0F, +0x1p-0F}},
    {{+0x3p-0F, -0x4p-0F}, {+0x2p-0F, -0x1p-0F}},
    {{-0x3p-0F, +0x4p-0F}, {+0x1p-0F, +0x2p-0F}},
    {{-0x3p-0F, -0x4p-0F}, {+0x1p-0F, -0x2p-0F}},
    {{+0x3p-2F, +0x4p-2F}, {+0x2p-1F, +0x1p-1F}},
    {{+0x3p-2F, -0x4p-2F}, {+0x2p-1F, -0x1p-1F}},
    {{-0x3p-2F, +0x4p-2F}, {+0x1p-1F, +0x2p-1F}},
    {{-0x3p-2F, -0x4p-2F}, {+0x1p-1F, -0x2p-1F}},
    {{+0x3p-4F, +0x4p-4F}, {+0x2p-2F, +0x1p-2F}},
    {{+0x3p-4F, -0x4p-4F}, {+0x2p-2F, -0x1p-2F}},
    {{-0x3p-4F, +0x4p-4F}, {+0x1p-2F, +0x2p-2F}},
    {{-0x3p-4F, -0x4p-4F}, {+0x1p-2F, -0x2p-2F}},

    // special cases
    {{+0.0F, +0.0F}, {+0.0F, +0.0F}, {true, true}},
    {{+0.0F, -0.0F}, {+0.0F, -0.0F}, {true, true}},
    {{-0.0F, +0.0F}, {+0.0F, +0.0F}, {true, true}},
    {{-0.0F, -0.0F}, {+0.0F, -0.0F}, {true, true}},
    {{+1.0F, +0.0F}, {+1.0F, +0.0F}, {false, true}},
    {{+1.0F, -0.0F}, {+1.0F, -0.0F}, {false, true}},
    {{-1.0F, +0.0F}, {+0.0F, +1.0F}, {true, false}},
    {{-1.0F, -0.0F}, {+0.0F, -1.0F}, {true, false}},
    {{+0.0F, +1.0F}, {+0x1.6a09e6p-1F, +0x1.6a09e6p-1F}},
    {{+0.0F, -1.0F}, {+0x1.6a09e6p-1F, -0x1.6a09e6p-1F}},
    {{-0.0F, +1.0F}, {+0x1.6a09e6p-1F, +0x1.6a09e6p-1F}},
    {{-0.0F, -1.0F}, {+0x1.6a09e6p-1F, -0x1.6a09e6p-1F}},

#if !FP_PRESET_FAST
    {{+float_inf, +0.0F}, {+float_inf, +0.0F}, {true, true}},
    {{+float_inf, -0.0F}, {+float_inf, -0.0F}, {true, true}},
    {{-float_inf, +0.0F}, {+0.0F, +float_inf}, {true, true}},
    {{-float_inf, -0.0F}, {+0.0F, -float_inf}, {true, true}},
    {{+float_inf, +1.0F}, {+float_inf, +0.0F}, {true, true}},
    {{+float_inf, -1.0F}, {+float_inf, -0.0F}, {true, true}},
    {{-float_inf, +1.0F}, {+0.0F, +float_inf}, {true, true}},
    {{-float_inf, -1.0F}, {+0.0F, -float_inf}, {true, true}},
    {{+float_inf, +float_inf}, {+float_inf, +float_inf}, {true, true}},
    {{+float_inf, -float_inf}, {+float_inf, -float_inf}, {true, true}},
    {{-float_inf, +float_inf}, {+float_inf, +float_inf}, {true, true}},
    {{-float_inf, -float_inf}, {+float_inf, -float_inf}, {true, true}},
    {{+1.0F, +float_inf}, {+float_inf, +float_inf}, {true, true}},
    {{+1.0F, -float_inf}, {+float_inf, -float_inf}, {true, true}},
    {{-1.0F, +float_inf}, {+float_inf, +float_inf}, {true, true}},
    {{-1.0F, -float_inf}, {+float_inf, -float_inf}, {true, true}},
    {{+0.0F, +float_inf}, {+float_inf, +float_inf}, {true, true}},
    {{+0.0F, -float_inf}, {+float_inf, -float_inf}, {true, true}},
    {{-0.0F, +float_inf}, {+float_inf, +float_inf}, {true, true}},
    {{-0.0F, -float_inf}, {+float_inf, -float_inf}, {true, true}},
    {{+float_inf, +float_nan}, {+float_inf, +float_nan}, {true, true}},
    {{+float_inf, -float_nan}, {+float_inf, -float_nan}, {true, true}},
    {{-float_inf, +float_nan}, {+float_nan, +float_inf}, {true, true}},
    {{-float_inf, -float_nan}, {+float_nan, -float_inf}, {true, true}},
    {{+float_nan, +float_inf}, {+float_inf, +float_inf}, {true, true}},
    {{+float_nan, -float_inf}, {+float_inf, -float_inf}, {true, true}},
    {{+float_nan, +0.0}, {+float_nan, +float_nan}, {true, true}},
    {{+float_nan, -0.0}, {+float_nan, -float_nan}, {true, true}},
    {{+0.0, +float_nan}, {+float_nan, +float_nan}, {true, true}},
    {{+0.0, -float_nan}, {+float_nan, -float_nan}, {true, true}},
    {{+float_nan, +float_nan}, {+float_nan, +float_nan}, {true, true}},
    {{+float_nan, -float_nan}, {+float_nan, -float_nan}, {true, true}},
#endif // !FP_PRESET_FAST

    // abs(z) overflows
    {{+0x1.fffffep+127F, +0x1.fffffep+127F}, {+0x1.19435cp+64F, +0x1.d20312p+62F}},
    {{-0x1.bb67aep+127F, +0x1.000000p+127F}, {+0x1.0907dcp+62F, +0x1.ee8dd4p+63F}},
    {{+0x1.fffffep+127F, -0x0.000002p-126F}, {+0x1.fffffep+63F, -0x0.000000p-126F}},

    // norm(z) overflows
    {{-0x1.08b2a2p+83F, -0x1.08b2a2p+84F}, {+0x1.216970p+41F, -0x1.d4473ap+41F}},
    {{+0x1.bc16d6p+63F, -0x1.4d1120p+63F}, {+0x1.f9c31ep+31F, -0x1.512cbep+30F}},

#if !WITH_FP_ABRUPT_UNDERFLOW
    // abs(z) underflows
    {{-0x0.000002p-126F, +0x0.000002p-126F}, {+0x1.498530p-76F, +0x1.8dc422p-75F}},
    {{+0x0.000002p-126F, +0x0.800000p-126F}, {+0x1.000002p-64F, +0x1.fffffcp-65F}},
#endif // !WITH_FP_ABRUPT_UNDERFLOW

    // abs(z) close to underflow
    {{+0x1.4p-126F, +0x1p-126F}, {+0x1.31a340p-63F, +0x1.acd900p-65F}},

    // norm(z) underflows
    {{+0x1.ef2d1p-83F, -0x1.ef2d10p-84F}, {+0x1.0314d8p-41F, -0x1.e9495ep-44F}},
    {{-0x1.622d7p-61F, -0x1.d83c94p-61F}, {+0x1.ebb76ep-32F, -0x1.ebb770p-31F}},

    // control flow edge cases
    {{+0x1p-2F, +0x1.fffffep-1F}, {+0x1.99b964p-1F, +0x1.3fe72ap-1F}},
    {{+0x1p-2F, +0x1.000000p+0F}, {+0x1.99b966p-1F, +0x1.3fe72ap-1F}},
    {{+0x1p-2F, +0x1.000002p+0F}, {+0x1.99b966p-1F, +0x1.3fe72cp-1F}},
    {{+0x1p+0F, +0x1p-25F}, {+0x1.000000p+0F, +0x1.000000p-26F}},
    {{+0x1p+0F, +0x1p-24F}, {+0x1.000000p+0F, +0x1.000000p-25F}},
    {{+0x1p+0F, +0x1p-23F}, {+0x1.000000p+0F, +0x1.000000p-24F}},
    {{+0x1p+0F, +0x1.fffffcp-2F}, {+0x1.077226p+0F, +0x1.f18770p-3F}},
    {{+0x1p+0F, +0x1.fffffep-2F}, {+0x1.077226p+0F, +0x1.f18772p-3F}},
    {{+0x1p+0F, +0x1.000000p-1F}, {+0x1.077226p+0F, +0x1.f18774p-3F}},
    {{+0x1p+0F, +0x1.000002p-1F}, {+0x1.077226p+0F, +0x1.f18778p-3F}},
    {{+0x1.fffffcp-103F, +0x1.fffffep-103F}, {+0x1.19435cp-51F, +0x1.d20314p-53F}},
    {{+0x1.fffffep-103F, +0x1.000000p-102F}, {+0x1.19435cp-51F, +0x1.d20314p-53F}},
    {{+0x1.000000p-102F, +0x1.000002p-102F}, {+0x1.19435ep-51F, +0x1.d20316p-53F}},
    {{+0x1.fffffcp-104F, +0x1.fffffep-103F}, {+0x1.cc8532p-52F, +0x1.1c9e00p-52F}},
    {{+0x1.fffffep-104F, +0x1.000000p-102F}, {+0x1.cc8532p-52F, +0x1.1c9e02p-52F}},
    {{+0x1.000000p-103F, +0x1.000002p-102F}, {+0x1.cc8534p-52F, +0x1.1c9e02p-52F}},
    {{+0x1.fffffep-104F, +0x1.fffffep-103F}, {+0x1.cc8532p-52F, +0x1.1c9e00p-52F}},
    {{+0x1.000000p-103F, +0x1.000000p-102F}, {+0x1.cc8532p-52F, +0x1.1c9e00p-52F}},
    {{+0x1.000002p-103F, +0x1.000002p-102F}, {+0x1.cc8534p-52F, +0x1.1c9e02p-52F}},
    {{+0x1.000000p-103F, +0x1.fffffep-103F}, {+0x1.cc8532p-52F, +0x1.1c9e00p-52F}},
    {{+0x1.000002p-103F, +0x1.000000p-102F}, {+0x1.cc8534p-52F, +0x1.1c9e00p-52F}},
    {{+0x1.000004p-103F, +0x1.000002p-102F}, {+0x1.cc8536p-52F, +0x1.1c9e02p-52F}},
    {{+0x1.000000p-126F, +0x1.fffffep-103F}, {+0x1.6a09e6p-52F, +0x1.6a09e4p-52F}},
    {{+0x1.000000p-126F, +0x1.000000p-102F}, {+0x1.6a09e8p-52F, +0x1.6a09e6p-52F}},
    {{+0x1.000000p-126F, +0x1.000002p-102F}, {+0x1.6a09e8p-52F, +0x1.6a09e8p-52F}},

#if !WITH_FP_ABRUPT_UNDERFLOW
    {{+0x0.000002p-126F, +0x1.fffffep-103F}, {+0x1.6a09e6p-52F, +0x1.6a09e6p-52F}},
    {{+0x0.000002p-126F, +0x1.000000p-102F}, {+0x1.6a09e6p-52F, +0x1.6a09e6p-52F}},
    {{+0x0.000002p-126F, +0x1.000002p-102F}, {+0x1.6a09e8p-52F, +0x1.6a09e8p-52F}},
#endif // !WITH_FP_ABRUPT_UNDERFLOW
};
