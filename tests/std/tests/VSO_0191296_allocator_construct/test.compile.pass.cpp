// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

int main() {} // COMPILE-ONLY

// Work around VSO-119998 "/Za's elided-copy-ctor check is still bogus":
#ifdef _MSC_EXTENSIONS
// This test is loosely derived from dev11_437519_container_requirements, except the specific condition tested for is
// use of allocator::construct rather than constructing actual temporaries everywhere. Parts of
// dev11_437519_container_requirements where regressions of this kind are unlikely and testing of them is very difficult
// have been omitted.
//
// n3485 is the reference document for all language standard citations (unless otherwise noted).
//
// The primary difference vs. dev11_437519_container_requirements is that the value_type given to each of the containers
// meets none of the type requirements related to construction, unless the construction goes through
// allocator::construct.
//
// This is achieved by having the value_types' constructors all take struct alloc_key as their first argument, which
// is supplied by the allocator::construct, but by none of the other direct interactions with the container.
//

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
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

//
//
// GENERAL UTILITIES
//
//

struct alloc_key {};

struct key {};

template <typename T>
struct faux_predicate {
    bool operator()(T const&) const {
        return false;
    }
};

template <typename T>
struct faux_compare {
    bool operator()(T const&, T const&) const {
        return false;
    }
};

template <typename T>
struct default_constructible_compare {
    default_constructible_compare()                                     = default;
    default_constructible_compare(default_constructible_compare const&) = default;
    default_constructible_compare& operator=(default_constructible_compare const&) = delete;

    bool operator()(T const&, T const&) const {
        return false;
    }
};

template <typename T>
struct copy_constructible_compare {
    copy_constructible_compare() = delete;
    copy_constructible_compare(key) {}
    copy_constructible_compare(copy_constructible_compare const&) = default;
    copy_constructible_compare& operator=(copy_constructible_compare const&) = delete;

    bool operator()(T const&, T const&) const {
        return false;
    }
};

template <typename T>
struct faux_hash {
    std::size_t operator()(T const&) const {
        return 0;
    }
};

template <typename T>
struct default_constructible_hash {
    default_constructible_hash()                                  = default;
    default_constructible_hash(default_constructible_hash const&) = default;
    default_constructible_hash& operator=(default_constructible_hash const&) = delete;

    bool operator()(T const&, T const&) const {
        return false;
    }
};

template <typename T>
struct copy_constructible_hash {
    copy_constructible_hash() = delete;
    copy_constructible_hash(key) {}
    copy_constructible_hash(copy_constructible_hash const&) = default;
    copy_constructible_hash& operator=(copy_constructible_hash const&) = delete;

    std::size_t operator()(T const&) const {
        return 0;
    }
};

//
//
// ALLOCATORS FOR TESTING ALLOCATOR CONCEPTS
//
//

template <typename T, bool POCCA = true, bool POCMA = true, bool POCS = true>
struct construct_applying_allocator {
    using value_type = T;

    template <typename Other>
    struct rebind {
        using other = construct_applying_allocator<Other, POCCA, POCMA, POCS>;
    };

    construct_applying_allocator()                                    = default;
    construct_applying_allocator(const construct_applying_allocator&) = default;
    template <typename Other>
    construct_applying_allocator(const construct_applying_allocator<Other, POCCA, POCMA, POCS>&) {}

    construct_applying_allocator& operator=(const construct_applying_allocator&) = default;

    using propagate_on_container_copy_assignment = std::bool_constant<POCCA>;
    using propagate_on_container_move_assignment = std::bool_constant<POCMA>;
    using propagate_on_container_swap            = std::bool_constant<POCS>;
    using is_always_equal                        = std::false_type;

    template <typename Objty, typename... Types>
    void construct(Objty* const ptr, Types&&... args) const {
        ::new (static_cast<void*>(ptr)) Objty(alloc_key{}, std::forward<Types>(args)...);
    }

    template <typename Ty1, typename Ty2, typename... Types>
    void construct(std::pair<Ty1, Ty2>* const ptr, Types&&... args) const {
        ConstructPair(ptr, std::forward<Types>(args)...);
    }

    template <typename Ty1, typename Ty2>
    static void ConstructPair(std::pair<Ty1, Ty2>* const ptr) {
        ConstructPair(ptr, std::piecewise_construct, std::tuple<>{}, std::tuple<>{});
    }

    template <typename Ty1, typename Ty2, typename Arg1, typename Arg2>
    static void ConstructPair(std::pair<Ty1, Ty2>* const ptr, Arg1&& arg1, Arg2&& arg2) {
        ConstructPair(ptr, std::piecewise_construct, std::forward_as_tuple(std::forward<Arg1>(arg1)),
            std::forward_as_tuple(std::forward<Arg2>(arg2)));
    }

    template <typename Ty1, typename Ty2, typename Arg1, typename Arg2>
    static void ConstructPair(std::pair<Ty1, Ty2>* const ptr, const std::pair<Arg1, Arg2>& src) {
        ConstructPair(
            ptr, std::piecewise_construct, std::forward_as_tuple(src.first), std::forward_as_tuple(src.second));
    }

    template <typename Ty1, typename Ty2, typename Arg1, typename Arg2>
    static void ConstructPair(std::pair<Ty1, Ty2>* const ptr, std::pair<Arg1, Arg2>&& src) {
        ConstructPair(ptr, std::piecewise_construct, std::forward_as_tuple(std::forward<Arg1>(src.first)),
            std::forward_as_tuple(std::forward<Arg2>(src.second)));
    }

    template <typename Ty1, typename Ty2, class... Types1, class... Types2>
    static void ConstructPair(std::pair<Ty1, Ty2>* const ptr, std::piecewise_construct_t, std::tuple<Types1...> tup1,
        std::tuple<Types2...> tup2) {
        ::new (ptr) std::pair<Ty1, Ty2>(std::piecewise_construct,
            std::tuple_cat(std::tuple<alloc_key>{}, std::tuple<Types1&&...>(std::move(tup1))),
            std::tuple_cat(std::tuple<alloc_key>{}, std::tuple<Types2&&...>(std::move(tup2))));
    }

    T* allocate(const size_t n) {
        return std::allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) {
        std::allocator<T>{}.deallocate(p, n);
    }

    template <typename Other>
    bool operator==(const construct_applying_allocator<Other, POCCA, POCMA, POCS>&) const noexcept {
        return true;
    }

    template <typename Other>
    bool operator!=(const construct_applying_allocator<Other, POCCA, POCMA, POCS>&) const noexcept {
        return false;
    }
};

//
//
// CONCEPT TEST TYPES
//
//

// These are test types that provide limited functionality and with which we can instantiate the
// Standard Library containers.  The types that only model a single concept have their full name
// spelled out; types that model several concepts are abbreviated (e.g. DefaultConstructible +
// MoveInsertable is abbreviated as dc_mi).

struct emplace_argument {
    emplace_argument() = default;
    emplace_argument(key) {}
};

// clang-format off

#define DEFINE_TYPE(name, def_ctor, copy_ctor, move_ctor, emp_ctor, swappable, copy_assign, move_assign, emp_assign) \
    class name {                                                                                                     \
    public:              name(key) { }                                                                               \
    public:              ~name() { }                                                                                 \
    private: def_ctor    name(alloc_key) { }                                                                         \
    private: copy_ctor   name(alloc_key, name const&) { }                                                            \
    private: move_ctor   name(alloc_key, name&&) { }                                                                 \
    private: emp_ctor    name(alloc_key, emplace_argument&&) { }                                                     \
    private: emp_ctor    name(alloc_key, emplace_argument&&, emplace_argument&&) { }                                 \
    private: emp_ctor    name(alloc_key, emplace_argument&&, emplace_argument&&, emplace_argument&&) { }             \
    private: swappable   friend void swap(name&, name&) {}                                                           \
    private: copy_assign name& operator=(name const&) { return *this; }                                              \
    private: move_assign name& operator=(name&&) { return *this; }                                                   \
    private: emp_assign  name& operator=(emplace_argument&&) { return *this; }                                       \
    private:             name() {}                                                                                   \
    private:             name(const name&) {}                                                                        \
    }

#define YES public:

//                                 default   copy   move   emplace                copy     move    emplace
//                                  ctor     ctor   ctor    ctor     swappable   assign   assign   assign
DEFINE_TYPE(erasable             ,         ,      ,      ,         ,           ,        ,        ,             );
DEFINE_TYPE(default_constructible, YES     ,      ,      ,         ,           ,        ,        ,             );
DEFINE_TYPE(copy_insertable      ,         , YES  , YES  ,         ,           ,        ,        ,             );
DEFINE_TYPE(move_insertable      ,         ,      , YES  ,         ,           ,        ,        ,             );
DEFINE_TYPE(emplace_constructible,         ,      ,      , YES     ,           ,        ,        ,             );
DEFINE_TYPE(copy_assignable      ,         ,      ,      ,         , YES       , YES    , YES    ,             );
DEFINE_TYPE(move_assignable      ,         ,      ,      ,         , YES       ,        , YES    ,             );
DEFINE_TYPE(equality_comparable  ,         ,      ,      ,         ,           ,        ,        ,             );
DEFINE_TYPE(less_comparable      ,         ,      ,      ,         ,           ,        ,        ,             );
DEFINE_TYPE(ca_ci                ,         , YES  , YES  ,         , YES       , YES    , YES    ,             );
DEFINE_TYPE(ci_ma                ,         , YES  , YES  ,         , YES       ,        , YES    ,             );
DEFINE_TYPE(dc_mi                , YES     ,      , YES  ,         ,           ,        ,        ,             );
DEFINE_TYPE(ec_ma_mi             ,         ,      , YES  , YES     , YES       ,        , YES    ,             );
DEFINE_TYPE(ec_mi                ,         ,      , YES  , YES     ,           ,        ,        ,             );
DEFINE_TYPE(ma_mi                ,         ,      , YES  ,         , YES       ,        , YES    ,             );
DEFINE_TYPE(ec_ea                ,         ,      ,      , YES     ,           ,        ,        , YES         );
DEFINE_TYPE(ec_ea_mi             ,         ,      , YES  , YES     ,           ,        ,        , YES         );

#undef YES

// clang-format on

bool operator==(equality_comparable const&, equality_comparable const&) {
    return true;
}
bool operator<(less_comparable const&, less_comparable const&) {
    return false;
}


//
//
// CONTAINER TRAITS
//
//

enum container_tag {
    tag_deque        = (1 << 0),
    tag_forward_list = (1 << 1),
    tag_list         = (1 << 2),
    tag_vector       = (1 << 3),

    tag_map      = (1 << 4),
    tag_multimap = (1 << 5),
    tag_set      = (1 << 6),
    tag_multiset = (1 << 7),

    tag_unordered_map      = (1 << 8),
    tag_unordered_multimap = (1 << 9),
    tag_unordered_set      = (1 << 10),
    tag_unordered_multiset = (1 << 11)
};

enum classification {
    c_is_reversible            = (1 << 0),
    c_is_optional              = (1 << 1),
    c_is_sequence              = (1 << 2),
    c_is_ordered_associative   = (1 << 3),
    c_is_unordered_associative = (1 << 4),
    c_is_unique_associative    = (1 << 5),

    c_is_ordinary_sequence = (c_is_reversible | c_is_optional | c_is_sequence),
    c_is_ordinary_unique_ordered_associative =
        (c_is_reversible | c_is_optional | c_is_ordered_associative | c_is_unique_associative),
    c_is_ordinary_nonunique_ordered_associative   = (c_is_reversible | c_is_optional | c_is_ordered_associative),
    c_is_ordinary_unique_unordered_associative    = (c_is_unordered_associative | c_is_unique_associative),
    c_is_ordinary_nonunique_unordered_associative = (c_is_unordered_associative),
    c_is_forward_list                             = (c_is_optional | c_is_sequence)
};

struct identity_bind_value {
    template <typename V>
    struct bind_value {
        typedef V type;
    };

    typedef emplace_argument emplace_argument_type;

    template <typename V>
    static V construct_value() {
        return {key()};
    }
};

struct map_associative_bind_value {
    template <typename V>
    struct bind_value {
        typedef std::pair<V const, V> type;
    };

    typedef std::pair<emplace_argument, emplace_argument> emplace_argument_type;

    template <typename V>
    static std::pair<const V, V> construct_value() {
        return {std::piecewise_construct, std::tuple<key>{}, std::tuple<key>{}};
    }
};

template <template <typename, typename> class C>
struct sequence_bind_container : identity_bind_value {
    template <typename V, typename A = construct_applying_allocator<V>>
    struct bind_container {
        typedef C<V, A> type;
    };
};

template <template <typename, typename, typename, typename> class C>
struct ordered_map_associative_bind_container : map_associative_bind_value {
    template <typename V, typename A = construct_applying_allocator<V>, typename P = faux_compare<V>>
    struct bind_container {
        typedef typename bind_value<V>::type value_type;
        typedef typename std::allocator_traits<A>::template rebind_alloc<value_type> alloc_type;

        typedef C<V, V, P, alloc_type> type;
    };
};

template <template <typename, typename, typename> class C>
struct ordered_set_associative_bind_container : identity_bind_value {
    template <typename V, typename A = construct_applying_allocator<V>, typename P = faux_compare<V>>
    struct bind_container {
        typedef C<V, P, A> type;
    };
};

template <template <typename, typename, typename, typename, typename> class C>
struct unordered_map_associative_bind_container : map_associative_bind_value {
    template <typename V, typename A = construct_applying_allocator<V>, typename H = faux_hash<V>,
        typename P = faux_compare<V>>
    struct bind_container {
        typedef typename bind_value<V>::type value_type;
        typedef typename std::allocator_traits<A>::template rebind_alloc<value_type> alloc_type;

        typedef C<V, V, H, P, alloc_type> type;
    };
};

template <template <typename, typename, typename, typename> class C>
struct unordered_set_associative_bind_container : identity_bind_value {
    template <typename V, typename A = construct_applying_allocator<V>, typename H = faux_hash<V>,
        typename P = faux_compare<V>>
    struct bind_container {
        typedef C<V, H, P, A> type;
    };
};

template <container_tag Tag, classification Features>
struct container_traits_base {
    static container_tag const tag       = Tag;
    static classification const features = Features;
};

template <container_tag Tag, classification Features, template <typename, typename> class C>
struct sequence_traits_base : container_traits_base<Tag, Features>, sequence_bind_container<C> {};

template <container_tag Tag, classification Features, template <typename, typename, typename, typename> class C>
struct ordered_map_associative_container_base : container_traits_base<Tag, Features>,
                                                ordered_map_associative_bind_container<C> {};

template <container_tag Tag, classification Features, template <typename, typename, typename> class C>
struct ordered_set_associative_container_base : container_traits_base<Tag, Features>,
                                                ordered_set_associative_bind_container<C> {};

template <container_tag Tag, classification Features,
    template <typename, typename, typename, typename, typename> class C>
struct unordered_map_associative_container_base : container_traits_base<Tag, Features>,
                                                  unordered_map_associative_bind_container<C> {};

template <container_tag Tag, classification Features, template <typename, typename, typename, typename> class C>
struct unordered_set_associative_container_base : container_traits_base<Tag, Features>,
                                                  unordered_set_associative_bind_container<C> {};

template <container_tag>
struct container_traits;

template <>
struct container_traits<tag_deque> : sequence_traits_base<tag_deque, c_is_ordinary_sequence, std::deque> {};

template <>
struct container_traits<tag_forward_list>
    : sequence_traits_base<tag_forward_list, c_is_forward_list, std::forward_list> {};

template <>
struct container_traits<tag_list> : sequence_traits_base<tag_list, c_is_ordinary_sequence, std::list> {};

template <>
struct container_traits<tag_vector> : sequence_traits_base<tag_vector, c_is_ordinary_sequence, std::vector> {};

template <>
struct container_traits<tag_map>
    : ordered_map_associative_container_base<tag_map, c_is_ordinary_unique_ordered_associative, std::map> {};

template <>
struct container_traits<tag_multimap>
    : ordered_map_associative_container_base<tag_multimap, c_is_ordinary_nonunique_ordered_associative, std::multimap> {
};

template <>
struct container_traits<tag_multiset>
    : ordered_set_associative_container_base<tag_multiset, c_is_ordinary_nonunique_ordered_associative, std::multiset> {
};

template <>
struct container_traits<tag_set>
    : ordered_set_associative_container_base<tag_set, c_is_ordinary_unique_ordered_associative, std::set> {};

template <>
struct container_traits<tag_unordered_map> : unordered_map_associative_container_base<tag_unordered_map,
                                                 c_is_ordinary_unique_unordered_associative, std::unordered_map> {};

template <>
struct container_traits<tag_unordered_multimap>
    : unordered_map_associative_container_base<tag_unordered_multimap, c_is_ordinary_nonunique_unordered_associative,
          std::unordered_multimap> {};

template <>
struct container_traits<tag_unordered_multiset>
    : unordered_set_associative_container_base<tag_unordered_multiset, c_is_ordinary_nonunique_unordered_associative,
          std::unordered_multiset> {};

template <>
struct container_traits<tag_unordered_set> : unordered_set_associative_container_base<tag_unordered_set,
                                                 c_is_ordinary_unique_unordered_associative, std::unordered_set> {};


//
//
// TEST GENERATORS
//
//

// clang-format off

#ifdef __clang__
    #pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif // __clang__

#define GENERATE_CONTAINER_TYPEDEFS(name, opt_typename)                             \
    typedef opt_typename Traits::template bind_container<name>::type c_of_ ## name; \
    typedef opt_typename Traits::template bind_value    <name>::type v_of_ ## name;

#define GENERATE_ALL_CONTAINER_TYPEDEFS(unused, opt_typename)        \
    GENERATE_CONTAINER_TYPEDEFS(erasable,              opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(default_constructible, opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(copy_insertable,       opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(move_insertable,       opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(emplace_constructible, opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(copy_assignable,       opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(move_assignable,       opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(equality_comparable,   opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(less_comparable,       opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(ca_ci,                 opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(ci_ma,                 opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(dc_mi,                 opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(ec_ma_mi,              opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(ec_mi,                 opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(ma_mi,                 opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(ec_ea,                 opt_typename) \
    GENERATE_CONTAINER_TYPEDEFS(ec_ea_mi,              opt_typename)

#define DEFINE_TEST_IMPL(name, opt_typename, template_parameters, specialization, preface_text, ...) \
    template <template_parameters>                                                                   \
    struct name specialization {                                                                     \
        preface_text                                                                                 \
        typedef container_traits<Tag> Traits;                                                        \
                                                                                                     \
        name() {                                                                                     \
            GENERATE_ALL_CONTAINER_TYPEDEFS(unused, opt_typename);                                   \
            __VA_ARGS__                                                                              \
        }                                                                                            \
    }

#define DEFINE_TEST(name, ...)                                                       \
    DEFINE_TEST_IMPL(                                                                \
        /* name                */ name,                                              \
        /* opt_typename        */ typename,                                          \
        /* template_parameters */ container_tag Tag,                                 \
        /* specialization      */ /* empty */,                                       \
        /* preface_text        */ /* empty */,                                       \
        /* ...                 */ __VA_ARGS__)

#define DEFINE_TEST_SPECIALIZATION(name, specialized_tag, ...)                       \
    DEFINE_TEST_IMPL(                                                                \
        /* name                */ name,                                              \
        /* opt_typename        */ /* empty */,                                       \
        /* template_parameters */ /* empty */,                                       \
        /* specialization      */ <specialized_tag>,                                 \
        /* preface_text        */ static container_tag const Tag = specialized_tag;, \
        /* ...                 */ __VA_ARGS__)

// These macros prevent the test named 'name' from being run for the container specified by the
// 'specialized_tag'.  The NOT_SUPPORTED_SPECIALIZATION macro should be used for tests that do not
// pass because the container is specified as not supporting the operation being tested.

#define NOT_SUPPORTED_SPECIALIZATION(name, specialized_tag) \
    DEFINE_TEST_SPECIALIZATION(name, specialized_tag, { })

// clang-format on


//
//
// CONTAINER REQUIREMENTS (23.2.1[container.requirements.general]/Table 96)
//
//


DEFINE_TEST(test_container_default_constructor, {
    // X u; and X()
    // [No Requires clause]
    c_of_erasable a;
    (c_of_erasable());
});


DEFINE_TEST(test_container_copy_constructor, {
    // X(a), X u(a), and X u = a
    // Requires:  T is CopyInsertable into X
    c_of_copy_insertable a;
    (c_of_copy_insertable(a));
    c_of_copy_insertable u0(a);
    c_of_copy_insertable u1 = a;
});


DEFINE_TEST(test_container_move_constructor, {
    // X u(rv) and X u = rv
    // [No Requires clause]
    c_of_erasable rv;
    c_of_erasable u0(std::move(rv));
    c_of_erasable u1 = std::move(u0);
});


DEFINE_TEST(test_container_swap, {
    // a.swap(b) and swap(a, b)
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable b;
    a.swap(b);
    swap(a, b);
});


template <container_tag Tag>
void check_all_container_requirements() {
    test_container_default_constructor<Tag>();
    test_container_copy_constructor<Tag>();
    test_container_move_constructor<Tag>();
    test_container_swap<Tag>();
};

//
//
// ALLOCATOR-AWARE CONTAINER REQUIREMENTS (23.2.1[container.requirements.general]/Table 99)
//
//

DEFINE_TEST(test_allocator_aware_allocator_constructor_default, {
    // X(m) and X u(m)
    // [No Requires clause]
    typename c_of_erasable::allocator_type m;
    (c_of_erasable(m));
    c_of_erasable u(m);
});


DEFINE_TEST(test_allocator_aware_allocator_constructor_copy, {
    // X(t, m) and X u(t, m)
    // Requires:  T is CopyInsertable into X
    typename c_of_copy_insertable::allocator_type m;
    c_of_copy_insertable t;
    (c_of_copy_insertable(t, m));
    c_of_copy_insertable u(t, m);
});


DEFINE_TEST(test_allocator_aware_allocator_constructor_move, {
    // X(rv, m) and X u(rv, m)
    // Requires:  T is MoveInsertable
    typename c_of_move_insertable::allocator_type m;
    (c_of_move_insertable(c_of_move_insertable(), m));
    c_of_move_insertable u(c_of_move_insertable(), m);
});


DEFINE_TEST(test_allocator_aware_assignment_from_rvalue_non_pocma, {
    typedef
        typename Traits::template bind_container<ma_mi, construct_applying_allocator<ma_mi, true, false, true>>::type
            container_type;

    // a = rv
    // Requires:  If allocator_traits<allocator_type>::propagate_on_container_move_assignment::value
    // is false, T is MoveInsertable into X and MoveAssignable.
    {
        container_type a;
        a = container_type();
    }
});


DEFINE_TEST(test_allocator_aware_assignment_from_rvalue_pocma, {
    typedef typename Traits::template bind_container<erasable,
        construct_applying_allocator<erasable, false, true, false>>::type container_type;

    // a = rv
    // [No Requires clause]
    //
    // (If the allocator _does_ propagate on move assignment, there are no additional requirements,
    // so T needs only be Erasable.)
    {
        container_type a;
        a = container_type();
    }
});


DEFINE_TEST(test_allocator_aware_assignment_from_lvalue, {
    // a = t
    // Requires:  T is CopyInsertable into X and CopyAssignable
    c_of_ca_ci a;
    c_of_ca_ci const b;
    a = b;
});


template <container_tag Tag>
void check_all_allocator_aware_requirements() {
    test_allocator_aware_allocator_constructor_default<Tag>();
    test_allocator_aware_allocator_constructor_copy<Tag>();
    test_allocator_aware_allocator_constructor_move<Tag>();
    test_allocator_aware_assignment_from_lvalue<Tag>();
    test_allocator_aware_assignment_from_rvalue_non_pocma<Tag>();
    test_allocator_aware_assignment_from_rvalue_pocma<Tag>();
}


//
//
// SEQUENCE CONTAINER REQUIREMENTS (23.2.3[sequence.reqmts]/Table 100)
//
//

DEFINE_TEST(test_sequence_count_constructor_copy, {
    // X(n, t) and X a(n, t)
    // Requires:  T shall be CopyInsertable into X
    c_of_copy_insertable(1, copy_insertable(key()));
    c_of_copy_insertable a(1, copy_insertable(key()));
});


DEFINE_TEST(test_sequence_range_constructor, {
    // X(i, j) and X a(i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i
    std::move_iterator<emplace_argument*> const i;
    c_of_emplace_constructible(i, i);
    c_of_emplace_constructible a(i, i);
});

DEFINE_TEST_SPECIALIZATION(test_sequence_range_constructor, tag_vector,
{
    // X(i, j) and X a(i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i.  For vector, if the iterator does
    // not meet the forward iterator requirements, T shall also be MoveInsertable into X.
    {
         std::move_iterator<emplace_argument*> i;
         c_of_emplace_constructible(i, i);
         c_of_emplace_constructible a(i, i);
}
{
    std::move_iterator<emplace_argument*> i;
    c_of_ec_mi(i, i);
    c_of_ec_mi a(i, i);
}
});

DEFINE_TEST(test_sequence_emplace, {
    // a.emplace(p, args)
    // Requires: T is EmplaceConstructible into X from args.
    c_of_emplace_constructible a;
    a.emplace(a.begin(), emplace_argument());
    a.emplace(a.begin(), emplace_argument(), emplace_argument());
    a.emplace(a.begin(), emplace_argument(), emplace_argument(), emplace_argument());
});

DEFINE_TEST(test_sequence_emplace_deque_vector, {
    // a.emplace(p, args)
    // Requires: T is EmplaceConstructible into X from args. For vector and deque, T is also
    // MoveInsertable into X and MoveAssignable.
    c_of_ec_ma_mi a;
    a.emplace(a.begin(), emplace_argument());
    a.emplace(a.begin(), emplace_argument(), emplace_argument());
    a.emplace(a.begin(), emplace_argument(), emplace_argument(), emplace_argument());
});

DEFINE_TEST_SPECIALIZATION(test_sequence_emplace, tag_deque, { test_sequence_emplace_deque_vector<Tag>(); });
DEFINE_TEST_SPECIALIZATION(test_sequence_emplace, tag_vector, { test_sequence_emplace_deque_vector<Tag>(); });
NOT_SUPPORTED_SPECIALIZATION(test_sequence_emplace, tag_forward_list);


DEFINE_TEST(test_sequence_copy_insert_one, {
    // a.insert(p, t)
    // Requires:  T shall be CopyInsertable into X.
    c_of_copy_insertable a;
    copy_insertable const t((key()));
    a.insert(a.begin(), t);
});

DEFINE_TEST(test_sequence_copy_insert_one_deque_vector, {
    // a.insert(p, t)
    // Requires:  T shall be CopyInsertable into X.  For vector and deque, T shall also be
    // MoveAssignable.
    c_of_ca_ci a;
    ca_ci const t((key()));
    a.insert(a.begin(), t);
});

DEFINE_TEST_SPECIALIZATION(test_sequence_copy_insert_one, tag_deque, { test_sequence_emplace_deque_vector<Tag>(); });
DEFINE_TEST_SPECIALIZATION(test_sequence_copy_insert_one, tag_vector, { test_sequence_emplace_deque_vector<Tag>(); });
NOT_SUPPORTED_SPECIALIZATION(test_sequence_copy_insert_one, tag_forward_list);


DEFINE_TEST(test_sequence_move_insert_one, {
    // a.insert(p, rv)
    // Requires:  T shall be MoveInsertable into X.
    c_of_move_insertable a;
    a.insert(a.begin(), move_insertable(key()));

    // Return type:  iterator
    static_assert(
        std::is_same_v<decltype(a.insert(a.begin(), move_insertable(key()))), typename c_of_move_insertable::iterator>,
        "");
});

DEFINE_TEST(test_sequence_move_insert_one_deque_vector, {
    // a.insert(p, rv)
    // Requires:  T shall be MoveInsertable into X.  For vector and deque, T shall also be
    // MoveAssignable.
    c_of_ma_mi a;
    a.insert(a.begin(), ma_mi(key()));
});

DEFINE_TEST_SPECIALIZATION(
    test_sequence_move_insert_one, tag_deque, { test_sequence_move_insert_one_deque_vector<Tag>(); });
DEFINE_TEST_SPECIALIZATION(
    test_sequence_move_insert_one, tag_vector, { test_sequence_move_insert_one_deque_vector<Tag>(); });
NOT_SUPPORTED_SPECIALIZATION(test_sequence_move_insert_one, tag_forward_list);


DEFINE_TEST(test_sequence_copy_insert_n, {
    // a.insert(p, n, t)
    // Requires:  T shall be CopyInsertable into X and CopyAssignable.
    c_of_ca_ci a;
    ca_ci const t((key()));
    a.insert(a.begin(), 1, t);
});

NOT_SUPPORTED_SPECIALIZATION(test_sequence_copy_insert_n, tag_forward_list);


DEFINE_TEST(test_sequence_range_insert, {
    // a.insert(p, i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i.
    c_of_emplace_constructible a;
    std::move_iterator<emplace_argument*> const i;
    a.insert(a.begin(), i, i);
});

DEFINE_TEST(test_sequence_range_insert_deque_vector,
{
    // a.insert(p, i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i.  For vector, if the iterator does
    // not meet the forward iterator requirements, T shall also be MoveInsertable into X and
    // MoveAssignable.

    // SPEC:  There is no way to meet the specification here.  Inserting elements into the middle of
    // a vector may require move construction and move assignment of elements after the insertion
    // point.  The same is true for deque.  The specification should be altered to read as follows:
    //
    //     For deque and vector, T shall also be MoveInsertable into X and MoveAssignable.
    //
    // The test here verifies this behavior.
    {
        c_of_ec_ma_mi a;
        std::move_iterator<emplace_argument*> const i;
        a.insert(a.begin(), i, i);
}
});

DEFINE_TEST_SPECIALIZATION(test_sequence_range_insert, tag_deque, { test_sequence_range_insert_deque_vector<Tag>(); });
DEFINE_TEST_SPECIALIZATION(test_sequence_range_insert, tag_vector, { test_sequence_range_insert_deque_vector<Tag>(); });
NOT_SUPPORTED_SPECIALIZATION(test_sequence_range_insert, tag_forward_list);

DEFINE_TEST(test_sequence_range_assign, {
    // [N4606]
    // a.assign(i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i and assignable from *i.

    c_of_ec_ea a;
    std::move_iterator<emplace_argument*> const i;
    a.assign(i, i);
});

DEFINE_TEST_SPECIALIZATION(test_sequence_range_assign, tag_vector,
{
    // [N4606]
    // a.assign(i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i and assignable from *i.
    // For vector, if the iterator does not meet the forward iterator requirements (24.2.5),
    // T shall also be MoveInsertable into X.

    {
        c_of_ec_ea a;
        std::move_iterator<emplace_argument*> i;
        a.assign(i, i);
}

{
    c_of_ec_ea_mi a;
    std::move_iterator<emplace_argument*> i;
    a.assign(i, i);
}
});


DEFINE_TEST(test_sequence_assign_n, {
    // a.assign(n, t)
    // Requires:  T shall be CopyInsertable into X and CopyAssignable.
    c_of_ca_ci a;
    ca_ci const t((key()));
    a.assign(1, t);
});


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_sequence) != 0>
struct check_all_sequence_requirements {
    check_all_sequence_requirements() {
        test_sequence_count_constructor_copy<Tag>();
        test_sequence_range_constructor<Tag>();

        test_sequence_emplace<Tag>();
        test_sequence_copy_insert_one<Tag>();
        test_sequence_move_insert_one<Tag>();
        test_sequence_copy_insert_n<Tag>();
        test_sequence_range_insert<Tag>();

        test_sequence_range_assign<Tag>();
        test_sequence_assign_n<Tag>();
    }
};

template <container_tag Tag>
struct check_all_sequence_requirements<Tag, false> {};


//
//
// OPTIONAL SEQUENCE CONTAINER REQUIREMENTS (23.2.3[sequence.reqmts]/Table 101)
//
//
DEFINE_TEST(test_optional_sequence_emplace_front, {
    // a.emplace_front(args)
    // Requires:  T shall be EmplaceConstructible into X from args
    c_of_emplace_constructible a;
    a.emplace_front(emplace_argument());
    a.emplace_front(emplace_argument(), emplace_argument());
    a.emplace_front(emplace_argument(), emplace_argument(), emplace_argument());
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_emplace_front, tag_vector);


DEFINE_TEST(test_optional_sequence_emplace_back, {
    // a.emplace_back(args)
    // Requires:  T shall be EmplaceConstructible into X from args
    c_of_emplace_constructible a;
    a.emplace_back(emplace_argument());
    a.emplace_back(emplace_argument(), emplace_argument());
    a.emplace_back(emplace_argument(), emplace_argument(), emplace_argument());
});

DEFINE_TEST_SPECIALIZATION(test_optional_sequence_emplace_back, tag_vector, {
    // a.emplace_back(args)
    // Requires:  T shall be EmplaceConstructible into X from args.  For vector, T shall be
    // MoveInsertable into X.
    c_of_ec_mi a;
    a.emplace_back(emplace_argument());
    a.emplace_back(emplace_argument(), emplace_argument());
    a.emplace_back(emplace_argument(), emplace_argument(), emplace_argument());
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_emplace_back, tag_forward_list);


DEFINE_TEST(test_optional_sequence_push_front,
{
    {
        // a.push_front(t)
        // Requires:  T shall be CopyInsertable into X
        c_of_copy_insertable a;
        copy_insertable const t((key()));
        a.push_front(t);
}

{
    // a.push_front(rv)
    // Requires:  T shall be MoveInsertable into X
    c_of_move_insertable a;
    a.push_front(move_insertable(key()));
}
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_push_front, tag_vector);


DEFINE_TEST(test_optional_sequence_push_back,
{
    {
        // a.push_back(t)
        // Requires:  T shall be CopyInsertable into X
        c_of_copy_insertable a;
        copy_insertable const t((key()));
        a.push_back(t);
}

{
    // a.push_back(rv)
    // Requires:  T shall be MoveInsertable into X
    c_of_move_insertable a;
    a.push_back(move_insertable(key()));
}
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_push_back, tag_forward_list);


DEFINE_TEST(test_optional_sequence_subscript, {
    // a[n] and a.at(n)
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable const b;

    (void) a[0];
    (void) b[0];

    (void) a.at(0);
    (void) b.at(0);
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_subscript, tag_forward_list);
NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_subscript, tag_list);


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_sequence) != 0>
struct check_all_optional_sequence_requirements {
    check_all_optional_sequence_requirements() {
        test_optional_sequence_emplace_front<Tag>();
        test_optional_sequence_emplace_back<Tag>();
        test_optional_sequence_push_front<Tag>();
        test_optional_sequence_push_back<Tag>();
        test_optional_sequence_subscript<Tag>();
    }
};

template <container_tag Tag>
struct check_all_optional_sequence_requirements<Tag, false> {};


//
//
// ORDERED ASSOCIATIVE CONTAINER REQUIREMENTS (23.2.4[associative.reqmts]/Table 102)
//
//

DEFINE_TEST(test_ordered_associative_default_constructor_with_comparer,
{
    // X(c) and X a(c);
    // Requires:  key_compare is CopyConstructible
    {
        typedef typename Traits::template bind_container<
            erasable,
            construct_applying_allocator<erasable>,
            copy_constructible_compare<erasable>
        >::type container_type;

        copy_constructible_compare<erasable> const c((key()));
        (container_type(c));
        container_type a(c);
}

// X() and X a;
// Requires:  key_compare is DefaultConstructible
{
    typedef typename Traits::template bind_container<erasable, construct_applying_allocator<erasable>,
        default_constructible_compare<erasable>>::type container_type;

    (container_type());
    container_type a;
}
});


DEFINE_TEST(test_ordered_associative_range_constructor_with_comparer, {
    // SPEC:  These are the only two operations for which traits are specified for key_compare, but
    // there are many other operations for which key_compare must meet other requirements (e.g. a
    // call to a.key_comp() necessitates that the key_comp type is move constructible.
    //
    // In the rest of the tests, I have assumed std::less for the comparer, which meets any possible
    // requirements, but it should be considered whether the comparer requirements should be more
    // thoroughly specified.

    std::move_iterator<typename Traits::emplace_argument_type*> i;
    typename c_of_emplace_constructible::key_compare c;
    c_of_emplace_constructible(i, i);
    c_of_emplace_constructible a1(i, i);
    c_of_emplace_constructible(i, i, c);
    c_of_emplace_constructible a2(i, i, c);
});


DEFINE_TEST(test_ordered_associative_emplace, {
    // a_uniq.emplace(args), a_eq.emplace(args)
    // Requires:  value_type shall be EmplaceConstructible into X from args
    c_of_emplace_constructible a;
    a.emplace(emplace_argument(), emplace_argument());

    // a.emplace_hint(p, args)
    // Requires:  equivalent to a.emplace(args)
    a.emplace_hint(a.begin(), emplace_argument(), emplace_argument());
});


DEFINE_TEST(test_ordered_associative_insert, {
    // a_uniq.insert(t) and a_eq.insert(t)
    // Requires:  If t is a non-const rvalue expression, value_type shall be MoveInsertable into X;
    // otherwise, value_type shall be CopyInsertable into X.
    c_of_copy_insertable ac;
    ac.insert(Traits::template construct_value<copy_insertable>());

    // a.insert(p, t)
    // Requires:  If t is a non-const rvalue expression, value_type shall be MoveInsertable into X;
    // otherwise, value_type shall be CopyInsertable into X.
    ac.insert(ac.begin(), Traits::template construct_value<copy_insertable>());

    // Returns:  iterator

    // a.insert(i, j)
    // Requires:  value_type shall be EmplaceConstructible into X from *i
    c_of_emplace_constructible ea;
    std::move_iterator<typename Traits::emplace_argument_type*> i;
    ea.insert(i, i);
});

template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_ordered_associative) != 0>
struct check_all_ordered_associative_requirements {
    check_all_ordered_associative_requirements() {
        test_ordered_associative_default_constructor_with_comparer<Tag>();
        test_ordered_associative_range_constructor_with_comparer<Tag>();
        test_ordered_associative_emplace<Tag>();
        test_ordered_associative_insert<Tag>();
    }
};

template <container_tag Tag>
struct check_all_ordered_associative_requirements<Tag, false> {};


//
//
// UNORDERED ASSOCIATIVE CONTAINER REQUIREMENTS (23.2.5[unord.req]/Table 103)
//
//

DEFINE_TEST(test_unordered_associative_default_constructors,
{
    // X(n, hf, eq) and X a(n, hf, eq)
    // Requires:  hasher and key_equal are CopyConstructible.
    {
        typedef typename Traits::template bind_container<
            erasable,
            construct_applying_allocator<erasable>,
            copy_constructible_hash<erasable>,
            copy_constructible_compare<erasable>
        >::type container_type;

        copy_constructible_hash<erasable> const hf((key()));
        copy_constructible_compare<erasable> const eq((key()));
        (container_type(0, hf, eq));
        container_type a(0, hf, eq);
}

// X(n, hf) and X a(n, hf)
// Requires:  hasher is CopyConstructible and key_equal is DefaultConstructible
{
    typedef typename Traits::template bind_container<erasable, construct_applying_allocator<erasable>,
        copy_constructible_hash<erasable>, default_constructible_compare<erasable>>::type container_type;

    copy_constructible_hash<erasable> const hf((key()));
    (container_type(0, hf));
    container_type a(0, hf);
}

// X(n) and X a(n)
// Requires:  hasher and key_equal are DefaultConstructible
{
    (c_of_erasable(0));
    c_of_erasable a(0);
}

// X() and X a
// Requires:  hasher and key_equal are DefaultConstructible
{
    (c_of_erasable());
    c_of_erasable a;
}
});


DEFINE_TEST(test_unordered_associative_range_constructors, {
    std::move_iterator<typename Traits::emplace_argument_type*> i;

    // X(i, j, n, hf, eq) and X a(i, j, n, hf, eq)
    // Requires:  hasher and key_equal are CopyConstructible.  value_type is EmplaceConstructible
    // into X from *i.
    {
        typedef typename Traits::template bind_container<emplace_constructible,
            construct_applying_allocator<emplace_constructible>, copy_constructible_hash<emplace_constructible>,
            copy_constructible_compare<emplace_constructible>>::type container_type;

        copy_constructible_hash<emplace_constructible> const hf((key()));
        copy_constructible_compare<emplace_constructible> const eq((key()));
        (container_type(i, i, 0, hf, eq));
        container_type a(i, i, 0, hf, eq);
    }

    // X(i, j, n, hf) and X a(i, j, n, hf)
    // Requires:  hasher is CopyConstructible and key_equal is DefaultConstructible.  value_type is
    // EmplaceConstructible into X from *i.
    {
        typedef typename Traits::template bind_container<emplace_constructible,
            construct_applying_allocator<emplace_constructible>, copy_constructible_hash<emplace_constructible>,
            default_constructible_compare<emplace_constructible>>::type container_type;

        copy_constructible_hash<emplace_constructible> const hf((key()));
        (container_type(i, i, 0, hf));
        container_type a(i, i, 0, hf);
    }

    // X(i, j, n) and X a(i, j, n)
    // Requires:  hasher and key_equal are DefaultConstructible.  value_type is EmplaceConstructible
    // into X from *i.
    {
        (c_of_emplace_constructible(i, i, 0));
        c_of_emplace_constructible a(i, i, 0);
    }

    // X(i, j) and X a(i, j)
    // Requires:  hasher and key_equal are DefaultConstructible.  value_type is EmplaceConstructible
    // into X from *i.
    {
        (c_of_emplace_constructible(i, i));
        c_of_emplace_constructible a(i, i);
    }
});


DEFINE_TEST(test_unordered_associative_rehash, {
    c_of_erasable a;

    // a.rehash(n)
    // [No Requires clause]
    a.rehash(0);

    // a.reserve(n)
    // [No Requires clause]
    a.reserve(0);
});


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_unordered_associative) != 0>
struct check_all_unordered_associative_requirements {
    check_all_unordered_associative_requirements() {
        test_unordered_associative_default_constructors<Tag>();
        test_unordered_associative_range_constructors<Tag>();

        // These operations are the same as their ordered counterparts
        test_ordered_associative_emplace<Tag>();
        test_ordered_associative_insert<Tag>();

        test_unordered_associative_rehash<Tag>();
    }
};

template <container_tag Tag>
struct check_all_unordered_associative_requirements<Tag, false> {};


//
//
// CONTAINER-SPECIFIC REQUIREMENTS (23.3[sequences], 23.4[associative], 23.5[unord]
//
// Whereas the tests above all verify container requirements specified in the Container Requirements
// section of the specification (clause 23.2), the tests below verify container requirements
// specified in the individual container specifications (in clauses 23.3, 23.4, and 23.5).  Tests
// are only provided for operations for which the Requires clause is different or for operations
// that are specific to the individual container (and are thus not tested in the above tests).
//
//

DEFINE_TEST(test_specific_sequence_allocator_constructor, {
    // explicit X(const Allocator& = Allocator());
    // [No Requires clause]
    //
    // Applies to:  deque, forward_list, list, vector
    c_of_erasable a((construct_applying_allocator<erasable>()));
});


DEFINE_TEST(test_specific_sequence_count_constructor_default, {
    // explicit X(size_type n);
    // Requires:  T shall be DefaultInsertable into X
    //
    // Applies to:  deque, forward_list, list, vector
    c_of_default_constructible(1);
    c_of_default_constructible a(1);
});


DEFINE_TEST(test_specific_sequence_count_constructor_copy, {
    // X(size_type n, const T& value, const Allocator& = Allocator())
    // Requires:  T shall be CopyInsertable into X
    //
    // Applies to:  deque, forward_list, list, vector
    c_of_copy_insertable a(1, copy_insertable(key()), construct_applying_allocator<copy_insertable>());
});


DEFINE_TEST(test_specific_sequence_range_constructor, {
    // X(InputIterator first, InputIterator last, const Allocator& = Allocator());
    // [No Requires clause]
    //
    // Applies to:  deque, forward_list, list, vector
    //
    // SPEC:  This has no Requires clause in any of the four Sequence container specifications.  It
    // should have the same requirements as the Sequence Container range constructor that does not
    // accept an allocator (the constructor of the form 'X(i, j)').
    //
    // That Requires clause states:  "T shall be EmplaceConstructible into X from *i.  For vector,
    // if the iterator does not meet the forward iterator requirements, T shall also be
    // MoveInsertable into X."
    std::move_iterator<emplace_argument*> const i;
    c_of_emplace_constructible a(i, i, construct_applying_allocator<emplace_constructible>());
});

DEFINE_TEST_SPECIALIZATION(test_specific_sequence_range_constructor, tag_vector,
{
    // X(InputIterator first, InputIterator last, const Allocator& = Allocator());
    // [See the primary test for comments.]
    //
    // Requires:  T shall be EmplaceConstructible into X from *i.  For vector, if the iterator does
    // not meet the forward iterator requirements, T shall also be MoveInsertable into X.
    {
         std::move_iterator<emplace_argument*> i;
         c_of_emplace_constructible(i, i, construct_applying_allocator<emplace_constructible>());
         c_of_emplace_constructible a(i, i, construct_applying_allocator<emplace_constructible>());
}
{
    std::move_iterator<emplace_argument*> i;
    c_of_ec_mi(i, i, construct_applying_allocator<ec_mi>());
    c_of_ec_mi a(i, i, construct_applying_allocator<ec_mi>());
}
});


DEFINE_TEST(test_specific_common_list_operations,
{
    // void remove(const T& value);
    // void remove_if(Predicate pred);
    // [No Requires clause]
    {
        c_of_equality_comparable a;
        c_of_erasable b;

        a.remove(equality_comparable(key()));
        b.remove_if(faux_predicate<erasable>());
}

// void unique();
// void unique(BinaryPredicate pred);
// [No Requires clause]
//
{
    c_of_equality_comparable a;
    c_of_erasable b;

    a.unique();
    b.unique(faux_compare<erasable>());
}

// void merge(forward_list& x);
// void merge(forward_list&& x);
// void merge(forward_list& x, Compare comp);
// void merge(forward_list&& x, Compare comp);
// [No relevant Requires clause]
{
    c_of_less_comparable a;
    c_of_erasable b;

    a.merge(a);
    a.merge(c_of_less_comparable());

    b.merge(b, faux_compare<erasable>());
    b.merge(c_of_erasable(), faux_compare<erasable>());
}

// void sort();
// void sort(Compare comp);
// [No relevant Requires clause]
{
    c_of_less_comparable a;
    c_of_erasable b;
    a.sort();
    b.sort(faux_compare<erasable>());
}

// void reverse();
// [No Requires clause]
{
    c_of_erasable a;
    a.reverse();
}
});


DEFINE_TEST(test_specific_ordered_associative_constructors,
{
    // X(c, allocator)
    // [No Requires clause]
    //
    // SPEC:  The Requires clause should be the same as the Requires clause for the equivalent
    // constructor in the Associative requirements (sans-allocator).  That states:  "Requires:
    // key_compare is CopyConstructible."
    {
        typedef typename Traits::template bind_container<
            erasable,
            construct_applying_allocator<erasable>,
            copy_constructible_compare<erasable>
        >::type container_type;

        copy_constructible_compare<erasable> const c((key()));
        container_type a(c, construct_applying_allocator<typename Traits::template bind_value<erasable>::type>());
}

// X(i, j, c) and X a(i, j, c);
// [No Requires clause]
//
// SPEC:  The Requires clause should be the same as the Requires clause for the equivalent
// constructor in the Associative requirements (sans-allocator).  That states:  "Requires:
// key_compare is CopyConstructible.  value_type is EmplaceConstructible into X from *i."

{
    typedef typename Traits::template bind_container<emplace_constructible,
        construct_applying_allocator<emplace_constructible>, copy_constructible_compare<emplace_constructible>>::type
        container_type;

    copy_constructible_compare<emplace_constructible> const c((key()));
    std::move_iterator<typename Traits::emplace_argument_type*> i;
    (container_type(i, i, c));
    container_type a(i, i, c);
}
});


DEFINE_TEST(test_specific_unordered_associative_constructors,
{
    // X(n, hf, eq, allocator)
    // [No Requires clause]
    //
    // SPEC:  The Requires clause should be the same as the Requires clause for the equivalent
    // constructor in the Unordered Associative requirements (sans-allocator).  That states:
    // "Requires:  hasher and key_equal are CopyConstructible."
    {
        typedef typename Traits::template bind_container<
            erasable,
            construct_applying_allocator<erasable>,
            copy_constructible_hash<erasable>,
            copy_constructible_compare<erasable>
        >::type container_type;

        copy_constructible_hash<erasable> const hf((key()));
        copy_constructible_compare<erasable> const eq((key()));
        using T = typename Traits::template bind_value<erasable>::type;
        (container_type(0, hf, eq, construct_applying_allocator<T>()));
        container_type a(0, hf, eq, construct_applying_allocator<T>());
}

// X(i, j, n, hf, eq, allocator)
//
// SPEC:  The Requires clause should be the same as the Requires clause for the equivalent
// constructor in the Unordered Associative requirements (sans-allocator).  That states:
// "Requires:  hasher and key_equal are CopyConstructible.  value_type is EmplaceConstructible
// into X from *i."
{
    typedef typename Traits::template bind_container<emplace_constructible,
        construct_applying_allocator<emplace_constructible>, copy_constructible_hash<emplace_constructible>,
        copy_constructible_compare<emplace_constructible>>::type container_type;
    std::move_iterator<typename Traits::emplace_argument_type*> i;

    copy_constructible_hash<emplace_constructible> const hf((key()));
    copy_constructible_compare<emplace_constructible> const eq((key()));
    (container_type(i, i, 0, hf, eq,
        construct_applying_allocator<typename Traits::template bind_value<emplace_constructible>::type>()));
    container_type a(i, i, 0, hf, eq,
        construct_applying_allocator<typename Traits::template bind_value<emplace_constructible>::type>());
}
});


DEFINE_TEST(check_all_specific_requirements, {});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_deque, {
    test_specific_sequence_count_constructor_default<Tag>();
    test_specific_sequence_count_constructor_copy<Tag>();
    test_specific_sequence_range_constructor<Tag>();
    test_specific_sequence_range_constructor<Tag>();

    // void resize(size_type sz);
    // Requires:  T shall be MoveInsertable and DefaultInsertable into *this
    {
        c_of_dc_mi a;
        a.resize(1);
    }

    // void resize(size_type sz, const T& c);
    // Requires:  T shall be MoveInsertable and CopyInsertable into *this
    {
        c_of_copy_insertable a;
        a.resize(1, copy_insertable(key()));
    }

    // void shrink_to_fit();
    // Requires:  T shall be MoveInsertable into *this
    {
        c_of_move_insertable a;
        a.shrink_to_fit();
    }
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_forward_list, {
    test_specific_sequence_count_constructor_default<Tag>();
    test_specific_sequence_count_constructor_copy<Tag>();
    test_specific_sequence_range_constructor<Tag>();
    test_specific_sequence_range_constructor<Tag>();

    // iterator insert_after(const_iterator position, const T& x);
    // iterator insert_after(const_iterator position, T&& x);
    // [No relevant Requires clause]
    //
    // SPEC: Should require T:CopyInsertable for lvalue, T:MoveInsertable for rvalue
    {
        c_of_copy_insertable a;
        a.insert_after(a.cbefore_begin(), copy_insertable(key()));
    }

    // iterator insert_after(const_iterator position, size_type n, const T& x);
    // [No relevant Requires clause]
    //
    // SPEC:  Should require T:CopyInsertable
    {
        c_of_copy_insertable a;
        a.insert_after(a.cbefore_begin(), 1, copy_insertable(key()));
    }

    // iterator insert_after(const_iterator position, InputIterator first, InputIterator last);
    // [No relevant Requires clause]
    //
    // SPEC:  Should require T:EmplaceConstructible into *this from *first.
    {
        std::move_iterator<emplace_argument*> i;
        c_of_emplace_constructible a;
        a.insert_after(a.cbefore_begin(), i, i);
    }

    // iterator emplace_after(const_iterator position, Args&&... args);
    // [No relevant Requires clause]
    //
    // SPEC:  Should require T:EmplaceConstructible from args
    {
        c_of_emplace_constructible a;
        a.emplace_after(a.cbefore_begin(), emplace_argument());
        a.emplace_after(a.cbefore_begin(), emplace_argument(), emplace_argument());
        a.emplace_after(a.cbefore_begin(), emplace_argument(), emplace_argument(), emplace_argument());
    }

    // void resize(size_type sz);
    // Requires:  T shall be DefaultInsertable into *this
    {
        c_of_default_constructible a;
        a.resize(1);
    }

    // void resize(size_type sz, const value_type& c);
    // Requires:  T shall be CopyInsertable into *this
    {
        c_of_copy_insertable a;
        a.resize(1, copy_insertable(key()));
    }

    test_specific_common_list_operations<Tag>();
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_list, {
    test_specific_sequence_count_constructor_default<Tag>();
    test_specific_sequence_count_constructor_copy<Tag>();
    test_specific_sequence_range_constructor<Tag>();
    test_specific_sequence_range_constructor<Tag>();

    // void resize(size_type sz);
    // Requires:  T shall be DefaultInsertable into *this

    // void resize(size_type sz, const T& c);
    // Requires:  T shall be CopyInsertable into *this

    test_specific_common_list_operations<Tag>();
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_vector, {
    test_specific_sequence_count_constructor_default<Tag>();
    test_specific_sequence_count_constructor_copy<Tag>();
    test_specific_sequence_range_constructor<Tag>();
    test_specific_sequence_range_constructor<Tag>();

    // void reserve(size_type n); and void shrink_to_fit();
    // Requires:  T shall be MoveInsertable into *this
    {
        c_of_move_insertable a;
        a.reserve(1);
        a.shrink_to_fit();
    }

    // void resize(size_type sz);
    // Requires:  T shall be MoveInsertable and DefaultInsertable into *this
    {
        c_of_dc_mi a;
        a.resize(1);
    }

    // void resize(size_type sz, const T& c);
    // Requires:  T shall be MoveInsertable into *this and CopyInsertable into *this
    {
        c_of_copy_insertable a;
        a.resize(1, copy_insertable(key()));
    }
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_map, {
    test_specific_ordered_associative_constructors<Tag>();

    // T& operator[](const key_type& x);
    // Requires:  key_type shall be CopyInsertable and mapped_type shall be DefaultInsertable into
    // *this
    {
        std::map<copy_insertable, default_constructible, faux_compare<copy_insertable>,
            construct_applying_allocator<std::pair<const copy_insertable, default_constructible>>>
            a;

        copy_insertable k((key()));
        a[k];
    }

    // T& operator[](key_type& x);
    // Requires:  mapped_type shall be DefaultInsertable into *this
    //
    // SPEC:  Presumably this should also say "key_type shall be MoveInsertable into *this," given
    // the Effects clause.
    {
        std::map<move_insertable, default_constructible, faux_compare<move_insertable>,
            construct_applying_allocator<std::pair<const move_insertable, default_constructible>>>
            a;

        a[move_insertable(key())];
    }

    // T& at(const key_type& x);
    // const T& at(const key_type& x) const;
    // [No Requires clause]
    {
        c_of_erasable a;
        c_of_erasable const b;

        (void) a.at(erasable(key()));
        (void) b.at(erasable(key()));
    }
});

DEFINE_TEST_SPECIALIZATION(
    check_all_specific_requirements, tag_multimap, { test_specific_ordered_associative_constructors<Tag>(); });

DEFINE_TEST_SPECIALIZATION(
    check_all_specific_requirements, tag_multiset, { test_specific_ordered_associative_constructors<Tag>(); });

DEFINE_TEST_SPECIALIZATION(
    check_all_specific_requirements, tag_set, { test_specific_ordered_associative_constructors<Tag>(); });

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_unordered_map, {
    test_specific_unordered_associative_constructors<Tag>();

    // mapped_type& operator[](const key_type& k);
    // mapped_type& operator[](key_type&& k);
    // Requires:  mapped_type shall be DefaultInsertable into *this.  For the first operator,
    // key_type shall be CopyInsertable into *this.  For the second operator, key_type shall be
    // MoveConstructible.
    {
        std::unordered_map<copy_insertable, default_constructible, faux_hash<copy_insertable>,
            faux_compare<copy_insertable>,
            construct_applying_allocator<std::pair<const copy_insertable, default_constructible>>>
            a;

        copy_insertable k((key()));
        a[k];

        std::unordered_map<move_insertable, default_constructible, faux_hash<move_insertable>,
            faux_compare<move_insertable>,
            construct_applying_allocator<std::pair<const move_insertable, default_constructible>>>
            b;

        b[move_insertable(key())];
    }

    // mapped_type& at(const key_type& k);
    // const mapped_type& at(const key_type& k) const;
    // [No Requires clause]
    {
        c_of_erasable a;
        c_of_erasable const b;

        (void) a.at(erasable(key()));
        (void) b.at(erasable(key()));
    }
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_unordered_multimap,
    { test_specific_unordered_associative_constructors<Tag>(); });

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_unordered_multiset,
    { test_specific_unordered_associative_constructors<Tag>(); });

DEFINE_TEST_SPECIALIZATION(
    check_all_specific_requirements, tag_unordered_set, { test_specific_unordered_associative_constructors<Tag>(); });


//
//
// TEST DRIVER
//
//

template <container_tag Tag>
void assert_container() {
    check_all_container_requirements<Tag>();
    check_all_allocator_aware_requirements<Tag>();

    check_all_sequence_requirements<Tag>();
    check_all_optional_sequence_requirements<Tag>();

    check_all_ordered_associative_requirements<Tag>();

    check_all_unordered_associative_requirements<Tag>();

    check_all_specific_requirements<Tag>();
}

void assert_all() {
    assert_container<tag_deque>();
    assert_container<tag_forward_list>();
    assert_container<tag_list>();
    assert_container<tag_vector>();

    assert_container<tag_map>();
    assert_container<tag_multimap>();
    assert_container<tag_multiset>();
    assert_container<tag_set>();

    assert_container<tag_unordered_map>();
    assert_container<tag_unordered_multimap>();
    assert_container<tag_unordered_multiset>();
    assert_container<tag_unordered_set>();
}
#endif // _MSC_EXTENSIONS
