// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Note: To properly test the fix for VSO-1775715, don't include any headers here.

// TRANSITION, Windows SDK 10.0.22621.0 causes this test to fail for ARM64EC with:
// "error LNK2019: unresolved external symbol fabsf referenced in function #fabsf$exit_thunk (EC Symbol)"
// It passes when built with Windows SDK 10.0.26100.0.
// UNSUPPORTED: arm64ec

import User;

int main() {
    user::prepare_test_environment();
    user::test_vso_1775715(0, "0 0 0b0 0B0 0x0 0X0 0");
    user::test_vso_1775715(77, "77 77 0b1001101 0B1001101 0x4d 0X4D 0115");
}
