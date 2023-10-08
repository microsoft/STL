// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <concepts>
#include <flat_map>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

using namespace std;

template <typename T>
concept IsFlatMap = _Is_specialization_v<remove_cvref_t<T&&>, flat_map>;

template <IsFlatMap T>
bool check_container_requirements(T&&) {
    return true;
}

template <IsFlatMap T>
consteval bool check_reversible_container_requirements()
    requires _Is_specialization_v<remove_cvref_t<T>, flat_map>
{
    using map_t = remove_cvref_t<T>;
    bool result = true;
    result &= is_same_v<std::reverse_iterator<typename map_t::iterator>, typename map_t::reverse_iterator>;
    result &= is_same_v<std::reverse_iterator<typename map_t::const_iterator>, typename map_t::const_reverse_iterator>;
    result &= is_same_v<decltype(declval<map_t>().begin()), typename map_t::iterator>;
    result &= is_same_v<decltype(declval<map_t>().end()), typename map_t::iterator>;
    result &= is_same_v<decltype(declval<map_t>().cbegin()), typename map_t::const_iterator>;
    result &= is_same_v<decltype(declval<map_t>().cend()), typename map_t::const_iterator>;
    result &= is_same_v<decltype(declval<map_t>().rbegin()), typename map_t::reverse_iterator>;
    result &= is_same_v<decltype(declval<map_t>().rend()), typename map_t::reverse_iterator>;
    result &= is_same_v<decltype(declval<map_t>().crbegin()), typename map_t::const_reverse_iterator>;
    result &= is_same_v<decltype(declval<map_t>().crend()), typename map_t::const_reverse_iterator>;
    result &= is_convertible_v<typename map_t::iterator, typename map_t::const_iterator>;
    result &= is_convertible_v<typename map_t::reverse_iterator, typename map_t::const_reverse_iterator>;
    return result;
}

template <IsFlatMap T>
constexpr bool check_reversible_container_requirements(T&&)
    requires _Is_specialization_v<remove_cvref_t<T&&>, flat_map>
{
    return check_reversible_container_requirements<T&&>();
}

template <IsFlatMap T>
bool check_requirements(T&& obj) {
    return check_container_requirements(forward<T>(obj)) && check_reversible_container_requirements(forward<T>(obj));
}

template <IsFlatMap T>
bool check_key_content(const T& obj, const typename T::key_container_type& expected) {
    const auto& actual = obj.keys();
    if (actual.size() != expected.size()) {
        return false;
    }
    return std::ranges::equal(actual, expected);
}

template <IsFlatMap T>
bool check_value_content(const T& obj, const typename T::mapped_container_type& expected) {
    const auto& actual = obj.values();
    if (actual.size() != expected.size()) {
        return false;
    }
    return std::ranges::equal(actual, expected);
}

template <typename T>
class MyAllocator : public std::allocator<T> {
public:
    using value_type = T;
    using std::allocator<T>::allocator;

    static size_t getActiveAllocationCount() {
        return s_allocations.load();
    }

    T* allocate(size_t n) {
        ++s_allocations;
        return std::allocator<T>::allocate(n);
    }

    T* allocate_at_least(size_t n) {
        ++s_allocations;
        return std::allocator<T>::allocate_at_least(n);
    }

    void deallocate(T* p, size_t n) noexcept {
        --s_allocations;
        std::allocator<T>::deallocate(p, n);
    }

private:
    static std::atomic<size_t> s_allocations;
};

template <typename T>
class Packaged {
private:
    T value;

public:
    Packaged() : value() {}
    template <typename U>
    Packaged(U&& u) : value(std::forward<U>(u)) {}

    T get() const {
        return value;
    }

    void set(T t) {
        value = t;
    }

    friend bool operator==(const Packaged& lhs, const Packaged& rhs) {
        return lhs.value == rhs.value;
    }

    friend bool operator==(const Packaged& lhs, const T& rhs) {
        return lhs.value == rhs;
    }

    friend bool operator==(const T& lhs, const Packaged& rhs) {
        return lhs == rhs.value;
    }
};

template <typename T>
class PackagedCompare : public std::less<Packaged<T>> {
public:
    using std::less<Packaged<T>>::less;
};

template <typename T>
class TransparentPackagedCompare : public PackagedCompare<T> {
public:
    using PackagedCompare<T>::PackagedCompare;
    using is_transparent = void;

    bool operator()(const T& lhs, const Packaged<T>& rhs) const {
        return lhs < rhs.get();
    }

    bool operator()(const Packaged<T>& lhs, const T& rhs) const {
        return lhs.get() < rhs;
    }
};

template <typename T>
std::atomic<size_t> MyAllocator<T>::s_allocations = 0;

void test_construction() {
    {
        std::flat_map<int, int> map;
        assert(check_requirements(map));
        assert(check_key_content(map, {}));
        assert(check_value_content(map, {}));
    }
    {
        std::vector<int> keys = {0, 1, 2, 3, 4, 2};
        std::vector<int> vals = {44, 2324, 635462, 433, 5, 7};
        std::flat_map map(keys, vals);
        assert(check_requirements(map));
        assert(check_key_content(map, {0, 1, 2, 3, 4}));
        assert(check_value_content(map, {44, 2324, 635462, 433, 5}));
    }
    {
        std::vector<int, MyAllocator<int>> keys = {0, 1, 2, 3, 4, 2};
        std::vector<int, MyAllocator<int>> vals = {44, 2324, 635462, 433, 5, 7};
        size_t activeAllocations                = MyAllocator<int>::getActiveAllocationCount();
        std::flat_map map(keys, vals, MyAllocator<int>());
        assert(check_key_content(map, {0, 1, 2, 3, 4}));
        assert(check_value_content(map, {44, 2324, 635462, 433, 5}));
        assert(MyAllocator<int>::getActiveAllocationCount() > activeAllocations);
    }
    {
        std::vector<int> keys = {0, 1, 2, 3, 38, 242};
        std::vector<int> vals = {44, 2324, 635462, 433, 5, 7};
        std::flat_map map(std::sorted_unique, keys, vals);
        assert(check_requirements(map));
        assert(check_key_content(map, {0, 1, 2, 3, 38, 242}));
        assert(check_value_content(map, {44, 2324, 635462, 433, 5, 7}));
    }
    {
        PackagedCompare<int> comp;
        std::flat_map<Packaged<int>, int, PackagedCompare<int>> map(comp);
        assert(check_requirements(map));
        assert(check_key_content(map, {}));
        assert(check_value_content(map, {}));
    }
    {
        PackagedCompare<int> comp;
        MyAllocator<Packaged<int>> alloc;
        std::flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>,
            std::vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            std::vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            map(comp, alloc);
        assert(check_requirements(map));
        assert(check_key_content(map, {}));
        assert(check_value_content(map, {}));
    }
    {
        MyAllocator<Packaged<int>> alloc;
        std::flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>,
            std::vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            std::vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            map(alloc);
        assert(check_requirements(map));
        assert(check_key_content(map, {}));
        assert(check_value_content(map, {}));
    }
    {
        PackagedCompare<int> comp;
        MyAllocator<Packaged<int>> alloc;
        std::vector<Packaged<int>, MyAllocator<Packaged<int>>> keys = {0, 1, 2, 3, 4, 2};
        std::vector<Packaged<int>, MyAllocator<Packaged<int>>> vals = {44, 2324, 635462, 433, 5, 7};
        std::flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>,
            std::vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            std::vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            map(comp, alloc);
        assert(check_requirements(map));
        assert(check_key_content(map, {}));
        assert(check_value_content(map, {}));
    }
}

void test_pointer_to_incomplete_type() {
    struct Test {
        std::unique_ptr<std::flat_map<Test, Test>> ptr;
    };

    Test t;
}

int main() {
    test_construction();
    return 0;
}
