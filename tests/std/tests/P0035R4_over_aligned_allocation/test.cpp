// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _ENABLE_EXTENDED_ALIGNED_STORAGE
#define _HAS_DEPRECATED_TEMPORARY_BUFFER 1
#define _SILENCE_CXX17_TEMPORARY_BUFFER_DEPRECATION_WARNING
#define _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING

#include <array>
#include <cassert>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace {
    constexpr std::size_t max_align_log  = 14;
    constexpr std::size_t max_size_shift = 3;

    template <class T>
    T* check_alignment(T* const ptr) {
        void* vp         = ptr;
        std::size_t size = sizeof(T);
        assert(std::align(alignof(T), sizeof(T), vp, size) == ptr);
        return ptr;
    }

    template <std::size_t Size, std::size_t Align>
    void test_aligned_storage() {
        // test aligned_storage with extended alignments
        static_assert(Size >= Align);
        static_assert(Size % Align == 0);
        using X = std::aligned_storage_t<Size, Align>;
        static_assert(sizeof(X) == Size);
        static_assert(alignof(X) == Align);
    }

    template <std::size_t Size, std::size_t Align>
    void test_case() {
        test_aligned_storage<Size, Align>();

        struct T {
            alignas(Align) unsigned char space[Size];
        };

        std::array<T*, 1024> pointers;

        for (auto& ptr : pointers) {
            ptr = check_alignment(new T);
        }
        for (auto const& ptr : pointers) {
            delete ptr;
        }

        std::allocator<T> a;
        for (auto& ptr : pointers) {
            ptr = check_alignment(a.allocate(1));
        }
        for (auto const& ptr : pointers) {
            a.deallocate(ptr, 1);
        }

        for (auto& ptr : pointers) {
            ptr = check_alignment(std::get_temporary_buffer<T>(1).first);
        }
        for (auto const& ptr : pointers) {
            std::return_temporary_buffer<T>(ptr);
        }
    }

    template <std::size_t X, std::size_t... Ys>
    void test_single_alignment(std::index_sequence<Ys...>) {
        // ...test with size (1 << (x + y)) and alignment (1 << x)
        std::size_t const align = std::size_t{1} << X;
        (test_case<(align << Ys), align>(), ...);
    }

    template <std::size_t... Xs>
    void test_alignments(std::index_sequence<Xs...>) {
        // ...and for every y in [0, max_size_shift)...
        (test_single_alignment<Xs>(std::make_index_sequence<max_size_shift>{}), ...);
    }
} // unnamed namespace

int main() {
    // for every x in [0, max_align_log)...
    test_alignments(std::make_index_sequence<max_align_log>{});
}
