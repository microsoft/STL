// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <shared_mutex>
#include <string>

// clang-format off
#include <Windows.h>
#include <dbghelp.h>
// clang-format on

#pragma comment(lib, "Dbghelp.lib")

namespace {
    static constexpr std::size_t max_line_size = 2000;

    class _NODISCARD srw_lock_guard {
    public:
        explicit srw_lock_guard(SRWLOCK& locked_) noexcept : locked(&locked_) {
            AcquireSRWLockExclusive(locked);
        }

        ~srw_lock_guard() {
            ReleaseSRWLockExclusive(locked);
        }

        srw_lock_guard(const srw_lock_guard&) = delete;
        srw_lock_guard& operator=(const srw_lock_guard&) = delete;

    private:
        SRWLOCK* locked;
    };

    class stacktrace_global_data_t {
    public:
        constexpr stacktrace_global_data_t() = default;

        ~stacktrace_global_data_t() {
            if (initialized) {
                SymCleanup(process_handle);
            }

            if (process_handle) {
                CloseHandle(process_handle);
            }
        }

        stacktrace_global_data_t(const stacktrace_global_data_t&) = delete;
        stacktrace_global_data_t& operator=(const stacktrace_global_data_t) = delete;


        std::string description(void* address) {
            srw_lock_guard lock{srw};
            clear_if_wrong_address(address);

            if (!is_description_valid) {
                bool success = try_initialize()
                            && SymFromAddr(process_handle, reinterpret_cast<uintptr_t>(address), nullptr, &info);
                if (!success) {
                    info.NameLen = 0;
                }
                is_description_valid = true;
            }

            return std::string(info.Name, info.NameLen);
        }

        std::string source_file(void* address) {
            srw_lock_guard lock{srw};

            initialize_line(address);

            return line.FileName ? line.FileName : "";
        }

        unsigned source_line(void* address) {
            srw_lock_guard lock{srw};

            initialize_line(address);

            return line.LineNumber;
        }

    private:
        bool try_initialize() {
            if (!initialize_attempted) {
                initialized = DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(),
                                  &process_handle, PROCESS_QUERY_INFORMATION, false, 0)
                           && SymInitialize(process_handle, nullptr, true);

                initialize_attempted = true;
            }

            return initialized;
        }

        void clear_if_wrong_address(void* address) {
            if (last_address != address) {
                is_description_valid = false;
                is_line_valid        = false;
                last_address         = address;
            }
        }

        void initialize_line(void* address) {
            clear_if_wrong_address(address);

            if (!is_line_valid) {
                bool success =
                    try_initialize()
                    && SymGetLineFromAddr(process_handle, reinterpret_cast<uintptr_t>(address), &displacement, &line);
                if (!success) {
                    line.FileName   = nullptr;
                    line.LineNumber = 0;
                    displacement    = 0;
                }
                is_line_valid = true;
            }
        }

        static constexpr SYMBOL_INFO init_symbol_info() {
            SYMBOL_INFO result  = {};
            result.SizeOfStruct = sizeof(SYMBOL_INFO);
            result.MaxNameLen   = max_line_size;
            return result;
        }

        SRWLOCK srw               = SRWLOCK_INIT;
        HANDLE process_handle     = nullptr;
        bool initialized          = false;
        bool initialize_attempted = false;
        void* last_address        = nullptr;
        bool is_description_valid = false;
        bool is_line_valid        = false;
        IMAGEHLP_LINE line        = {sizeof(IMAGEHLP_LINE)};
        DWORD displacement        = 0;
        SYMBOL_INFO info          = init_symbol_info();
        wchar_t buffer[max_line_size];
    };

    static stacktrace_global_data_t stacktrace_global_data;

} // namespace

[[nodiscard]] unsigned short __stdcall __std_stacktrace_capture(
    unsigned long _FramesToSkip, unsigned long _FramesToCapture, void** _BackTrace, unsigned long* _BackTraceHash) {
#ifdef _DEBUG
    _FramesToSkip += 1; // compensate absense of tail call optimization here
#endif
    return CaptureStackBackTrace(_FramesToSkip, _FramesToCapture, _BackTrace, _BackTraceHash);
}

[[nodiscard]] _STD string __stdcall __std_stacktrace_description(void* _Address) {
    return stacktrace_global_data.description(_Address);
}

[[nodiscard]] _STD string __stdcall __std_stacktrace_source_file(void* _Address) {
    return stacktrace_global_data.source_file(_Address);
}

[[nodiscard]] unsigned __stdcall __std_stacktrace_source_line(void* _Address) {
    return stacktrace_global_data.source_line(_Address);
}
