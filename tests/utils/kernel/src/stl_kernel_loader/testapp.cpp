// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "install.h"
#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include <stl_kernel/stl_kernel.h>
#include <string>

#include <windows.h>

struct DeviceOpener {
    HANDLE h = INVALID_HANDLE_VALUE;
    explicit DeviceOpener(const char* driverName) {
        std::string dosName = "\\\\.\\";
        dosName += driverName;
        h = CreateFile(
            dosName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (h == INVALID_HANDLE_VALUE) {
            throw_get_last_error(__FUNCTION__);
        }
    }
    ~DeviceOpener() {
        CloseHandle(h);
    }
};

// returns true if the test passed
bool do_test(const char* path) {
    // We have a secret handshake with the driver and the build system.
    // The DOS name of the driver is the same as the absolute path,
    // except the \ and : are replaced with ".".  Shhh.  Don't tell anyone.
    std::string driverName = path;
    std::replace_if(
        driverName.begin(), driverName.end(), [](char c) { return c == '\\' || c == ':'; }, '.');

    driver_loader sentry(path, driverName.c_str());
    DeviceOpener dev(driverName.c_str());

    ULONG bytesReturned = 0;
    char InputBuffer[1];
    TestResults OutputBuffer;

    auto success = DeviceIoControl(dev.h, (DWORD) IOCTL_SIOCTL_METHOD_RUN_TEST, &InputBuffer, sizeof(InputBuffer),
        &OutputBuffer, sizeof(OutputBuffer), &bytesReturned, nullptr);

    if (!success) {
        throw_get_last_error(__FUNCTION__);
    }

    if (OutputBuffer.main_return != 0 || OutputBuffer.tests_failed != 0) {
        printf("retval: %d\ntests failed: %d\noutput =\n%s\n", OutputBuffer.main_return, OutputBuffer.tests_failed,
            OutputBuffer.output);
        return false;
    }
    return true;
}

int __cdecl main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <some\\path\\foo.sys>\n", argv[0]);
        return 2;
    }
    try {
        bool result = do_test(argv[1]);
        return result ? 0 : 1;
    } catch (const std::exception& e) {
        printf("%s\n", e.what());
    }
    return 2;
}
