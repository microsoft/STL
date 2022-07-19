// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <complex>

#include "floating_point_utils.hpp"
#include "test.hpp"

template <typename T>
constexpr T pi_over_4_v = T{0.7853981633974483};
template <typename T>
constexpr T pi_over_2_v = T{1.5707963267948966};
template <typename T>
constexpr T pi_3_over_4_v = T{2.356194490192345};
template <typename T>
constexpr T pi_v = T{3.141592653589793};

constexpr complex_unary_test_case<double> log_double_cases[] = {
    // normal cases
    {{+0x1.8p+0, +0x1p+1}, {+0x1.d5240f0e0e078p-1, +0x1.dac670561bb4fp-1}},
    {{+0x1.8p+0, -0x1p+1}, {+0x1.d5240f0e0e078p-1, -0x1.dac670561bb4fp-1}},
    {{-0x1.8p+0, +0x1p+1}, {+0x1.d5240f0e0e078p-1, +0x1.1b6e192ebbe44p+1}},
    {{-0x1.8p+0, -0x1p+1}, {+0x1.d5240f0e0e078p-1, -0x1.1b6e192ebbe44p+1}},
    {{+0x1.8p-1, +0x1p+0}, {+0x1.c8ff7c79a9a22p-3, +0x1.dac670561bb4fp-1}},
    {{+0x1.8p-1, -0x1p+0}, {+0x1.c8ff7c79a9a22p-3, -0x1.dac670561bb4fp-1}},
    {{-0x1.8p-1, +0x1p+0}, {+0x1.c8ff7c79a9a22p-3, +0x1.1b6e192ebbe44p+1}},
    {{-0x1.8p-1, -0x1p+0}, {+0x1.c8ff7c79a9a22p-3, -0x1.1b6e192ebbe44p+1}},
    {{+0x1.8p-2, +0x1p-1}, {-0x1.e148a1a2726cep-2, +0x1.dac670561bb4fp-1}},
    {{+0x1.8p-2, -0x1p-1}, {-0x1.e148a1a2726cep-2, -0x1.dac670561bb4fp-1}},
    {{-0x1.8p-2, +0x1p-1}, {-0x1.e148a1a2726cep-2, +0x1.1b6e192ebbe44p+1}},
    {{-0x1.8p-2, -0x1p-1}, {-0x1.e148a1a2726cep-2, -0x1.1b6e192ebbe44p+1}},

    // DevCom-10088405: Incorrect result for std::complex operations on ARM64 platform
    {{0.1, 1.2}, {0.18578177821624148, 1.4876550949064553}},
    {{-1.1698230349239351, 0.46519593659281616}, {0.23025850929940467, 2.763102111592855}},

    // special cases
    {{+1.0, +0.0}, {0.0, +0.0}, {true, true}},
    {{+1.0, -0.0}, {0.0, -0.0}, {true, true}},
    {{+0.0, +1.0}, {0.0, +pi_over_2_v<double>}, {true, false}},
    {{+0.0, -1.0}, {0.0, -pi_over_2_v<double>}, {true, false}},
    {{-0.0, +1.0}, {0.0, +pi_over_2_v<double>}, {true, false}},
    {{-0.0, -1.0}, {0.0, -pi_over_2_v<double>}, {true, false}},
    {{-1.0, +0.0}, {0.0, +pi_v<double>}, {true, false}},
    {{-1.0, -0.0}, {0.0, -pi_v<double>}, {true, false}},

#if !FP_PRESET_FAST
    {{+0.0, +0.0}, {-double_inf, +0.0}, {true, true}},
    {{+0.0, -0.0}, {-double_inf, -0.0}, {true, true}},
    {{-0.0, +0.0}, {-double_inf, +pi_v<double>}, {true, false}},
    {{-0.0, -0.0}, {-double_inf, -pi_v<double>}, {true, false}},
    {{+double_inf, +0.0}, {+double_inf, +0.0}, {true, true}},
    {{+double_inf, -0.0}, {+double_inf, -0.0}, {true, true}},
    {{+double_inf, +1.0}, {+double_inf, +0.0}, {true, true}},
    {{+double_inf, -1.0}, {+double_inf, -0.0}, {true, true}},
    {{+double_inf, +double_inf}, {+double_inf, +pi_over_4_v<double>}, {true, false}},
    {{+double_inf, -double_inf}, {+double_inf, -pi_over_4_v<double>}, {true, false}},
    {{+1.0, +double_inf}, {+double_inf, +pi_over_2_v<double>}, {true, false}},
    {{+1.0, -double_inf}, {+double_inf, -pi_over_2_v<double>}, {true, false}},
    {{+0.0, +double_inf}, {+double_inf, +pi_over_2_v<double>}, {true, false}},
    {{+0.0, -double_inf}, {+double_inf, -pi_over_2_v<double>}, {true, false}},
    {{-0.0, +double_inf}, {+double_inf, +pi_over_2_v<double>}, {true, false}},
    {{-0.0, -double_inf}, {+double_inf, -pi_over_2_v<double>}, {true, false}},
    {{-1.0, +double_inf}, {+double_inf, +pi_over_2_v<double>}, {true, false}},
    {{-1.0, -double_inf}, {+double_inf, -pi_over_2_v<double>}, {true, false}},
    {{-double_inf, +double_inf}, {+double_inf, +pi_3_over_4_v<double>}, {true, false}},
    {{-double_inf, -double_inf}, {+double_inf, -pi_3_over_4_v<double>}, {true, false}},
    {{-double_inf, +1.0}, {+double_inf, +pi_v<double>}, {true, false}},
    {{-double_inf, -1.0}, {+double_inf, -pi_v<double>}, {true, false}},
    {{-double_inf, +0.0}, {+double_inf, +pi_v<double>}, {true, false}},
    {{-double_inf, -0.0}, {+double_inf, -pi_v<double>}, {true, false}},
    {{+double_inf, double_nan}, {+double_inf, double_nan}, {true, true}},
    {{-double_inf, double_nan}, {+double_inf, double_nan}, {true, true}},
    {{double_nan, +double_inf}, {+double_inf, double_nan}, {true, true}},
    {{double_nan, -double_inf}, {+double_inf, double_nan}, {true, true}},
    {{double_nan, +0.0}, {double_nan, double_nan}, {true, true}},
    {{+0.0, double_nan}, {double_nan, double_nan}, {true, true}},
    {{double_nan, double_nan}, {double_nan, double_nan}, {true, true}},
#endif // !FP_PRESET_FAST

    // abs(z) overflows
    {{+0x1.fffffffffffffp+1023, +0x1.fffffffffffffp+1023}, {+0x1.63108c75a1936p+9, +0x1.921fb54442d18p-1}},
    {{-0x1.bb67ae8584caap+1023, +0x1.0000000000000p+1023}, {+0x1.62e42fefa39efp+9, +0x1.4f1a6c638d03fp+1}},
    {{+0x1.fffffffffffffp+1023, -0x0.0000000000001p-1022}, {+0x1.62e42fefa39efp+9, -0x0.0000000000000p-1022}},

    // norm(z) overflows
    {{-0x1.4e718d7d7625ap+664, -0x1.4e718d7d7625ap+665}, {+0x1.cd525d6474bb8p+8, -0x1.0468a8ace4df6p+1}},
    {{+0x1.ca3d8e6d80cbbp+511, -0x1.57ae2ad22098cp+511}, {+0x1.6300e9ed15a44p+8, -0x1.4978fa3269ee1p-1}},

#if !WITH_FP_ABRUPT_UNDERFLOW
    // abs(z) underflows
    {{-0x0.0000000000001p-1022, +0x0.0000000000001p-1022}, {-0x1.740bf7c0d927cp+9, +0x1.2d97c7f3321d2p+1}},
    {{+0x0.0000000000001p-1022, +0x0.8000000000000p-1022}, {-0x1.628b76e3a7b61p+9, +0x1.921fb54442d16p+0}},
#endif // !WITH_FP_ABRUPT_UNDERFLOW

    // abs(z) close to underflow
    {{+0x1.4p-1022, +0x1p-1022}, {-0x1.61f684c577299p+9, +0x1.5977a5103ea92p-1}},

    // norm(z) underflows
    {{+0x1.87e92154ef7acp-664, -0x1.87e92154ef7acp-665}, {-0x1.cbb65944f5e2bp+8, -0x1.dac670561bb4fp-2}},
    {{-0x1.9be34ac46b18fp-513, -0x1.1297872d9cbb5p-512}, {-0x1.62991d5d62a5ep+8, -0x1.1b6e192ebbe44p+1}},

    // z close to 1
    {{+0x1.0000000000001p+0, -0.0}, {+0x1.fffffffffffffp-53, -0.0}, {false, true}},
    {{+0x1.fffffffffffffp-1, +0.0}, {-0x1.0000000000000p-53, +0.0}, {false, true}},
#if !WITH_FP_ABRUPT_UNDERFLOW
    {{+0x1.0000000000001p+0, -0x0.0000000000001p-1022}, {+0x1.fffffffffffffp-53, -0x0.0000000000001p-1022}},
    {{+0x1.0000000000000p+0, +0x0.0000000000001p-1022}, {+0x0.0000000000000p-1022, +0x0.0000000000001p-1022}},
    {{+0x1.fffffffffffffp-1, -0x0.0000000000001p-1022}, {-0x1.0000000000000p-53, -0x0.0000000000001p-1022}},
#endif // !WITH_FP_ABRUPT_UNDERFLOW
    {{+0x1.0000000000001p+0, +0x1p-1022}, {+0x1.fffffffffffffp-53, +0x0.fffffffffffffp-1022}},
    {{+0x1.0000000000000p+0, -0x1p-1022}, {+0x0.0000000000000p-1022, -0x1.0000000000000p-1022}},
    {{+0x1.fffffffffffffp-1, +0x1p-1022}, {-0x1.0000000000000p-53, +0x1.0000000000001p-1022}},
    {{+0x1.0000000000001p+0, -0x1p-52}, {+0x1.0000000000000p-52, -0x1.ffffffffffffep-53}},
    {{+0x1.0000000000000p+0, +0x1p-52}, {+0x1.0000000000000p-105, +0x1.0000000000000p-52}},
    {{+0x1.fffffffffffffp-1, -0x1p-52}, {-0x1.ffffffffffffep-54, -0x1.0000000000000p-52}},
    {{+0x1.fffffffffffffp-1, +0x1p-26}, {+0x1.0000000000000p-107, +0x1.0000000000000p-26}},

    // z close to -1, i, or -i
    {{-0x1.0000000000001p+0, -0x1p-52}, {+0x1.0000000000000p-52, -0x1.921fb54442d18p+1}},
    {{-0x1.0000000000000p+0, +0x1p-52}, {+0x1.0000000000000p-105, +0x1.921fb54442d18p+1}},
    {{-0x1.fffffffffffffp-1, -0x1p-52}, {-0x1.ffffffffffffep-54, -0x1.921fb54442d18p+1}},
    {{+0x1p-52, +0x1.0000000000001p+0}, {+0x1.0000000000000p-52, +0x1.921fb54442d17p+0}},
    {{-0x1p-52, +0x1.0000000000000p+0}, {+0x1.0000000000000p-105, +0x1.921fb54442d19p+0}},
    {{+0x1p-52, +0x1.fffffffffffffp-1}, {-0x1.ffffffffffffep-54, +0x1.921fb54442d17p+0}},
    {{-0x1p-52, -0x1.0000000000001p+0}, {+0x1.0000000000000p-52, -0x1.921fb54442d19p+0}},
    {{+0x1p-52, -0x1.0000000000000p+0}, {+0x1.0000000000000p-105, -0x1.921fb54442d17p+0}},
    {{-0x1p-52, -0x1.fffffffffffffp-1}, {-0x1.ffffffffffffep-54, -0x1.921fb54442d19p+0}},

    // abs(z) close to 1
    {{+0x1.6a09e667f3bccp-1, +0x1.6a09e667f3bccp-1}, {-0x1.98d4d0da05571p-54, +0x1.921fb54442d18p-1}},
    {{+0x1.6a09e667f3bcdp-1, -0x1.6a09e667f3bcdp-1}, {+0x1.3b3efbf5e2229p-54, -0x1.921fb54442d18p-1}},
    {{-0x1.3333333333333p-1, -0x1.999999999999ap-1}, {+0x1.999999999999ap-56, -0x1.1b6e192ebbe44p+1}},
    {{-0x1.3333333333333p-1, +0x1.9999999999999p-1}, {-0x1.3333333333333p-54, +0x1.1b6e192ebbe44p+1}},
    {{+0x1.69fbe76c8b439p-1, +0x1.69fbe76c8b439p-1}, {-0x1.3cb7c059d6699p-13, +0x1.921fb54442d18p-1}},
    {{-0x1.d89d89d89d89ep-1, +0x1.89d89d89d89d6p-2}, {-0x1.3b13b13b13b0cp-57, +0x1.5f97315254857p+1}},

    // control flow edge cases
    {{+0x1p-1, +0x1.fffffffffffffp-2}, {-0x1.62e42fefa39f0p-2, +0x1.921fb54442d18p-1}},
    {{+0x1p-1, +0x1.0000000000000p-1}, {-0x1.62e42fefa39efp-2, +0x1.921fb54442d18p-1}},
    {{+0x1p-1, +0x1.0000000000001p-1}, {-0x1.62e42fefa39edp-2, +0x1.921fb54442d19p-1}},
    {{+0x1p-1, +0x1.a887293fd6f33p+0}, {+0x1.193ea7aad0309p-1, +0x1.4727f6d4d118cp+0}},
    {{+0x1p-1, +0x1.a887293fd6f34p+0}, {+0x1.193ea7aad030ap-1, +0x1.4727f6d4d118dp+0}},
    {{+0x1p-1, +0x1.a887293fd6f35p+0}, {+0x1.193ea7aad030cp-1, +0x1.4727f6d4d118dp+0}},
    {{+6.703903964971297e+153, +6e+153}, {+0x1.627e0d1e7a85dp+8, +0x1.75c8a07421461p-1}},
    {{+6.703903964971298e+153, +6e+153}, {+0x1.627e0d1e7a85dp+8, +0x1.75c8a07421461p-1}},
    {{+1e-154, +1.4156865331029228e-146}, {-0x1.4fd46e5c84953p+8, +0x1.921fb525ec2fcp+0}},
    {{+1e-154, +1.415686533102923e-146}, {-0x1.4fd46e5c84953p+8, +0x1.921fb525ec2fcp+0}},
};

constexpr complex_unary_test_case<float> log_float_cases[] = {
    // normal cases
    {{+0x1.8p+0F, +0x1p+1F}, {+0x1.d52410p-1F, +0x1.dac670p-1F}},
    {{+0x1.8p+0F, -0x1p+1F}, {+0x1.d52410p-1F, -0x1.dac670p-1F}},
    {{-0x1.8p+0F, +0x1p+1F}, {+0x1.d52410p-1F, +0x1.1b6e1ap+1F}},
    {{-0x1.8p+0F, -0x1p+1F}, {+0x1.d52410p-1F, -0x1.1b6e1ap+1F}},
    {{+0x1.8p-1F, +0x1p+0F}, {+0x1.c8ff7cp-3F, +0x1.dac670p-1F}},
    {{+0x1.8p-1F, -0x1p+0F}, {+0x1.c8ff7cp-3F, -0x1.dac670p-1F}},
    {{-0x1.8p-1F, +0x1p+0F}, {+0x1.c8ff7cp-3F, +0x1.1b6e1ap+1F}},
    {{-0x1.8p-1F, -0x1p+0F}, {+0x1.c8ff7cp-3F, -0x1.1b6e1ap+1F}},
    {{+0x1.8p-2F, +0x1p-1F}, {-0x1.e148a2p-2F, +0x1.dac670p-1F}},
    {{+0x1.8p-2F, -0x1p-1F}, {-0x1.e148a2p-2F, -0x1.dac670p-1F}},
    {{-0x1.8p-2F, +0x1p-1F}, {-0x1.e148a2p-2F, +0x1.1b6e1ap+1F}},
    {{-0x1.8p-2F, -0x1p-1F}, {-0x1.e148a2p-2F, -0x1.1b6e1ap+1F}},

    // special cases
    {{+1.0F, +0.0F}, {0.0F, +0.0F}, {true, true}},
    {{+1.0F, -0.0F}, {0.0F, -0.0F}, {true, true}},
    {{+0.0F, +1.0F}, {0.0F, +pi_over_2_v<float>}, {true, false}},
    {{+0.0F, -1.0F}, {0.0F, -pi_over_2_v<float>}, {true, false}},
    {{-0.0F, +1.0F}, {0.0F, +pi_over_2_v<float>}, {true, false}},
    {{-0.0F, -1.0F}, {0.0F, -pi_over_2_v<float>}, {true, false}},
    {{-1.0F, +0.0F}, {0.0F, +pi_v<float>}, {true, false}},
    {{-1.0F, -0.0F}, {0.0F, -pi_v<float>}, {true, false}},

#if !FP_PRESET_FAST
    {{+0.0F, +0.0F}, {-float_inf, +0.0F}, {true, true}},
    {{+0.0F, -0.0F}, {-float_inf, -0.0F}, {true, true}},
    {{-0.0F, +0.0F}, {-float_inf, +pi_v<float>}, {true, false}},
    {{-0.0F, -0.0F}, {-float_inf, -pi_v<float>}, {true, false}},
    {{+float_inf, +0.0F}, {+float_inf, +0.0F}, {true, true}},
    {{+float_inf, -0.0F}, {+float_inf, -0.0F}, {true, true}},
    {{+float_inf, +1.0F}, {+float_inf, +0.0F}, {true, true}},
    {{+float_inf, -1.0F}, {+float_inf, -0.0F}, {true, true}},
    {{+float_inf, +float_inf}, {+float_inf, +pi_over_4_v<float>}, {true, false}},
    {{+float_inf, -float_inf}, {+float_inf, -pi_over_4_v<float>}, {true, false}},
    {{+1.0F, +float_inf}, {+float_inf, +pi_over_2_v<float>}, {true, false}},
    {{+1.0F, -float_inf}, {+float_inf, -pi_over_2_v<float>}, {true, false}},
    {{+0.0F, +float_inf}, {+float_inf, +pi_over_2_v<float>}, {true, false}},
    {{+0.0F, -float_inf}, {+float_inf, -pi_over_2_v<float>}, {true, false}},
    {{-0.0F, +float_inf}, {+float_inf, +pi_over_2_v<float>}, {true, false}},
    {{-0.0F, -float_inf}, {+float_inf, -pi_over_2_v<float>}, {true, false}},
    {{-1.0F, +float_inf}, {+float_inf, +pi_over_2_v<float>}, {true, false}},
    {{-1.0F, -float_inf}, {+float_inf, -pi_over_2_v<float>}, {true, false}},
    {{-float_inf, +float_inf}, {+float_inf, +pi_3_over_4_v<float>}, {true, false}},
    {{-float_inf, -float_inf}, {+float_inf, -pi_3_over_4_v<float>}, {true, false}},
    {{-float_inf, +1.0F}, {+float_inf, +pi_v<float>}, {true, false}},
    {{-float_inf, -1.0F}, {+float_inf, -pi_v<float>}, {true, false}},
    {{-float_inf, +0.0F}, {+float_inf, +pi_v<float>}, {true, false}},
    {{-float_inf, -0.0F}, {+float_inf, -pi_v<float>}, {true, false}},
    {{+float_inf, float_nan}, {+float_inf, float_nan}, {true, true}},
    {{-float_inf, float_nan}, {+float_inf, float_nan}, {true, true}},
    {{float_nan, +float_inf}, {+float_inf, float_nan}, {true, true}},
    {{float_nan, -float_inf}, {+float_inf, float_nan}, {true, true}},
    {{float_nan, +0.0F}, {float_nan, float_nan}, {true, true}},
    {{+0.0F, float_nan}, {float_nan, float_nan}, {true, true}},
    {{float_nan, float_nan}, {float_nan, float_nan}, {true, true}},
#endif // !FP_PRESET_FAST

    // abs(z) overflows
    {{+0x1.fffffep+127F, +0x1.fffffep+127F}, {+0x1.644714p+6F, +0x1.921fb6p-1F}},
    {{-0x1.bb67aep+127F, +0x1.000000p+127F}, {+0x1.62e430p+6F, +0x1.4f1a6cp+1F}},
    {{+0x1.fffffep+127F, -0x0.000002p-126F}, {+0x1.62e430p+6F, -0x0.000000p-126F}},

    // norm(z) overflows
    {{-0x1.08b2a2p+83F, -0x1.08b2a2p+84F}, {+0x1.d2f46cp+5F, -0x1.0468a8p+1F}},
    {{+0x1.bc16d6p+63F, -0x1.4d1120p+63F}, {+0x1.6389c2p+5F, -0x1.4978fap-1F}},

#if !WITH_FP_ABRUPT_UNDERFLOW
    // abs(z) underflows
    {{-0x0.000002p-126F, +0x0.000002p-126F}, {-0x1.9bbabcp+6F, +0x1.2d97c8p+1F}},
    {{+0x0.000002p-126F, +0x0.800000p-126F}, {-0x1.601e68p+6F, +0x1.921fb2p+0F}},
#endif // !WITH_FP_ABRUPT_UNDERFLOW

    // abs(z) close to underflow
    {{+0x1.4p-126F, +0x1p-126F}, {-0x1.5b76d6p+6F, +0x1.5977a6p-1F}},

    // norm(z) underflows
    {{+0x1.ef2d10p-83F, -0x1.ef2d10p-84F}, {-0x1.c6144ap+5F, -0x1.dac670p-2F}},
    {{-0x1.622d70p-61F, -0x1.d83c94p-61F}, {-0x1.4b9280p+5F, -0x1.1b6e1ap+1F}},

    // z close to 1
    {{+0x1.000002p+0F, -0.0F}, {+0x1.fffffep-24F, -0.0F}, {false, true}},
    {{+0x1.fffffep-1F, +0.0F}, {-0x1.000000p-24F, +0.0F}, {false, true}},
#if !WITH_FP_ABRUPT_UNDERFLOW
    {{+0x1.000002p+0F, -0x0.000002p-126F}, {+0x1.fffffep-24F, -0x0.000002p-126F}},
    {{+0x1.000000p+0F, +0x0.000002p-126F}, {+0x0.000000p-126F, +0x0.000002p-126F}},
    {{+0x1.fffffep-1F, -0x0.000002p-126F}, {-0x1.000000p-24F, -0x0.000002p-126F}},
#endif // !WITH_FP_ABRUPT_UNDERFLOW
    {{+0x1.000002p+0F, +0x1.000000p-126F}, {+0x1.fffffep-24F, +0x0.fffffep-126F}},
    {{+0x1.000000p+0F, -0x1.000000p-126F}, {+0x0.000000p+0F, -0x1.000000p-126F}},
    {{+0x1.fffffep-1F, +0x1.000000p-126F}, {-0x1.000000p-24F, +0x1.000002p-126F}},
    {{+0x1.000002p+0F, -0x1.000000p-23F}, {+0x1.000000p-23F, -0x1.fffffcp-24F}},
    {{+0x1.000000p+0F, +0x1.000000p-23F}, {+0x1.000000p-47F, +0x1.000000p-23F}},
    {{+0x1.fffffep-1F, -0x1.000000p-23F}, {-0x1.fffffcp-25F, -0x1.000000p-23F}},
    {{+0x1.fffffep-1F, +0x1.6a09e6p-12F}, {-0x1.302ae0p-52F, +0x1.6a09e6p-12F}},

    // z close to -1, i, or -i
    {{-0x1.000002p+0F, -0x1.000000p-23F}, {+0x1.000000p-23F, -0x1.921fb4p+1F}},
    {{-0x1.000000p+0F, +0x1.000000p-23F}, {+0x1.000000p-47F, +0x1.921fb4p+1F}},
    {{-0x1.fffffep-1F, -0x1.000000p-23F}, {-0x1.fffffcp-25F, -0x1.921fb4p+1F}},
    {{+0x1.000000p-23F, +0x1.000002p+0F}, {+0x1.000000p-23F, +0x1.921fb4p+0F}},
    {{-0x1.000000p-23F, +0x1.000000p+0F}, {+0x1.000000p-47F, +0x1.921fb8p+0F}},
    {{+0x1.000000p-23F, +0x1.fffffep-1F}, {-0x1.fffffcp-25F, +0x1.921fb4p+0F}},
    {{-0x1.000000p-23F, -0x1.000002p+0F}, {+0x1.000000p-23F, -0x1.921fb8p+0F}},
    {{+0x1.000000p-23F, -0x1.000000p+0F}, {+0x1.000000p-47F, -0x1.921fb4p+0F}},
    {{-0x1.000000p-23F, -0x1.fffffep-1F}, {-0x1.fffffcp-25F, -0x1.921fb8p+0F}},

    // abs(z) close to 1
    {{+0x1.6a09e6p-1F, +0x1.6a09e6p-1F}, {-0x1.26055cp-26F, +0x1.921fb6p-1F}},
    {{+0x1.6a09e8p-1F, -0x1.6a09e8p-1F}, {+0x1.20888ep-24F, -0x1.921fb6p-1F}},
    {{-0x1.333334p-1F, -0x1.99999ap-1F}, {+0x1.99999ap-26F, -0x1.1b6e1ap+1F}},
    {{-0x1.333332p-1F, +0x1.99999ap-1F}, {-0x1.999998p-27F, +0x1.1b6e18p+1F}},
    {{+0x1.69fbe8p-1F, +0x1.69fbe8p-1F}, {-0x1.3caab8p-13F, +0x1.921fb6p-1F}},
    {{-0x1.d89d8ap-1F, +0x1.89d89ep-2F}, {+0x1.d89d8ap-28F, +0x1.5f9732p+1F}},

    // control flow edge cases
    {{+0x1p-1F, +0x1.fffffep-2F}, {-0x1.62e432p-2F, +0x1.921fb4p-1F}},
    {{+0x1p-1F, +0x1.000000p-1F}, {-0x1.62e430p-2F, +0x1.921fb6p-1F}},
    {{+0x1p-1F, +0x1.000002p-1F}, {-0x1.62e42cp-2F, +0x1.921fb8p-1F}},
    {{+0x1p-1F, +0x1.a88728p+0F}, {+0x1.193ea6p-1F, +0x1.4727f6p+0F}},
    {{+0x1p-1F, +0x1.a8872ap+0F}, {+0x1.193ea8p-1F, +0x1.4727f6p+0F}},
    {{+0x1p-1F, +0x1.a8872cp+0F}, {+0x1.193eaap-1F, +0x1.4727f8p+0F}},
    {{+9.223371e+18F, +9e+18F}, {+0x1.60059cp+5F, +0x1.8bd930p-1F}},
    {{+9.2233715e+18F, +9e+18F}, {+0x1.60059cp+5F, +0x1.8bd930p-1F}},
    {{+7e-20F, +4.440892e-16F}, {-0x1.1acdd6p+5F, +0x1.921560p+0F}},
    {{+7e-20F, +4.4408926e-16F}, {-0x1.1acdd6p+5F, +0x1.921560p+0F}},
};
