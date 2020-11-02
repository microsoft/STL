// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <string>

#include <windows.h>

class scm_handle {
private:
    SC_HANDLE h;

public:
    scm_handle();
    ~scm_handle();
    scm_handle(const scm_handle&) = delete;
    scm_handle& operator=(const scm_handle&) = delete;

    SC_HANDLE get() {
        return h;
    }
};

class driver_loader {
private:
    scm_handle sc_handle;
    std::string driver_name;

public:
    driver_loader(const char* path, const char* name);
    ~driver_loader();

    driver_loader(const scm_handle&) = delete;
    driver_loader& operator=(const scm_handle&) = delete;
};

[[noreturn]] void throw_get_last_error(const char* routine);
