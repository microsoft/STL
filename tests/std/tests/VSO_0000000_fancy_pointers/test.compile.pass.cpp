// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <deque>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <set>
#include <sstream>
#include <string>
#include <syncstream>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <instantiate_containers_iterators_common.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename T>
struct always_false : std::false_type {};

int main() {} // COMPILE-ONLY

template <typename Val>
class fancy_pointer {
public:
    typedef Val* rep_type;

    rep_type rep;
    unsigned char unused[4];

    fancy_pointer() : rep(nullptr) {}

    /* implicit */ fancy_pointer(std::nullptr_t) : rep(nullptr) {}
    /* implicit */ fancy_pointer(int) = delete;

    fancy_pointer& operator=(std::nullptr_t) {
        rep = nullptr;
        return *this;
    }
    fancy_pointer& operator=(int) = delete;

    template <typename UVal, std::enable_if_t<std::is_convertible_v<UVal*, Val*>, int> = 0>
    /* implicit */ fancy_pointer(const fancy_pointer<UVal>& other) : rep(other.rep) {}

    // allow explicit conversion from fancy_pointer<cv void> to fancy_pointer<T>
    template <typename UVal, std::enable_if_t<std::conjunction_v<std::negation<std::is_convertible<UVal*, Val*>>,
                                                  std::is_void<UVal>, std::negation<std::is_void<Val>>>,
                                 int> = 0>
    explicit fancy_pointer(const fancy_pointer<UVal>& other) : rep(static_cast<Val*>(other.rep)) {}

    // intentionally user-provided to test the Small String Optimization's union
    fancy_pointer(const fancy_pointer& other) : rep(other.rep) {}
    fancy_pointer& operator=(const fancy_pointer& other) {
        rep = other.rep;
        return *this;
    }
    ~fancy_pointer() {}

    // evil, to test that std::addressof() is being used
    void operator&() const = delete;

    explicit operator bool() const {
        return rep != nullptr;
    }

    std::add_lvalue_reference_t<Val> operator*() const {
        return *rep;
    }

    Val* operator->() const {
        return rep;
    }

    fancy_pointer& operator++() {
        ++rep;
        return *this;
    }

    fancy_pointer operator++(int) {
        static_assert(always_false<Val>::value, "avoid postincrement");
        fancy_pointer result = *this;
        ++rep;
        return result;
    }

    fancy_pointer& operator--() {
        --rep;
        return *this;
    }

    fancy_pointer operator--(int) {
        static_assert(always_false<Val>::value, "avoid postdecrement");
        fancy_pointer result = *this;
        --rep;
        return result;
    }

#ifdef _WIN64
    fancy_pointer& operator+=(int rhs) {
        rep += rhs;
        return *this;
    }
#endif // _WIN64

    fancy_pointer& operator+=(std::ptrdiff_t rhs) {
        rep += rhs;
        return *this;
    }

    fancy_pointer& operator+=(std::size_t rhs) {
        rep += rhs;
        return *this;
    }

#ifdef _WIN64
    fancy_pointer operator+(int rhs) const {
        return fancy_pointer{rep + rhs};
    }
#endif // _WIN64

    fancy_pointer operator+(std::ptrdiff_t rhs) const {
        return fancy_pointer{rep + rhs};
    }

    fancy_pointer operator+(std::size_t rhs) const {
        return fancy_pointer{rep + rhs};
    }

#ifdef _WIN64
    fancy_pointer& operator-=(int rhs) {
        rep -= rhs;
        return *this;
    }
#endif // _WIN64

    fancy_pointer& operator-=(std::ptrdiff_t rhs) {
        rep -= rhs;
        return *this;
    }

    fancy_pointer& operator-=(std::size_t rhs) {
        rep -= rhs;
        return *this;
    }

#ifdef _WIN64
    fancy_pointer operator-(int rhs) const {
        return fancy_pointer{rep - rhs};
    }
#endif // _WIN64

    fancy_pointer operator-(std::ptrdiff_t rhs) const {
        return fancy_pointer{rep - rhs};
    }

    fancy_pointer operator-(std::size_t rhs) const {
        return fancy_pointer{rep - rhs};
    }

#ifdef _WIN64
    std::add_lvalue_reference_t<Val> operator[](int offset) const {
        return rep[offset];
    }
#endif // _WIN64

    std::add_lvalue_reference_t<Val> operator[](std::ptrdiff_t offset) const {
        return rep[offset];
    }

    std::add_lvalue_reference_t<Val> operator[](std::size_t offset) const {
        return rep[offset];
    }

private:
    explicit fancy_pointer(rep_type cRep) : rep(cRep) {}
};

template <typename T>
auto operator+(std::ptrdiff_t lhs, const fancy_pointer<T>& rhs) -> decltype(rhs + lhs) {
    return rhs + lhs;
}

template <typename T, typename U>
auto operator-(const fancy_pointer<T>& lhs, const fancy_pointer<U>& rhs) -> decltype(lhs.rep - rhs.rep) {
    return lhs.rep - rhs.rep;
}

template <typename T, typename U>
auto operator==(const fancy_pointer<T>& lhs, const fancy_pointer<U>& rhs) -> decltype(lhs.rep == rhs.rep) {
    return lhs.rep == rhs.rep;
}

template <typename T>
bool operator==(const fancy_pointer<T>& lhs, std::nullptr_t) {
    return lhs.rep == nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const fancy_pointer<T>& rhs) {
    return nullptr == rhs.rep;
}

template <typename T>
bool operator==(const fancy_pointer<T>&, int) = delete;

template <typename T>
bool operator==(int, const fancy_pointer<T>&) = delete;

template <typename T, typename U>
auto operator!=(const fancy_pointer<T>& lhs, const fancy_pointer<U>& rhs) -> decltype(lhs.rep != rhs.rep) {
    return lhs.rep != rhs.rep;
}

template <typename T>
bool operator!=(const fancy_pointer<T>& lhs, std::nullptr_t) {
    return lhs.rep != nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const fancy_pointer<T>& rhs) {
    return nullptr != rhs.rep;
}

template <typename T>
bool operator!=(const fancy_pointer<T>&, int) = delete;

template <typename T>
bool operator!=(int, const fancy_pointer<T>&) = delete;

template <typename T, typename U>
auto operator<(const fancy_pointer<T>& lhs, const fancy_pointer<U>& rhs) -> decltype(lhs.rep < rhs.rep) {
    return lhs.rep < rhs.rep;
}

template <typename T, typename U>
auto operator>(const fancy_pointer<T>& lhs, const fancy_pointer<U>& rhs) -> decltype(lhs.rep > rhs.rep) {
    return lhs.rep > rhs.rep;
}

template <typename T, typename U>
auto operator>=(const fancy_pointer<T>& lhs, const fancy_pointer<U>& rhs) -> decltype(lhs.rep >= rhs.rep) {
    return lhs.rep >= rhs.rep;
}

template <typename T, typename U>
auto operator<=(const fancy_pointer<T>& lhs, const fancy_pointer<U>& rhs) -> decltype(lhs.rep <= rhs.rep) {
    return lhs.rep <= rhs.rep;
}

template <typename Val, bool IsVoid = std::is_void_v<Val>>
struct impl_pointer_to {
    static fancy_pointer<Val> pointer_to(Val& r) {
        fancy_pointer<Val> result;
        result.rep = std::addressof(r);
        return result;
    }
};

template <typename Val>
struct impl_pointer_to<Val, true> {
    // no pointer_to for void
};

namespace std {
    template <typename Val>
    struct pointer_traits<fancy_pointer<Val>> : impl_pointer_to<Val> {
        typedef fancy_pointer<Val> pointer;
        typedef Val element_type;
        typedef ptrdiff_t difference_type;

        template <typename U>
        using rebind = fancy_pointer<U>;
    };

    template <typename Val>
    struct iterator_traits<fancy_pointer<Val>> {
        typedef random_access_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;
        typedef remove_const_t<Val> value_type;
        typedef Val& reference;
        typedef fancy_pointer<Val> pointer;
    };
} // namespace std

template <typename Val>
struct fancy_allocator {
    fancy_allocator() = default;

    // See [allocator.requirements] table 28
    typedef fancy_pointer<Val> pointer;
    // default const_pointer
    // default void_pointer
    // default const_void_pointer
    typedef Val value_type;
    // default size_type
    // default difference_type
    // default rebind
    pointer allocate(std::size_t n) {
        std::allocator<Val> alloc;
        pointer result;
        result.rep = alloc.allocate(n);
        return result;
    }
    // default allocate(n, y)
    void deallocate(pointer p, std::size_t n) {
        std::allocator<Val> alloc;
        alloc.deallocate(p.rep, n);
    }
    // default max_size()
    // operator==(a1, a2) declared as free function below
    // operator!=(a1, a2) ditto
    // operator==(a, b) ditto
    // operator!=(a, b) ditto
    fancy_allocator(const fancy_allocator&)            = default;
    fancy_allocator(fancy_allocator&&)                 = default;
    fancy_allocator& operator=(const fancy_allocator&) = delete;
    fancy_allocator& operator=(fancy_allocator&&)      = delete;

    template <typename U>
    explicit fancy_allocator(const fancy_allocator<U>&) {}

    template <typename C, typename... Args>
    void construct(C* c, Args&&... args) {
        ::new (static_cast<void*>(c)) C(std::forward<Args>(args)...);
    }

    template <typename C, typename... Args>
    void construct(const fancy_pointer<C>&, Args&&...) {
        // note: static_assert rather than =delete because we want allocator_traits to think we provide this
        static_assert(always_false<C>::value, "construct takes unfancy pointer");
    }

    template <typename C>
    void destroy(C* c) {
        c->~C();
    }

    template <typename C>
    void destroy(const fancy_pointer<C>&) {
        static_assert(always_false<C>::value, "destroy takes unfancy pointer");
    }

    // default select_on_container_copy_construction
    // default propagate_on_container_copy_assignment
    // default propagate_on_container_move_assignment
    // default propagate_on_container_swap
    // default is_always_equal
};

template <typename T, typename U>
bool operator==(const fancy_allocator<T>&, const fancy_allocator<U>&) {
    return true;
}

template <typename T, typename U>
bool operator!=(const fancy_allocator<T>&, const fancy_allocator<U>&) {
    return false;
}

struct trivial {
    bool operator<(const trivial&) const {
        return false;
    }

    bool operator==(const trivial&) const {
        return true;
    }
};

using T1 = int;
using T2 = trivial;
using T3 = std::vector<int>;

using V = double;

struct omni_hash {
    template <typename X>
    std::size_t operator()(const X&) const {
        return 0;
    }
};

template class std::deque<T1, fancy_allocator<T1>>;
template class std::deque<T2, fancy_allocator<T2>>;
template class std::deque<T3, fancy_allocator<T3>>;

template class std::forward_list<T1, fancy_allocator<T1>>;
template class std::forward_list<T2, fancy_allocator<T2>>;
template class std::forward_list<T3, fancy_allocator<T3>>;

template class std::list<T1, fancy_allocator<T1>>;
template class std::list<T2, fancy_allocator<T2>>;
template class std::list<T3, fancy_allocator<T3>>;

template class std::vector<T1, fancy_allocator<T1>>;
template class std::vector<T2, fancy_allocator<T2>>;
template class std::vector<T3, fancy_allocator<T3>>;

template class std::vector<bool, fancy_allocator<bool>>;

template class std::map<T1, V, std::less<>, fancy_allocator<std::pair<const T1, V>>>;
template class std::map<T2, V, std::less<>, fancy_allocator<std::pair<const T2, V>>>;
template class std::map<T3, V, std::less<>, fancy_allocator<std::pair<const T3, V>>>;

template class std::multimap<T1, V, std::less<>, fancy_allocator<std::pair<const T1, V>>>;
template class std::multimap<T2, V, std::less<>, fancy_allocator<std::pair<const T2, V>>>;
template class std::multimap<T3, V, std::less<>, fancy_allocator<std::pair<const T3, V>>>;

template class std::set<T1, std::less<>, fancy_allocator<T1>>;
template class std::set<T2, std::less<>, fancy_allocator<T2>>;
template class std::set<T3, std::less<>, fancy_allocator<T3>>;

template class std::multiset<T1, std::less<>, fancy_allocator<T1>>;
template class std::multiset<T2, std::less<>, fancy_allocator<T2>>;
template class std::multiset<T3, std::less<>, fancy_allocator<T3>>;

template class std::unordered_map<T1, V, omni_hash, std::equal_to<>, fancy_allocator<std::pair<const T1, V>>>;
template class std::unordered_map<T2, V, omni_hash, std::equal_to<>, fancy_allocator<std::pair<const T2, V>>>;
template class std::unordered_map<T3, V, omni_hash, std::equal_to<>, fancy_allocator<std::pair<const T3, V>>>;

template class std::unordered_multimap<T1, V, omni_hash, std::equal_to<>, fancy_allocator<std::pair<const T1, V>>>;
template class std::unordered_multimap<T2, V, omni_hash, std::equal_to<>, fancy_allocator<std::pair<const T2, V>>>;
template class std::unordered_multimap<T3, V, omni_hash, std::equal_to<>, fancy_allocator<std::pair<const T3, V>>>;

template class std::unordered_set<T1, omni_hash, std::equal_to<>, fancy_allocator<T1>>;
template class std::unordered_set<T2, omni_hash, std::equal_to<>, fancy_allocator<T2>>;
template class std::unordered_set<T3, omni_hash, std::equal_to<>, fancy_allocator<T3>>;

template class std::unordered_multiset<T1, omni_hash, std::equal_to<>, fancy_allocator<T1>>;
template class std::unordered_multiset<T2, omni_hash, std::equal_to<>, fancy_allocator<T2>>;
template class std::unordered_multiset<T3, omni_hash, std::equal_to<>, fancy_allocator<T3>>;

template class std::basic_string<char, std::char_traits<char>, fancy_allocator<char>>;
template class std::basic_stringbuf<char, std::char_traits<char>, fancy_allocator<char>>;

#if _HAS_CXX20
template class std::basic_syncbuf<char, std::char_traits<char>, fancy_allocator<char>>;
template class std::basic_osyncstream<char, std::char_traits<char>, fancy_allocator<char>>;
#endif

#if _HAS_CXX23
STATIC_ASSERT(std::is_standard_layout_v<std::allocation_result<fancy_pointer<int>>>);
STATIC_ASSERT(!std::is_trivially_copyable_v<std::allocation_result<fancy_pointer<int>>>);

#ifdef __cpp_lib_concepts
STATIC_ASSERT(std::is_same_v<decltype(std::allocate_at_least(std::declval<fancy_allocator<int>&>(), std::size_t{})),
    std::allocation_result<fancy_pointer<int>>>);
#endif // __cpp_lib_concepts
#endif // _HAS_CXX23

void instantiate() {
    random_iterators_test<std::deque<T1, fancy_allocator<T1>>>();
    fwd_iterators_test<std::forward_list<T1, fancy_allocator<T1>>>();
    bidi_iterators_test<std::list<T1, fancy_allocator<T1>>>();
    random_iterators_test<std::vector<T1, fancy_allocator<T1>>>();
    random_iterators_test<std::vector<bool, fancy_allocator<bool>>>();
    bidi_iterators_test<std::set<int, std::less<>, fancy_allocator<int>>>();
    random_iterators_test<std::basic_string<char, std::char_traits<char>, fancy_allocator<char>>>();
}

// Test that the fake vector _Hash uses to avoid allocator::construct is layout compatible with std::vector:
using vec      = std::vector<int>;
using hash_vec = std::_Hash_vec<std::allocator<int>>;
STATIC_ASSERT(sizeof(vec) == sizeof(hash_vec));
STATIC_ASSERT(alignof(vec) == alignof(hash_vec));

using vec_fancy      = std::vector<int, fancy_allocator<int>>;
using hash_vec_fancy = std::_Hash_vec<fancy_allocator<int>>;
STATIC_ASSERT(sizeof(vec_fancy) == sizeof(hash_vec_fancy));
STATIC_ASSERT(alignof(vec_fancy) == alignof(hash_vec_fancy));
