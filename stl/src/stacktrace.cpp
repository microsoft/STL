// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <format>
#include <memory>
#include <shared_mutex>

// clang-format off
#include <initguid.h> // should be before any header that includes <guiddef.h>
#include <DbgEng.h>
// clang-format on

#pragma comment(lib, "DbgEng.lib")

// The below function pointer types be in sync with <stacktrace>

using _Stacktrace_string_fill_callback = size_t (*)(char*, size_t, void* _Context);

using _Stacktrace_string_fill = size_t (*)(size_t, void* _Str, void* _Context, _Stacktrace_string_fill_callback);

namespace {
    template <class F>
    size_t string_fill(_Stacktrace_string_fill callback, size_t size, void* str, F f) {
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
        SRWLOCK* locked;
    };

    IDebugClient* debug_client   = nullptr;
    IDebugSymbols* debug_symbols = nullptr;
    IDebugControl* debug_control = nullptr;
    bool attached                = false;
    bool initialize_attempted    = false;
    SRWLOCK srw                  = SRWLOCK_INIT;

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
            }
        }

        atexit([] {
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
        });

        initialize_attempted = true;

        return debug_symbols != nullptr;
    }

    size_t get_description(const void* const address, void* str, size_t off, _Stacktrace_string_fill fill) {
        if (!try_initialize()) {
            return 0;
        }

        size_t size          = 20; // a guess, will retry with greater if wrong
        HRESULT hr           = E_UNEXPECTED;
        ULONG64 displacement = 0;

        for (;;) {
            ULONG new_size = 0;

            size_t new_off = string_fill(
                fill, off + size, str, [address, off, size, &new_size, &hr, &displacement](char* s, size_t) {
                    hr = debug_symbols->GetNameByOffset(reinterpret_cast<uintptr_t>(address), s + off,
                        static_cast<ULONG>(size), &new_size, &displacement);

                    return (hr == S_OK) ? off + new_size - 1 : off;
                });

            if (hr == S_OK) {
                off = new_off;
                break;
            } else if (hr == S_FALSE) {
                size = new_size; // retry with bigger buffer
            } else {
                return off;
            }
        }

        if (displacement != 0) {
            constexpr size_t max_disp_num = std::size("+0x1111222233334444") - 1; // maximum possible line number

            off = string_fill(fill, off + max_disp_num, str, [displacement, off](char* s, size_t) {
                return std::format_to_n(s + off, max_disp_num, "+{:#x}", displacement).out - s;
            });
        }

        return off;
    }

    size_t source_file(const void* const address, void* str, size_t off, ULONG* line, _Stacktrace_string_fill fill) {
        if (!try_initialize()) {
            return 0;
        }

        HRESULT hr  = E_UNEXPECTED;
        size_t size = 20; // a guess, will retry with greater if wrong

        for (;;) {
            ULONG new_size = 0;

            size_t new_off =
                string_fill(fill, off + size, str, [address, off, size, line, &new_size, &hr](char* s, size_t) {
                    hr = debug_symbols->GetLineByOffset(reinterpret_cast<uintptr_t>(address), line, s + off,
                        static_cast<ULONG>(size), &new_size, nullptr);

                    return (hr == S_OK) ? off + new_size - 1 : off;
                });

            if (hr == S_OK) {
                off = new_off;
                break;
            } else if (hr == S_FALSE) {
                size = new_size; // retry with bigger buffer
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
        if (!try_initialize()) {
            return 0;
        }

        ULONG line = 0;

        if (FAILED(debug_symbols->GetLineByOffset(
                reinterpret_cast<uintptr_t>(address), &line, nullptr, 0, nullptr, nullptr))) {
            return 0;
        }

        return line;
    }

    size_t address_to_string(const void* address, void* str, size_t off, _Stacktrace_string_fill fill) {
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
[[nodiscard]] unsigned short __stdcall __std_stacktrace_capture(
    unsigned long _FramesToSkip, unsigned long _FramesToCapture, void** _BackTrace, unsigned long* _BackTraceHash) {
#ifdef _DEBUG
    _FramesToSkip += 1; // compensate absense of tail call optimization here
#endif
    return CaptureStackBackTrace(_FramesToSkip, _FramesToCapture, _BackTrace, _BackTraceHash);
}

void __stdcall __std_stacktrace_description(const void* _Address, void* _Str, _Stacktrace_string_fill _Fill) {
    srw_lock_guard lock{srw};

    get_description(_Address, _Str, 0, _Fill);
}

void __stdcall __std_stacktrace_source_file(const void* _Address, void* _Str, _Stacktrace_string_fill _Fill) {
    srw_lock_guard lock{srw};

    source_file(_Address, _Str, 0, nullptr, _Fill);
}

unsigned __stdcall __std_stacktrace_source_line(const void* _Address) {
    srw_lock_guard lock{srw};

    return source_line(_Address);
}

void __stdcall __std_stacktrace_address_to_string(const void* _Address, void* _Str, _Stacktrace_string_fill _Fill) {
    srw_lock_guard lock{srw};

    address_to_string(_Address, _Str, 0, _Fill);
}

void __stdcall __std_stacktrace_to_string(
    const void* _Addresses, size_t _Size, void* _Str, _Stacktrace_string_fill _Fill) {
    srw_lock_guard lock{srw};

    auto data = reinterpret_cast<const void* const*>(_Addresses);

    size_t off = 0;

    for (std::size_t i = 0; i != _Size; ++i) {
        if (off != 0) {
            off = string_fill(_Fill, off + 1, _Str, [](char* s, size_t sz) {
                s[sz - 1] = '\n';
                return sz;
            });
        }
        off = address_to_string(data[i], _Str, off, _Fill);
    }
}
_END_EXTERN_C
