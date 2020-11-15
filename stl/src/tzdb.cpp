// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <xtzdb.h>

#include <Windows.h>

#pragma comment(lib, "Advapi32")

_EXTERN_C

_RegistryLeapSecondInfo* __stdcall __std_tzdb_get_reg_leap_seconds(
    const size_t prev_reg_ls_size, size_t* current_reg_ls_size) {
    // On exit---
    //    *current_reg_ls_size <= prev_reg_ls_size, *reg_ls_data == nullptr --> no new data
    //    *current_reg_ls_size >  prev_reg_ls_size, *reg_ls_data != nullptr --> new data, successfully read
    //    *current_reg_ls_size == 0,                *reg_ls_data != nullptr --> new data, failed reading
    //    *current_reg_ls_size >  prev_reg_ls_size, *reg_ls_data == nullptr --> new data, failed allocation

    constexpr auto reg_key_name    = TEXT("SYSTEM\\CurrentControlSet\\Control\\LeapSecondInformation");
    constexpr auto reg_subkey_name = TEXT("LeapSeconds");
    *current_reg_ls_size           = 0;
    HKEY leap_sec_key              = 0;

    LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, reg_key_name, 0, KEY_READ, &leap_sec_key);
    if (status != ERROR_SUCCESS) {
        // May not exist on older systems. Treat this as equivalent to the key existing but with no data.
        return nullptr;
    }

    DWORD byte_size = 0;
    status          = RegQueryValueEx(leap_sec_key, reg_subkey_name, nullptr, nullptr, nullptr, &byte_size);
    static_assert(sizeof(_RegistryLeapSecondInfo) == 12);
    const auto ls_size   = byte_size / 12;
    *current_reg_ls_size = ls_size;

    _RegistryLeapSecondInfo* reg_ls_data = nullptr;
    if ((status == ERROR_SUCCESS || status == ERROR_MORE_DATA) && ls_size > prev_reg_ls_size) {
        try {
            reg_ls_data = new _RegistryLeapSecondInfo[ls_size];
            status      = RegQueryValueEx(
                leap_sec_key, reg_subkey_name, nullptr, nullptr, reinterpret_cast<LPBYTE>(reg_ls_data), &byte_size);
            if (status != ERROR_SUCCESS) {
                *current_reg_ls_size = 0;
            }
        } catch (...) {
        }
    }

    RegCloseKey(leap_sec_key);
    if (status != ERROR_SUCCESS) {
        SetLastError(status);
    }

    return reg_ls_data;
}

_END_EXTERN_C
