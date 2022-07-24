// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#define STRINGIFY2(X) #X
#define STRINGIFY(X)  STRINGIFY2(X)

template <class NH>
bool empty_test(NH&& nh) {
    return nh.empty() && !nh;
}

template <class NH>
bool not_empty_test(NH&& nh) {
    return !nh.empty() && nh;
}

#define CHECK_EMPTY(...)     assert(empty_test(__VA_ARGS__))
#define CHECK_NOT_EMPTY(...) assert(not_empty_test(__VA_ARGS__))

template <template <class...> class, template <class...> class>
constexpr bool same_t = false;
template <template <class...> class C>
constexpr bool same_t<C, C> = true;

template <template <class...> class Actual, template <class...> class... Potentials>
constexpr bool is_one_of_t = (same_t<Actual, Potentials> || ...);

template <template <class...> class C>
constexpr bool mappish = is_one_of_t<C, std::map, std::multimap, std::unordered_map, std::unordered_multimap>;

template <template <class...> class C>
constexpr bool ordered = is_one_of_t<C, std::map, std::multimap, std::set, std::multiset>;

long long allocation_count = 0;
bool allocation_allowed    = false;

struct allocation_guard {
    bool value_ = allocation_allowed;

    allocation_guard() = default;
    explicit allocation_guard(bool const new_value) : value_{std::exchange(allocation_allowed, new_value)} {}

    ~allocation_guard() {
        allocation_allowed = value_;
    }
};

bool construct_destroy_exact = false;

template <class T>
struct tracked_allocator {
    using value_type = T;

    constexpr tracked_allocator() noexcept = default;
    template <class U>
    constexpr tracked_allocator(tracked_allocator<U> const&) noexcept {}

    T* allocate(size_t n) {
        assert(allocation_allowed);
        T* ptr = std::allocator<T>{}.allocate(n);
        ++allocation_count;
        return ptr;
    }

    void deallocate(T* ptr, size_t n) noexcept {
        assert(allocation_allowed);
        std::allocator<T>{}.deallocate(ptr, n);
        --allocation_count;
    }

    template <class U, class... Args>
    void construct(U* ptr, Args&&... args) {
        if constexpr (!std::is_same_v<U, value_type>) {
            assert(!construct_destroy_exact);
            printf("construct\n");
        }
        std::allocator<T> alloc;
        std::allocator_traits<std::allocator<T>>::construct(alloc, ptr, std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U* ptr) {
        if constexpr (!std::is_same_v<U, value_type>) {
            assert(!construct_destroy_exact);
            printf("destroy\n");
        }
        std::allocator<T> alloc;
        std::allocator_traits<std::allocator<T>>::destroy(alloc, ptr);
    }

    template <class U>
    bool operator==(tracked_allocator<U> const&) const noexcept {
        return true;
    }
    template <class U>
    bool operator!=(tracked_allocator<U> const&) const noexcept {
        return false;
    }
};

template <class Container, class Key>
auto test_extract(Container& cont, Key const& absent, Key const& present) {
    allocation_guard guard{false};
    auto const size = cont.size();
    auto nh         = cont.extract(absent);
    CHECK_EMPTY(nh);
    nh = cont.extract(present);
    CHECK_NOT_EMPTY(nh);
    assert(cont.size() == size - 1);
    return nh;
}

template <class Container>
auto test_extract(Container& cont, typename Container::const_iterator pos) {
    allocation_guard guard{false};
    auto const size = cont.size();
    auto nh         = cont.extract(pos);
    CHECK_NOT_EMPTY(nh);
    assert(cont.size() == size - 1);
    return nh;
}

template <class NodeHandle, class Validator1, class Validator2>
void test_node_handle(NodeHandle& nh1, NodeHandle& nh2, Validator1 v1, Validator2 v2) {
    allocation_guard guard{false};

    using ATraits = std::allocator_traits<typename NodeHandle::allocator_type>;

    // empty and operator bool do not throw
    static_assert(noexcept(nh1.empty()));
    static_assert(noexcept(static_cast<bool>(nh1)));

    // Nothrow destruction
    static_assert(std::is_nothrow_destructible_v<NodeHandle>);

    assert(v1(nh1));
    assert(v2(nh2));

    // Nothrow/constexpr default construction
    static_assert(std::is_nothrow_default_constructible_v<NodeHandle>);
    CHECK_EMPTY(NodeHandle{});
#ifdef __cpp_constinit
#pragma warning(suppress : 4640) // C4640 emitted by MSVC because 'NodeHandle' type has non-trivial dtor
    { static constinit NodeHandle static_handle{}; }
#endif // ^^^ __cpp_constinit ^^^

    // No copies!
    static_assert(!std::is_copy_constructible_v<NodeHandle>);
    static_assert(!std::is_copy_assignable_v<NodeHandle>);

    // Nothrow move construction (empty)
    static_assert(std::is_nothrow_move_constructible_v<NodeHandle>);
    {
        NodeHandle empty_nh;
        NodeHandle nh = std::move(empty_nh);
        CHECK_EMPTY(nh);
    }

    // Move assignment (self, empty)
    static_assert(std::is_nothrow_move_assignable_v<NodeHandle>); // Extension (strengthened)
    {
        NodeHandle empty_nh;
        empty_nh = std::move(empty_nh);
        CHECK_EMPTY(empty_nh);
    }

    // Move construction (!empty)
    auto nh3 = std::move(nh1);
    CHECK_EMPTY(nh1);
    assert(v1(nh3));

    // Move assignment (empty = empty)
    nh1 = NodeHandle{};
    CHECK_EMPTY(nh1);

    // Move assignment (empty = !empty)
    nh1 = std::move(nh3);
    CHECK_EMPTY(nh3);
    assert(v1(nh1));

    using std::swap;

    // "Conditionally" nothrow swap
    constexpr bool should_not_throw = ATraits::propagate_on_container_swap::value || ATraits::is_always_equal::value;
    static_assert(!should_not_throw || std::is_nothrow_swappable_v<NodeHandle>);
    static_assert(!should_not_throw || noexcept(nh1.swap(nh3)));
    static_assert(std::is_nothrow_swappable_v<NodeHandle>); // Extension (strengthened)
    static_assert(noexcept(nh1.swap(nh3))); // Extension (strengthened)

    // non-member swap (!empty, empty)
    swap(nh1, nh3);
    CHECK_EMPTY(nh1);
    assert(v1(nh3));
    // member swap (empty, !empty)
    nh1.swap(nh3);
    CHECK_EMPTY(nh3);
    assert(v1(nh1));

    // member swap (!empty, empty)
    nh1.swap(nh3);
    CHECK_EMPTY(nh1);
    assert(v1(nh3));
    // non-member swap (empty, !empty)
    swap(nh1, nh3);
    CHECK_EMPTY(nh3);
    assert(v1(nh1));

    {
        NodeHandle empty_nh;
        CHECK_EMPTY(empty_nh);

        // non-member swap (empty, empty)
        swap(nh3, empty_nh);
        CHECK_EMPTY(empty_nh);
        CHECK_EMPTY(nh3);

        // member swap (empty, empty)
        nh3.swap(empty_nh);
        CHECK_EMPTY(empty_nh);
        CHECK_EMPTY(nh3);
    }

    // non-member swap (!empty, !empty)
    swap(nh1, nh2);
    assert(v1(nh2));
    assert(v2(nh1));
    // member swap (!empty, !empty)
    nh1.swap(nh2);
    assert(v1(nh1));
    assert(v2(nh2));

    // Move assignment (self, !empty)
    auto const count   = allocation_count;
    allocation_allowed = true;
    nh2                = std::move(nh2);
    allocation_allowed = false;
    CHECK_EMPTY(nh2);
    assert(allocation_count == count - 1);

    // Move assignment (self, empty)
    nh2 = std::move(nh2);
    CHECK_EMPTY(nh2);
}

template <template <class...> class C, class Order = std::less<>>
auto init_from(std::initializer_list<std::pair<int, int>> il) {
    allocation_guard guard{true};
    if constexpr (mappish<C>) {
        using A = tracked_allocator<std::pair<int const, int>>;
        if constexpr (ordered<C>) {
            return C<int, int, Order, A>(il.begin(), il.end());
        } else {
            return C<int, int, std::hash<int>, std::equal_to<>, A>(il.begin(), il.end());
        }
    } else {
        using A = tracked_allocator<int>;

        auto settish = [] {
            if constexpr (ordered<C>) {
                return C<int, Order, A>{};
            } else {
                return C<int, std::hash<int>, std::equal_to<>, A>{};
            }
        }();

        for (auto const& pr : il) {
            settish.emplace(pr.first);
        }
        return settish;
    }
}

template <class P, class F = decltype(P::first)>
constexpr bool is_int_pair = std::is_same_v<int, std::remove_cv_t<F>>;

struct projected_less {
    constexpr bool operator()(int i, int j) const {
        return i < j;
    }
    template <class P, std::enable_if_t<is_int_pair<P>, int> = 0>
    constexpr bool operator()(P const& p, int j) const {
        return p.first < j;
    }
    template <class P, std::enable_if_t<is_int_pair<P>, int> = 0>
    constexpr bool operator()(int j, P const& p) const {
        return j < p.first;
    }
    template <class P1, class P2, std::enable_if_t<is_int_pair<P1> && is_int_pair<P2>, int> = 0>
    constexpr bool operator()(P1 const& p1, P2 const& p2) const {
        return p1.first < p2.first;
    }

    using is_transparent = std::true_type;
};

struct projected_greater {
    template <class T, class U>
    constexpr auto operator()(T const& t, U const& u) const -> decltype(projected_less{}(u, t)) {
        return projected_less{}(u, t);
    }

    using is_transparent = std::true_type;
};

struct projected_hash {
    size_t operator()(int i) const {
        return std::hash<int>{}(i);
    }
    template <class P, std::enable_if_t<is_int_pair<P>, int> = 0>
    size_t operator()(P const& p) const {
        return std::hash<int>{}(p.first);
    }
};

struct projected_equal {
    constexpr bool operator()(int i, int j) const {
        return i == j;
    }
    template <class P, std::enable_if_t<is_int_pair<P>, int> = 0>
    constexpr bool operator()(P const& p, int j) const {
        return p.first == j;
    }
    template <class P, std::enable_if_t<is_int_pair<P>, int> = 0>
    constexpr bool operator()(int j, P const& p) const {
        return j == p.first;
    }
    template <class P1, class P2, std::enable_if_t<is_int_pair<P1> && is_int_pair<P2>, int> = 0>
    constexpr bool operator()(P1 const& p1, P2 const& p2) const {
        return p1.first == p2.first;
    }

    using is_transparent = std::true_type;
};

template <template <class...> class C, bool Reverse = false, bool = ordered<C>, bool = mappish<C>>
struct extended_merge_ctype_;

template <template <class...> class C, bool Reverse>
struct extended_merge_ctype_<C, Reverse, true, true> {
    using type = C<int, int, std::conditional_t<Reverse, std::greater<>, std::less<>>>;
};
template <template <class...> class C, bool Reverse>
struct extended_merge_ctype_<C, Reverse, false, true> {
    using type = C<int, int, std::hash<int>, std::equal_to<>>;
};
template <template <class...> class C, bool Reverse>
struct extended_merge_ctype_<C, Reverse, true, false> {
    using type = C<std::pair<int const, int>, std::conditional_t<Reverse, projected_greater, projected_less>>;
};
template <template <class...> class C, bool Reverse>
struct extended_merge_ctype_<C, Reverse, false, false> {
    using type = C<std::pair<int const, int>, projected_hash, projected_equal>;
};
template <template <class...> class C, bool Reverse = false>
using extended_merge_ctype = typename extended_merge_ctype_<C, Reverse>::type;

enum class should_move : bool { no, yes };
enum class should_reverse : bool { no, yes };

template <template <class...> class C1, template <class...> class C2, should_reverse Reverse, should_move Move>
void test_merge_single() {
    auto const count = allocation_count;
    {
        allocation_guard guard{true};
        auto c1        = extended_merge_ctype<C1>{{1, -1}, {2, -2}, {3, -3}};
        auto const key = [&](int i) {
            if constexpr (mappish<C1>) {
                return i;
            } else {
                return std::pair<int const, int>{i, 99999};
            }
        }(1);
        auto const pos     = std::as_const(c1).find(key);
        auto c2            = extended_merge_ctype<C2, Reverse == should_reverse::yes>{{0, 0}, {3, 3}};
        allocation_allowed = false;
        assert(c1.get_allocator() == c2.get_allocator());
        if constexpr (Move == should_move::yes) {
            c2.merge(std::move(c1));
        } else {
            c2.merge(c1);
        }
        assert(c1.size() <= 1u);
        assert(4 <= c2.size() && c2.size() <= 5);
#if _ITERATOR_DEBUG_LEVEL != 1 // merge invalidates iterators under IDL1
        auto const d = std::distance(c2.cbegin(), pos);
        if constexpr (ordered<C2>) {
            assert(d == (Reverse == should_reverse::yes ? static_cast<decltype(d)>(c2.size()) - 2 : 1));
        }
#endif // _ITERATOR_DEBUG_LEVEL != 1
        allocation_allowed = true;
    }
    assert(allocation_count == count);
}

template <template <class...> class C1, template <class...> class C2>
void test_merge() {
    test_merge_single<C1, C2, should_reverse::no, should_move::no>();
    test_merge_single<C1, C2, should_reverse::no, should_move::yes>();
    if constexpr (ordered<C2>) {
        test_merge_single<C1, C2, should_reverse::yes, should_move::no>();
        test_merge_single<C1, C2, should_reverse::yes, should_move::yes>();
    }
}

template <template <class...> class C>
void test_key_mutation() {
    auto const count = allocation_count;
    {
        allocation_guard guard{true};
        auto m             = init_from<C>({{1, -1}, {2, -2}, {3, -3}});
        allocation_allowed = false;
        auto nh            = m.extract(1);
        if constexpr (mappish<C>) {
            nh.key() = 42;
        } else {
            nh.value() = 42;
        }
        m.insert(std::move(nh));
        auto const pos = m.find(42);
        assert(pos != m.end());
        if constexpr (mappish<C>) {
            assert(pos->second == -1);
        }
        allocation_allowed = true;
    }
    assert(allocation_count == count);
}

template <template <class...> class Map>
void test_map() {
    auto const count = allocation_count;
    {
        using A = tracked_allocator<std::pair<int const, char>>;
        using M = Map<int, char, std::less<>, A>;
        allocation_guard guard{true};
        M m{{0, 'x'}, {1, 'y'}};
        allocation_allowed = false;

        auto nh1 = test_extract(m, 42, 0);
        using NH = decltype(nh1);
        static_assert(std::is_same_v<typename NH::allocator_type, typename M::allocator_type>);
        static_assert(std::is_same_v<typename NH::key_type, typename M::key_type>);
        static_assert(std::is_same_v<typename NH::mapped_type, typename M::mapped_type>);

        auto nh2 = test_extract(m, m.begin());
        static_assert(std::is_same_v<NH, decltype(nh2)>);

        test_node_handle(
            nh1, nh2,
            [&m](NH& h) {
                return static_cast<bool>(h) && !h.empty() && h.key() == 0 && h.mapped() == 'x'
                    && h.get_allocator() == m.get_allocator();
            },
            [&m](NH& h) {
                return static_cast<bool>(h) && !h.empty() && h.key() == 1 && h.mapped() == 'y'
                    && h.get_allocator() == m.get_allocator();
            });

        allocation_allowed = true;
        m.clear();
        M m2{{0, 'x'}, {1, 'y'}};
        allocation_allowed = false;
        m.insert(m2.extract(m2.begin()));
        m.insert(m.end(), m2.extract(m2.begin()));
        assert(m2.empty());
        assert(m.size() == 2u);
        assert(m.begin()->first == 0);
        assert(m.begin()->second == 'x');
        assert(std::next(m.begin())->first == 1);
        assert(std::next(m.begin())->second == 'y');

        allocation_allowed = true;
    }
    assert(allocation_count == count);

    test_key_mutation<Map>();

    test_merge<Map, std::map>();
    test_merge<Map, std::multimap>();
    test_merge<Map, std::set>(); // Extension (standard doesn't require map<->set node compatibility)
    test_merge<Map, std::multiset>(); // Extension (ditto)
}

template <template <class...> class Set>
void test_set() {
    auto const count = allocation_count;
    {
        using S = Set<int, std::less<>, tracked_allocator<int>>;
        allocation_guard guard{true};
        S s{0, 1};
        allocation_allowed = false;

        auto nh1 = test_extract(s, 42, 0);
        using NH = decltype(nh1);
        static_assert(std::is_same_v<typename NH::allocator_type, typename S::allocator_type>);
        static_assert(std::is_same_v<typename NH::value_type, typename S::value_type>);

        auto nh2 = test_extract(s, s.begin());
        static_assert(std::is_same_v<NH, decltype(nh2)>);

        test_node_handle(
            nh1, nh2,
            [&s](NH& h) {
                return static_cast<bool>(h) && !h.empty() && h.value() == 0 && h.get_allocator() == s.get_allocator();
            },
            [&s](NH& h) {
                return static_cast<bool>(h) && !h.empty() && h.value() == 1 && h.get_allocator() == s.get_allocator();
            });

        allocation_allowed = true;
        s.clear();
        S m2{{0, 1}};
        allocation_allowed = false;
        s.insert(m2.extract(m2.begin()));
        s.insert(s.end(), m2.extract(m2.begin()));
        assert(m2.empty());
        assert(s.size() == 2u);
        assert(*s.begin() == 0);
        assert(*std::next(s.begin()) == 1);
        allocation_allowed = true;
    }
    assert(allocation_count == count);

    test_key_mutation<Set>();

    test_merge<Set, std::map>(); // Extension (standard doesn't require map<->set node compatibility)
    test_merge<Set, std::multimap>(); // Extension (ditto)
    test_merge<Set, std::set>();
    test_merge<Set, std::multiset>();
}

template <template <class...> class Map>
void test_unordered_map() {
    auto const count = allocation_count;
    {
        using A = tracked_allocator<std::pair<int const, char>>;
        using M = Map<int, char, std::hash<int>, std::equal_to<>, A>;
        allocation_guard guard{true};
        M m{{0, 'x'}, {1, 'y'}};
        allocation_allowed = false;

        auto nh1 = test_extract(m, 42, 0);
        using NH = decltype(nh1);
        static_assert(std::is_same_v<typename NH::allocator_type, typename decltype(m)::allocator_type>);
        static_assert(std::is_same_v<typename NH::key_type, int>);
        static_assert(std::is_same_v<typename NH::mapped_type, char>);

        auto nh2 = test_extract(m, m.begin());
        static_assert(std::is_same_v<NH, decltype(nh2)>);

        test_node_handle(
            nh1, nh2,
            [&m](NH& h) {
                return static_cast<bool>(h) && !h.empty() && h.key() == 0 && h.mapped() == 'x'
                    && h.get_allocator() == m.get_allocator();
            },
            [&m](NH& h) {
                return static_cast<bool>(h) && !h.empty() && h.key() == 1 && h.mapped() == 'y'
                    && h.get_allocator() == m.get_allocator();
            });

        allocation_allowed = true;
        m.clear();
        M m2{{0, 'x'}, {1, 'y'}};
        allocation_allowed = false;
        m.insert(m2.extract(m2.begin()));
        m.insert(m.end(), m2.extract(m2.begin()));
        assert(m2.empty());
        assert(m.size() == 2u);
        if (m.begin()->first == 0) {
            assert(m.begin()->second == 'x');
            assert(std::next(m.begin())->first == 1);
            assert(std::next(m.begin())->second == 'y');
        } else {
            assert(m.begin()->first == 1);
            assert(m.begin()->second == 'y');
            assert(std::next(m.begin())->first == 0);
            assert(std::next(m.begin())->second == 'x');
        }

        allocation_allowed = true;
    }
    assert(allocation_count == count);

    test_key_mutation<Map>();

    test_merge<Map, std::unordered_map>();
    test_merge<Map, std::unordered_multimap>();
    test_merge<Map, std::unordered_set>(); // Extension (standard doesn't require map<->set node compatibility)
    test_merge<Map, std::unordered_multiset>(); // Extension (ditto)
}

template <template <class...> class Set>
void test_unordered_set() {
    auto const count = allocation_count;
    {
        using S = Set<int, std::hash<int>, std::equal_to<>, tracked_allocator<int>>;
        allocation_guard guard{true};
        S s{0, 1};
        allocation_allowed = false;

        auto nh1 = test_extract(s, 42, 0);
        using NH = decltype(nh1);
        static_assert(std::is_same_v<typename NH::allocator_type, typename decltype(s)::allocator_type>);
        static_assert(std::is_same_v<typename NH::value_type, int>);

        auto nh2 = test_extract(s, s.begin());
        static_assert(std::is_same_v<NH, decltype(nh2)>);

        test_node_handle(
            nh1, nh2,
            [&s](NH& h) {
                return static_cast<bool>(h) && !h.empty() && h.value() == 0 && h.get_allocator() == s.get_allocator();
            },
            [&s](NH& h) {
                return static_cast<bool>(h) && !h.empty() && h.value() == 1 && h.get_allocator() == s.get_allocator();
            });

        allocation_allowed = true;
        s.clear();
        S m2{{0, 1}};
        allocation_allowed = false;
        s.insert(m2.extract(m2.begin()));
        s.insert(s.end(), m2.extract(m2.begin()));
        assert(m2.empty());
        assert(s.size() == 2u);
        if (*s.begin() == 0) {
            assert(*std::next(s.begin()) == 1);
        } else {
            assert(*s.begin() == 1);
            assert(*std::next(s.begin()) == 0);
        }

        allocation_allowed = true;
    }
    assert(allocation_count == count);

    test_key_mutation<Set>();

    test_merge<Set, std::unordered_map>(); // Extension (standard doesn't require map<->set node compatibility)
    test_merge<Set, std::unordered_multimap>(); // Extension (ditto)
    test_merge<Set, std::unordered_set>();
    test_merge<Set, std::unordered_multiset>();
}

void test_gh_1309() {
    // Guard against regression of GH-1309, in which node handles were incorrectly destroying the user value with a node
    // allocator rather than a value_type allocator as the Standard requires.

    allocation_guard guard{true};

    {
        using A  = tracked_allocator<std::pair<int const, char>>;
        using M  = std::map<int, char, std::less<>, A>;
        using NH = M::node_type;
        NH nh1;
        NH nh2;
        {
            M m{{0, 'x'}, {1, 'y'}};
            nh1 = m.extract(0);
            nh2 = m.extract(1);
        }
        construct_destroy_exact = true;
        nh1                     = std::move(nh2);
    }
    construct_destroy_exact = false;

    {
        using A  = tracked_allocator<std::pair<int const, char>>;
        using M  = std::multimap<int, char, std::less<>, A>;
        using NH = M::node_type;
        NH nh1;
        NH nh2;
        {
            M m{{0, 'x'}, {0, 'y'}};
            nh1 = m.extract(0);
            nh2 = m.extract(0);
        }
        construct_destroy_exact = true;
        nh1                     = std::move(nh2);
    }
    construct_destroy_exact = false;

    {
        using S  = std::set<int, std::less<>, tracked_allocator<int>>;
        using NH = S::node_type;
        NH nh1;
        NH nh2;
        {
            S s{0, 1};
            nh1 = s.extract(0);
            nh2 = s.extract(s.begin());
        }
        construct_destroy_exact = true;
        nh1                     = std::move(nh2);
    }
    construct_destroy_exact = false;

    {
        using S  = std::multiset<int, std::less<>, tracked_allocator<int>>;
        using NH = S::node_type;
        NH nh1;
        NH nh2;
        {
            S s{0, 0};
            nh1 = s.extract(0);
            nh2 = s.extract(s.begin());
        }
        construct_destroy_exact = true;
        nh1                     = std::move(nh2);
    }
    construct_destroy_exact = false;

    {
        using A  = tracked_allocator<std::pair<int const, char>>;
        using M  = std::unordered_map<int, char, std::hash<int>, std::equal_to<>, A>;
        using NH = M::node_type;
        NH nh1;
        NH nh2;
        {
            M m{{0, 'x'}, {1, 'y'}};
            nh1 = m.extract(0);
            nh2 = m.extract(m.begin());
        }
        construct_destroy_exact = true;
        nh1                     = std::move(nh2);
    }
    construct_destroy_exact = false;

    {
        using A  = tracked_allocator<std::pair<int const, char>>;
        using M  = std::unordered_multimap<int, char, std::hash<int>, std::equal_to<>, A>;
        using NH = M::node_type;
        NH nh1;
        NH nh2;
        {
            M m{{0, 'x'}, {0, 'y'}};
            nh1 = m.extract(0);
            nh2 = m.extract(m.begin());
        }
        construct_destroy_exact = true;
        nh1                     = std::move(nh2);
    }
    construct_destroy_exact = false;

    {
        using S  = std::unordered_set<int, std::hash<int>, std::equal_to<>, tracked_allocator<int>>;
        using NH = S::node_type;
        NH nh1;
        NH nh2;
        {
            S s{0, 1};
            nh1 = s.extract(0);
            nh2 = s.extract(s.begin());
        }
        construct_destroy_exact = true;
        nh1                     = std::move(nh2);
    }
    construct_destroy_exact = false;

    {
        using S  = std::unordered_multiset<int, std::hash<int>, std::equal_to<>, tracked_allocator<int>>;
        using NH = S::node_type;
        NH nh1;
        NH nh2;
        {
            S s{0, 0};
            nh1 = s.extract(0);
            nh2 = s.extract(s.begin());
        }
        construct_destroy_exact = true;
        nh1                     = std::move(nh2);
    }
    construct_destroy_exact = false;
}

int main() {
    test_map<std::map>();
    test_map<std::multimap>();

    test_set<std::set>();
    test_set<std::multiset>();

    test_unordered_map<std::unordered_map>();
    test_unordered_map<std::unordered_multimap>();

    test_unordered_set<std::unordered_set>();
    test_unordered_set<std::unordered_multiset>();

    test_gh_1309();
}
