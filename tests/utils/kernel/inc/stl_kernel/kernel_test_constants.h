// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This header is intended to be force included into the .cpp under test.
// KERNEL_TEST_NAME will then be provided as a /D switch and preprocessor
// concatenated into one big string literal.
extern const wchar_t* const STL_KERNEL_NT_DEVICE_NAME  = L"\\Device\\" KERNEL_TEST_NAME;
extern const wchar_t* const STL_KERNEL_DOS_DEVICE_NAME = L"\\DosDevices\\" KERNEL_TEST_NAME;
