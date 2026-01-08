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
#include <ranges>
#include <type_traits>
#include <vector>

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
bool check_container_requirements(T&&) {
    return true;
}

template <IsFlatMap T>
consteval bool check_reversible_container_requirements() {
    using map_t = remove_cvref_t<T>;
    return is_same_v<reverse_iterator<typename map_t::iterator>, typename map_t::reverse_iterator>
        && is_same_v<reverse_iterator<typename map_t::const_iterator>, typename map_t::const_reverse_iterator>
        && is_same_v<decltype(declval<map_t>().begin()), typename map_t::iterator>
        && is_same_v<decltype(declval<map_t>().end()), typename map_t::iterator>
        && is_same_v<decltype(declval<map_t>().cbegin()), typename map_t::const_iterator>
        && is_same_v<decltype(declval<map_t>().cend()), typename map_t::const_iterator>
        && is_same_v<decltype(declval<map_t>().rbegin()), typename map_t::reverse_iterator>
        && is_same_v<decltype(declval<map_t>().rend()), typename map_t::reverse_iterator>
        && is_same_v<decltype(declval<map_t>().crbegin()), typename map_t::const_reverse_iterator>
        && is_same_v<decltype(declval<map_t>().crend()), typename map_t::const_reverse_iterator>
        && is_convertible_v<typename map_t::iterator, typename map_t::const_iterator>
        && is_convertible_v<typename map_t::reverse_iterator, typename map_t::const_reverse_iterator>;
}

template <IsFlatMap T>
constexpr bool check_reversible_container_requirements(T&&) {
    return check_reversible_container_requirements<T&&>();
}

template <IsFlatMap T>
bool check_requirements(T&& obj) {
    return check_container_requirements(forward<T>(obj)) && check_reversible_container_requirements(forward<T>(obj));
}

template <IsFlatMap T>
bool check_key_content(const T& obj, const typename T::key_container_type& expected) {
    return ranges::equal(obj.keys(), expected);
}

template <IsFlatMap T>
bool check_value_content(const T& obj, const typename T::mapped_container_type& expected) {
    return ranges::equal(obj.values(), expected);
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

void test_construction() {
    {
        flat_map<int, int> fmap;
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
        flat_multimap<int, int> fmmap;
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {}));
        assert(check_value_content(fmmap, {}));
    }
    {
        vector<int> keys = {0, 1, 2, 3, 4, 2};
        vector<int> vals = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(keys, vals);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
        flat_multimap fmmap(keys, vals);
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
        vector<int, MyAllocator<int>> keys = {0, 1, 2, 3, 4, 2};
        vector<int, MyAllocator<int>> vals = {44, 2324, 635462, 433, 5, 7};
        size_t activeAllocations           = MyAllocator<int>::getActiveAllocationCount();
        flat_map fmap(keys, vals, MyAllocator<int>());
        assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
        assert(MyAllocator<int>::getActiveAllocationCount() > activeAllocations);
        activeAllocations = MyAllocator<int>::getActiveAllocationCount();
        flat_multimap fmmap(keys, vals); // FIXME, should use MyAllocator<int>(); deduction guides must be constrained
        assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
        assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5},
            {
                {0, 1, subrange_type::equal},
                {2, 3, subrange_type::permutation},
                {4, 5, subrange_type::equal},
            }));
        assert(MyAllocator<int>::getActiveAllocationCount() > activeAllocations);
    }
    {
        static_assert(is_constructible_v<flat_map<int, int>, sorted_unique_t, vector<int>, vector<int>>);
        vector<int> keys = {0, 1, 2, 3, 38, 242};
        vector<int> vals = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(sorted_unique, keys, vals);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {0, 1, 2, 3, 38, 242}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5, 7}));
        static_assert(!is_constructible_v<flat_multimap<int, int>, sorted_unique_t, vector<int>, vector<int>>);
    }
    {
        static_assert(!is_constructible_v<flat_map<int, int>, sorted_equivalent_t, vector<int>, vector<int>>);
        static_assert(is_constructible_v<flat_multimap<int, int>, sorted_equivalent_t, vector<int>, vector<int>>);
        vector<int, MyAllocator<int>> keys = {0, 1, 2, 2, 3, 4};
        vector<int, MyAllocator<int>> vals = {44, 2324, 635462, 7, 433, 5};
        flat_multimap fmmap(sorted_equivalent, keys, vals);
        assert(check_key_content(fmmap, {0, 1, 2, 2, 3, 4}));
        assert(check_value_content(fmmap, {44, 2324, 635462, 7, 433, 5})); // guaranteed by N4971 [flat.multimap.cons]/6
    }
    // FIXME, test more flat_map and flat_multimap constructors
    {
        PackagedCompare<int> comp;
        flat_map<Packaged<int>, int, PackagedCompare<int>> fmap(comp);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
        flat_multimap<Packaged<int>, int, PackagedCompare<int>> fmmap(comp);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {}));
        assert(check_value_content(fmmap, {}));
    }
    {
        PackagedCompare<int> comp;
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>, vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmap(comp, alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
        flat_multimap<Packaged<int>, Packaged<int>, PackagedCompare<int>,
            vector<Packaged<int>, MyAllocator<Packaged<int>>>, vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmmap(comp, alloc);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {}));
        assert(check_value_content(fmmap, {}));
    }
    {
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>, vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmap(alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
        flat_multimap<Packaged<int>, Packaged<int>, PackagedCompare<int>,
            vector<Packaged<int>, MyAllocator<Packaged<int>>>, vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmmap(alloc);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {}));
        assert(check_value_content(fmmap, {}));
    }
    {
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, int, PackagedCompare<int>, vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            vector<int, MyAllocator<int>>>
            fmap(alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
        flat_multimap<Packaged<int>, int, PackagedCompare<int>, vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            vector<int, MyAllocator<int>>>
            fmmap(alloc);
        assert(check_requirements(fmmap));
        assert(check_key_content(fmmap, {}));
        assert(check_value_content(fmmap, {}));
    }
    {
        PackagedCompare<int> comp;
        MyAllocator<int> alloc;
        vector<Packaged<int>, MyAllocator<Packaged<int>>> keys = {0, 1, 2, 3, 4, 2};
        vector<int, MyAllocator<int>> vals                     = {44, 2324, 635462, 433, 5, 7};
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
        vector<Packaged<int>, MyAllocator<Packaged<int>>> keys = {0, 1, 2, 3, 4, 2};
        vector<int, MyAllocator<int>> vals                     = {44, 2324, 635462, 433, 5, 7};
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

        using fm = flat_map<int, int, less<>, vector<int>, vector<int>>;
        fm m0;
        fm m1(m0);
        fm m2(m0, ator);
        fm m3(move(m0));
        fm m4(move(m1), ator);

        using fmm = flat_multimap<int, int, less<>, vector<int>, vector<int>>;
        fmm mm0;
        fmm mm1(mm0);
        fmm mm2(mm0, ator);
        fmm mm3(move(mm0));
        fmm mm4(move(mm1), ator);
    }
}

void test_erase_if() {
    {
        vector<int> keys      = {0, 1, 2, 3, 4, 2};
        vector<int> vals      = {44, 2324, 635462, 433, 5, 7};
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

void test_throwing_compare_swap() {
    test_throwing_compare_swap_single<flat_map, vector, vector>();
    test_throwing_compare_swap_single<flat_map, vector, deque>();
    test_throwing_compare_swap_single<flat_map, deque, vector>();
    test_throwing_compare_swap_single<flat_map, deque, deque>();

    test_throwing_compare_swap_single<flat_multimap, vector, vector>();
    test_throwing_compare_swap_single<flat_multimap, vector, deque>();
    test_throwing_compare_swap_single<flat_multimap, deque, vector>();
    test_throwing_compare_swap_single<flat_multimap, deque, deque>();
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
void test_generic_count_gh_5992() {
    {
        flat_map<int, int, divless> m{{1, 0}, {2, 0}, {11, 0}, {12, 0}};
        assert(2 == m.count(strange_int{0}));
    }
    {
        flat_multimap<int, int, divless> mm{{1, 0}, {2, 0}, {11, 0}, {12, 0}};
        assert(2 == mm.count(strange_int{0}));
    }
}

int main() {
    test_construction();
    test_pointer_to_incomplete_type();
    test_erase_if();
    test_insert();
    test_insert_range();
    test_gh_4344();
    test_insert_or_assign();
    test_comparison();
    test_throwing_compare_swap();
    test_lookup_call_on_temporaries();
    test_generic_count_gh_5992();
}
