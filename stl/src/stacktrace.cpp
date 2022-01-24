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

    static constexpr std::size_t table_size_power_of_two_factor = 10;

    static constexpr std::size_t table_size = 1 << table_size_power_of_two_factor;

    static constexpr SYMBOL_INFO init_symbol_info() {
        SYMBOL_INFO result  = {};
        result.SizeOfStruct = sizeof(SYMBOL_INFO);
        result.MaxNameLen   = max_line_size;
        return result;
    }

    struct stacktrace_address_table_entry_t {
        void* address             = nullptr;
        bool is_description_valid = false;
        bool is_line_valid        = false;
        IMAGEHLP_LINE line        = {sizeof(IMAGEHLP_LINE)};
        SYMBOL_INFO info          = init_symbol_info();
        wchar_t buffer[max_line_size];
    };

    static stacktrace_address_table_entry_t stacktrace_address_table[table_size];

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
        stacktrace_global_data_t() = default;

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

        static stacktrace_address_table_entry_t& hash_table_entry(void* address) {
            auto index = reinterpret_cast<size_t>(address);
            index ^= index >> table_size_power_of_two_factor;
            index &= (1 << table_size_power_of_two_factor) - 1;
            return stacktrace_address_table[index];
        }

        std::string description(void* address) {
            auto& entry = hash_table_entry(address);
            srw_lock_guard lock{srw};
            clear_if_wrong_address(entry, address);

            if (!entry.is_description_valid) {
                bool success = try_initialize()
                            && SymFromAddr(process_handle, reinterpret_cast<uintptr_t>(address), nullptr, &entry.info);
                if (!success) {
                    entry.info.NameLen = 0;
                }
                entry.is_description_valid = true;
            }

            return std::string(entry.info.Name, entry.info.NameLen);
        }

        std::string source_file(void* address) {
            auto& entry = hash_table_entry(address);
            srw_lock_guard lock{srw};

            initialize_line(entry, address);

            return entry.line.FileName ? entry.line.FileName : "";
        }

        unsigned source_line(void* address) {
            auto& entry = hash_table_entry(address);
            srw_lock_guard lock{srw};

            initialize_line(entry, address);

            return entry.line.LineNumber;
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

        void clear_if_wrong_address(stacktrace_address_table_entry_t& entry, void* address) {
            if (entry.address != address) {
                entry.is_description_valid = false;
                entry.is_line_valid        = false;
                entry.address              = address;
            }
        }

        void initialize_line(stacktrace_address_table_entry_t& entry, void* address) {
            clear_if_wrong_address(entry, address);

            if (!entry.is_line_valid) {
                DWORD d = 0;
                bool success =
                    try_initialize()
                    && SymGetLineFromAddr(process_handle, reinterpret_cast<uintptr_t>(address), &d, &entry.line);
                if (!success) {
                    entry.line.FileName   = nullptr;
                    entry.line.LineNumber = 0;
                }
                entry.is_line_valid = true;
            }
        }

        SRWLOCK srw               = SRWLOCK_INIT;
        HANDLE process_handle     = nullptr;
        bool initialized          = false;
        bool initialize_attempted = false;
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
