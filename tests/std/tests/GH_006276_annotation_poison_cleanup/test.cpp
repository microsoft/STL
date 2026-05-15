// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: x64 || x86 || arm64

#if defined(__clang__) && defined(_M_ARM64) // TRANSITION, LLVM-184902, fixed in Clang 23
#pragma comment(linker, "/INFERASANLIBS")
int main() {}
#else // ^^^ workaround / no workaround vvv

#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension
#pragma warning(disable : 4324) // '%s': structure was padded due to alignment specifier
#pragma warning(disable : 4365) // '%s': conversion from '%s' to '%s', signed/unsigned mismatch

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#define NO_SANITIZER_ADDRESS __attribute__((no_sanitize_address))
#else // ^^^ clang / msvc vvv
#define NO_SANITIZER_ADDRESS __declspec(no_sanitize_address)
#endif // __clang__

#include <stdio.h>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

extern "C" uintptr_t __asan_shadow_memory_dynamic_address;

NO_SANITIZER_ADDRESS unsigned char* shadow_addr_of(const void* addr) {
    return (unsigned char*) (((uintptr_t) addr >> 3) + __asan_shadow_memory_dynamic_address);
}

NO_SANITIZER_ADDRESS unsigned char shadow_byte_of(const void* addr) {
    return *shadow_addr_of(addr);
}

NO_SANITIZER_ADDRESS void print_shadow_bytes(const void* addr, const size_t string_size) {
    for (size_t i = 0; i < string_size; i += 8) {
        printf("%02x ", shadow_byte_of(reinterpret_cast<const char*>(addr) + i));
    }
    printf("\n");
}

template <size_t ArenaSize, size_t AllocationAlignment>
class asan_unaware_arena {
    // In practice, custom memory pools should also be annotated for ASan.
    // For simplicity, we aren't doing that so we can directly see the
    // effects from only the container poisoning.
public:
    asan_unaware_arena() noexcept {
        fprintf(stderr, "Creating arena at %p with shadow at %p\n", _alloc_buffer, shadow_addr_of(_alloc_buffer));
    }

    ~asan_unaware_arena() noexcept {
        fprintf(stderr, "Shadow during destruction (should be all 00):\t");
        print_shadow();

        // Shadow should be cleared. If it isn't, this memset will trigger an ASan container-overflow error.
        // This will likely appear as unknown-error since partial poisoning relies on nearby
        // shadow bytes to determine error type.
        memset(_alloc_buffer, 0, ArenaSize);
    }

    asan_unaware_arena(const asan_unaware_arena&)            = delete;
    asan_unaware_arena& operator=(const asan_unaware_arena&) = delete;

    asan_unaware_arena(asan_unaware_arena&&)            = delete;
    asan_unaware_arena& operator=(asan_unaware_arena&&) = delete;

    char* allocate(size_t num_bytes) {
        if (_next_alloc + num_bytes > _alloc_buffer + ArenaSize) {
            throw bad_alloc();
        }

        char* result = _next_alloc;
        _next_alloc += num_bytes;
        _next_alloc = reinterpret_cast<char*>((reinterpret_cast<uintptr_t>(_next_alloc) + (AllocationAlignment - 1))
                                              & ~(AllocationAlignment - 1)); // align the next allocation pointer.

        fprintf(stderr, "Allocated %p -> %p (%zu bytes) from arena, %p -> %p (%zu bytes) in shadow\n", result,
            _next_alloc, num_bytes, shadow_addr_of(result), shadow_addr_of(_next_alloc),
            shadow_addr_of(_next_alloc) - shadow_addr_of(result));

        return result;
    }

    void print_shadow() const noexcept {
        print_shadow_bytes(_alloc_buffer, ArenaSize);
    }

private:
    alignas(AllocationAlignment) char _alloc_buffer[ArenaSize]{};
    char* _next_alloc = _alloc_buffer;
};

template <typename T, size_t AllocSize, size_t Alignment>
struct arena_reuse_allocator {
    using value_type                                           = T;
    static constexpr size_t Size                               = AllocSize;
    static constexpr size_t _Minimum_asan_allocation_alignment = Alignment;

    template <typename OtherCharType>
    struct rebind {
        using other = arena_reuse_allocator<OtherCharType, Size, Alignment>;
    };

    arena_reuse_allocator(asan_unaware_arena<AllocSize, Alignment>* a) noexcept : _arena(a) {}

    template <typename OtherCharType>
    arena_reuse_allocator(const arena_reuse_allocator<OtherCharType, Size, Alignment>& rhs) noexcept
        : _arena(rhs._arena) {}

    T* allocate(size_t n) {
        return reinterpret_cast<T*>(_arena->allocate(n * sizeof(T)));
    }

    void deallocate(T*, size_t) noexcept {}

    asan_unaware_arena<AllocSize, Alignment>* _arena;
};

const size_t arena_size = 256;

template <size_t Alignment>
void test_string_poisoning() {
    fprintf(stderr, "\nTesting string with allocator alignment of %zu\n", Alignment);

    asan_unaware_arena<arena_size, Alignment> test_arena;
    arena_reuse_allocator<wchar_t, arena_size, Alignment> alloc(&test_arena);

    basic_string<wchar_t, char_traits<wchar_t>, decltype(alloc)> test_string(L"a 24 length string repr", alloc);
    fprintf(stderr, "Shadow after string constructor:\t\t");
    test_arena.print_shadow();

    test_string.append(L"o"); // add any character to trigger resize
    fprintf(stderr, "Shadow after string resize:\t\t\t");
    test_arena.print_shadow();
}

template <size_t Alignment>
void test_vector_poisoning() {
    fprintf(stderr, "\nTesting vector with allocator alignment of %zu\n", Alignment);

    asan_unaware_arena<arena_size, Alignment> test_arena;
    arena_reuse_allocator<wchar_t, arena_size, Alignment> alloc(&test_arena);

    vector<wchar_t, decltype(alloc)> test_vector(23, L'a', alloc);
    fprintf(stderr, "Shadow after vector constructor:\t\t");
    test_arena.print_shadow();

    test_vector.push_back(L'o'); // trigger resize
    fprintf(stderr, "Shadow after vector resize:\t\t\t");
    test_arena.print_shadow();
}

int main() {
    // Annotations for std::string and std::vector follow different
    // paths based off allocation alignment, so test with both.

    // Alignment >= 8 is aligned with shadow bytes' 8-byte granularity, so string/vector
    // annotations try to maximize coverage by poisoning past the allocation, since the allocator
    // should not hand out that memory anyway.

    // Alignment < 8 is not aligned with shadow bytes, so string/vector annotations
    // are more conservative and only poison the memory that was handed out by the
    // allocator, leaving some at the end unpoisoned.

    test_string_poisoning<2>(); // under poisoned code path
    test_string_poisoning<8>(); // over poisoned code path

    test_vector_poisoning<2>(); // under poisoned code path
    test_vector_poisoning<8>(); // over poisoned code path

    return 0;
}

#endif // ^^^ no workaround ^^^
