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

// Test MSVC STL-specific SCARY-ness

template <bool>
struct flat_map_unique_if_impl;
template <>
struct flat_map_unique_if_impl<true> {
    template <class Key, class Mapped, class Comp, class KeyCont, class MappedCont>
    using type = flat_map<Key, Mapped, Comp, KeyCont, MappedCont>;
};
template <>
struct flat_map_unique_if_impl<false> {
    template <class Key, class Mapped, class Comp, class KeyCont, class MappedCont>
    using type = flat_multimap<Key, Mapped, Comp, KeyCont, MappedCont>;
};

template <bool IsUnique, class Key, class Mapped, class Comp, class KeyCont, class MappedCont>
using flat_map_unique_if = flat_map_unique_if_impl<IsUnique>::template type<Key, Mapped, Comp, KeyCont, MappedCont>;

template <bool IsUnique, class Comparator, class Alloc1, class Alloc2>
void test_scary_ness_one() { // COMPILE-ONLY
    using Iter = flat_map<int, int>::iterator;
    using OtherIter =
        flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>::iterator;
    static_assert(is_same_v<Iter, OtherIter>);

    using ConstIter = flat_map<int, int>::const_iterator;
    using OtherConstIter =
        flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>::const_iterator;
    static_assert(is_same_v<ConstIter, OtherConstIter>);

    using Cont = flat_map<int, int, less<int>, vector<int, Alloc1>, vector<int, Alloc2>>::containers;
    using OtherCont =
        flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>::containers;
    static_assert(is_same_v<Cont, OtherCont>);

    using ValueComp = flat_map<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>::value_compare;
    using OtherValueComp1 =
        flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>::value_compare;
    using OtherValueComp2 = flat_map_unique_if<IsUnique, int, int, Comparator, vector<int>, vector<int>>::value_compare;
    using OtherValueComp3 =
        flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, deque<int, Alloc2>>::value_compare;
    using OtherValueComp4 =
        flat_map_unique_if<IsUnique, int, int, Comparator, deque<int, Alloc1>, vector<int, Alloc2>>::value_compare;
    static_assert(is_same_v<ValueComp, OtherValueComp1>);
    static_assert(is_same_v<ValueComp, OtherValueComp2>);
    static_assert(is_same_v<ValueComp, OtherValueComp3>);
    static_assert(is_same_v<ValueComp, OtherValueComp4>);
}

void test_scary_ness() { // COMPILE-ONLY
    test_scary_ness_one<true, greater<int>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, greater<int>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, greater<int>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<true, less<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, less<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, less<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<true, greater<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, greater<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, greater<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, greater<int>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, greater<int>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, greater<int>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, less<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, less<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, less<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, greater<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, greater<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, greater<>, MyAllocator<int>, allocator<int>>();
}

int main() {
    test_construction();
    test_pointer_to_incomplete_type();
    test_erase_if();
    test_insert();
    test_gh_4344();
    test_insert_or_assign();
    test_comparison();
}
