// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1659496
#include <functional>
#include <future>
#include <memory>
#include <type_traits>
#include <utility>
#include <valarray>
#if _HAS_CXX17
#include <optional>
#endif // _HAS_CXX17

using namespace std;

template <class T>
struct adl_proof_allocator_provider {
    struct alloc;
};

// adl_proof_allocator<T> is equality-comparable even if T is ADL-incompatible.
template <class T>
using adl_proof_allocator = typename adl_proof_allocator_provider<T>::alloc;

template <class, class = void>
constexpr bool is_adl_proof_allocator = false;

template <class Alloc>
constexpr bool
    is_adl_proof_allocator<Alloc, void_t<typename adl_proof_allocator_provider<typename Alloc::value_type>::alloc>> =
        is_same_v<typename adl_proof_allocator_provider<typename Alloc::value_type>::alloc, Alloc>;

template <class T>
struct adl_proof_allocator_provider<T>::alloc {
    using value_type = T;

    template <class U>
    struct rebind {
        using other = typename adl_proof_allocator_provider<U>::alloc;
    };

    alloc() = default;
    template <class OtherAlloc, enable_if_t<is_adl_proof_allocator<OtherAlloc>, int> = 0>
    constexpr alloc(const OtherAlloc&) noexcept {}

    T* allocate(const size_t n) {
        return allocator<T>{}.allocate(n);
    }

#if _HAS_CXX23
    allocation_result<T*> allocate_at_least(const size_t n) {
        return allocator<T>{}.allocate_at_least(n);
    }
#endif // _HAS_CXX23

    void deallocate(T* const p, const size_t n) {
        return allocator<T>{}.deallocate(p, n);
    }

    template <class OtherAlloc, enable_if_t<is_adl_proof_allocator<OtherAlloc>, int> = 0>
    friend constexpr bool operator==(const alloc&, const OtherAlloc&) noexcept {
        return true;
    }
#if !_HAS_CXX20
    template <class OtherAlloc, enable_if_t<is_adl_proof_allocator<OtherAlloc>, int> = 0>
    friend constexpr bool operator!=(const alloc&, const OtherAlloc&) noexcept {
        return false;
    }
#endif // !_HAS_CXX20
};

template <class T>
struct holder {
    T t;
};

struct incomplete;

template <class Tag>
struct tagged_identity {
    template <class U>
    constexpr U&& operator()(U&& u) const noexcept {
        return std::forward<U>(u);
    }
};

template <class Tag>
struct tagged_large_identity {
    template <class U>
    constexpr U&& operator()(U&& u) const noexcept {
        return std::forward<U>(u);
    }

    alignas(64) unsigned char unused[64]{};
};

using validator                 = holder<incomplete>*;
using validating_identity       = tagged_identity<holder<incomplete>>;
using validating_large_identity = tagged_large_identity<holder<incomplete>>;

using simple_identity       = tagged_identity<void>;
using simple_large_identity = tagged_large_identity<void>;

void test_function() {
    function<void(validator)>{};
    function<void(validator)>{nullptr};
    function<void(validator)>{simple_identity{}};
    function<void(validator)>{simple_large_identity{}};

    function<void(int)>{validating_identity{}};
    function<void(int)>{validating_large_identity{}};

#if !_HAS_CXX17
    function<void(validator)>{allocator_arg, adl_proof_allocator<unsigned char>{}};
    function<void(validator)>{allocator_arg, adl_proof_allocator<unsigned char>{}, nullptr};
    function<void(validator)>{allocator_arg, adl_proof_allocator<unsigned char>{}, simple_identity{}};
    function<void(validator)>{allocator_arg, adl_proof_allocator<unsigned char>{}, simple_large_identity{}};

    function<void(int)>{allocator_arg, adl_proof_allocator<unsigned char>{}, validating_identity{}};
    function<void(int)>{allocator_arg, adl_proof_allocator<unsigned char>{}, validating_large_identity{}};
#endif // !_HAS_CXX17
}

void test_packaged_task() {
    packaged_task<void(validator)>{};
    packaged_task<void(validator)>{simple_identity{}};
    packaged_task<void(validator)>{simple_large_identity{}};

    packaged_task<void(int)>{validating_identity{}};
    packaged_task<void(int)>{validating_large_identity{}};

#if !_HAS_CXX17
    packaged_task<void(validator)>{allocator_arg, adl_proof_allocator<unsigned char>{}, simple_identity{}};
    packaged_task<void(validator)>{allocator_arg, adl_proof_allocator<unsigned char>{}, simple_large_identity{}};

    packaged_task<void(int)>{allocator_arg, adl_proof_allocator<unsigned char>{}, validating_identity{}};
    packaged_task<void(int)>{allocator_arg, adl_proof_allocator<unsigned char>{}, validating_large_identity{}};
#endif // !_HAS_CXX17
}

void test_promise() {
    promise<validator>{};
    promise<validator>{allocator_arg, adl_proof_allocator<unsigned char>{}};

    promise<validator&>{};
    promise<validator&>{allocator_arg, adl_proof_allocator<unsigned char>{}};
}

void test_valarray() {
    using validator_class = holder<validator>;

    valarray<validator_class> valarr1(42);

    validator_class a[1]{};
    valarray<validator_class> valarr2(a, 1);
    valarr2.resize(172, a[0]);

    valarray<validator_class> valarr3(a[0], 1);
    valarr3 = valarr2[slice{0, 1, 1}];

    auto valarr4 = valarr1;
    valarr4      = valarr1;

    auto valarr5 = std::move(valarr2);
    valarr5      = std::move(valarr3);
}

#if _HAS_CXX17
void test_optional() {
    optional<validator> o{};
    o.emplace();
    o = optional<validator>{};
}
#endif // _HAS_CXX17

#if _HAS_CXX23
void test_move_only_function() {
    move_only_function<void(validator) const>{simple_identity{}};
    move_only_function<void(validator) const>{simple_large_identity{}};
    move_only_function<void(int) const>{validating_identity{}};
    move_only_function<void(int) const>{validating_large_identity{}};
}
#endif // _HAS_CXX23
#endif // ^^^ no workaround ^^^
