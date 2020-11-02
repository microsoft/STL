// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "install.h"

#include <stdexcept>
#include <string>

#include <windows.h>

[[noreturn]] void throw_get_last_error(const char* routine) {
    auto gle                = GetLastError();
    char error_buffer[1024] = {0};

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, // lpSource
        gle, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error_buffer, static_cast<DWORD>(std::size(error_buffer)),
        nullptr /* va_list arguments */);

    std::string err_text = "Error: ";
    err_text += routine;
    err_text += " failed.  GetLastError = " + std::to_string(GetLastError());
    err_text += "\n";
    err_text += error_buffer;
    throw std::runtime_error(err_text);
}

scm_handle::scm_handle()
    : h(OpenSCManager(nullptr /* lpMachineName */, nullptr /* lpDatabaseName */, SC_MANAGER_ALL_ACCESS)) {
    if (!h) {
        throw_get_last_error(__FUNCTION__);
    }
}

scm_handle::~scm_handle() {
    CloseServiceHandle(h);
}

struct service_handle {
    SC_HANDLE h;

    service_handle(SC_HANDLE scm, const char* name) noexcept : h(OpenService(scm, name, SERVICE_ALL_ACCESS)) {}
    service_handle() noexcept : h(nullptr) {}

    void attach(SC_HANDLE new_h) noexcept {
        if (h)
            CloseServiceHandle(h);
        h = new_h;
    }

    ~service_handle() {
        if (h)
            CloseServiceHandle(h);
    }
};

static void stop_and_remove_driver(SC_HANDLE scm, const char* name) noexcept {
    service_handle driver(scm, name);

    if (driver.h == nullptr) {
        // already gone.  Post-condition met
        return;
    }
    // ignore the returns, as there isn't anything we can do if the delete fails
    SERVICE_STATUS serviceStatus;
    ControlService(driver.h, SERVICE_CONTROL_STOP, &serviceStatus);
    DeleteService(driver.h);
}

static void install_and_start_driver(SC_HANDLE scm, const char* path, const char* name) {
    service_handle driver;
    driver.attach(CreateService(scm,
        name, // lpServiceName
        name, // lpDisplayName
        SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path,
        nullptr, // lpLoadOrderGroup
        nullptr, // lpdwTagId
        nullptr, // lpDependencies
        nullptr, // lpServiceStartName
        nullptr // lpServiceStartName
        ));

    if (driver.h == nullptr) {
        throw_get_last_error("CreateService");
    }

    auto success = StartService(driver.h, 0 /*num args*/, nullptr /*args*/);
    if (!success) {
        throw_get_last_error("StartService");
    }
}

driver_loader::driver_loader(const char* path, const char* name) : driver_name(name) {
    // clean up any previous runs in case something went horribly wrong
    stop_and_remove_driver(sc_handle.get(), driver_name.c_str());
    install_and_start_driver(sc_handle.get(), path, name);
}

driver_loader::~driver_loader() {
    stop_and_remove_driver(sc_handle.get(), driver_name.c_str());
}
