// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdio>
#include <cstdlib>

// clang-format off
#include <initguid.h> // should be before any header that includes <guiddef.h>
#include <DbgEng.h>
#include <DbgHelp.h>
#include <Shlwapi.h>
// clang-format on

// The below function pointer types must be in sync with <stacktrace>

using _Stacktrace_string_fill_callback = size_t(__stdcall*)(char* _Data, size_t _Size, void* _Context) _NOEXCEPT_FNPTR;

using _Stacktrace_string_fill = size_t(__stdcall*)(
    size_t _Size, void* _String, void* _Context, _Stacktrace_string_fill_callback _Callback);

namespace {
    template <class F>
    size_t string_fill(const _Stacktrace_string_fill callback, const size_t size, void* const str, F f) {
        return callback(size, str, &f,
            [](char* s, size_t sz, void* context) noexcept -> size_t { return (*static_cast<F*>(context))(s, sz); });
    }

    // TRANSITION, GH-2285. Use SRWLOCK instead of std::mutex to avoid nontrivial constructor and nontrivial destructor
    void lock_and_uninitialize() noexcept;

    class [[nodiscard]] dbg_eng_data {
    public:
        dbg_eng_data() noexcept {
            AcquireSRWLockExclusive(&srw);
        }

        ~dbg_eng_data() {
            ReleaseSRWLockExclusive(&srw);
        }

        dbg_eng_data(const dbg_eng_data&)            = delete;
        dbg_eng_data& operator=(const dbg_eng_data&) = delete;

        void uninitialize() noexcept {
            // "Phoenix singleton" - destroy and set to null, so that it can be initialized later again

            if (debug_client != nullptr) {
                if (attached) {
                    (void) debug_client->DetachProcesses();
                    attached = false;
                }

                debug_client->Release();
                debug_client = nullptr;
            }

            if (debug_control != nullptr) {
                debug_control->Release();
                debug_control = nullptr;
            }

            if (debug_symbols != nullptr) {
                debug_symbols->Release();
                debug_symbols = nullptr;
            }

            if (dbgeng != nullptr) {
                (void) FreeLibrary(dbgeng);
                dbgeng = nullptr;
            }

            initialize_attempted = false;
        }

        [[nodiscard]] bool try_initialize() noexcept {
            if (!initialize_attempted) {
                initialize_attempted = true;

                if (std::atexit(lock_and_uninitialize) != 0) {
                    return false;
                }

                dbgeng = LoadLibraryExW(L"dbgeng.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

                if (dbgeng != nullptr) {
                    const auto debug_create =
                        reinterpret_cast<decltype(&DebugCreate)>(GetProcAddress(dbgeng, "DebugCreate"));

                    // Deliberately not calling CoInitialize[Ex]. DbgEng.h API works fine without it.
                    // COM initialization may have undesired interference with user's code.
                    if (debug_create != nullptr
                        && SUCCEEDED(debug_create(IID_IDebugClient, reinterpret_cast<void**>(&debug_client)))
                        && SUCCEEDED(
                            debug_client->QueryInterface(IID_IDebugSymbols, reinterpret_cast<void**>(&debug_symbols)))
                        && SUCCEEDED(debug_client->QueryInterface(
                            IID_IDebugControl, reinterpret_cast<void**>(&debug_control)))) {
                        attached = SUCCEEDED(debug_client->AttachProcess(
                            0, GetCurrentProcessId(), DEBUG_ATTACH_NONINVASIVE | DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND));
                        if (attached) {
                            (void) debug_control->WaitForEvent(0, INFINITE);
                        }

                        constexpr ULONG add_options = SYMOPT_CASE_INSENSITIVE | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS
                                                    | SYMOPT_LOAD_LINES | SYMOPT_OMAP_FIND_NEAREST
                                                    | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_AUTO_PUBLICS
                                                    | SYMOPT_NO_PROMPTS;

                        constexpr ULONG remove_options = SYMOPT_NO_CPP | SYMOPT_LOAD_ANYTHING
                                                       | SYMOPT_NO_UNQUALIFIED_LOADS | SYMOPT_EXACT_SYMBOLS
                                                       | SYMOPT_IGNORE_NT_SYMPATH | SYMOPT_PUBLICS_ONLY
                                                       | SYMOPT_NO_PUBLICS | SYMOPT_NO_IMAGE_SEARCH;

                        (void) debug_symbols->AddSymbolOptions(add_options);
                        (void) debug_symbols->RemoveSymbolOptions(remove_options);
                    }
                }
            }

            return attached;
        }

        size_t get_description(
            const void* const address, void* const str, size_t off, const _Stacktrace_string_fill fill) {
            // Initially pass the current capacity, will retry with bigger buffer if it fails.
            size_t size          = fill(0, str, nullptr, nullptr) - off;
            HRESULT hr           = E_UNEXPECTED;
            ULONG64 displacement = 0;

            for (;;) {
                ULONG new_size = 0;

                const size_t new_off = string_fill(
                    fill, off + size, str, [address, off, size, &new_size, &hr, &displacement](char* s, size_t) {
                        hr = debug_symbols->GetNameByOffset(reinterpret_cast<uintptr_t>(address), s + off,
                            static_cast<ULONG>(size + 1), &new_size, &displacement);

                        return (hr == S_OK) ? off + new_size - 1 : off;
                    });

                if (hr == S_OK) {
                    off = new_off;
                    break;
                } else if (hr == S_FALSE) {
                    size = new_size - 1; // retry with bigger buffer
                } else {
                    return off;
                }
            }

            if (displacement != 0) {
                constexpr size_t max_disp_num = sizeof("+0x1122334455667788") - 1; // maximum possible offset

                off = string_fill(fill, off + max_disp_num, str, [displacement, off](char* s, size_t) {
                    const int ret = std::snprintf(s + off, max_disp_num, "+0x%llX", displacement);
                    if (ret <= 0) {
                        std::abort(); // formatting error
                    }
                    return off + ret;
                });
            }

            return off;
        }

        size_t source_file(const void* const address, void* const str, size_t off, ULONG* const line,
            const _Stacktrace_string_fill fill) {
            // Initially pass the current capacity, will retry with bigger buffer if fails.
            size_t size = fill(0, str, nullptr, nullptr) - off;
            HRESULT hr  = E_UNEXPECTED;

            for (;;) {
                ULONG new_size = 0;

                const size_t new_off =
                    string_fill(fill, off + size, str, [address, off, size, line, &new_size, &hr](char* s, size_t) {
                        hr = debug_symbols->GetLineByOffset(reinterpret_cast<uintptr_t>(address), line, s + off,
                            static_cast<ULONG>(size + 1), &new_size, nullptr);

                        return (hr == S_OK) ? off + new_size - 1 : off;
                    });

                if (hr == S_OK) {
                    off = new_off;
                    break;
                } else if (hr == S_FALSE) {
                    size = new_size - 1; // retry with bigger buffer
                } else {
                    if (line) {
                        *line = 0;
                    }

                    return off;
                }
            }

            return off;
        }

        [[nodiscard]] unsigned int source_line(const void* const address) noexcept {
            ULONG line = 0;

            if (FAILED(debug_symbols->GetLineByOffset(
                    reinterpret_cast<uintptr_t>(address), &line, nullptr, 0, nullptr, nullptr))) {
                return 0;
            }

            return line;
        }

        size_t address_to_string(
            const void* const address, void* const str, size_t off, const _Stacktrace_string_fill fill) {
            ULONG line = 0;

            off = source_file(address, str, off, &line, fill);

            if (line != 0) {
                constexpr size_t max_line_num = sizeof("(4294967295): ") - 1; // maximum possible line number

                off = string_fill(fill, off + max_line_num, str, [line, off](char* s, size_t) {
                    const int ret = std::snprintf(s + off, max_line_num, "(%u): ", line);
                    if (ret <= 0) {
                        std::abort(); // formatting error
                    }
                    return off + ret;
                });
            }

            return get_description(address, str, off, fill);
        }

    private:
        inline static SRWLOCK srw                  = SRWLOCK_INIT;
        inline static IDebugClient* debug_client   = nullptr;
        inline static IDebugSymbols* debug_symbols = nullptr;
        inline static IDebugControl* debug_control = nullptr;
        inline static bool attached                = false;
        inline static bool initialize_attempted    = false;
        inline static HMODULE dbgeng               = nullptr;
    };

    void lock_and_uninitialize() noexcept {
        dbg_eng_data locked_data;

        locked_data.uninitialize();
    }
} // namespace

_EXTERN_C
#pragma optimize("", off) // inhibit tail call optimization to have consistent _Frames_to_skip adjustment here
[[nodiscard]] unsigned short __stdcall __std_stacktrace_capture(unsigned long _Frames_to_skip,
    const unsigned long _Frames_to_capture, void** const _Back_trace, unsigned long* const _Back_trace_hash) noexcept {
    return CaptureStackBackTrace(_Frames_to_skip + 1, _Frames_to_capture, _Back_trace, _Back_trace_hash);
}
#pragma optimize("", on) // end inhibit tail call optimization

// Some of these functions may throw (They would propagate bad_alloc potentially thrown from
// string::resize_and_overwrite)

void __stdcall __std_stacktrace_description(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    dbg_eng_data locked_data;

    if (!locked_data.try_initialize()) {
        return;
    }

    locked_data.get_description(_Address, _Str, 0, _Fill);
}

void __stdcall __std_stacktrace_source_file(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    dbg_eng_data locked_data;

    if (!locked_data.try_initialize()) {
        return;
    }

    locked_data.source_file(_Address, _Str, 0, nullptr, _Fill);
}

[[nodiscard]] unsigned int __stdcall __std_stacktrace_source_line(const void* const _Address) noexcept {
    dbg_eng_data locked_data;

    if (!locked_data.try_initialize()) {
        return 0;
    }

    return locked_data.source_line(_Address);
}

void __stdcall __std_stacktrace_address_to_string(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    dbg_eng_data locked_data;

    if (!locked_data.try_initialize()) {
        return;
    }

    locked_data.address_to_string(_Address, _Str, 0, _Fill);
}

void __stdcall __std_stacktrace_to_string(const void* const* const _Addresses, const size_t _Size, void* const _Str,
    const _Stacktrace_string_fill _Fill) noexcept(false) {
    dbg_eng_data locked_data;

    if (!locked_data.try_initialize()) {
        return;
    }

    size_t off = 0;

    for (size_t i = 0; i != _Size; ++i) {
        if (off != 0) {
            off = string_fill(_Fill, off + 1, _Str, [](char* s, size_t sz) {
                s[sz - 1] = '\n';
                return sz;
            });
        }

        constexpr size_t max_entry_num = sizeof("65536> ") - 1; // maximum possible entry number

        off = string_fill(_Fill, off + max_entry_num, _Str, [off, i](char* s, size_t) {
            const int ret = std::snprintf(s + off, max_entry_num, "%u> ", static_cast<unsigned int>(i));
            if (ret <= 0) {
                std::abort(); // formatting error
            }
            return off + ret;
        });

        off = locked_data.address_to_string(_Addresses[i], _Str, off, _Fill);
    }
}
_END_EXTERN_C
