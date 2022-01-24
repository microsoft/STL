// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <format>
#include <shared_mutex>
#include <string>

// clang-format off
#include <Shlwapi.h>
#include <Windows.h>
#include <dbghelp.h>
// clang-format on

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Shlwapi.lib")

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


        std::string description(const void* const address) {
            srw_lock_guard lock{srw};
            clear_if_wrong_address(address);

            if (!is_description_valid) {
                bool success = try_initialize()
                            && SymFromAddr(process_handle, reinterpret_cast<uintptr_t>(address), nullptr, &info);

                if (success) {
                    function_address.clear();
                    function_name = std::string_view(info.Name, info.NameLen);
                    offset        = reinterpret_cast<ptrdiff_t>(address) - static_cast<ptrdiff_t>(info.Address);
                } else {
                    function_address = std::format("{}", address);
                    function_name    = function_address;
                    offset           = 0;
                }

                if (!GetModuleHandleExW(
                        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                        reinterpret_cast<LPCWSTR>(address), &module_handle)) {
                    module_handle = nullptr;
                }

                if (module_handle) {
                    GetModuleFileNameA(module_handle, module_path, MAX_PATH);
                    module_name = PathFindFileNameA(module_path);
                } else {
                    module_name = "";
                }

                is_description_valid = true;
            }

            if (offset > 0) {
                return std::format("{}!{}+{:#x}", module_name, function_name, offset);

            } else {
                return std::format("{}!{}", module_name, function_name);
            }
        }

        std::string source_file(const void* const address) {
            srw_lock_guard lock{srw};

            initialize_line(address);

            return line.FileName ? line.FileName : "";
        }

        unsigned source_line(const void* const address) {
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

        void clear_if_wrong_address(const void* const address) {
            if (last_address != address) {
                is_description_valid = false;
                is_line_valid        = false;
                last_address         = address;
            }
        }

        void initialize_line(const void* const address) {
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
        const void* last_address  = nullptr;
        HMODULE module_handle     = nullptr;
        bool is_description_valid = false;
        bool is_line_valid        = false;
        IMAGEHLP_LINE line        = {sizeof(IMAGEHLP_LINE)};
        DWORD displacement        = 0;
        ptrdiff_t offset          = 0;
        SYMBOL_INFO info          = init_symbol_info();
        char buffer[max_line_size];
        char module_path[MAX_PATH];
        const char* module_name = nullptr;
        std::string_view function_name;
        std::string function_address;
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

[[nodiscard]] std::string __stdcall __std_stacktrace_description(const void* _Address) {
    return stacktrace_global_data.description(_Address);
}

[[nodiscard]] std::string __stdcall __std_stacktrace_source_file(const void* _Address) {
    return stacktrace_global_data.source_file(_Address);
}

[[nodiscard]] unsigned __stdcall __std_stacktrace_source_line(const void* _Address) {
    return stacktrace_global_data.source_line(_Address);
}

[[nodiscard]] std::string __stdcall __std_stacktrace_address_to_string(const void* _Address) {
    std::string result;
    auto line = __std_stacktrace_source_line(_Address);
    auto desc = __std_stacktrace_description(_Address);

    if (!desc.empty()) {
        if (line == 0) {
            return desc;
        } else {
            return std::format("{}({}): {}", __std_stacktrace_source_file(_Address), line, desc);
        }
    }

    return result;
}

[[nodiscard]] std::string __stdcall __std_stacktrace_to_string(const void* _Addresses, size_t _Size) {
    auto data = reinterpret_cast<const void* const*>(_Addresses);
    std::string result;
    for (std::size_t i = 0; i != _Size; ++i) {
        auto str = __std_stacktrace_address_to_string(data[i]);
        if (!result.empty()) {
            result.push_back('\n');
            result.append(str);
        } else {
            result = std::move(str);
        }
    }
    return result;
}
