// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <format>
#include <memory>

// clang-format off
#include <initguid.h> // should be before any header that includes <guiddef.h>
#include <DbgEng.h>
#include <Shlwapi.h>
// clang-format on

#pragma comment(lib, "DbgEng.lib")
#pragma comment(lib, "Shlwapi.lib")

// The below function pointer types be in sync with <stacktrace>

using _Stacktrace_string_fill_callback = size_t (*)(char*, size_t, void* _Context);

using _Stacktrace_string_fill = size_t (*)(size_t, void* _Str, void* _Context, _Stacktrace_string_fill_callback);

namespace {
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
    SRWLOCK srw                    = SRWLOCK_INIT;

    void uninitialize() {
        srw_lock_guard lock{srw};

        // "Phoenix singleton" - destroy and set to null, so that can initialize later again

        if (debug_client != nullptr) {
            if (attached) {
                debug_client->DetachProcesses();
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

        initialize_attempted = false;
    }

    bool try_initialize() {
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

                // If this failes, will use IDebugSymbols
                debug_symbols->QueryInterface(IID_IDebugSymbols3, reinterpret_cast<void**>(&debug_symbols3));
            }
        }

        if (atexit(uninitialize) != 0) {
            uninitialize();
            return false;
        }

        initialize_attempted = true;

        return debug_symbols != nullptr;
    }

    void ensure_module_symbols_loaded_from_current_dir(const void* const address) {
        ULONG index  = 0;
        ULONG64 base = 0;
        if (FAILED(debug_symbols->GetModuleByOffset(reinterpret_cast<uintptr_t>(address), 0, &index, &base))) {
            return;
        }

        DEBUG_MODULE_PARAMETERS params;
        if (FAILED(debug_symbols->GetModuleParameters(1, &base, index, &params))) {
            return;
        }

        if (params.SymbolType != DEBUG_SYMTYPE_DEFERRED) {
            return;
        }

        if (debug_symbols3) {
            ULONG wide_name_size = 0;

            if (FAILED(debug_symbols3->GetModuleNameStringWide(
                    DEBUG_MODNAME_IMAGE, index, base, nullptr, 0, &wide_name_size))) {
                return;
            }

            auto image_path = std::make_unique<wchar_t[]>(wide_name_size);

            if (debug_symbols3->GetModuleNameStringWide(
                    DEBUG_MODNAME_IMAGE, index, base, image_path.get(), wide_name_size, nullptr)
                != S_OK) {
                return;
            }

            PathRemoveFileSpecW(image_path.get());

            debug_symbols3->AppendSymbolPathWide(image_path.get());
        } else {
            auto image_path = std::make_unique<char[]>(params.ImageNameSize);

            if (FAILED(debug_symbols->GetModuleNames(index, base, image_path.get(), params.ImageNameSize, nullptr,
                    nullptr, 0, nullptr, nullptr, 0, nullptr))) {
                return;
            }

            PathRemoveFileSpecA(image_path.get());

            debug_symbols->AppendSymbolPath(image_path.get());
        }
    }

    size_t get_description(const void* const address, void* const str, size_t off, const _Stacktrace_string_fill fill) {
        // Initially pass the current capacity, will retry with bigger buffer if fails.
        size_t size          = fill(0, str, nullptr, nullptr) - off;
        HRESULT hr           = E_UNEXPECTED;
        ULONG64 displacement = 0;

        for (;;) {
            ULONG new_size = 0;

            size_t new_off = string_fill(
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
            constexpr size_t max_disp_num = std::size("+0x1111222233334444") - 1; // maximum possible offset

            off = string_fill(fill, off + max_disp_num, str, [displacement, off](char* s, size_t) {
                return std::format_to_n(s + off, max_disp_num, "+{:#x}", displacement).out - s;
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

            size_t new_off =
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

    [[nodiscard]] unsigned source_line(const void* const address) {
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
            constexpr size_t max_line_num = std::size("(4294967295): ") - 1; // maximum possible line number

            off = string_fill(fill, off + max_line_num, str, [line, off](char* s, size_t) {
                return std::format_to_n(s + off, max_line_num, "({}): ", line).out - s;
            });
        }

        return get_description(address, str, off, fill);
    }

} // namespace

_EXTERN_C
[[nodiscard]] unsigned short __stdcall __std_stacktrace_capture(unsigned long _FramesToSkip,
    const unsigned long _FramesToCapture, void** const _BackTrace, unsigned long* const _BackTraceHash) noexcept {
#ifdef _DEBUG
    _FramesToSkip += 1; // compensate absense of tail call optimization here
#endif
    return CaptureStackBackTrace(_FramesToSkip, _FramesToCapture, _BackTrace, _BackTraceHash);
}

// Some of these exports may throw (They would propagate bad_alloc potentially thrown from string::resize_and_overwrite)

void __stdcall __std_stacktrace_description(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return;
    }

    ensure_module_symbols_loaded_from_current_dir(_Address);

    get_description(_Address, _Str, 0, _Fill);
}

void __stdcall __std_stacktrace_source_file(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return;
    }

    ensure_module_symbols_loaded_from_current_dir(_Address);

    source_file(_Address, _Str, 0, nullptr, _Fill);
}

unsigned __stdcall __std_stacktrace_source_line(const void* const _Address) noexcept(false) {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return;
    }

    ensure_module_symbols_loaded_from_current_dir(_Address);

    return source_line(_Address);
}

void __stdcall __std_stacktrace_address_to_string(
    const void* const _Address, void* const _Str, const _Stacktrace_string_fill _Fill) noexcept(false) {
    const srw_lock_guard lock{srw};

    if (!try_initialize()) {
        return;
    }

    ensure_module_symbols_loaded_from_current_dir(_Address);

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

    for (std::size_t i = 0; i != _Size; ++i) {
        if (off != 0) {
            off = string_fill(_Fill, off + 1, _Str, [](char* s, size_t sz) {
                s[sz - 1] = '\n';
                return sz;
            });
        }

        ensure_module_symbols_loaded_from_current_dir(data[i]);

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
