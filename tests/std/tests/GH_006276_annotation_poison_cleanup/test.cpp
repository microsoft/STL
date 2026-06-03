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

#include <cstdio>
#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <test_asan_support.hpp>

using namespace std;

template <size_t ArenaSize, size_t AllocationAlignment>
class asan_unaware_arena {
    // In practice, custom memory pools should also be annotated for ASan.
    // For simplicity, we aren't doing that so we can directly see the
    // effects from only the container poisoning.
public:
    asan_unaware_arena() noexcept {
        fprintf(stderr, "Creating arena at %p with shadow at %p\n", _alloc_buffer, std_testing::asan::shadow_addr_of(_alloc_buffer));
    }

    ~asan_unaware_arena() noexcept {
        fputs("Shadow during destruction (should be all 00):\n", stderr);
        // Shadow should be cleared, otherwise the container has left scope leaving behind poisoned shadow bytes.
        std_testing::asan::verify_container_poisoning(_alloc_buffer, ArenaSize, ArenaSize, AllocationAlignment >= 8);
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
            _next_alloc, num_bytes, std_testing::asan::shadow_addr_of(result), std_testing::asan::shadow_addr_of(_next_alloc),
            std_testing::asan::shadow_addr_of(_next_alloc) - std_testing::asan::shadow_addr_of(result));

        return result;
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

    arena_reuse_allocator() noexcept : _arena(std::make_shared<asan_unaware_arena<AllocSize, Alignment>>()) {}

    template <typename OtherCharType>
    arena_reuse_allocator(const arena_reuse_allocator<OtherCharType, Size, Alignment>& rhs) noexcept
        : _arena(rhs._arena) {}

    T* allocate(size_t n) {
        return reinterpret_cast<T*>(_arena->allocate(n * sizeof(T)));
    }

    void deallocate(T*, size_t) noexcept {}

    std::shared_ptr<asan_unaware_arena<AllocSize, Alignment>> _arena;
};

const size_t arena_size = 256;

template <size_t Alignment>
void test_string_poisoning() {
    fprintf(stderr, "\nTesting string with allocator alignment of %zu\n", Alignment);

    basic_string<wchar_t, char_traits<wchar_t>, arena_reuse_allocator<wchar_t, arena_size, Alignment>> test_string(L"a 24 length string repr");
    fputs("Shadow after string constructor:\n", stderr);
    // Should not see any poisoning, since allocation size == string size.
    assert(std_testing::asan::verify_container_poisoning(test_string.data(), 
        (test_string.size() + 1) * sizeof(wchar_t), // +1 for null terminator 
        (test_string.capacity() + 1) * sizeof(wchar_t), // +1 for null terminator 
        Alignment >= 8));

    test_string.append(L"o"); // add any character to trigger resize
    fputs("Shadow after string resize:\n", stderr);
    // Should see poisoning, since the allocation should have resized more than +1.
    assert(std_testing::asan::verify_container_poisoning(test_string.data(), 
        (test_string.size() + 1) * sizeof(wchar_t), // +1 for null terminator 
        (test_string.capacity() + 1) * sizeof(wchar_t), // +1 for null terminator
        Alignment >= 8));
}

template <size_t Alignment>
void test_vector_poisoning() {
    fprintf(stderr, "\nTesting vector with allocator alignment of %zu\n", Alignment);

    vector<wchar_t, arena_reuse_allocator<wchar_t, arena_size, Alignment>> test_vector(23, L'a');
    fputs("Shadow after vector constructor:\n", stderr);
    // Should not see any poisoning, since allocation size == vector size.
    assert(std_testing::asan::verify_container_poisoning(test_vector.data(), 
        test_vector.size() * sizeof(wchar_t), 
        test_vector.capacity() * sizeof(wchar_t), 
        Alignment >= 8));

    test_vector.push_back(L'o'); // trigger resize
    fputs("Shadow after vector resize:\n", stderr);
    // Should see poisoning, since allocation should have resized more than +1.
    assert(std_testing::asan::verify_container_poisoning(test_vector.data(), 
        test_vector.size() * sizeof(wchar_t), 
        test_vector.capacity() * sizeof(wchar_t), 
        Alignment >= 8));
}

template <size_t Alignment>
void test_string_poisoning_shrink() {
    fprintf(stderr, "\nTesting string shrink with allocator alignment of %zu\n", Alignment);

    basic_string<wchar_t, char_traits<wchar_t>, arena_reuse_allocator<wchar_t, arena_size, Alignment>> test_string(L"a 24 length string repr");
    fputs("Shadow after string constructor:\n", stderr);
    // Should not see any poisoning, since allocation size == string size.
    assert(std_testing::asan::verify_container_poisoning(test_string.data(), 
        (test_string.size() + 1) * sizeof(wchar_t), // +1 for null terminator
        (test_string.capacity() + 1) * sizeof(wchar_t), // +1 for null terminator
        Alignment >= 8));

    test_string.pop_back(); // trigger size reduction
    fputs("Shadow after string shrink:\n", stderr);
    // Should see poisoning, since the allocation should have shrunk more than +1.
    assert(std_testing::asan::verify_container_poisoning(test_string.data(), 
        (test_string.size() + 1) * sizeof(wchar_t), // +1 for null terminator
        (test_string.capacity() + 1) * sizeof(wchar_t), // +1 for null terminator
        Alignment >= 8));
}

template <size_t Alignment>
void test_vector_poisoning_shrink() {
    fprintf(stderr, "\nTesting vector shrink with allocator alignment of %zu\n", Alignment);

    vector<wchar_t, arena_reuse_allocator<wchar_t, arena_size, Alignment>> test_vector(16, L'a');
    fputs("Shadow after vector constructor:\n", stderr);
    // Should not see any poisoning, since allocation size == vector size.
    assert(std_testing::asan::verify_container_poisoning(test_vector.data(), 
        test_vector.size() * sizeof(wchar_t), 
        test_vector.capacity() * sizeof(wchar_t), 
        Alignment >= 8));

    test_vector.pop_back(); // trigger size reduction
    fputs("Shadow after vector shrink:\n", stderr);
    // Should see poisoning, since allocation should have shrunk more than +1.
    assert(std_testing::asan::verify_container_poisoning(test_vector.data(), 
        test_vector.size() * sizeof(wchar_t), 
        test_vector.capacity() * sizeof(wchar_t), 
        Alignment >= 8));
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

    test_string_poisoning_shrink<2>(); // under poisoned code path
    test_string_poisoning_shrink<8>(); // over poisoned code path

    test_vector_poisoning<2>(); // under poisoned code path
    test_vector_poisoning<8>(); // over poisoned code path

    test_vector_poisoning_shrink<2>(); // under poisoned code path
    test_vector_poisoning_shrink<8>(); // over poisoned code path

    return 0;
}

#endif // ^^^ no workaround ^^^
