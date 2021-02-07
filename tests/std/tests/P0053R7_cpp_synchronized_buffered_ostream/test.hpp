// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <memory>
#include <streambuf>
#include <string>
#include <syncstream>
#include <type_traits>
#include <utility>

using namespace std;

constexpr size_t Min_size_allocation = 50;
constexpr size_t Min_syncbuf_size    = 32;

template <class Ty, bool ThrowOnSync = false>
class string_buffer : public basic_streambuf<Ty, char_traits<Ty>> { // represents the wrapped object in syncbuf
public:
    string_buffer()  = default;
    ~string_buffer() = default;

    streamsize xsputn(const Ty* ptr, streamsize n) override {
        str.append(ptr, static_cast<string::size_type>(n));
        return n;
    }

    int sync() override {
        if constexpr (ThrowOnSync) {
            return -1;
        } else {
            return 0;
        }
    }

    string str;
};

class small_size_allocation {
public:
    using size_type = size_t;

    [[nodiscard]] size_type max_size() const noexcept {
        return Min_size_allocation;
    }
};

template <class Ty>
class allocator_base {
public:
    using value_type = Ty;
    using pointer    = Ty*;

    [[nodiscard]] pointer allocate(const size_t n) {
        return allocator<value_type>{}.allocate(n);
    }

    void deallocate(const pointer ptr, const size_t n) noexcept {
        allocator<value_type>{}.deallocate(ptr, n);
    }
};

template <class Ty>
class fancy_ptr {
private:
    uint64_t ptr = 0;

public:
    fancy_ptr() = default;
    fancy_ptr(nullptr_t) {}
    explicit fancy_ptr(Ty* _ptr) : ptr(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(_ptr))) {}
    template <class Other>
    explicit fancy_ptr(const fancy_ptr<Other>& right) : ptr(right.ptr) {}

    Ty& operator*() const {
        return *reinterpret_cast<Ty*>(static_cast<uintptr_t>(ptr));
    }

    template <class Other>
    static fancy_ptr<Other> pointer_to(Other& obj) noexcept {
        return fancy_ptr<Other>(addressof(obj));
    }
};

template <class Ty>
class fancy_ptr_allocator {
public:
    using value_type                             = Ty;
    using pointer                                = fancy_ptr<Ty>;
    using size_type                              = size_t;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = true_type;

    constexpr fancy_ptr_allocator() noexcept = default;

    template <class Other>
    constexpr fancy_ptr_allocator(const fancy_ptr_allocator<Other>&) noexcept {}

    [[nodiscard]] pointer allocate(const size_t n) {
        auto ptr = allocator<value_type>{}.allocate(n);
        return static_cast<pointer>(ptr);
    }

    void deallocate(const pointer ptr, const size_t n) noexcept {
        allocator<value_type>{}.deallocate(&*ptr, n);
    }
};

template <class Ty, class Other>
[[nodiscard]] bool operator==(const fancy_ptr_allocator<Ty>&, const fancy_ptr_allocator<Other>&) noexcept {
    return true;
}

template <class Ty>
class small_size_fancy_ptr_allocator : public small_size_allocation {
public:
    using value_type                             = Ty;
    using pointer                                = fancy_ptr<Ty>;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = true_type;

    constexpr small_size_fancy_ptr_allocator() noexcept = default;

    template <class Other>
    constexpr small_size_fancy_ptr_allocator(const small_size_fancy_ptr_allocator<Other>&) noexcept {}

    [[nodiscard]] pointer allocate(const size_t n) {
        auto ptr = allocator<value_type>{}.allocate(n);
        return static_cast<pointer>(ptr);
    }

    void deallocate(const pointer ptr, const size_t n) noexcept {
        allocator<value_type>{}.deallocate(&*ptr, n);
    }
};

template <class Ty, class Other>
[[nodiscard]] bool operator==(
    const small_size_fancy_ptr_allocator<Ty>&, const small_size_fancy_ptr_allocator<Other>&) noexcept {
    return true;
}

template <class Ty>
class small_size_allocator : public allocator_base<Ty>, public small_size_allocation {
public:
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = true_type;

    constexpr small_size_allocator() noexcept = default;

    template <class Other>
    constexpr small_size_allocator(const small_size_allocator<Other>&) noexcept {}
};

template <class Ty, class Other>
[[nodiscard]] bool operator==(const small_size_allocator<Ty>&, const small_size_allocator<Other>&) noexcept {
    return true;
}

template <class Ty>
class non_move_assignable_non_equal_allocator : public allocator_base<Ty> {
public:
    using size_type                              = size_t;
    using propagate_on_container_move_assignment = false_type;
    using propagate_on_container_swap            = true_type;
    using is_always_equal                        = false_type;

    constexpr non_move_assignable_non_equal_allocator() noexcept = default;

    template <class Other>
    constexpr non_move_assignable_non_equal_allocator(const non_move_assignable_non_equal_allocator<Other>&) noexcept {}
};

template <class Ty, class Other>
[[nodiscard]] bool operator==(const non_move_assignable_non_equal_allocator<Ty>&,
    const non_move_assignable_non_equal_allocator<Other>&) noexcept {
    return false;
}

template <class Ty>
class non_move_assignable_equal_allocator : public allocator_base<Ty> {
public:
    using size_type                              = size_t;
    using propagate_on_container_move_assignment = false_type;
    using propagate_on_container_swap            = true_type;

    constexpr non_move_assignable_equal_allocator() noexcept = default;

    template <class Other>
    constexpr non_move_assignable_equal_allocator(const non_move_assignable_equal_allocator<Other>&) noexcept {}
};

template <class Ty, class Other>
[[nodiscard]] bool operator==(
    const non_move_assignable_equal_allocator<Ty>&, const non_move_assignable_equal_allocator<Other>&) noexcept {
    return true;
}

template <class Ty>
class non_swappable_equal_allocator : public allocator_base<Ty> {
public:
    using size_type                              = size_t;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = false_type;

    constexpr non_swappable_equal_allocator() noexcept = default;

    template <class Other>
    constexpr non_swappable_equal_allocator(const non_swappable_equal_allocator<Other>&) noexcept {}
};

template <class Ty, class Other>
[[nodiscard]] bool operator==(
    const non_swappable_equal_allocator<Ty>&, const non_swappable_equal_allocator<Other>&) noexcept {
    return true;
}
