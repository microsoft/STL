// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test_decls.h"
#include <ntddk.h>
#include <stl_kernel/stl_kernel.h>

// This isn't the "real" cassert, but our interposed header.  We want to get
// at the globals that have the error text and the failure count
#include <cassert>

struct device_handle {
    DEVICE_OBJECT* h = nullptr;

    device_handle() = default;
    ~device_handle() {
        if (h) {
            IoDeleteDevice(h);
        }
    }

    DEVICE_OBJECT* release() {
        DEVICE_OBJECT* retval = h;
        h                     = nullptr;
        return retval;
    }
};

class irp_sentry {
private:
    IRP* irp;
    NTSTATUS& status;

public:
    irp_sentry(IRP* irp, NTSTATUS& status) : irp(irp), status(status) {}
    ~irp_sentry() {
        irp->IoStatus.Status = status;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
    }
    irp_sentry(const irp_sentry&) = delete;
    irp_sentry& operator=(const irp_sentry&) = delete;
};

extern "C" {
// Device driver routine declarations.
DRIVER_INITIALIZE DriverEntry;

_Dispatch_type_(IRP_MJ_CREATE) _Dispatch_type_(IRP_MJ_CLOSE) DRIVER_DISPATCH StlKernelCreateClose;

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH StlKernelDeviceControl;

DRIVER_UNLOAD PAGE, StlKernelUnloadDriver;

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, StlKernelCreateClose)
#pragma alloc_text(PAGE, StlKernelDeviceControl)
#pragma alloc_text(PAGE, StlKernelUnloadDriver)

NTSTATUS DriverEntry(DRIVER_OBJECT* driver, UNICODE_STRING* /*reg_path*/) {
    UNICODE_STRING nt_name;
    RtlInitUnicodeString(&nt_name, STL_KERNEL_NT_DEVICE_NAME);

    device_handle device;
    NTSTATUS status = IoCreateDevice(driver,
        0, // DeviceExtensionSize
        &nt_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN,
        FALSE, // BOOLEAN Exclusive
        &device.h);

    if (!NT_SUCCESS(status)) {
        DbgPrint("stl_kernel: Couldn't create the device object\n");
        return status;
    }

    driver->MajorFunction[IRP_MJ_CREATE]         = StlKernelCreateClose;
    driver->MajorFunction[IRP_MJ_CLOSE]          = StlKernelCreateClose;
    driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = StlKernelDeviceControl;
    driver->DriverUnload                         = StlKernelUnloadDriver;

    UNICODE_STRING dos_name;
    RtlInitUnicodeString(&dos_name, STL_KERNEL_DOS_DEVICE_NAME);

    status = IoCreateSymbolicLink(&dos_name, &nt_name);

    if (!NT_SUCCESS(status)) {
        DbgPrint("stl_kernel: Couldn't create symbolic link\n");
        return status;
    }

    KeInitializeGuardedMutex(&g_assert_mutex);

    device.release(); // everything worked, so "commit" the operation.
    return status;
}

NTSTATUS StlKernelCreateClose(DEVICE_OBJECT*, IRP* irp) {
    PAGED_CODE();

    irp->IoStatus.Status      = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;

    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

void StlKernelUnloadDriver(DRIVER_OBJECT* driver) {
    PAGED_CODE();

    device_handle dev;
    dev.h = driver->DeviceObject;

    UNICODE_STRING dos_name;
    RtlInitUnicodeString(&dos_name, STL_KERNEL_DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&dos_name);
}

NTSTATUS StlKernelDeviceControl(DEVICE_OBJECT*, IRP* irp) {
    PAGED_CODE();

    NTSTATUS status = STATUS_SUCCESS;

    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(irp);
    ULONG outBufLength       = irpSp->Parameters.DeviceIoControl.OutputBufferLength;
    irp_sentry sentry(irp, status);

    if (outBufLength < sizeof(TestResults)) {
        status = STATUS_INVALID_PARAMETER;
        return status;
    }

    auto control_code = irpSp->Parameters.DeviceIoControl.IoControlCode;
    if (control_code != IOCTL_SIOCTL_METHOD_RUN_TEST) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        DbgPrint("stl_kernel ERROR: unrecognized IOCTL %x\n", control_code);
        return status;
    }

    TestResults* outBuf = static_cast<TestResults*>(irp->AssociatedIrp.SystemBuffer);

    // set up the assertion framework
    g_test_failures   = &outBuf->tests_failed;
    g_output_buffer   = outBuf->output;
    g_space_available = sizeof(outBuf->output);

    // This is where the magic happens
    outBuf->main_return = main();

    irp->IoStatus.Information = sizeof(TestResults);

    return status;
}

} // extern "C"
