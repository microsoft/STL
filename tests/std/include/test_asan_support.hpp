// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#define NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#else // ^^^ clang / msvc vvv
#define NO_SANITIZE_ADDRESS __declspec(no_sanitize_address)
#endif // __clang__

#ifdef __SANITIZE_ADDRESS__
#include <cassert>
#include <cstdio>

extern "C" uintptr_t __asan_shadow_memory_dynamic_address;
extern "C" void* __sanitizer_contiguous_container_find_bad_address(const void* beg, const void* mid, const void* end) noexcept;
extern "C" void __asan_describe_address(void*) noexcept;

namespace std_testing {
    namespace asan {
        constexpr uintptr_t shadow_granularity = 8;

        const char* round_down_to_shadow_granularity(const char* const addr) {
           return reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(addr) & ~(shadow_granularity - 1));
        }

        const char* round_up_to_shadow_granularity(const char* const addr) {
           return reinterpret_cast<const char*>((reinterpret_cast<uintptr_t>(addr) + shadow_granularity - 1) & ~(shadow_granularity - 1));
        }

        NO_SANITIZE_ADDRESS unsigned char* shadow_addr_of(const void* const addr) {
           return reinterpret_cast<unsigned char*>((reinterpret_cast<uintptr_t>(addr) >> 3) + __asan_shadow_memory_dynamic_address);
        }

        NO_SANITIZE_ADDRESS unsigned char shadow_byte_of(const void* addr) {
           return *shadow_addr_of(addr);
        }

        void print_shadow_bytes(const void* addr, size_t num_bytes, const void* error_addr = nullptr, unsigned char expected_shadow_byte = 0xff /*unused shadow byte*/) {
            constexpr size_t shadow_bytes_per_line = 16;
            constexpr uintptr_t bytes_per_line_mask = (shadow_bytes_per_line * shadow_granularity) - 1;

            const char* begin = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(addr) & ~(shadow_granularity - 1)); // align down to shadow boundary
            const char* end = reinterpret_cast<const char*>(addr) + num_bytes;
 
            if (error_addr) {
                assert(error_addr >= begin && error_addr <= end);
                fprintf(stderr, "ERROR: Expected %02x shadow byte at %p, but got %02x.\n", expected_shadow_byte, error_addr, shadow_byte_of(error_addr));
            }

            fprintf(stderr, "Shadow for addresses %p -> %p (%p -> %p):", begin, end, shadow_addr_of(begin), shadow_addr_of(end));
 
            const char* const print_begin = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(begin) - bytes_per_line_mask & ~bytes_per_line_mask);
            const char* const print_end = reinterpret_cast<const char*>((reinterpret_cast<uintptr_t>(end) + bytes_per_line_mask) & ~bytes_per_line_mask);
            for (const char* p = print_begin; p < print_end; p += 8) {
                if ((reinterpret_cast<uintptr_t>(p) & bytes_per_line_mask) == 0) {
                    fprintf(stderr, "\n %p: ", p);
                }
                if (reinterpret_cast<uintptr_t>(p) == (reinterpret_cast<uintptr_t>(error_addr) & ~(shadow_granularity - 1))) {
                    fprintf(stderr, "\b[%02x]", shadow_byte_of(p));
                } else {
                    fprintf(stderr, "%02x ", shadow_byte_of(p));
                }
            }
            fprintf(stderr, "\n");
        }

        bool verify_poisoning_cleared(void* ptr, size_t num_bytes) {
            const char* const begin = round_down_to_shadow_granularity(reinterpret_cast<const char*>(ptr));
            const char*       end   = reinterpret_cast<const char*>(ptr) + num_bytes;
            
            void* bad_addr = __sanitizer_contiguous_container_find_bad_address(begin, end, end); 
            if (bad_addr) {
                print_shadow_bytes(ptr, num_bytes, bad_addr, 0);
                __asan_describe_address(bad_addr);
                return false;
            }
            return true;
        }

        bool verify_container_poisoning(const void* const addr, const size_t valid_size, const size_t total_capacity, const bool is_overpoisoned) {
            const char* const begin = round_down_to_shadow_granularity(reinterpret_cast<const char*>(addr));
            const char* const mid   = reinterpret_cast<const char*>(addr) + valid_size;
            const char*       end   = reinterpret_cast<const char*>(addr) + total_capacity;

            if (is_overpoisoned) {
                end = round_up_to_shadow_granularity(end);
            }

            void* bad_addr = __sanitizer_contiguous_container_find_bad_address(begin, mid, end);
            if (bad_addr) {
                const char* const b1 = round_down_to_shadow_granularity(mid);
                const char* const b2 = round_up_to_shadow_granularity(mid);

                const unsigned char expected_byte = static_cast<unsigned char>(
                    (bad_addr >= b1 && bad_addr < b2) ? static_cast<unsigned char>(mid - b1) : (bad_addr < mid) ? 0 : 0xfc
                );

                print_shadow_bytes(addr, total_capacity, bad_addr, expected_byte);
                __asan_describe_address(bad_addr);
                return false;
            }
            return true;
        }
    } // namespace asan
} // namespace std_testing
#endif // __SANITIZER_ADDRESS__

