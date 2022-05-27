// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <yvals.h>

#include <cstdio>
#include <cstdlib>

// clang-format off
#include <initguid.h> // should be before any header that includes <guiddef.h>
#include <DbgEng.h>
#include <Shlwapi.h>
// clang-format on

#pragma comment(lib, "DbgEng.lib")
#pragma comment(lib, "Shlwapi.lib")

namespace {
    // The below function pointer types must be in sync with <stacktrace>

    using _Stacktrace_string_fill_callback = size_t(__stdcall*)(char* _Data, size_t _Size, void* _Context);

    using _Stacktrace_string_fill = size_t(__stdcall*)(
        size_t _Size, void* _String, void* _Context, _Stacktrace_string_fill_callback _Callback);

    template <class F>
    size_t string_fill(const _Stacktrace_string_fill callback, const size_t size, void* const str, F f) {
        return callback(size, str, &f,
            [](char* s, size_t sz, void* context) -> size_t { return (*static_cast<F*>(context))(s, sz); });
    }

    // TRANSITION, GH-2285. Use SRWLOCK instead of std::mutex to avoid nontrivial constructor and nontrivial destructor
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
        SRWLOCK* const locked;
    };

    IDebugClient* debug_client     = nullptr;
    IDebugSymbols* debug_symbols   = nullptr;
    IDebugSymbols3* debug_symbols3 = nullptr;
    IDebugControl* debug_control   = nullptr;
    bool attached                  = false;
    bool initialize_attempted      = false;
    HMODULE dbgeng                 = nullptr;
    SRWLOCK srw                    = SRWLOCK_INIT;

    void uninitialize() {
        srw_lock_guard lock{srw};

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

    bool try_initialize() {
        if (!initialize_attempted) {
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
                    && SUCCEEDED(
                        debug_client->QueryInterface(IID_IDebugControl, reinterpret_cast<void**>(&debug_control)))) {
                    attached = SUCCEEDED(debug_client->AttachProcess(
                        0, GetCurrentProcessId(), DEBUG_ATTACH_NONINVASIVE | DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND));
                    if (attached) {
                        (void) debug_control->WaitForEvent(0, INFINITE);
                    }

                    // If this fails, will use IDebugSymbols
                    (void) debug_symbols->QueryInterface(IID_IDebugSymbols3, reinterpret_cast<void**>(&debug_symbols3));

                    // clang-format off
                    constexpr ULONG add_options = 0x1     /* SYMOPT_CASE_INSENSITIVE */
                                                | 0x2     /* SYMOPT_UNDNAME */
                                                | 0x4     /* SYMOPT_DEFERRED_LOADS */
                                                | 0x10    /* SYMOPT_LOAD_LINES */
                                                | 0x20    /* SYMOPT_OMAP_FIND_NEAREST */
                                                | 0x100   /* SYMOPT_FAIL_CRITICAL_ERRORS */
                                                | 0x10000 /* SYMOPT_AUTO_PUBLICS */
                                                | 0x80000 /* SYMOPT_NO_PROMPTS */;

                    constexpr ULONG remove_options = 0x8     /* SYMOPT_NO_CPP */
                                                   | 0x40    /* SYMOPT_LOAD_ANYTHING */
                                                   | 0x100   /* SYMOPT_NO_UNQUALIFIED_LOADS */
                                                   | 0x400   /* SYMOPT_EXACT_SYMBOLS */
                                                   | 0x1000  /* SYMOPT_IGNORE_NT_SYMPATH */
                                                   | 0x4000  /* SYMOPT_PUBLICS_ONLY */
                                                   | 0x8000  /* SYMOPT_NO_PUBLICS */
                                                   | 0x20000 /* SYMOPT_NO_IMAGE_SEARCH */;
                    // clang-format on

                    (void) debug_symbols->AddSymbolOptions(add_options);
                    (void) debug_symbols->RemoveSymbolOptions(remove_options);
                }
            }
        }

        if (std::atexit(uninitialize) != 0) {
            uninitialize();
            return false;
        }

        initialize_attempted = true;

        return debug_symbols != nullptr;
    }

    size_t get_description(const void* const address, void* const str, size_t off, const _Stacktrace_string_fill fill) {
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
            constexpr size_t max_disp_num = sizeof("+0x1111222233334444") - 1; // maximum possible offset

            off = string_fill(fill, off + max_disp_num, str, [displacement, off](char* s, size_t) {
                const int ret = std::snprintf(s + off, max_disp_num, "+0x%llX", displacement);
                _STL_VERIFY(ret > 0, "formatting error");
                return off + ret;
            });
        }

        return off;
    }

    size_t source_file(
        const void* const address, void* const str, size_t off, ULONG* const line, const _Stacktrace_string_fill fill) {
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

    [[nodiscard]] unsigned int source_line(const void* const address) {
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
                _STL_VERIFY(ret > 0, "formatting error");
                return off + ret;
            });
        }

        return get_description(address, str, off, fill);
    }

} // namespace

_EXTERN_C
[[nodiscard]] unsigned short __stdcall __std_stacktrace_capture(unsigned long _Frames_to_skip,
    const unsigned long _Frames_to_capture, void** const _Back_trace, unsigned long* const _Back_trace_hash) noexcept {
#ifdef _DEBUG
    _Frames_to_skip += 1; // compensate for absence of tail call optimization here
#endif
    return CaptureStackBackTrace(_Frames_to_skip, _Frames_to_capture, _Back_trace, _Back_trace_hash);
}

// Some of these functions may throw (They would propagate bad_alloc potentially thrown from
// string::resize_and_overwrite)

void __stdcall __std_stacktrace_description(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return;
    }

    get_description(_Address, _Str, 0, _Fill);
}

void __stdcall __std_stacktrace_source_file(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return;
    }

    source_file(_Address, _Str, 0, nullptr, _Fill);
}

[[nodiscard]] unsigned int __stdcall __std_stacktrace_source_line(const void* const _Address) noexcept {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return 0;
    }

    return source_line(_Address);
}

void __stdcall __std_stacktrace_address_to_string(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return;
    }

    address_to_string(_Address, _Str, 0, _Fill);
}

void __stdcall __std_stacktrace_to_string(const void* const _Addresses, const size_t _Size, void* const _Str,
    const _Stacktrace_string_fill _Fill) noexcept(false) {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return;
    }

    const auto data = reinterpret_cast<const void* const*>(_Addresses);

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
            _STL_VERIFY(ret > 0, "formatting error");
            return off + ret;
        });

        off = address_to_string(data[i], _Str, off, _Fill);
    }
}

[[nodiscard]] void* __stdcall __std_stacktrace_get_debug_interface() noexcept {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return nullptr;
    }

    return debug_symbols;
}
_END_EXTERN_C
