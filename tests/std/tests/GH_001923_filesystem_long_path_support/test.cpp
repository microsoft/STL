// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <filesystem>
#include <iostream>
#include <system_error>

#pragma warning(push) // TRANSITION, OS-23694920
#pragma warning(disable : 4668) // 'MEOW' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#include <Windows.h>
#pragma warning(pop)

#include "test_filesystem_support.hpp"

using namespace std;

bool are_long_paths_enabled() {
    // https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=powershell
    // https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-reggetvaluew
    DWORD registry_value = 0;
    DWORD buffer_size    = sizeof(registry_value);
    const LSTATUS status = RegGetValueW(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet\Control\FileSystem)",
        L"LongPathsEnabled", RRF_RT_REG_DWORD, nullptr, &registry_value, &buffer_size);

    switch (status) {
    case ERROR_SUCCESS:
        assert(buffer_size == sizeof(registry_value));
        return registry_value != 0;
    case ERROR_FILE_NOT_FOUND:
        return false; // The registry value doesn't exist, so long paths aren't enabled.
    case ERROR_MORE_DATA:
        cout << "RegGetValueW() returned ERROR_MORE_DATA; this should not be possible." << endl;
        break;
    case ERROR_UNSUPPORTED_TYPE:
        cout << "RegGetValueW() returned ERROR_UNSUPPORTED_TYPE; the value exists but has a weird type." << endl;
        break;
    default:
        cout << "RegGetValueW() returned " << status << "." << endl;
        break;
    }

    assert(status == ERROR_SUCCESS || status == ERROR_FILE_NOT_FOUND); // Equivalent to assert(false), but clearer.
    return false;
}

int main() {
    const test_temp_directory tempDir("filesystem_long_path_support");

    const filesystem::path long_path =
        tempDir.directoryPath / "JulietteAndromedaMeow" / "ThisDirectoryNameIsFiftyCharactersLongBlahBlahBlah"
        / "ThisDirectoryNameIsFiftyCharactersLongBlahBlahBlah" / "ThisDirectoryNameIsFiftyCharactersLongBlahBlahBlah"
        / "ThisDirectoryNameIsFiftyCharactersLongBlahBlahBlah" / "ThisDirectoryNameIsFiftyCharactersLongBlahBlahBlah"
        / "ThisDirectoryNameIsFiftyCharactersLongBlahBlahBlah" / "ClarissaMelpomeneMeow"; // well over 300 characters

    error_code ec{};
    const bool created = filesystem::create_directories(long_path, ec);
    assert(!ec);
    assert(created);
}
