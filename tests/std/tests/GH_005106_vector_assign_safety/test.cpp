// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <iostream>
#include <ranges>
#include <vector>

struct throwing_t {
    int* throw_after_ = nullptr;
    throwing_t() {
        throw 0;
    }

    throwing_t(int& throw_after_n) : throw_after_(&throw_after_n) {
        if (throw_after_n == 0) {
            throw 0;
        }
        --throw_after_n;
    }

    throwing_t(const throwing_t& rhs) : throw_after_(rhs.throw_after_) {
        if (throw_after_ == nullptr || *throw_after_ == 0) {
            throw 1;
        }
        --*throw_after_;
    }

    throwing_t& operator=(const throwing_t& rhs) {
        throw_after_ = rhs.throw_after_;
        if (throw_after_ == nullptr || *throw_after_ == 0) {
            throw 1;
        }
        --*throw_after_;
        return *this;
    }

    friend bool operator==(const throwing_t& lhs, const throwing_t& rhs) {
        return lhs.throw_after_ == rhs.throw_after_;
    }
    friend bool operator!=(const throwing_t& lhs, const throwing_t& rhs) {
        return lhs.throw_after_ != rhs.throw_after_;
    }
};

template <class T, class IterCat>
struct throwing_iterator {
    using iterator_category = IterCat;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using reference         = T&;
    using pointer           = T*;

    int i_;
    T v_;

    constexpr throwing_iterator(int i = 0, const T& v = T()) : i_(i), v_(v) {}

    reference operator*() {
        if (i_ == 1) {
            throw 1;
        }
        return v_;
    }

    friend bool operator==(const throwing_iterator& lhs, const throwing_iterator& rhs) {
        return lhs.i_ == rhs.i_;
    }
    friend bool operator!=(const throwing_iterator& lhs, const throwing_iterator& rhs) {
        return lhs.i_ != rhs.i_;
    }

    throwing_iterator& operator++() {
        ++i_;
        return *this;
    }

    throwing_iterator operator++(int) {
        auto tmp = *this;
        ++i_;
        return tmp;
    }
};

template <class T, bool POCMA>
class toggle_pocma_allocator {
    template <class, bool>
    friend class toggle_pocma_allocator;

public:
    using propagate_on_container_move_assignment = std::integral_constant<bool, POCMA>;
    using value_type                             = T;

    template <class U>
    struct rebind {
        using other = toggle_pocma_allocator<U, POCMA>;
    };

    constexpr toggle_pocma_allocator(int id) : id_(id) {}

    template <class U>
    constexpr toggle_pocma_allocator(const toggle_pocma_allocator<U, POCMA>& other) : id_(other.id_) {}

    constexpr T* allocate(std::size_t n) {
        return std::allocator<T>().allocate(n);
    }

    constexpr void deallocate(T* p, std::size_t n) {
        std::allocator<T>().deallocate(p, n);
    }

    constexpr int id() const {
        return id_;
    }

    template <class U>
    constexpr friend bool operator==(const toggle_pocma_allocator& lhs, const toggle_pocma_allocator<U, POCMA>& rhs) {
        return lhs.id() == rhs.id();
    }

    template <class U>
    constexpr friend bool operator!=(const toggle_pocma_allocator& lhs, const toggle_pocma_allocator<U, POCMA>& rhs) {
        return !(lhs == rhs);
    }

private:
    int id_;
};

template <class T>
using pocma_allocator = toggle_pocma_allocator<T, true>;
template <class T>
using non_pocma_allocator = toggle_pocma_allocator<T, false>;


void test() {
    {
        int throw_after = 10;
        throwing_t t    = throw_after;
        std::vector<throwing_t> in(6, t);
        std::vector<throwing_t> v(3, t);
        try { // Throw in copy-assignment operator from element type during construction
            v = in;
        } catch (int) {
        }
        assert(v.size() == 3);
    }

    {
        int throw_after = 10;
        throwing_t t    = throw_after;
        non_pocma_allocator<throwing_t> alloc1(1);
        non_pocma_allocator<throwing_t> alloc2(2);
        std::vector<throwing_t, non_pocma_allocator<throwing_t>> in(6, t, alloc1);
        std::vector<throwing_t, non_pocma_allocator<throwing_t>> v(3, t, alloc2);
        try { // Throw in move-assignment operator from element type during construction
            v = std::move(in);
        } catch (int) {
        }
        assert(v.size() == 3);
    }

    {
        int throw_after = 10;
        throwing_t t    = throw_after;
        std::initializer_list<throwing_t> in{t, t, t, t, t, t};
        std::vector<throwing_t> v(3, t);
        try { // Throw in operator=(initializer_list<_Ty>) from element type during construction
            v = in;
        } catch (int) {
        }
        assert(v.size() == 3);
    }

    {
        std::vector<int> v(3);
        try { // Throw in assign(_Iter, _Iter) from forward iterator during construction
            v.assign(throwing_iterator<int, std::forward_iterator_tag>(),
                throwing_iterator<int, std::forward_iterator_tag>(6));
        } catch (int) {
        }
        assert(v.size() == 3);
    }

    {
        int throw_after = 10;
        throwing_t t    = throw_after;
        std::vector<throwing_t> in(6, t);
        std::vector<throwing_t> v(3, t);
        try { // Throw in assign(_Iter, _Iter) from element type during construction
            v.assign(in.begin(), in.end());
        } catch (int) {
        }
        assert(v.size() == 3);
    }

    {
        int throw_after = 10;
        throwing_t t    = throw_after;
        std::vector<throwing_t> in(6, t);
        std::vector<throwing_t> v(3, t);
        try { // Throw in assign_range(_Rng&&) from element type during construction
            v.assign_range(in);
        } catch (int) {
        }
        assert(v.size() == 3);
    }

    {
        int throw_after = 4;
        throwing_t t    = throw_after;
        std::vector<throwing_t> v(3, t);
        try { // Throw in assign(size_type, const _Ty&) from element type during construction
            v.assign(6, t);
        } catch (int) {
        }
        assert(v.size() == 3);
    }

    {
        int throw_after = 10;
        throwing_t t    = throw_after;
        std::initializer_list<throwing_t> in{t, t, t, t, t, t};
        std::vector<throwing_t> v(3, t);
        try { // Throw in assign(initializer_list<_Ty>) from element type during construction
            v.assign(in);
        } catch (int) {
        }
        assert(v.size() == 3);
    }
}

int main() {
    test();
}
