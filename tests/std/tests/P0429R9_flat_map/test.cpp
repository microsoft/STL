// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <deque>
#include <flat_map>
#include <functional>
#include <memory>
#include <print>
#include <ranges>
#include <type_traits>
#include <vector>

#include <test_container_requirements.hpp>
#include <test_death.hpp>
using namespace std;

// See GH-5965: Speculative resolution of LWG-3963 "Different flat_(multi)map specializations
// should be able to share same nested classes" is not likely to be accepted
static_assert(!is_same_v<flat_map<int, int>::containers, flat_multimap<int, int>::containers>);
static_assert(!is_same_v<flat_map<int, int>::value_compare, flat_multimap<int, int>::value_compare>);

template <class T, template <class...> class Tmpl>
constexpr bool is_specialization_v = false;
template <template <class...> class Tmpl, class... Ts>
constexpr bool is_specialization_v<Tmpl<Ts...>, Tmpl> = true;

template <typename T>
concept IsFlatMap =
    is_specialization_v<remove_cvref_t<T>, flat_map> || is_specialization_v<remove_cvref_t<T>, flat_multimap>;

template <IsFlatMap T>
void assert_all_requirements(const T& s) {
    assert_container_requirements(s);
    assert_reversible_container_requirements(s);
    assert_three_way_comparability<T>();
    assert_map_requirements<T>();

    assert_noexcept_requirements(s);
    assert_noexcept_requirements(const_cast<T&>(s));

    assert_is_sorted_maybe_unique<_Is_specialization_v<T, flat_set>>(s);
}

template <IsFlatMap T>
bool check_key_content(const T& obj, const typename T::key_container_type& expected) {
    return ranges::equal(obj.keys(), expected);
}

template <IsFlatMap T>
bool check_value_content(const T& obj, const typename T::mapped_container_type& expected) {
    return ranges::equal(obj.values(), expected);
}

template <IsFlatMap T>
bool assert_check_content(const T& obj, const type_identity_t<T>& expected) {
    if (!ranges::equal(obj, expected)) {
        println(stderr, "Unexpected content!\nExpected {}", expected);
        println(stderr, "Actual {}", obj);
        return false;
    }
    return true;
}

enum class subrange_type : bool {
    equal,
    permutation,
};

struct subrange_t { // represents a closed subrange [first_index, last_index]
    ptrdiff_t first_index;
    ptrdiff_t last_index;
    subrange_type type;

    subrange_t(const ptrdiff_t first_index_, const ptrdiff_t last_index_, const subrange_type type_) noexcept
        : first_index(first_index_), last_index(last_index_), type(type_) {
        assert(first_index <= last_index);
    }
};

template <IsFlatMap T>
bool check_value_content(
    const T& obj, const typename T::mapped_container_type& expected, const vector<subrange_t>& subranges) {
    const auto& actual = obj.values();
    if (actual.size() != expected.size()) {
        return false;
    }

    // Verify that the subranges cover the entire range with no gaps or overlaps.
    // We assert instead of returning false because any problems would be caused by the check_value_content() call.
    if (expected.empty()) {
        assert(subranges.empty());
    } else {
        assert(!subranges.empty());
        assert(subranges.front().first_index == 0);
        assert(subranges.back().last_index == static_cast<ptrdiff_t>(expected.size() - 1));
        const auto is_gap_or_overlap = [](const subrange_t& sr1, const subrange_t& sr2) {
            return sr1.last_index + 1 != sr2.first_index;
        };
        assert(ranges::adjacent_find(subranges, is_gap_or_overlap) == subranges.end());
    }

    return ranges::all_of(subranges, [&expected, &actual](const subrange_t& subrange) {
        const auto& [first_index, last_index, type] = subrange;
        const ranges::subrange actual_subrange{actual.begin() + first_index, actual.begin() + last_index + 1};
        const ranges::subrange expected_subrange{expected.begin() + first_index, expected.begin() + last_index + 1};

        if (type == subrange_type::equal) {
            return ranges::equal(actual_subrange, expected_subrange);
        } else {
            return ranges::is_permutation(actual_subrange, expected_subrange);
        }
    });
}

template <typename T>
class MyAllocator : public allocator<T> {
public:
    using value_type = T;
    using allocator<T>::allocator;

    static size_t getActiveAllocationCount() {
        return s_allocations.load();
    }

    T* allocate(size_t n) {
        ++s_allocations;
        return allocator<T>::allocate(n);
    }

    T* allocate_at_least(size_t n) {
        ++s_allocations;
        return allocator<T>::allocate_at_least(n);
    }

    void deallocate(T* p, size_t n) noexcept {
        --s_allocations;
        allocator<T>::deallocate(p, n);
    }

private:
    static inline atomic<size_t> s_allocations{0};
};

template <class T, class U>
struct almost_pair {
    using first_type  = T;
    using second_type = U;

    T first;
    U second;

    constexpr operator pair<T, U>() const {
        return {first, second};
    }
};

namespace std {
    template <class T, class U, size_t N>
    struct tuple_element<N, almost_pair<T, U>> {
        using type = conditional_t<N == 1, T, U>;
    };

    // template<class T, class U>
    // struct tuple_size<almost_pair<T, U>> {
    //     using type = conditional_t<N == 1, T, U>;
    // };

} // namespace std

template <typename T>
class Packaged {
private:
    T value;

public:
    Packaged() : value() {}
    template <typename U>
        requires constructible_from<T, U&&>
    Packaged(U&& u) : value(forward<U>(u)) {}

    T get() const {
        return value;
    }

    void set(T t) {
        value = t;
    }

    friend bool operator==(const Packaged&, const Packaged&) = default;

    friend bool operator==(const Packaged& lhs, const T& rhs) {
        return lhs.value == rhs;
    }

    friend auto operator<=>(const Packaged&, const Packaged&) = default;
};

template <typename T>
struct PackagedCompare : less<Packaged<T>> {};

template <typename T>
struct TransparentPackagedCompare : PackagedCompare<T> {
    using is_transparent = void;

    bool operator()(const Packaged<T>& lhs, const Packaged<T>& rhs) const {
        return PackagedCompare<T>::operator()(lhs, rhs);
    }

    bool operator()(const T& lhs, const Packaged<T>& rhs) const {
        return lhs < rhs.get();
    }

    bool operator()(const Packaged<T>& lhs, const T& rhs) const {
        return lhs.get() < rhs;
    }
};

struct MyAllocatorCounter {
    MyAllocatorCounter() : activeAllocations{MyAllocator<int>::getActiveAllocationCount()} {}

    bool check_then_reset() {
        const bool allocated_some = MyAllocator<int>::getActiveAllocationCount() > activeAllocations;
        activeAllocations         = MyAllocator<int>::getActiveAllocationCount();
        return allocated_some;
    }

    size_t activeAllocations;
};

struct key_comparer {
    const auto& extract_key(const auto& obj) const {
        if constexpr (requires { obj.key; }) {
            return obj.key;
        } else {
            return obj;
        }
    }

    bool operator()(const auto& lhs, const auto& rhs) const {
        return extract_key(lhs) < extract_key(rhs);
    }

    using is_transparent = int;
};

template <template <class...> class KeyCont, template <class...> class MappedCont>
void test_construction() {
    // Using CTAD, the given MyAllocator is only used by the container when the constructor (deduction guide)
    // directly accepts KeyContainer and MappedContainer or using two deduction guides:
    //      flat_map(from_range_t, _Rng&&, _Compare, _Allocator)
    //      flat_map(from_range_t, _Rng&&, _Allocator)
    // In other cases we have to fully specify template arguments, such as for  flat_map(_Iter, _Iter, _Allocator)
    using flat_map_my_allocator =
        flat_map<int, int, less<int>, KeyCont<int, MyAllocator<int>>, MappedCont<int, MyAllocator<int>>>;
    using flat_multimap_my_allocator =
        flat_multimap<int, int, less<int>, KeyCont<int, MyAllocator<int>>, MappedCont<int, MyAllocator<int>>>;

    {
        // Test flat_map() and flat_map(const key_compare&)
        const less<int> compare;
        {
            flat_map<int, int> fmap;
            flat_map<int, int> fmap1(compare);

            assert(check_requirements(fmap));
            assert(check_key_content(fmap, {}));
            assert(check_value_content(fmap, {}));
            assert(fmap == fmap1);
        }
        {
            flat_multimap<int, int> fmmap;
            flat_multimap<int, int> fmmap1(compare);

            assert(check_requirements(fmmap));
            assert(check_key_content(fmmap, {}));
            assert(check_value_content(fmmap, {}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(key_cont, mapped_cont, comp = key_comp()())
        KeyCont<int> keys    = {0, 1, 2, 3, 4, 2};
        MappedCont<int> vals = {44, 2324, 635462, 433, 5, 7};
        {
            flat_map fmap(keys, vals);
            flat_map fmap1(keys, vals, less<int>());

            assert(check_requirements(fmap));
            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            flat_multimap fmmap(keys, vals);
            flat_multimap fmmap1(keys, vals, less<int>());

            assert(check_requirements(fmmap));
            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5},
                {
                    {0, 1, subrange_type::equal},
                    {2, 3, subrange_type::permutation},
                    {4, 5, subrange_type::equal},
                }));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(const key_cont&, const mapped_cont&, const key_comp&, const alloc&)
        // and  flat_map(const key_cont&, const mapped_cont&, const alloc&)
        KeyCont<int, MyAllocator<int>> keys    = {0, 1, 2, 3, 4, 2};
        MappedCont<int, MyAllocator<int>> vals = {44, 2324, 635462, 433, 5, 7};
        {
            MyAllocatorCounter allocation_counter;
            flat_map fmap(keys, vals, MyAllocator<int>());
            assert(allocation_counter.check_then_reset());
            flat_map fmap1(keys, vals, less<int>(), MyAllocator<int>());
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            MyAllocatorCounter allocation_counter;
            flat_multimap fmmap(keys, vals, MyAllocator<int>());
            assert(allocation_counter.check_then_reset());
            flat_multimap fmmap1(keys, vals, less<int>(), MyAllocator<int>());
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5},
                {
                    {0, 1, subrange_type::equal},
                    {2, 3, subrange_type::permutation},
                    {4, 5, subrange_type::equal},
                }));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(_Sorted_t, key_cont, mapped_cont, comp = key_comp()())
        {
            KeyCont<int> keys    = {0, 1, 2, 3, 38, 242};
            MappedCont<int> vals = {44, 2324, 635462, 433, 5, 7};

            flat_map fmap(sorted_unique, keys, vals);
            flat_map fmap1(sorted_unique, keys, vals, less<int>());

            assert(check_requirements(fmap));
            assert(check_key_content(fmap, {0, 1, 2, 3, 38, 242}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5, 7}));
            assert(fmap == fmap1);
        }
        {
            KeyCont<int> keys    = {0, 1, 2, 2, 3, 4};
            MappedCont<int> vals = {44, 2324, 635462, 7, 433, 5};

            flat_multimap fmmap(sorted_equivalent, keys, vals);
            flat_multimap fmmap1(sorted_equivalent, keys, vals, less<int>());

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(
                fmmap, {44, 2324, 635462, 7, 433, 5})); // guaranteed by N4971 [flat.multimap.cons]/6
            assert(fmmap == fmmap1);
        }
        {
            using non_multi = flat_map<int, int, less<int>, KeyCont<int>, MappedCont<int>>;
            using multi     = flat_multimap<int, int, less<int>, KeyCont<int>, MappedCont<int>>;
            static_assert(is_constructible_v<non_multi, sorted_unique_t, KeyCont<int>, MappedCont<int>>);
            static_assert(!is_constructible_v<multi, sorted_unique_t, KeyCont<int>, MappedCont<int>>);
            static_assert(!is_constructible_v<non_multi, sorted_equivalent_t, KeyCont<int>, MappedCont<int>>);
            static_assert(is_constructible_v<multi, sorted_equivalent_t, KeyCont<int>, MappedCont<int>>);
        }
    }
    {
        // Test flat_map(_Sorted_t, const key_cont&, const mapped_cont&, const key_comp&, const alloc&)
        // and flat_map(_Sorted_t, const key_cont&, const mapped_cont&, const alloc&)
        {
            KeyCont<int, MyAllocator<int>> keys    = {0, 1, 2, 3, 4};
            MappedCont<int, MyAllocator<int>> vals = {44, 2324, 635462, 433, 5};

            MyAllocatorCounter allocation_counter;
            flat_map fmap(sorted_unique, keys, vals, MyAllocator<int>());
            assert(allocation_counter.check_then_reset());
            flat_map fmap1(sorted_unique, keys, vals, less<int>(), MyAllocator<int>());
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            KeyCont<int, MyAllocator<int>> keys    = {0, 1, 2, 2, 3, 4};
            MappedCont<int, MyAllocator<int>> vals = {44, 2324, 635462, 7, 433, 5};

            MyAllocatorCounter allocation_counter;
            flat_multimap fmmap(sorted_equivalent, keys, vals, MyAllocator<int>());
            assert(allocation_counter.check_then_reset());
            flat_multimap fmmap1(sorted_equivalent, keys, vals, less<int>(), MyAllocator<int>());
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5},
                {
                    {0, 1, subrange_type::equal},
                    {2, 3, subrange_type::permutation},
                    {4, 5, subrange_type::equal},
                }));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(iter, iter, comp = key_compare())
        almost_pair<int, int> value_types[]{{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}};

        {
            flat_map fmap{ranges::begin(value_types), ranges::end(value_types)};
            flat_map fmap1{ranges::begin(value_types), ranges::end(value_types), less<int>{}};

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            flat_multimap fmmap{ranges::begin(value_types), ranges::end(value_types)};
            flat_multimap fmmap1{ranges::begin(value_types), ranges::end(value_types), less<int>{}};

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(iter, iter, const key_comp&, const alloc&)
        // and  flat_map(iter, iter, const alloc&)
        almost_pair<int, int> value_types[]{{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}};
        {
            MyAllocatorCounter allocation_counter;
            flat_map_my_allocator fmap{ranges::begin(value_types), ranges::end(value_types), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_map_my_allocator fmap1{
                ranges::begin(value_types), ranges::end(value_types), less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            MyAllocatorCounter allocation_counter;
            flat_multimap_my_allocator fmmap{ranges::begin(value_types), ranges::end(value_types), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_multimap_my_allocator fmmap1{
                ranges::begin(value_types), ranges::end(value_types), less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(from_range_t, _Rng &&, const key_compare&)
        // and  flat_map(from_range_t, _Rng &&)
        almost_pair<int, int> value_types[]{{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}};
        {
            flat_map fmap{from_range, value_types};
            flat_map fmap1{from_range, value_types, less<int>()};

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }

        {
            flat_multimap fmmap{from_range, value_types};
            flat_multimap fmmap1{from_range, value_types, less<int>()};

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(from_range_t, _Rng &&, const key_compare&, const alloc&)
        // and  flat_map(from_range_t, _Rng &&, const alloc&)
        // These constructors with allocators have a corresponding deduction guide -  can rely on CTAD
        almost_pair<int, int> value_types[]{{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}};

        {
            MyAllocatorCounter allocation_counter;
            flat_map fmap{from_range, value_types, MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_map fmap1{from_range, value_types, less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            MyAllocatorCounter allocation_counter;
            flat_multimap fmmap{from_range, value_types, MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_multimap fmmap1{from_range, value_types, less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(_Sorted_t, iter, iter, comp = key_comp())
        {
            almost_pair<int, int> value_types[]{{0, 44}, {1, 2324}, {2, 635462}, {3, 433}, {4, 5}};

            flat_map fmap{sorted_unique, ranges::begin(value_types), ranges::end(value_types)};
            flat_map fmap1{sorted_unique, ranges::begin(value_types), ranges::end(value_types), less<int>{}};

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            almost_pair<int, int> value_types[]{{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}};

            flat_multimap fmmap{sorted_equivalent, ranges::begin(value_types), ranges::end(value_types)};
            flat_multimap fmmap1{sorted_equivalent, ranges::begin(value_types), ranges::end(value_types), less<int>{}};

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(_Sorted_t, iter, iter, const key_comp&, const alloc&)
        // and flat_map(_Sorted_t, iter, iter, const alloc&)
        {
            almost_pair<int, int> value_types[]{{0, 44}, {1, 2324}, {2, 635462}, {3, 433}, {4, 5}};

            MyAllocatorCounter allocation_counter;
            flat_map_my_allocator fmap{
                sorted_unique, ranges::begin(value_types), ranges::end(value_types), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_map_my_allocator fmap1{
                sorted_unique, ranges::begin(value_types), ranges::end(value_types), less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            almost_pair<int, int> value_types[]{{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}};

            MyAllocatorCounter allocation_counter;
            flat_multimap_my_allocator fmmap{
                sorted_equivalent, ranges::begin(value_types), ranges::end(value_types), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_multimap_my_allocator fmmap1{sorted_equivalent, ranges::begin(value_types), ranges::end(value_types),
                less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(initializer_list, comp = key_comp())
        {
            flat_map fmap{{pair{0, 44}, pair{1, 2324}, pair{2, 635462}, pair{2, 7}, pair{3, 433}, pair{4, 5}}};
            flat_map fmap1{
                {pair{0, 44}, pair{1, 2324}, pair{2, 635462}, pair{2, 7}, pair{3, 433}, pair{4, 5}}, less<int>{}};

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            flat_multimap fmmap{{pair{0, 44}, pair{1, 2324}, pair{2, 635462}, pair{2, 7}, pair{3, 433}, pair{4, 5}}};
            flat_multimap fmmap1{
                {pair{0, 44}, pair{1, 2324}, pair{2, 635462}, pair{2, 7}, pair{3, 433}, pair{4, 5}}, less<int>{}};

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(initializer_list, const key_comp&, const alloc&)
        // and  flat_map(initializer_list, const alloc&)
        {
            MyAllocatorCounter allocation_counter;
            flat_map_my_allocator fmap{{{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}}, MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_map_my_allocator fmap1{
                {{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}}, less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            MyAllocatorCounter allocation_counter;
            flat_multimap_my_allocator fmmap{
                {{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}}, MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_multimap_my_allocator fmmap1{
                {{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}}, less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(_Sorted_t, initializer_list, comp = key_comp())
        {
            flat_map fmap{sorted_unique, {pair{0, 44}, pair{1, 2324}, pair{2, 635462}, pair{3, 433}, pair{4, 5}}};
            flat_map fmap1{
                sorted_unique, {pair{0, 44}, pair{1, 2324}, pair{2, 635462}, pair{3, 433}, pair{4, 5}}, less<int>{}};

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            flat_multimap fmmap{
                sorted_equivalent, {pair{0, 44}, pair{1, 2324}, pair{2, 635462}, pair{2, 7}, pair{3, 433}, pair{4, 5}}};
            flat_multimap fmmap1{sorted_equivalent,
                {pair{0, 44}, pair{1, 2324}, pair{2, 635462}, pair{2, 7}, pair{3, 433}, pair{4, 5}}, less<int>{}};

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    {
        // Test flat_map(_Sorted_t, initializer_list, const key_comp&, const alloc&)
        // and  flat_map(_Sorted_t, initializer_list, const alloc&)
        {
            MyAllocatorCounter allocation_counter;
            flat_map_my_allocator fmap{
                sorted_unique, {{0, 44}, {1, 2324}, {2, 635462}, {3, 433}, {4, 5}}, MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_map_my_allocator fmap1{
                sorted_unique, {{0, 44}, {1, 2324}, {2, 635462}, {3, 433}, {4, 5}}, less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
            assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
            assert(fmap == fmap1);
        }
        {
            MyAllocatorCounter allocation_counter;
            flat_multimap_my_allocator fmmap{
                sorted_equivalent, {{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}}, MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());
            flat_multimap_my_allocator fmmap1{sorted_equivalent,
                {{0, 44}, {1, 2324}, {2, 635462}, {2, 7}, {3, 433}, {4, 5}}, less<int>(), MyAllocator<int>()};
            assert(allocation_counter.check_then_reset());

            assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
            assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5}));
            assert(fmmap == fmmap1);
        }
    }
    // FIXME, verify that all flat_map and flat_multimap constructors are tested
    {
        PackagedCompare<int> comp;
        {
            flat_map<Packaged<int>, int, PackagedCompare<int>> fmap;
            flat_map<Packaged<int>, int, PackagedCompare<int>> fmap1(comp);

            assert(check_requirements(fmap));
            assert(check_key_content(fmap, {}));
            assert(check_value_content(fmap, {}));
            assert(fmap == fmap1);
        }
        {
            flat_multimap<Packaged<int>, int, PackagedCompare<int>> fmmap;
            flat_multimap<Packaged<int>, int, PackagedCompare<int>> fmmap1(comp);

            assert(check_requirements(fmmap));
            assert(check_key_content(fmmap, {}));
            assert(check_value_content(fmmap, {}));
            assert(fmmap == fmmap1);
        }
    }
    {
        PackagedCompare<int> comp;
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>, KeyCont<Packaged<int>, MyAllocator<Packaged<int>>>,
            MappedCont<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmap(comp, alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
        flat_multimap<Packaged<int>, Packaged<int>, PackagedCompare<int>,
            KeyCont<Packaged<int>, MyAllocator<Packaged<int>>>, MappedCont<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmmap(comp, alloc);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {}));
        assert(check_value_content(fmmap, {}));
    }
    {
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>, KeyCont<Packaged<int>, MyAllocator<Packaged<int>>>,
            MappedCont<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmap(alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
        flat_multimap<Packaged<int>, Packaged<int>, PackagedCompare<int>,
            KeyCont<Packaged<int>, MyAllocator<Packaged<int>>>, MappedCont<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmmap(alloc);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {}));
        assert(check_value_content(fmmap, {}));
    }
    {
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, int, PackagedCompare<int>, KeyCont<Packaged<int>, MyAllocator<Packaged<int>>>,
            MappedCont<int, MyAllocator<int>>>
            fmap(alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
        flat_multimap<Packaged<int>, int, PackagedCompare<int>, KeyCont<Packaged<int>, MyAllocator<Packaged<int>>>,
            MappedCont<int, MyAllocator<int>>>
            fmmap(alloc);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {}));
        assert(check_value_content(fmmap, {}));
    }
    {
        PackagedCompare<int> comp;
        MyAllocator<int> alloc;
        KeyCont<Packaged<int>, MyAllocator<Packaged<int>>> keys = {0, 1, 2, 3, 4, 2};
        MappedCont<int, MyAllocator<int>> vals                  = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(keys, vals, comp, alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
        flat_multimap fmmap(keys, vals, comp, alloc);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
        assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5},
            {
                {0, 1, subrange_type::equal},
                {2, 3, subrange_type::permutation},
                {4, 5, subrange_type::equal},
            }));
    }
    {
        TransparentPackagedCompare<int> comp;
        MyAllocator<int> alloc;
        KeyCont<Packaged<int>, MyAllocator<Packaged<int>>> keys = {0, 1, 2, 3, 4, 2};
        MappedCont<int, MyAllocator<int>> vals                  = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(keys, vals, comp, alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
        flat_multimap fmmap(keys, vals, comp, alloc);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
        assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5},
            {
                {0, 1, subrange_type::equal},
                {2, 3, subrange_type::permutation},
                {4, 5, subrange_type::equal},
            }));
    }
    // Test GH-4779 (access from _Flat_map_base to derived flat_map/flat_multimap)
    {
        allocator<int> ator;

        using fm = flat_map<int, int, less<>, KeyCont<int>, MappedCont<int>>;
        fm m0;
        fm m1(m0);
        fm m2(m0, ator);
        fm m3(move(m0));
        fm m4(move(m1), ator);

        using fmm = flat_multimap<int, int, less<>, KeyCont<int>, MappedCont<int>>;
        fmm mm0;
        fmm mm1(mm0);
        fmm mm2(mm0, ator);
        fmm mm3(move(mm0));
        fmm mm4(move(mm1), ator);
    }
}

template <template <class...> class KeyCont, template <class...> class MappedCont>
void test_erase_if() {
    {
        KeyCont<int> keys     = {0, 1, 2, 3, 4, 2};
        MappedCont<int> vals  = {44, 2324, 635462, 433, 5, 7};
        auto even_key_odd_val = [](pair<const int&, const int&> p) { return p.first % 2 == 0 && p.second % 2 != 0; };
        flat_map fmap(keys, vals);
        const auto erased_num = erase_if(fmap, even_key_odd_val);
        assert(erased_num == 1);
        assert(fmap.size() == 4);
        assert(check_key_content(fmap, {0, 1, 2, 3}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433}));
        flat_multimap fmmap(keys, vals);
        const auto erased_num_m = erase_if(fmmap, even_key_odd_val);
        assert(erased_num_m == 2);
        assert(fmmap.size() == 4);
        assert(check_key_content(fmmap, {0, 1, 2, 3}));
        assert(check_value_content(fmmap, {44, 2324, 635462, 433}));
    }
}

struct Incomplete;
template <class T>
struct MyType {
    T* ptr;

    bool operator==(const MyType&) const  = default;
    auto operator<=>(const MyType&) const = default;
};

void test_pointer_to_incomplete_type() {
    struct Test {
        unique_ptr<flat_map<Test, Test>> ptr;
        unique_ptr<flat_multimap<Test, Test>> ptr_m;
    };

    Test t;
    flat_map<MyType<Incomplete>, shared_ptr<MyType<Incomplete>>> fmap;
    flat_multimap<MyType<Incomplete>, shared_ptr<MyType<Incomplete>>> fmmap;
}

void test_insert() {
    flat_map<int, char> fm;

    const auto res1 = fm.insert({10, 'm'});
    assert(res1.first->first == 10);
    assert(res1.first->second == 'm');
    assert(res1.second);

    const auto res2 = fm.insert({10, 'n'});
    assert(res2.first->first == 10);
    assert(res2.first->second == 'm');
    assert(!res2.second);

    const flat_map<int, char>::value_type val_pair{90, 'w'};
    const auto res3 = fm.insert(val_pair);
    assert(res3.first->first == 90);
    assert(res3.first->second == 'w');
    assert(res3.second);

    const auto res4 = fm.insert(fm.cbegin(), std::pair<int, char>{30, 'c'});
    assert(res4->first == 30);
    assert(res4->second == 'c');

    const auto res5 = fm.insert(fm.cbegin(), std::pair<char, char>{static_cast<char>(40), 'd'});
    assert(res5->first == 40);
    assert(res5->second == 'd');

    assert(check_key_content(fm, {10, 30, 40, 90}));
    assert(check_value_content(fm, {'m', 'c', 'd', 'w'}));

    flat_multimap<int, char> fmm;

    const auto it1 = fmm.insert({10, 'm'});
    assert(it1->first == 10);
    assert(it1->second == 'm');

    const auto it2 = fmm.insert({10, 'n'});
    assert(it2->first == 10);
    assert(it2->second == 'n');

    const auto it3 = fmm.insert(fmm.cend(), {70, 'p'});
    assert(it3->first == 70);
    assert(it3->second == 'p');

    const flat_multimap<int, char>::value_type val_pair2{90, 'w'};
    const auto it4 = fmm.insert(val_pair2);
    assert(it4->first == 90);
    assert(it4->second == 'w');

    const auto it5 = fmm.insert(fmm.cend(), std::pair<char, char>{static_cast<char>(70), 'q'});
    assert(it5->first == 70);
    assert(it5->second == 'q');

    assert(check_key_content(fmm, {10, 10, 70, 70, 90}));
    // N4981 [associative.reqmts.general]/68 and /72 specify the values of the result to be {'m', 'n', 'p', 'q', 'w'}.
    assert(check_value_content(fmm, {'m', 'n', 'p', 'q', 'w'}));
}

void test_insert_range() {
    {
        flat_map<int, char> fm{{1, 'p'}, {4, 'q'}, {9, 'r'}};
        using char_type_array = flat_map<int, char>::value_type[];

        fm.insert_range(char_type_array{{16, 'x'}, {9, 'y'}, {4, 'z'}});
        assert(check_key_content(fm, {1, 4, 9, 16}));
        assert(check_value_content(fm, {'p', 'q', 'r', 'x'}));

        fm.insert_range(sorted_unique, char_type_array{{9, 'a'}, {16, 'b'}, {25, 'c'}});
        assert(check_key_content(fm, {1, 4, 9, 16, 25}));
        assert(check_value_content(fm, {'p', 'q', 'r', 'x', 'c'}));
    }
    {
        flat_multimap<int, char> fmm{{1, 'p'}, {4, 'q'}, {9, 'r'}};
        using char_type_array = flat_multimap<int, char>::value_type[];

        fmm.insert_range(char_type_array{{16, 'x'}, {9, 'y'}, {4, 'z'}});
        assert(check_key_content(fmm, {1, 4, 4, 9, 9, 16}));
        assert(check_value_content(fmm, {'p', 'q', 'z', 'r', 'y', 'x'}));

        fmm.insert_range(sorted_equivalent, char_type_array{{9, 'a'}, {16, 'b'}, {25, 'c'}});
        assert(check_key_content(fmm, {1, 4, 4, 9, 9, 9, 16, 16, 25}));
        assert(check_value_content(fmm, {'p', 'q', 'z', 'r', 'y', 'a', 'x', 'b', 'c'}));
    }
}

// GH-4344 <flat_map> Fix compile errors
void test_gh_4344() {
    flat_map<int, char> fm;

    const auto p1 = fm.try_emplace(10, 'm');
    assert(p1.first->first == 10);
    assert(p1.first->second == 'm');
    assert(p1.second);

    const auto p2 = fm.try_emplace(70, 'e');
    assert(p2.first->first == 70);
    assert(p2.first->second == 'e');
    assert(p2.second);

    const auto p3 = fm.try_emplace(20, 'o');
    assert(p3.first->first == 20);
    assert(p3.first->second == 'o');
    assert(p3.second);

    const auto p4 = fm.try_emplace(90, 'w');
    assert(p4.first->first == 90);
    assert(p4.first->second == 'w');
    assert(p4.second);

    const auto p5 = fm.try_emplace(70, 'X');
    assert(p5.first->first == 70);
    assert(p5.first->second == 'e');
    assert(!p5.second);

    assert(check_key_content(fm, {10, 20, 70, 90}));
    assert(check_value_content(fm, {'m', 'o', 'e', 'w'}));
}

class direct_init_only {
private:
    unsigned int n_ = 0u;

public:
    struct src_type {
        unsigned int n_ = 0u;

        friend bool operator==(const src_type&, const src_type&) = default;
    };

    explicit direct_init_only(const src_type& s) noexcept : n_(s.n_) {}
    direct_init_only(initializer_list<src_type>) = delete;

    direct_init_only& operator=(const src_type& s) noexcept {
        n_ = s.n_;
        return *this;
    }

    friend bool operator==(const direct_init_only&, const direct_init_only&) = default;
};

void test_insert_or_assign() {
    flat_map<int, char> fm;

    const auto p1 = fm.insert_or_assign(10, 'm');
    assert(p1.first->first == 10);
    assert(p1.first->second == 'm');
    assert(p1.second);

    const auto p2 = fm.insert_or_assign(70, 'e');
    assert(p2.first->first == 70);
    assert(p2.first->second == 'e');
    assert(p2.second);

    const auto p3 = fm.insert_or_assign(20, 'o');
    assert(p3.first->first == 20);
    assert(p3.first->second == 'o');
    assert(p3.second);

    const auto p4 = fm.insert_or_assign(90, 'w');
    assert(p4.first->first == 90);
    assert(p4.first->second == 'w');
    assert(p4.second);

    const auto p5 = fm.insert_or_assign(70, 'X');
    assert(p5.first->first == 70);
    assert(p5.first->second == 'X');
    assert(!p5.second);

    assert(check_key_content(fm, {10, 20, 70, 90}));
    assert(check_value_content(fm, {'m', 'o', 'X', 'w'}));

    const auto it6 = fm.insert_or_assign(fm.cbegin(), 10, 'b');
    assert(it6->first == 10);
    assert(it6->second == 'b');

    const char ch  = 'a';
    const auto it7 = fm.insert_or_assign(fm.cbegin(), 20, ch);
    assert(it7->first == 20);
    assert(it7->second == 'a');

    assert(check_key_content(fm, {10, 20, 70, 90}));
    assert(check_value_content(fm, {'b', 'a', 'X', 'w'}));

    // ensure direct-initialization
    flat_map<int, direct_init_only> direct_fm;
    direct_fm.insert_or_assign(direct_fm.end(), 0, direct_init_only::src_type{42u});

    assert(check_key_content(direct_fm, {0}));
    assert(check_value_content(direct_fm, {direct_init_only(direct_init_only::src_type{42u})}));
}

void test_comparison() {
    {
        flat_map<int, char> fm1{{1, '1'}, {2, '7'}, {3, '2'}, {4, '9'}};

        assert(fm1 == fm1);
        assert(!(fm1 != fm1));
        assert(!(fm1 < fm1));
        assert(!(fm1 > fm1));
        assert(fm1 <= fm1);
        assert(fm1 >= fm1);
        assert(fm1 <=> fm1 == strong_ordering::equal);

        flat_map<int, char> fm2{{1, '1'}, {7, '2'}, {2, '3'}, {9, '4'}};

        assert(!(fm1 == fm2));
        assert(fm1 != fm2);
        assert(!(fm1 < fm2));
        assert(fm1 > fm2);
        assert(!(fm1 <= fm2));
        assert(fm1 >= fm2);
        assert(fm1 <=> fm2 == strong_ordering::greater);
    }

    {
        flat_map<int, char, greater<int>> fm3{{1, '1'}, {2, '7'}, {3, '2'}, {4, '9'}};
        flat_map<int, char, greater<int>> fm4{{1, '1'}, {7, '2'}, {2, '3'}, {9, '4'}};

        assert(!(fm3 == fm4));
        assert(fm3 != fm4);
        assert(fm3 < fm4);
        assert(!(fm3 > fm4));
        assert(fm3 <= fm4);
        assert(!(fm3 >= fm4));
        assert(fm3 <=> fm4 == strong_ordering::less);
    }
    {
        flat_multimap<int, char> fmm1{
            {3, '2'}, {1, '7'}, {4, '1'}, {1, '8'}, {5, '2'}, {9, '8'}, {2, '1'}, {6, '8'}, {5, '2'}};

        assert(fmm1 == fmm1);
        assert(!(fmm1 != fmm1));
        assert(!(fmm1 < fmm1));
        assert(!(fmm1 > fmm1));
        assert(fmm1 <= fmm1);
        assert(fmm1 >= fmm1);
        assert(fmm1 <=> fmm1 == strong_ordering::equal);

        flat_multimap<int, char> fmm2{
            {2, '3'}, {7, '1'}, {1, '4'}, {8, '1'}, {2, '5'}, {8, '9'}, {1, '2'}, {8, '6'}, {2, '5'}};

        assert(!(fmm1 == fmm2));
        assert(fmm1 != fmm2);
        assert(!(fmm1 < fmm2));
        assert(fmm1 > fmm2);
        assert(!(fmm1 <= fmm2));
        assert(fmm1 >= fmm2);
        assert(fmm1 <=> fmm2 == strong_ordering::greater);
    }
    {
        flat_multimap<int, char, greater<int>> fmm3{
            {3, '2'}, {1, '7'}, {4, '1'}, {1, '8'}, {5, '2'}, {9, '8'}, {2, '1'}, {6, '8'}, {5, '2'}};
        flat_multimap<int, char, greater<int>> fmm4{
            {2, '3'}, {7, '1'}, {1, '4'}, {8, '1'}, {2, '5'}, {8, '9'}, {1, '2'}, {8, '6'}, {2, '5'}};

        assert(!(fmm3 == fmm4));
        assert(fmm3 != fmm4);
        assert(!(fmm3 < fmm4));
        assert(fmm3 > fmm4);
        assert(!(fmm3 <= fmm4));
        assert(fmm3 >= fmm4);
        assert(fmm3 <=> fmm4 == strong_ordering::greater);
    }
}

template <template <class...> class Map>
void test_map_operations_transparent() {
    struct shouldnt_convert {
        int key;
        /* implicit */ [[noreturn]] operator int() const {
            abort();
        }
    };

    Map<int, char, key_comparer> fm{{0, 'a'}, {3, 'g'}, {5, 't'}};
    assert(check_key_content(fm, {0, 3, 5}));
    assert(check_value_content(fm, {'a', 'g', 't'}));

    assert(fm.find(shouldnt_convert{0}) != fm.end());
    assert(fm.count(shouldnt_convert{3}) == 1);
    assert(!fm.contains(shouldnt_convert{1}));
    assert(fm.lower_bound(shouldnt_convert{-1}) == fm.begin());
    assert(fm.lower_bound(shouldnt_convert{8}) == fm.end());
    assert(fm.upper_bound(shouldnt_convert{2}) == fm.find(3));
    auto [first, last] = fm.equal_range(shouldnt_convert{5});
    assert(first + 1 == last);
}

namespace test_throwing_swap {
    struct unique_exception {};

    template <class T>
    struct throwing_less {
        static bool operator()(const T& left, const T& right) {
            return left < right;
        }

        bool throws_;
    };

    template <class T>
    void swap(throwing_less<T>& lhs, throwing_less<T>& rhs) {
        if (lhs.throws_ || rhs.throws_) {
            throw unique_exception{};
        }
    }
} // namespace test_throwing_swap

template <template <class...> class FlatMapCont, template <class...> class KeyCont,
    template <class...> class MappedCont>
void test_throwing_compare_swap_single() {
    using test_throwing_swap::unique_exception;
    using comparator = test_throwing_swap::throwing_less<int>;

    using map_type =
        FlatMapCont<int, char, comparator, KeyCont<int, allocator<int>>, MappedCont<char, allocator<char>>>;
    using value_type = map_type::value_type;
    static_assert(!is_nothrow_swappable_v<map_type>);
    {
        map_type m1{{{1, 'x'}, {2, 'y'}, {3, 'z'}}, comparator{false}};
        map_type m2{{{4, 'A'}, {5, 'B'}, {6, 'C'}}, comparator{false}};
        m1.swap(m2);
        assert(ranges::equal(m1, initializer_list<value_type>{{4, 'A'}, {5, 'B'}, {6, 'C'}}));
        assert(ranges::equal(m2, initializer_list<value_type>{{1, 'x'}, {2, 'y'}, {3, 'z'}}));
    }
    {
        map_type m1{{{1, 'x'}, {2, 'y'}, {3, 'z'}}, comparator{false}};
        map_type m2{{{4, 'A'}, {5, 'B'}, {6, 'C'}}, comparator{false}};
        ranges::swap(m1, m2);
        assert(ranges::equal(m1, initializer_list<value_type>{{4, 'A'}, {5, 'B'}, {6, 'C'}}));
        assert(ranges::equal(m2, initializer_list<value_type>{{1, 'x'}, {2, 'y'}, {3, 'z'}}));
    }
    {
        map_type m1{{{1, 'x'}, {2, 'y'}, {3, 'z'}}, comparator{false}};
        map_type m2{{{4, 'A'}, {5, 'B'}, {6, 'C'}}, comparator{true}};
        try {
            m1.swap(m2);
            assert(false);
        } catch (const unique_exception&) {
            assert(m1.empty());
            assert(m2.empty());
        } catch (...) {
            assert(false);
        }
    }
    {
        map_type m1{{{1, 'x'}, {2, 'y'}, {3, 'z'}}, comparator{false}};
        map_type m2{{{4, 'A'}, {5, 'B'}, {6, 'C'}}, comparator{true}};
        try {
            ranges::swap(m1, m2);
            assert(false);
        } catch (const unique_exception&) {
            assert(m1.empty());
            assert(m2.empty());
        } catch (...) {
            assert(false);
        }
    }
}

// Test that changes in GH-5987 did not break calls of lookup member functions by using deducing this.
template <typename T>
void test_lookup_call_on_temporaries_single() {
    (void) T{}.lower_bound(42);
    (void) T{}.lower_bound('a');
    (void) T{}.upper_bound(42);
    (void) T{}.upper_bound('a');
    (void) T{}.equal_range(42);
    (void) T{}.equal_range('a');
    (void) T{}.find(42);
    (void) T{}.find('a');
}

void test_lookup_call_on_temporaries() {
    test_lookup_call_on_temporaries_single<flat_map<int, int>>();
    test_lookup_call_on_temporaries_single<flat_multimap<int, int>>();
    try {
        (void) flat_map<int, int>{}.at(42);
        (void) flat_map<int, int>{}.at('a');
    } catch (...) {
    }
}

// Test that hint to emplace/insert is respected, when possible; check returned iterator
template <template <class...> class KeyC, template <class...> class ValueC>
void test_insert_hint_is_respected() {
    using lt = std::less<int>;

    {
        flat_multimap<int, char, lt, KeyC<int>, ValueC<char>> a{{-1, 'x'}, {-1, 'x'}, {1, 'x'}, {1, 'x'}};
        bool problem_seen                      = false;
        auto const assert_inserted_at_position = [&a, &problem_seen](
                                                     const int expected_index, const auto insert_position) {
            const auto expected_position = a.begin() + expected_index;
            if (expected_position != insert_position) {
                println("Wrong insert position: expected {}, actual {}\nContainer after insert {}", expected_index,
                    insert_position - a.begin(), a);
                problem_seen = true;
            }
        };

        struct pseudopair {
            int i;
            char c;

            operator pair<int, char>() const {
                return pair{i, c};
            }
        };

        pair pair0c{0, 'c'};
        pair const pair0f{0, 'f'};
        // hint is greater
        assert(assert_check_content(a, {{-1, 'x'}, {-1, 'x'}, {1, 'x'}, {1, 'x'}}));
        assert_inserted_at_position(2, a.emplace_hint(a.end(), 0, 'a'));
        assert(assert_check_content(a, {{-1, 'x'}, {-1, 'x'}, {0, 'a'}, {1, 'x'}, {1, 'x'}}));
        assert_inserted_at_position(3, a.emplace_hint(a.find(1), pair{0, 'b'}));
        assert(assert_check_content(a, {{-1, 'x'}, {-1, 'x'}, {0, 'a'}, {0, 'b'}, {1, 'x'}, {1, 'x'}}));
        assert_inserted_at_position(4, a.insert(a.upper_bound(0), std::move(pair0c)));
        assert(assert_check_content(a, {{-1, 'x'}, {-1, 'x'}, {0, 'a'}, {0, 'b'}, {0, 'c'}, {1, 'x'}, {1, 'x'}}));
        // hint is correct
        assert_inserted_at_position(4, a.emplace_hint(a.upper_bound(0) - 1, 0, 'd'));
        assert(assert_check_content(
            a, {{-1, 'x'}, {-1, 'x'}, {0, 'a'}, {0, 'b'}, {0, 'd'}, {0, 'c'}, {1, 'x'}, {1, 'x'}}));
        assert_inserted_at_position(3, a.emplace_hint(a.begin() + 3, pseudopair{0, 'e'}));
        assert(assert_check_content(
            a, {{-1, 'x'}, {-1, 'x'}, {0, 'a'}, {0, 'e'}, {0, 'b'}, {0, 'd'}, {0, 'c'}, {1, 'x'}, {1, 'x'}}));
        assert_inserted_at_position(2, a.insert(a.lower_bound(0), pair0f));
        assert(assert_check_content(
            a, {{-1, 'x'}, {-1, 'x'}, {0, 'f'}, {0, 'a'}, {0, 'e'}, {0, 'b'}, {0, 'd'}, {0, 'c'}, {1, 'x'}, {1, 'x'}}));
        // hint is less
        assert_inserted_at_position(2, a.emplace_hint(a.lower_bound(0) - 1, pair{0, 'g'}));
        assert(assert_check_content(a, {{-1, 'x'}, {-1, 'x'}, {0, 'g'}, {0, 'f'}, {0, 'a'}, {0, 'e'}, {0, 'b'},
                                           {0, 'd'}, {0, 'c'}, {1, 'x'}, {1, 'x'}}));
        assert_inserted_at_position(2, a.insert(a.begin(), pseudopair{0, 'h'}));
        assert(assert_check_content(a, {{-1, 'x'}, {-1, 'x'}, {0, 'h'}, {0, 'g'}, {0, 'f'}, {0, 'a'}, {0, 'e'},
                                           {0, 'b'}, {0, 'd'}, {0, 'c'}, {1, 'x'}, {1, 'x'}}));

        assert(!problem_seen);

        assert(4 == erase_if(a, [](const auto pair) { return pair.second <= 'd'; }));
        assert(assert_check_content(
            a, {{-1, 'x'}, {-1, 'x'}, {0, 'h'}, {0, 'g'}, {0, 'f'}, {0, 'e'}, {1, 'x'}, {1, 'x'}}));
        assert(4 == a.erase(0));
        assert(assert_check_content(a, {{-1, 'x'}, {-1, 'x'}, {1, 'x'}, {1, 'x'}}));
    }
}

template <template <class...> class KeyCont, template <class...> class MappedCont>
void test_key_mapped_cont_combinations() {
    test_construction<KeyCont, MappedCont>();
    test_insert_hint_is_respected<KeyCont, MappedCont>();
    test_throwing_compare_swap_single<flat_map, KeyCont, MappedCont>();
    test_throwing_compare_swap_single<flat_multimap, KeyCont, MappedCont>();
    test_erase_if<KeyCont, MappedCont>();
}

// Test heterogeneous lookup and erase operations when the compare object does not satisfy strict_weak_order (GH-5992)
enum class strange_int {};

// No overload divless::operator()(strange_int, strange_int), does not satisfy std::strict_weak_order
struct divless {
    using is_transparent = void;

    template <class X, class Y>
        requires (is_same_v<X, int> && is_same_v<Y, strange_int>)
              || (is_same_v<X, strange_int> && is_same_v<Y, int>) || (is_same_v<X, int> && is_same_v<Y, int>)
    constexpr bool operator()(X x, Y y) const noexcept {
        if constexpr (is_same_v<X, strange_int>) {
            return static_cast<int>(x) < y / 10;
        } else if constexpr (is_same_v<Y, strange_int>) {
            return x / 10 < static_cast<int>(y);
        } else {
            return x < y;
        }
    }
};
static_assert(!strict_weak_order<divless, int, strange_int>);

// ranges:: algorithms can't be called with divless compare, as it does not satisfy std::strict_weak_order
void test_non_strict_weak_order_compare() {
    {
        flat_map<int, int, divless> cont{{1, 0}, {2, 0}, {11, 0}, {12, 0}};
        assert(2 == cont.count(strange_int{0}));

        assert(cont.contains(strange_int{0}));
        assert(!cont.contains(strange_int{2}));

        assert(cont.begin() + 2 == cont.lower_bound(strange_int{1}));
        assert(cont.begin() + 2 == cont.upper_bound(strange_int{0}));

        const auto [first, last] = cont.equal_range(strange_int{0});
        assert(first == cont.begin());
        assert(last == cont.begin() + 2);

        assert(cont.end() != cont.find(strange_int{1}));
        assert(cont.end() == cont.find(strange_int{3}));

        assert(2 == cont.erase(strange_int{0}));
        assert(assert_check_content(cont, {{11, 0}, {12, 0}}));
    }
    {
        flat_multimap<int, int, divless> cont{{1, 0}, {2, 0}, {11, 0}, {12, 0}};
        assert(2 == cont.count(strange_int{0}));

        assert(cont.contains(strange_int{0}));
        assert(!cont.contains(strange_int{2}));

        assert(cont.begin() + 2 == cont.lower_bound(strange_int{1}));
        assert(cont.begin() + 2 == cont.upper_bound(strange_int{0}));

        const auto [first, last] = cont.equal_range(strange_int{0});
        assert(first == cont.begin());
        assert(last == cont.begin() + 2);

        assert(cont.end() != cont.find(strange_int{1}));
        assert(cont.end() == cont.find(strange_int{3}));

        assert(2 == cont.erase(strange_int{0}));
        assert(assert_check_content(cont, {{11, 0}, {12, 0}}));
    }
}

void run_normal_tests() {
    test_key_mapped_cont_combinations<vector, vector>();
    test_key_mapped_cont_combinations<vector, deque>();
    test_key_mapped_cont_combinations<deque, vector>();
    test_key_mapped_cont_combinations<deque, deque>();
    test_pointer_to_incomplete_type();
    test_insert();
    test_insert_range();
    test_gh_4344();
    test_insert_or_assign();
    test_comparison();

    test_map_operations_transparent<flat_map>();
    test_map_operations_transparent<flat_multimap>();

    test_lookup_call_on_temporaries();
    test_non_strict_weak_order_compare();
}

enum class cont_type { multi, unique };

template <cont_type type>
void test_death_construct_unsorted_initializer_list() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    C cont(sorted, {{137, 'a'}, {42, 'g'}, {3337, 'f'}, {15, 'r'}});
}

template <cont_type type>
void test_death_construct_unsorted_iter_iter() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    vector<typename C::value_type> values{{137, 'a'}, {42, 'g'}, {3337, 'f'}, {15, 'r'}};
    C cont(sorted, values.begin(), values.end());
}

template <cont_type type>
void test_death_construct_unsorted_container() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    typename C::key_container_type keys{137, 42, 3337, 15};
    typename C::mapped_container_type mapped{'a', 'g', 'f', 'r'};
    C cont(sorted, keys, mapped);
}

template <cont_type type>
void test_death_replace_unsorted_container() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    C cont;
    cont.replace({137, 42, 3337, 15}, {'a', 'g', 'f', 'r'});
}

template <cont_type type>
void test_death_insert_unsorted_iter_iter() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    vector<typename C::value_type> values{{137, 'a'}, {42, 'g'}, {3337, 'f'}, {15, 'r'}};
    C cont;
    cont.insert(sorted, values.begin(), values.end());
}

template <cont_type type>
void test_death_insert_unsorted_range() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    vector<typename C::value_type> values{{137, 'a'}, {42, 'g'}, {3337, 'f'}, {15, 'r'}};
    C cont;
    cont.insert_range(sorted, values);
}

template <cont_type type>
void test_death_insert_unsorted_initializer_list() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    C cont;
    cont.insert(sorted, {{137, 'a'}, {42, 'g'}, {3337, 'f'}, {15, 'r'}});
}

template <cont_type type>
void test_death_construct_duplicates_initializer_list() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    C cont(sorted, {{42, 'a'}, {137, 'g'}, {137, 'f'}, {3337, 'r'}});
}

template <cont_type type>
void test_death_construct_duplicates_iter_iter() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    vector<typename C::value_type> values{{42, 'a'}, {137, 'g'}, {137, 'f'}, {3337, 'r'}};
    C cont(sorted, values.begin(), values.end());
}

template <cont_type type>
void test_death_construct_duplicates_container() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    typename C::key_container_type keys{42, 137, 137, 3337};
    typename C::mapped_container_type mapped{'a', 'g', 'f', 'r'};
    C cont(sorted, keys, mapped);
}

template <cont_type type>
void test_death_replace_duplicates_container() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    C cont;
    cont.replace({42, 137, 137, 3337}, {'a', 'g', 'f', 'r'});
}

template <cont_type type>
void test_death_insert_duplicates_iter_iter() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    vector<typename C::value_type> values{{42, 'a'}, {137, 'g'}, {137, 'f'}, {3337, 'r'}};
    C cont;
    cont.insert(sorted, values.begin(), values.end());
}

template <cont_type type>
void test_death_insert_duplicates_range() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    vector<typename C::value_type> values{{42, 'a'}, {137, 'g'}, {137, 'f'}, {3337, 'r'}};
    C cont;
    cont.insert_range(sorted, values);
}

template <cont_type type>
void test_death_insert_duplicates_initializer_list() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    C cont;
    cont.insert(sorted, {{42, 'a'}, {137, 'g'}, {137, 'f'}, {3337, 'r'}});
}

template <cont_type type>
void test_death_different_size_ctor() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    typename C::key_container_type keys{42, 137, 137, 3337, 0};
    typename C::mapped_container_type mapped{'a'};
    C cont(keys, mapped);
}

template <cont_type type>
void test_death_different_size_ctor_sorted() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    typename C::key_container_type keys{42, 137, 137, 3337, 0};
    typename C::mapped_container_type mapped{'a'};
    C cont(sorted, keys, mapped);
}

template <cont_type type>
void test_death_different_size_replace() {
    using C = conditional_t<type == cont_type::unique, flat_map<int, char>, flat_multimap<int, char>>;
    C cont;
    cont.replace({42, 137, 137, 3337, 0}, {'a'});
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] { run_normal_tests(); });

#if defined(_DEBUG)
    exec.add_death_tests({

        // Tests shared between flat_set and flat_map - violation of sorted elements
        test_death_construct_unsorted_initializer_list<cont_type::unique>,
        test_death_construct_unsorted_initializer_list<cont_type::multi>,
        test_death_construct_unsorted_iter_iter<cont_type::unique>,
        test_death_construct_unsorted_iter_iter<cont_type::multi>,
        test_death_construct_unsorted_container<cont_type::unique>,
        test_death_construct_unsorted_container<cont_type::multi>,
        test_death_replace_unsorted_container<cont_type::unique>,
        test_death_replace_unsorted_container<cont_type::multi>,
        test_death_insert_unsorted_iter_iter<cont_type::unique>,
        test_death_insert_unsorted_iter_iter<cont_type::multi>,
        test_death_insert_unsorted_range<cont_type::unique>,
        test_death_insert_unsorted_range<cont_type::multi>,
        test_death_insert_unsorted_initializer_list<cont_type::unique>,
        test_death_insert_unsorted_initializer_list<cont_type::multi>,

        // Tests shared between flat_set and flat_map - violation of unique elements
        test_death_construct_duplicates_initializer_list<cont_type::unique>,
        test_death_construct_duplicates_iter_iter<cont_type::unique>,
        test_death_construct_duplicates_container<cont_type::unique>,
        test_death_replace_duplicates_container<cont_type::unique>,
        test_death_insert_duplicates_iter_iter<cont_type::unique>,
        test_death_insert_duplicates_range<cont_type::unique>,
        test_death_insert_duplicates_initializer_list<cont_type::unique>,

        // Tests not present in flat_set - mismatch of length of key and mapped containers
        test_death_different_size_ctor<cont_type::unique>,
        test_death_different_size_ctor<cont_type::multi>,
        test_death_different_size_ctor_sorted<cont_type::unique>,
        test_death_different_size_ctor_sorted<cont_type::multi>,
        test_death_different_size_replace<cont_type::unique>,
        test_death_different_size_replace<cont_type::multi>,
    });
#endif // defined(_DEBUG)

    return exec.run(argc, argv);
}
