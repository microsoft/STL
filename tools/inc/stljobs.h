// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <assert.h>
#include <atomic>
#include <exception>
#include <iterator>
#include <limits.h>
#include <memory>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <string_view>
#include <utility>

#include <Windows.h>
#include <userenv.h>

struct api_exception : std::exception {
    const char* api;
    unsigned long lastError;

    explicit api_exception(const char* const api_, const unsigned long lastError_) noexcept
        : api(api_), lastError(lastError_) {}

    [[noreturn]] void give_up() const {
        fflush(stdout);
        fprintf(stderr, "The API \"%s\" failed unexpectedly; last error 0x%08lX\n", api, lastError);
        abort();
    }

    [[nodiscard]] const char* what() const noexcept override {
        return "win32 exception";
    }
};

[[noreturn]] inline void api_failure(const char* const api, const unsigned long lastError = GetLastError()) {
    throw api_exception{api, lastError};
}

void close_handle(const HANDLE toClose) noexcept {
    if (!CloseHandle(toClose)) {
        assert(false);
    }
}

struct invalid_handle_value_policy {
    static constexpr HANDLE Empty = INVALID_HANDLE_VALUE;
};

struct null_handle_policy {
    static constexpr HANDLE Empty{};
};

template <class EmptyPolicy>
class handle {
public:
    handle() = default;

    explicit handle(const HANDLE hInitial) noexcept : impl(hInitial) {}

    handle(handle&& other) noexcept : impl(std::exchange(other.impl, EmptyPolicy::Empty)) {}

    handle& operator=(handle&& other) noexcept {
        handle moved = std::move(other);
        swap(moved, *this);
        return *this;
    }

    ~handle() noexcept {
        if (impl != EmptyPolicy::Empty) {
            close_handle(impl);
        }
    }

    friend void swap(handle& lhs, handle& rhs) noexcept {
        using std::swap;
        swap(lhs.impl, rhs.impl);
    }

    void close() noexcept {
        if (impl != EmptyPolicy::Empty) {
            close_handle(impl);
            impl = EmptyPolicy::Empty;
        }
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return impl != EmptyPolicy::Empty;
    }

    [[nodiscard]] HANDLE get() const noexcept {
        return impl;
    }

    void attach(const HANDLE newHandle) & noexcept {
        handle captured{newHandle};
        swap(captured, *this);
    }

    [[nodiscard]] HANDLE detach() noexcept {
        return std::exchange(impl, EmptyPolicy::Empty);
    }

private:
    HANDLE impl{EmptyPolicy::Empty};
};

inline handle<invalid_handle_value_policy> create_file(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile) {
    handle<invalid_handle_value_policy> result{CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)};
    if (!result) {
        api_failure("CreateFileW");
    }

    return result;
}

inline handle<invalid_handle_value_policy> create_named_pipe(LPCWSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
    DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize, DWORD nDefaultTimeOut,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
    handle<invalid_handle_value_policy> result{CreateNamedPipeW(lpName, dwOpenMode, dwPipeMode, nMaxInstances,
        nOutBufferSize, nInBufferSize, nDefaultTimeOut, lpSecurityAttributes)};
    if (!result) {
        api_failure("CreateNamedPipeW");
    }

    return result;
}


const auto is_exactly_space = [](const wchar_t c) { return c == L' '; };

[[nodiscard]] inline handle<null_handle_policy> create_job_that_will_be_killed_when_closed() {
    handle<null_handle_policy> hJob{CreateJobObjectW(nullptr, nullptr)};
    if (!hJob) {
        api_failure("CreateJobObjectW");
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
    limits.BasicLimitInformation.LimitFlags =
        JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
    if (!SetInformationJobObject(hJob.get(), JobObjectExtendedLimitInformation, &limits, sizeof(limits))) {
        api_failure("SetInformationJobObject");
    }

    return hJob;
}

inline void put_self_in_job() {
    auto hJob = create_job_that_will_be_killed_when_closed();

    // Put ourselves in that job
    if (!AssignProcessToJobObject(hJob.get(), GetCurrentProcess())) {
        api_failure("AssignProcessToJobObject");
    }

    // Purposely leak hJob to avoid terminating ourselves
    (void) hJob.detach();
}

class no_input_pipe {
public:
    no_input_pipe() {
        SECURITY_ATTRIBUTES inheritSa;
        inheritSa.nLength              = sizeof(inheritSa);
        inheritSa.lpSecurityDescriptor = nullptr;
        inheritSa.bInheritHandle       = TRUE;
        HANDLE read;
        HANDLE write;
        if (!CreatePipe(&read, &write, &inheritSa, 0)) {
            api_failure("CreatePipe");
        }

        devNull.attach(write);
        close_handle(read);
    }

    no_input_pipe(const no_input_pipe&) = delete;
    no_input_pipe& operator=(const no_input_pipe&) = delete;

    [[nodiscard]] HANDLE get() const noexcept {
        return devNull.get();
    }

    [[nodiscard]] static const no_input_pipe& instance() {
        static no_input_pipe instance_;
        return instance_;
    }

private:
    handle<null_handle_policy> devNull;
};

class tp_io {
public:
    tp_io() = default;

    explicit tp_io(handle<invalid_handle_value_policy>&& fileHandle_, const PTP_WIN32_IO_CALLBACK callback,
        void* const pv, const PTP_CALLBACK_ENVIRON pcbe = nullptr)
        : io(CreateThreadpoolIo(fileHandle_.get(), callback, pv, pcbe)) {
        if (!io) {
            api_failure("CreateThreadpoolIo");
        }

        fileHandle = std::move(fileHandle_);
    }

    tp_io(tp_io&& other) noexcept : fileHandle(std::move(other.fileHandle)), io(std::exchange(other.io, nullptr)) {}

    ~tp_io() {
        if (io != nullptr) {
            close();
        }
    }

    friend void swap(tp_io& lhs, tp_io& rhs) noexcept {
        using std::swap;
        swap(lhs.fileHandle, rhs.fileHandle);
        swap(lhs.io, rhs.io);
    }

    tp_io& operator=(tp_io&& other) noexcept {
        tp_io moved{std::move(other)};
        swap(moved, *this);
        return *this;
    }

    [[nodiscard]] HANDLE get_file() const noexcept {
        return fileHandle.get();
    }

    void start_threadpool_io() noexcept {
        StartThreadpoolIo(io);
    }

    void cancel_threadpool_io() noexcept {
        CancelThreadpoolIo(io);
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return io != nullptr;
    }

    handle<invalid_handle_value_policy> close() noexcept {
        assert(io != nullptr);
        WaitForThreadpoolIoCallbacks(io, TRUE);
        CloseThreadpoolIo(io);
        io = nullptr;
        return std::move(fileHandle);
    }

    void wait(const bool cancelPending) noexcept {
        WaitForThreadpoolIoCallbacks(io, cancelPending);
    }

private:
    handle<invalid_handle_value_policy> fileHandle{};
    PTP_IO io{};
};

struct output_collecting_pipe {
    static constexpr unsigned long kernelBufferSize = 4096;
    static constexpr unsigned long bufferSize       = 4096;

    output_collecting_pipe() {
        // generate a random name for the pipe (we must use a named pipe because anonymous pipes from CreatePipe can't
        // be used in asynchronous mode)
        std::random_device rd;
        constexpr size_t pipeNameBufferCount = 15 + 8 * 8 + 1;
        //                                                123456789012345
        wchar_t pipeNameBuffer[pipeNameBufferCount] = LR"(\\.\pipe\Local\)";
        wchar_t* pipeNameCursor                     = pipeNameBuffer + 15;
        for (int values = 0; values < 8; ++values) {
            unsigned int randomValue = rd();
            for (int hexits = 0; hexits < 8; ++hexits) {
                *pipeNameCursor++ = L"0123456789ABCDEF"[randomValue & 0xFu];
                randomValue >>= 4;
            }
        }

        *pipeNameCursor = L'\0';

        auto readHandle = create_named_pipe(pipeNameBuffer,
            PIPE_ACCESS_INBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS, 1, 0, kernelBufferSize, 0,
            nullptr);

        if (!SetFileCompletionNotificationModes(
                readHandle.get(), FILE_SKIP_COMPLETION_PORT_ON_SUCCESS | FILE_SKIP_SET_EVENT_ON_HANDLE)) {
            api_failure("SetFileCompletionNotificationModes");
        }

        SECURITY_ATTRIBUTES inheritSa;
        inheritSa.nLength              = sizeof(inheritSa);
        inheritSa.lpSecurityDescriptor = nullptr;
        inheritSa.bInheritHandle       = TRUE;
        writeHandle = create_file(pipeNameBuffer, GENERIC_WRITE | FILE_READ_ATTRIBUTES, 0, &inheritSa, OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED, HANDLE{});

        readIo = tp_io{std::move(readHandle), callback, this, nullptr};

        start();
    }

    ~output_collecting_pipe() noexcept {
        if (readIo) {
            if (running.load()) {
                if (!CancelIoEx(readIo.get_file(), &overlapped)) {
                    api_failure("CancelIoEx"); // slams into noexcept
                }
            }

            readIo.close();
        }
    }

    output_collecting_pipe(const output_collecting_pipe&) = delete;
    output_collecting_pipe& operator=(const output_collecting_pipe&) = delete;

    void start() {
        [[maybe_unused]] const bool oldRunning = running.exchange(true);
        assert(!oldRunning);
        read_some();
    }

    void stop() {
        if (running.exchange(false)) {
            if (!CancelIoEx(readIo.get_file(), &overlapped)) {
                api_failure("CancelIoEx");
            }

            readIo.wait(false);
        }
    }

    [[nodiscard]] std::string extract_and_reset() {
        stop();
        auto first = targetBuffer.data();
        auto last  = first + validTill;
        first      = std::find_if_not(first, last, is_exactly_space);
        last = std::find_if_not(std::reverse_iterator(last), std::reverse_iterator(first), is_exactly_space).base();
        std::string result(first, static_cast<size_t>(last - first));
        validTill = 0;
        start();
        return result;
    }

    [[nodiscard]] HANDLE get_write_pipe() noexcept {
        return writeHandle.get();
    }

private:
    static void __stdcall callback(PTP_CALLBACK_INSTANCE, void* const thisRaw, void*, const ULONG ioResult,
        const ULONG_PTR bytes, PTP_IO) noexcept {
        switch (ioResult) {
        case ERROR_SUCCESS:
        case ERROR_OPERATION_ABORTED:
            break;
        default:
            api_failure("StartThreadpoolIo + ReadFile callback", ioResult); // slams into noexcept
            break;
        }

        const auto this_ = static_cast<output_collecting_pipe*>(thisRaw);
        this_->validTill += bytes;
        if (this_->running.load()) {
            this_->read_some();
        }
    }

    void ensure_target_buffer_space() {
        if (bufferSize <= targetBuffer.size() - validTill) {
            // already has enough space
            return;
        }

        targetBuffer.resize(validTill + bufferSize);
        targetBuffer.resize(std::min(static_cast<size_t>(ULONG_MAX), targetBuffer.capacity()));
    }

    void read_some() {
        assert(running.load());
        DWORD bytesRead = 0;
        readIo.start_threadpool_io();
        for (;;) {
            ensure_target_buffer_space();
            if (!ReadFile(readIo.get_file(), targetBuffer.data() + validTill,
                    static_cast<unsigned long>(targetBuffer.size() - validTill), &bytesRead, &overlapped)) {
                break;
            }

            validTill += bytesRead;
        }

        const auto lastError = GetLastError();
        if (lastError == ERROR_IO_PENDING) {
            return;
        }

        readIo.cancel_threadpool_io();
        api_failure("ReadFile", lastError);
    }

    std::atomic<bool> running{};
    std::string targetBuffer; // if running, owned by a threadpool thread, otherwise owned by the calling thread
    size_t validTill{};
    handle<invalid_handle_value_policy> writeHandle;
    tp_io readIo;
    OVERLAPPED overlapped{};
};

struct execution_result {
    unsigned long exitCode;
    std::string output;
};

class environment_block {
public:
    environment_block() {
        default_block defaultBlock;
        auto blockEnd = static_cast<const wchar_t*>(defaultBlock.blockRaw);
        for (;;) {
            const auto thisLen = wcslen(blockEnd);
            blockEnd += thisLen;
            ++blockEnd;

            if (thisLen == 0) {
                break;
            }
        }

        block.assign(static_cast<const wchar_t*>(defaultBlock.blockRaw), blockEnd);
    }

    [[nodiscard]] void* get() noexcept {
        return block.data();
    }

    void append_environment(const std::wstring_view key, const std::wstring_view value) {
        block.reserve(block.size() + key.size() + value.size() + 2);
        block.append(key);
        block.push_back('=');
        block.append(value);
        block.push_back('\0');
    }

private:
    struct default_block {
        void* blockRaw;
        default_block() {
            if (!CreateEnvironmentBlock(&blockRaw, HANDLE{}, FALSE)) {
                api_failure("CreateEnvironmentBlock");
            }
        }

        ~default_block() noexcept {
            if (!DestroyEnvironmentBlock(blockRaw)) {
                api_failure("DestroyEnvironmentBlock"); // slams into noexcept
            }
        }

        default_block(const default_block&) = delete;
        default_block& operator=(const default_block&) = delete;
    };

    std::wstring block;
};

struct create_process_result {
    handle<null_handle_policy> hProcess;
    handle<null_handle_policy> hThread;
    unsigned long dwProcessId;
    unsigned long dwThreadId;
};

inline create_process_result create_process(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo) {
    PROCESS_INFORMATION procInfo;
    if (!CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
            dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, &procInfo)) {
        api_failure("CreateProcessW");
    }

    return {handle<null_handle_policy>{procInfo.hProcess}, handle<null_handle_policy>{procInfo.hThread},
        procInfo.dwProcessId, procInfo.dwThreadId};
}

class thread_proc_attribute_list {
public:
    thread_proc_attribute_list()                             = default;
    thread_proc_attribute_list(thread_proc_attribute_list&&) = default;
    thread_proc_attribute_list& operator=(thread_proc_attribute_list&&) = default;

    explicit thread_proc_attribute_list(const unsigned long attributeCount) {
        size_t size;
        if (InitializeProcThreadAttributeList(nullptr, attributeCount, 0, &size)) {
            fputs("First call to InitializeProcThreadAttributeList should not succeed.", stderr);
            abort();
        }

        const auto lastError = GetLastError();
        if (lastError != ERROR_INSUFFICIENT_BUFFER) {
            api_failure("InitializeProcThreadAttributeList", lastError);
        }

        buffer = std::make_unique<unsigned char[]>(size);
        if (!InitializeProcThreadAttributeList(
                reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(buffer.get()), attributeCount, 0, &size)) {
            api_failure("InitializeProcThreadAttributeList");
        }
    }

    ~thread_proc_attribute_list() {
        DeleteProcThreadAttributeList(reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(buffer.get()));
    }

    void update_attribute(DWORD_PTR Attribute, PVOID lpValue, SIZE_T cbSize) {
        if (!UpdateProcThreadAttribute(reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(buffer.get()), 0, Attribute,
                lpValue, cbSize, nullptr, nullptr)) {
            api_failure("UpdateProcThreadAttribute");
        }
    }

    [[nodiscard]] LPPROC_THREAD_ATTRIBUTE_LIST get() const noexcept {
        return reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(buffer.get());
    }

private:
    std::unique_ptr<unsigned char[]> buffer;
};

struct subprocess_executive {
    subprocess_executive() = default;
    explicit subprocess_executive(const environment_block& environment_) : environment(environment_) {}
    explicit subprocess_executive(environment_block&& environment_) : environment(std::move(environment_)) {}

    [[nodiscard]] HANDLE get_wait_handle() const noexcept {
        return runningProcess.get();
    }

    void begin_execution(const wchar_t* const applicationName, wchar_t* const commandLine,
        const unsigned long creationFlags = 0, const wchar_t* const currentDirectory = nullptr) {
        thread_proc_attribute_list procAttributeList{2};

        // only inherit these pipe handles, not other handles that might be concurrently in use in this program
        HANDLE inheritTheseHandles[] = {no_input_pipe::instance().get(), output.get_write_pipe()};
        procAttributeList.update_attribute(
            PROC_THREAD_ATTRIBUTE_HANDLE_LIST, &inheritTheseHandles, sizeof(inheritTheseHandles));

        // turn on all reasonable corruption detecting mitigations
        unsigned long long mitigations = PROCESS_CREATION_MITIGATION_POLICY_DEP_ENABLE
                                         | PROCESS_CREATION_MITIGATION_POLICY_SEHOP_ENABLE
                                         | PROCESS_CREATION_MITIGATION_POLICY_FORCE_RELOCATE_IMAGES_ALWAYS_ON_REQ_RELOCS
                                         | PROCESS_CREATION_MITIGATION_POLICY_HEAP_TERMINATE_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_BOTTOM_UP_ASLR_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_HIGH_ENTROPY_ASLR_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_STRICT_HANDLE_CHECKS_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_WIN32K_SYSTEM_CALL_DISABLE_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_EXTENSION_POINT_DISABLE_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_CONTROL_FLOW_GUARD_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_FONT_DISABLE_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_REMOTE_ALWAYS_ON
                                         | PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_LOW_LABEL_ALWAYS_ON;
        procAttributeList.update_attribute(PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY, &mitigations, sizeof(mitigations));

        STARTUPINFOEXW startupInfo{};
        startupInfo.StartupInfo.cb          = sizeof(startupInfo);
        startupInfo.StartupInfo.dwFlags     = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        startupInfo.StartupInfo.hStdInput   = inheritTheseHandles[0];
        startupInfo.StartupInfo.hStdOutput  = inheritTheseHandles[1];
        startupInfo.StartupInfo.hStdError   = inheritTheseHandles[1];
        startupInfo.StartupInfo.wShowWindow = SW_HIDE;
        startupInfo.lpAttributeList         = procAttributeList.get();

        runningJob = create_job_that_will_be_killed_when_closed();

        auto procInfo = create_process(applicationName, commandLine, nullptr, nullptr, TRUE,
            creationFlags | CREATE_UNICODE_ENVIRONMENT | CREATE_SUSPENDED, environment.get(), currentDirectory,
            &startupInfo.StartupInfo);

        runningProcess = std::move(procInfo.hProcess);
        if (!AssignProcessToJobObject(runningJob.get(), runningProcess.get())) {
            api_failure("AssignProcessToJobObject");
        }

        if (ResumeThread(procInfo.hThread.get()) == static_cast<DWORD>(-1)) {
            api_failure("ResumeThread");
        }
    }

    execution_result complete() {
        DWORD exitCode;
        if (!GetExitCodeProcess(runningProcess.get(), &exitCode)) {
            api_failure("GetExitCodeProcess");
        }

        runningProcess.close();
        runningJob.close();

        return {exitCode, output.extract_and_reset()};
    }

private:
    output_collecting_pipe output;
    environment_block environment;
    handle<null_handle_policy> runningProcess;
    handle<null_handle_policy> runningJob;
};
