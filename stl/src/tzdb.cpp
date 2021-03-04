// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <internal_shared.h>
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

    constexpr auto reg_key_name    = LR"(SYSTEM\CurrentControlSet\Control\LeapSecondInformation)";
    constexpr auto reg_subkey_name = L"LeapSeconds";
    *current_reg_ls_size           = 0;
    HKEY leap_sec_key              = 0;

    LSTATUS status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, reg_key_name, 0, KEY_READ, &leap_sec_key);
    if (status != ERROR_SUCCESS) {
        // May not exist on older systems. Treat this as equivalent to the key existing but with no data.
        return nullptr;
    }

    DWORD byte_size = 0;
    status          = RegQueryValueExW(leap_sec_key, reg_subkey_name, nullptr, nullptr, nullptr, &byte_size);
    static_assert(sizeof(_RegistryLeapSecondInfo) == 12);
    const auto ls_size   = byte_size / 12;
    *current_reg_ls_size = ls_size;

    _RegistryLeapSecondInfo* reg_ls_data = nullptr;
    if ((status == ERROR_SUCCESS || status == ERROR_MORE_DATA) && ls_size > prev_reg_ls_size) {
        try {
            reg_ls_data = new _RegistryLeapSecondInfo[ls_size];
            status      = RegQueryValueExW(
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

void __stdcall __std_decalloc_reg_leap_seconds(_RegistryLeapSecondInfo* _Rlsi) {
    delete[] _Rlsi;
}

_NODISCARD void* __stdcall __std_calloc_crt(const size_t count, const size_t size) {
    return _calloc_crt(count, size);
}

void __stdcall __std_free_crt(void* p) {
    _free_crt(p);
}

_END_EXTERN_C
