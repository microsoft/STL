// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <format>
#include <memory>
#include <shared_mutex>
#include <string>

// clang-format off
#include <initguid.h> // should be before any header that includes <guiddef.h>
#include <DbgEng.h>
// clang-format on

namespace {

    struct com_release_t {
        void operator()(auto* p) {
            p->Release();
        }
    };

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
        constexpr stacktrace_global_data_t() noexcept = default;
        constexpr ~stacktrace_global_data_t()         = default;

        stacktrace_global_data_t(const stacktrace_global_data_t&) = delete;
        stacktrace_global_data_t& operator=(const stacktrace_global_data_t) = delete;

        [[nodiscard]] std::string description(const void* const address) {
            clear_if_wrong_address(address);

            if (!is_description_valid) {
                bool success = try_initialize()
                            && SUCCEEDED(debug_symbols->GetNameByOffset(reinterpret_cast<uintptr_t>(address), buffer,
                                static_cast<ULONG>(std::size(buffer)), &buffer_size, &symbol_displacement));

                if (!success) {
                    buffer_size =
                        static_cast<ULONG>(std::format_to_n(buffer, std::size(buffer), "{}", address).out - buffer);
                    symbol_displacement = 0;
                }

                is_description_valid = true;
            }

            if (symbol_displacement != 0) {
                return std::format("{}+{:#x}", std::string_view(buffer, buffer_size), symbol_displacement);
            } else {
                return std::string(std::string_view(buffer, buffer_size));
            }
        }

        [[nodiscard]] std::string source_file(const void* const address) {
            initialize_line(address);

            return std::string(std::string_view(file_name, file_name_size));
        }

        [[nodiscard]] unsigned source_line(const void* const address) {
            initialize_line(address);

            return line;
        }

        [[nodiscard]] std::string address_to_string(const void* _Address) {

            auto cur_line = source_line(_Address);
            auto cur_desc = description(_Address);

            if (cur_line == 0) {
                return cur_desc;
            } else {
                return std::format("{}({}): {}", source_file(_Address), cur_line, cur_desc);
            }
        }


    private:
        bool try_initialize();

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
                    && SUCCEEDED(debug_symbols->GetLineByOffset(reinterpret_cast<uintptr_t>(address), &line, file_name,
                        static_cast<ULONG>(std::size(file_name)), &file_name_size, &line_displacement));
                if (!success) {
                    line              = 0;
                    file_name_size    = 0;
                    line_displacement = 0;
                }
                is_line_valid = true;
            }
        }


        IDebugClient* debug_client   = nullptr;
        IDebugSymbols* debug_symbols = nullptr;
        IDebugControl* debug_control = nullptr;
        bool attached                = false;
        bool initialize_attempted    = false;
        const void* last_address     = nullptr;
        bool is_description_valid    = false;
        bool is_line_valid           = false;
        char buffer[2000]            = {};
        ULONG buffer_size            = 0;
        ULONG64 symbol_displacement  = 0;
        ULONG line                   = 0;
        char file_name[MAX_PATH]     = {};
        ULONG file_name_size         = 0;
        ULONG64 line_displacement    = 0;
    };

    stacktrace_global_data_t stacktrace_global_data;
    SRWLOCK srw = SRWLOCK_INIT;

    bool stacktrace_global_data_t::try_initialize() {
        if (!initialize_attempted) {
            // Deliberately not calling CoInitialize[Ex]
            // DbgEng.h API works fine without it. COM initialization may have undesired interference with user's code

            if (SUCCEEDED(DebugCreate(IID_IDebugClient, reinterpret_cast<void**>(&debug_client)))
                && SUCCEEDED(debug_client->QueryInterface(IID_IDebugSymbols, reinterpret_cast<void**>(&debug_symbols)))
                && SUCCEEDED(
                    debug_client->QueryInterface(IID_IDebugControl, reinterpret_cast<void**>(&debug_control)))) {
                attached = SUCCEEDED(debug_client->AttachProcess(
                    0, GetCurrentProcessId(), DEBUG_ATTACH_NONINVASIVE | DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND));
                if (attached) {
                    debug_control->WaitForEvent(0, INFINITE);
                }
            }
        }

        atexit([] {
            // "Phoenix singleton" - destroy and set to null, so that can initialize later again

            if (stacktrace_global_data.debug_client != nullptr) {
                if (stacktrace_global_data.attached) {
                    stacktrace_global_data.debug_client->DetachProcesses();
                    stacktrace_global_data.attached = false;
                }

                stacktrace_global_data.debug_client->Release();
                stacktrace_global_data.debug_client = nullptr;
            }

            if (stacktrace_global_data.debug_control != nullptr) {
                stacktrace_global_data.debug_control->Release();
                stacktrace_global_data.debug_control = nullptr;
            }

            if (stacktrace_global_data.debug_symbols != nullptr) {
                stacktrace_global_data.debug_symbols->Release();
                stacktrace_global_data.debug_symbols = nullptr;
            }

            stacktrace_global_data.initialize_attempted = false;
        });

        initialize_attempted = true;

        return stacktrace_global_data.debug_symbols != nullptr;
    }


} // namespace

[[nodiscard]] unsigned short __stdcall __std_stacktrace_capture(
    unsigned long _FramesToSkip, unsigned long _FramesToCapture, void** _BackTrace, unsigned long* _BackTraceHash) {
#ifdef _DEBUG
    _FramesToSkip += 1; // compensate absense of tail call optimization here
#endif
    return CaptureStackBackTrace(_FramesToSkip, _FramesToCapture, _BackTrace, _BackTraceHash);
}

[[nodiscard]] std::string __stdcall __std_stacktrace_description(const void* _Address) {
    srw_lock_guard lock{srw};
    return stacktrace_global_data.description(_Address);
}

[[nodiscard]] std::string __stdcall __std_stacktrace_source_file(const void* _Address) {
    srw_lock_guard lock{srw};
    return stacktrace_global_data.source_file(_Address);
}

[[nodiscard]] unsigned __stdcall __std_stacktrace_source_line(const void* _Address) {
    srw_lock_guard lock{srw};
    return stacktrace_global_data.source_line(_Address);
}

[[nodiscard]] std::string __stdcall __std_stacktrace_address_to_string(const void* _Address) {
    srw_lock_guard lock{srw};
    return stacktrace_global_data.address_to_string(_Address);
}

[[nodiscard]] std::string __stdcall __std_stacktrace_to_string(const void* _Addresses, size_t _Size) {
    srw_lock_guard lock{srw};
    auto data = reinterpret_cast<const void* const*>(_Addresses);
    std::string result;
    for (std::size_t i = 0; i != _Size; ++i) {
        auto str = stacktrace_global_data.address_to_string(data[i]);
        if (!result.empty()) {
            result.push_back('\n');
            result.append(str);
        } else {
            result = std::move(str);
        }
    }
    return result;
}
