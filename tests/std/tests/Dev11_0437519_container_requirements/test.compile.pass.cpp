// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// This test program verifies all of the container requirements for the Standard Library containers,
// including the four non-array sequence containers (deque, forward_list, list, vector), the four
// associative containers (map, multimap, multiset, and set), and the four unordered associative
// containers (unordered_map, unordered_multimap, unordered_multiset, and unordered_set).
//
// n3485 is the reference document for all language standard citations (unless otherwise noted).
//
// A brief overview of the contents of this file:
//
// The first 650 lines or so of this file (everything up to the first use of DEFINE_TEST) are test
// utilities:  stub allocators, comparers, and other kinds of types with which we can instantiate
// containers; test types that define a restricted set of operations; and a set of container traits
// classes that unify the interfaces for the containers so we can use a common set of tests for
// them (these traits also define a few useful properties for each container so we know what to
// test for each of them).
//
//
// For each operation (or collection of related operations) specified in the Container Requirements,
// we define a test using the DEFINE_TEST macro.  These tests can be instantiated for any container.
// The DEFINE_TEST generates a set of typedefs (named c_{value_type}, where {value_type} is the type
// of object stored in that container) that can be used within the test, for brevity.  In the test,
// we simply attempt to perform the operation being tested using a container instantiated with a
// value_type that provides only the capabilities required by the specification.
//
// If the requirements are different for some individual containers (e.g. vector::push_back requires
// more than the push_back member of other containers), a specialized test is written for those
// containers using the DEFINE_TEST_SPECIALIZATION macro.
//
// If a particular operation is not supported for a container (e.g., forward_list has no push_back),
// that test is disabled for that container using the NOT_SUPPORTED_SPECIALIZATION macro.
//
// All test names start with "test_"
//
// For each group of related tests (generally divided by category as they are in the specification,
// e.g., General Container Requirements, Allocator-Aware Container Requirements, Sequence Container
// Requirements, etc.), we define a class template that instantiates all of the tests in that group.
// The names of these classes start with "check_".  These classes will only instantiate tests for
// supported classes of containers.  For example, check_all_sequence_requirements only instantiates
// the sequence container requirements tests for sequence containers.
//
// At the very bottom of the file there is a function template named assert_container that
// instantiates all of the check_ class templates for an individual container.  This function
// template is instantiated once for each of the twelve containers being tested, by the assert_all
// function.

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

struct key {};

template <typename T>
struct faux_predicate {
    bool operator()(T const&) const noexcept {
        return false;
    }
};

template <typename T>
struct faux_compare {
    bool operator()(T const&, T const&) const noexcept {
        return false;
    }
};

template <typename T>
struct default_constructible_compare {
    default_constructible_compare()                                                = default;
    default_constructible_compare(default_constructible_compare const&)            = default;
    default_constructible_compare& operator=(default_constructible_compare const&) = delete;

    bool operator()(T const&, T const&) const noexcept {
        return false;
    }
};

template <typename T>
struct copy_constructible_compare {
    copy_constructible_compare() = delete;
    copy_constructible_compare(key) {}
    copy_constructible_compare(copy_constructible_compare const&)            = default;
    copy_constructible_compare& operator=(copy_constructible_compare const&) = delete;

    bool operator()(T const&, T const&) const noexcept {
        return false;
    }
};

template <typename T>
struct faux_hash {
    std::size_t operator()(T const&) const noexcept {
        return 0;
    }
};

template <typename T>
struct default_constructible_hash {
    default_constructible_hash()                                             = default;
    default_constructible_hash(default_constructible_hash const&)            = default;
    default_constructible_hash& operator=(default_constructible_hash const&) = delete;

    bool operator()(T const&, T const&) const noexcept {
        return false;
    }
};

template <typename T>
struct copy_constructible_hash {
    copy_constructible_hash() = delete;
    copy_constructible_hash(key) {}
    copy_constructible_hash(copy_constructible_hash const&)            = default;
    copy_constructible_hash& operator=(copy_constructible_hash const&) = delete;

    std::size_t operator()(T const&) const noexcept {
        return 0;
    }
};


//
//
// ITERATORS FOR TESTING RANGE MEMBERS
//
//

// Several functions have different requirements depending on the category of the iterator with
// which the function is called.

template <typename I, typename T>
struct input_iterator_base {
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef std::input_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;

    input_iterator_base()                                      = default;
    input_iterator_base(input_iterator_base const&)            = default;
    input_iterator_base& operator=(input_iterator_base const&) = default;

    I& operator++() {
        return static_cast<I&>(*this);
    }
    void operator++(int) = delete; // avoid postincrement

    friend bool operator==(I const&, I const&) {
        return true;
    }
    friend bool operator!=(I const&, I const&) {
        return false;
    }

    T& operator*() const {
        throw 0;
    }
    T* operator->() const {
        throw 0;
    }
};

template <typename I, typename T>
struct forward_iterator_base : input_iterator_base<I, T> {
    typedef std::forward_iterator_tag iterator_category;
};

template <typename I, typename T>
struct bidirectional_iterator_base : forward_iterator_base<I, T> {
    typedef std::bidirectional_iterator_tag iterator_category;

    I& operator--() {
        return *this;
    }
    void operator--(int) = delete; // avoid postdecrement
};

template <typename I, typename T>
struct random_access_iterator_base : bidirectional_iterator_base<I, T> {
    typedef std::random_access_iterator_tag iterator_category;

    friend I operator+(I const&, std::ptrdiff_t) {
        return I();
    }
    friend I operator+(std::ptrdiff_t, I const&) {
        return I();
    }
    friend I operator-(I const&, std::ptrdiff_t) {
        return I();
    }
    friend std::ptrdiff_t operator-(I const&, I const&) {
        return 0;
    }

    friend I& operator+=(I& x, std::ptrdiff_t) {
        return x;
    }
    friend I& operator-=(I& x, std::ptrdiff_t) {
        return x;
    }

    T& operator[](std::ptrdiff_t) const = delete; // avoid subscript, N4849 [algorithms.requirements]/8

    friend bool operator<(I const&, I const&) {
        return false;
    }
    friend bool operator>(I const&, I const&) {
        return false;
    }
    friend bool operator<=(I const&, I const&) {
        return true;
    }
    friend bool operator>=(I const&, I const&) {
        return true;
    }
};

template <typename T>
struct input_iterator : input_iterator_base<input_iterator<T>, T> {};
template <typename T>
struct forward_iterator : forward_iterator_base<forward_iterator<T>, T> {};
template <typename T>
struct bidirectional_iterator : bidirectional_iterator_base<bidirectional_iterator<T>, T> {};
template <typename T>
struct random_access_iterator : random_access_iterator_base<random_access_iterator<T>, T> {};


//
//
// ALLOCATORS FOR TESTING ALLOCATOR CONCEPTS
//
//

// To verify the allocator-aware container requirements, we need two allocators--one that has
// propagate_on_container_move_assignment set to true; the other with it set to false.  For all
// other tests, we simply use std::allocator.

template <typename T>
struct pocma_allocator {
    typedef T value_type;

    pocma_allocator() = default;

    template <typename U>
    pocma_allocator(pocma_allocator<U> const&) {}

    value_type* allocate(std::size_t) {
        throw std::bad_alloc();
    }
    void deallocate(value_type*, std::size_t) {}

    typedef std::true_type propagate_on_container_move_assignment;

    friend bool operator==(pocma_allocator const&, pocma_allocator const&) {
        return true;
    }
    friend bool operator!=(pocma_allocator const&, pocma_allocator const&) {
        return false;
    }
};

template <typename T>
struct non_pocma_allocator {
    typedef T value_type;

    non_pocma_allocator() = default;

    template <typename U>
    non_pocma_allocator(non_pocma_allocator<U> const&) {}

    value_type* allocate(std::size_t) {
        throw std::bad_alloc();
    }
    void deallocate(value_type*, std::size_t) {}

    typedef std::false_type propagate_on_container_move_assignment;

    friend bool operator==(non_pocma_allocator const&, non_pocma_allocator const&) {
        return true;
    }
    friend bool operator!=(non_pocma_allocator const&, non_pocma_allocator const&) {
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

#define DEFINE_TYPE(name, dtor, def_ctor, copy_ctor, move_ctor, emp_ctor, copy_assign, move_assign, emp_assign) \
    class name {                                                                                                \
    public:              name(key) { }                                                                          \
    private: dtor        ~name() { }                                                                            \
    private: def_ctor    name() { }                                                                             \
    private: copy_ctor   name(name const&) { }                                                                  \
    private: move_ctor   name(name&&) { }                                                                       \
    private: emp_ctor    name(emplace_argument&&) { }                                                           \
    private: emp_ctor    name(emplace_argument&&, emplace_argument&&) { }                                       \
    private: emp_ctor    name(emplace_argument&&, emplace_argument&&, emplace_argument&&) { }                   \
    private: copy_assign name& operator=(name const&) { return *this; }                                         \
    private: move_assign name& operator=(name&&) noexcept { return *this; }                                     \
    private: emp_assign  name& operator=(emplace_argument&&) { return *this; }                                  \
    }

#define YES public:

//                                        default   copy   move   emplace    copy     move    emplace
//                                 dtor    ctor     ctor   ctor    ctor     assign   assign   assign
DEFINE_TYPE(erasable             , YES  ,         ,      ,      ,         ,        ,        ,         );
DEFINE_TYPE(default_constructible, YES  , YES     ,      ,      ,         ,        ,        ,         );
DEFINE_TYPE(copy_insertable      , YES  ,         , YES  , YES  ,         ,        ,        ,         );
DEFINE_TYPE(move_insertable      , YES  ,         ,      , YES  ,         ,        ,        ,         );
DEFINE_TYPE(emplace_constructible, YES  ,         ,      ,      , YES     ,        ,        ,         );
DEFINE_TYPE(copy_assignable      , YES  ,         ,      ,      ,         , YES    , YES    ,         );
DEFINE_TYPE(move_assignable      , YES  ,         ,      ,      ,         ,        , YES    ,         );
DEFINE_TYPE(equality_comparable  , YES  ,         ,      ,      ,         ,        ,        ,         );
DEFINE_TYPE(less_comparable      , YES  ,         ,      ,      ,         ,        ,        ,         );

DEFINE_TYPE(ca_ci                , YES  ,         , YES  , YES  ,         , YES    , YES    ,         );
DEFINE_TYPE(ci_ma                , YES  ,         , YES  , YES  ,         ,        , YES    ,         );
DEFINE_TYPE(dc_mi                , YES  , YES     ,      , YES  ,         ,        ,        ,         );
DEFINE_TYPE(ec_ma_mi             , YES  ,         ,      , YES  , YES     ,        , YES    ,         );
DEFINE_TYPE(ec_mi                , YES  ,         ,      , YES  , YES     ,        ,        ,         );
DEFINE_TYPE(ma_mi                , YES  ,         ,      , YES  ,         ,        , YES    ,         );
DEFINE_TYPE(ec_ea                , YES  ,         ,      ,      , YES     ,        ,        , YES     );
DEFINE_TYPE(ec_ea_mi             , YES  ,         ,      , YES  , YES     ,        ,        , YES     );

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
        return V(key());
    }
};

struct map_associative_bind_value {
    template <typename V>
    struct bind_value {
        typedef std::pair<V const, V> type;
    };

    typedef std::pair<emplace_argument, emplace_argument> emplace_argument_type;

    template <typename V>
    static std::pair<V, V> construct_value() {
        return std::pair<V, V>(key(), key());
    }
};

template <template <typename, typename> class C>
struct sequence_bind_container : identity_bind_value {
    template <typename V, typename A = std::allocator<V>>
    struct bind_container {
        typedef C<V, A> type;
    };
};

template <template <typename, typename, typename, typename> class C>
struct ordered_map_associative_bind_container : map_associative_bind_value {
    template <typename V, typename A = std::allocator<V>, typename P = faux_compare<V>>
    struct bind_container {
        typedef typename bind_value<V>::type value_type;
        typedef typename std::allocator_traits<A>::template rebind_alloc<value_type> alloc_type;

        typedef C<V, V, P, alloc_type> type;
    };
};

template <template <typename, typename, typename> class C>
struct ordered_set_associative_bind_container : identity_bind_value {
    template <typename V, typename A = std::allocator<V>, typename P = faux_compare<V>>
    struct bind_container {
        typedef C<V, P, A> type;
    };
};

template <template <typename, typename, typename, typename, typename> class C>
struct unordered_map_associative_bind_container : map_associative_bind_value {
    template <typename V, typename A = std::allocator<V>, typename H = faux_hash<V>, typename P = faux_compare<V>>
    struct bind_container {
        typedef typename bind_value<V>::type value_type;
        typedef typename std::allocator_traits<A>::template rebind_alloc<value_type> alloc_type;

        typedef C<V, V, H, P, alloc_type> type;
    };
};

template <template <typename, typename, typename, typename> class C>
struct unordered_set_associative_bind_container : identity_bind_value {
    template <typename V, typename A = std::allocator<V>, typename H = faux_hash<V>, typename P = faux_compare<V>>
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

DEFINE_TEST(test_container_typedefs,
{
    // X::value_type, X::reference, X::const_reference
    {
        STATIC_ASSERT(std::is_same_v<v_of_erasable       , typename c_of_erasable::value_type     >);
        STATIC_ASSERT(std::is_same_v<v_of_erasable&      , typename c_of_erasable::reference      >);
        STATIC_ASSERT(std::is_same_v<v_of_erasable const&, typename c_of_erasable::const_reference>);
}

// X::iterator
{
    STATIC_ASSERT(
        std::is_same_v<v_of_erasable, typename std::iterator_traits<typename c_of_erasable::iterator>::value_type>);

    STATIC_ASSERT(std::is_base_of_v<std::forward_iterator_tag,
        typename std::iterator_traits<typename c_of_erasable::iterator>::iterator_category>);

    STATIC_ASSERT(std::is_convertible_v<typename c_of_erasable::iterator, typename c_of_erasable::const_iterator>);
}

// X::const_iterator
{
    STATIC_ASSERT(
        std::is_same_v<v_of_erasable, typename std::iterator_traits<typename c_of_erasable::iterator>::value_type>);

    STATIC_ASSERT(std::is_base_of_v<std::forward_iterator_tag,
        typename std::iterator_traits<typename c_of_erasable::const_iterator>::iterator_category>);
}

// X::difference_type
{
    STATIC_ASSERT(std::is_signed_v<typename c_of_erasable::difference_type>);

    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::difference_type,
        typename std::iterator_traits<typename c_of_erasable::iterator>::difference_type>);

    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::difference_type,
        typename std::iterator_traits<typename c_of_erasable::const_iterator>::difference_type>);
}

// X::size_type
{
    STATIC_ASSERT(std::is_unsigned_v<typename c_of_erasable::size_type>);

    // Note:  This check is overly strict:
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::size_type,
        std::make_unsigned_t<typename c_of_erasable::difference_type>>);
}
});


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


DEFINE_TEST(test_container_begin_end, {
    // a.begin(), a.end(), a.cbegin(), a.cend()
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable const b;
    typename c_of_erasable::iterator it0(a.begin());
    typename c_of_erasable::iterator it1(a.end());

    typename c_of_erasable::const_iterator it2(a.cbegin());
    typename c_of_erasable::const_iterator it3(a.cend());

    typename c_of_erasable::const_iterator it4(b.begin());
    typename c_of_erasable::const_iterator it5(b.end());

    typename c_of_erasable::const_iterator it6(b.cbegin());
    typename c_of_erasable::const_iterator it7(b.cend());

    STATIC_ASSERT(std::is_same_v<decltype(a.begin()), typename c_of_erasable::iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.end()), typename c_of_erasable::iterator>);

    STATIC_ASSERT(std::is_same_v<decltype(a.cbegin()), typename c_of_erasable::const_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.cend()), typename c_of_erasable::const_iterator>);

    STATIC_ASSERT(std::is_same_v<decltype(b.begin()), typename c_of_erasable::const_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.end()), typename c_of_erasable::const_iterator>);

    (void) it0;
    (void) it1;
    (void) it2;
    (void) it3;
    (void) it4;
    (void) it5;
    (void) it6;
    (void) it7;
});


DEFINE_TEST(test_container_equality_comparison, {
    // a == b and a != b
    // Requires:  T is EqualityComparable
    c_of_equality_comparable const a;
    c_of_equality_comparable const b;
    (void) (a == b);
    (void) (a != b);

    STATIC_ASSERT(std::is_convertible_v<decltype(a == b), bool>);
    STATIC_ASSERT(std::is_convertible_v<decltype(a != b), bool>);
});


DEFINE_TEST(test_container_swap, {
    // a.swap(b) and swap(a, b)
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable b;
    a.swap(b);
    STATIC_ASSERT(noexcept(a.swap(b))); // strengthened
    swap(a, b);
    STATIC_ASSERT(noexcept(swap(a, b))); // strengthened

    STATIC_ASSERT(std::is_same_v<decltype(a.swap(b)), void>);
    STATIC_ASSERT(std::is_same_v<decltype(swap(a, b)), void>);
});


DEFINE_TEST(test_container_size, {
    // a.size(), a.max_size(), and a.empty()
    c_of_erasable const a;
    (void) a.size();
    STATIC_ASSERT(noexcept(a.size())); // strengthened
    (void) a.max_size();
    (void) a.empty();
    STATIC_ASSERT(noexcept(a.empty())); // strengthened

    STATIC_ASSERT(std::is_same_v<decltype(a.size()), typename c_of_erasable::size_type>);
    STATIC_ASSERT(std::is_same_v<decltype(a.max_size()), typename c_of_erasable::size_type>);
    STATIC_ASSERT(std::is_convertible_v<decltype(a.empty()), bool>);
});

DEFINE_TEST_SPECIALIZATION(test_container_size, tag_forward_list, {
    // a.max_size(), and a.empty()  [forward_list has no size() member]
    c_of_erasable const a;
    (void) a.max_size();
    (void) a.empty();
    STATIC_ASSERT(noexcept(a.empty())); // strengthened

    STATIC_ASSERT(std::is_same_v<decltype(a.max_size()), typename c_of_erasable::size_type>);
    STATIC_ASSERT(std::is_convertible_v<decltype(a.empty()), bool>);
});


template <container_tag Tag>
void check_all_container_requirements() {
    test_container_typedefs<Tag>();
    test_container_default_constructor<Tag>();
    test_container_copy_constructor<Tag>();
    test_container_move_constructor<Tag>();
    // Move Assignment is verified in the Allocator-Aware Container Requirements
    test_container_begin_end<Tag>();
    test_container_equality_comparison<Tag>();
    test_container_swap<Tag>();
    // Copy Assignment is verified in the Allocator-Aware Container Requirements
    test_container_size<Tag>();
};


//
//
// REVERSIBLE CONTAINER REQUIREMENTS (23.2.1[container.requirements.general]/Table 97)
//
//

DEFINE_TEST(test_reversible_typedefs,
{
    // X::reverse_iterator
    {
        STATIC_ASSERT(std::is_same_v<
            typename std::iterator_traits<typename c_of_erasable::reverse_iterator>::value_type,
            typename c_of_erasable::value_type
        >);

        STATIC_ASSERT(std::is_same_v<
            typename c_of_erasable::reverse_iterator,
            std::reverse_iterator<typename c_of_erasable::iterator>
        >);
}

// X::const_reverse_iterator
{
    STATIC_ASSERT(
        std::is_same_v<typename std::iterator_traits<typename c_of_erasable::const_reverse_iterator>::value_type,
            typename c_of_erasable::value_type>);

    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::const_reverse_iterator,
        std::reverse_iterator<typename c_of_erasable::const_iterator>>);
}
});


DEFINE_TEST(test_reversible_rbegin_rend, {
    // a.rbegin(), a.rend(), a.crbegin(), a.crend()
    c_of_erasable a;
    c_of_erasable const b;
    typename c_of_erasable::reverse_iterator it0(a.rbegin());
    typename c_of_erasable::reverse_iterator it1(a.rend());

    typename c_of_erasable::const_reverse_iterator it2(a.crbegin());
    typename c_of_erasable::const_reverse_iterator it3(a.crend());

    typename c_of_erasable::const_reverse_iterator it4(b.rbegin());
    typename c_of_erasable::const_reverse_iterator it5(b.rend());

    typename c_of_erasable::const_reverse_iterator it6(b.crbegin());
    typename c_of_erasable::const_reverse_iterator it7(b.crend());

    STATIC_ASSERT(std::is_same_v<decltype(a.rbegin()), typename c_of_erasable::reverse_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.rend()), typename c_of_erasable::reverse_iterator>);

    STATIC_ASSERT(std::is_same_v<decltype(a.crbegin()), typename c_of_erasable::const_reverse_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.crend()), typename c_of_erasable::const_reverse_iterator>);

    STATIC_ASSERT(std::is_same_v<decltype(b.rbegin()), typename c_of_erasable::const_reverse_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.rend()), typename c_of_erasable::const_reverse_iterator>);

    (void) it0;
    (void) it1;
    (void) it2;
    (void) it3;
    (void) it4;
    (void) it5;
    (void) it6;
    (void) it7;
});


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_reversible) != 0>
struct check_all_reversible_requirements {
    check_all_reversible_requirements() {
        test_reversible_typedefs<Tag>();
        test_reversible_rbegin_rend<Tag>();
    }
};

template <container_tag Tag>
struct check_all_reversible_requirements<Tag, false> {};


//
//
// OPTIONAL CONTAINER OPERATIONS (23.2.1[container.requirements.general]/Table 98)
//
//

DEFINE_TEST(test_optional_relational_comparison, {
    // a < b; a > b; a <= b; a >= b
    // Requires:  < is defined for values of T. < is a total ordering relationship.
    //
    // SPEC:  This is listed as a precondition; it should probably be a Requires.
    c_of_less_comparable const a;
    c_of_less_comparable const b;

    (void) (a < b);
    (void) (a > b);
    (void) (a <= b);
    (void) (a >= b);

    STATIC_ASSERT(std::is_convertible_v<decltype(a < b), bool>);
    STATIC_ASSERT(std::is_convertible_v<decltype(a > b), bool>);
    STATIC_ASSERT(std::is_convertible_v<decltype(a <= b), bool>);
    STATIC_ASSERT(std::is_convertible_v<decltype(a >= b), bool>);
});


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_optional) != 0>
struct check_all_optional_container_requirements {
    check_all_optional_container_requirements() {
        test_optional_relational_comparison<Tag>();
    }
};

template <container_tag Tag>
struct check_all_optional_container_requirements<Tag, false> {};


//
//
// ALLOCATOR-AWARE CONTAINER REQUIREMENTS (23.2.1[container.requirements.general]/Table 99)
//
//

DEFINE_TEST(test_allocator_aware_typedefs, {
    // allocator_type
    // Requires:  allocator_type::value_type is the same as X::value_type
    STATIC_ASSERT(
        std::is_same_v<typename c_of_erasable::allocator_type::value_type, typename c_of_erasable::value_type>);
});


DEFINE_TEST(test_allocator_aware_get_allocator, {
    // a.get_allocator()
    // [No Requires clause]
    //
    // SPEC:  'Expression' is missing the 'a.' (it just says 'get_allocator()')
    c_of_erasable const a;
    STATIC_ASSERT(noexcept(a.get_allocator()));
    (void) a.get_allocator();

    STATIC_ASSERT(std::is_same_v<decltype(a.get_allocator()), typename c_of_erasable::allocator_type>);
});


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
    typedef typename Traits::template bind_container<ma_mi, non_pocma_allocator<ma_mi>>::type container_type;

    // a = rv
    // Requires:  If allocator_traits<allocator_type>::propagate_on_container_move_assignment::value
    // is false, T is MoveInsertable into X and MoveAssignable.
    {
        container_type a;
        a = container_type();

        STATIC_ASSERT(std::is_same_v<decltype(a = container_type()), container_type&>);
    }
});


DEFINE_TEST(test_allocator_aware_assignment_from_rvalue_pocma, {
    typedef typename Traits::template bind_container<erasable, pocma_allocator<erasable>>::type container_type;

    // a = rv
    // [No Requires clause]
    //
    // (If the allocator _does_ propagate on move assignment, there are no additional requirements,
    // so T needs only be Erasable.)
    {
        container_type a;
        a = container_type();

        STATIC_ASSERT(std::is_same_v<decltype(a = container_type()), container_type&>);
    }
});


DEFINE_TEST(test_allocator_aware_assignment_from_lvalue, {
    // a = t
    // Requires:  T is CopyInsertable into X and CopyAssignable
    c_of_ca_ci a;
    c_of_ca_ci const b;
    a = b;

    STATIC_ASSERT(std::is_same_v<decltype(a = a), c_of_ca_ci&>);
});


template <container_tag Tag>
void check_all_allocator_aware_requirements() {
    test_allocator_aware_typedefs<Tag>();
    test_allocator_aware_get_allocator<Tag>();
    // X() and X u; are verified in the General Container Requirements
    test_allocator_aware_allocator_constructor_default<Tag>();
    test_allocator_aware_allocator_constructor_copy<Tag>();
    // X(rv) and X u(rv) are verified in the General Container Requirements
    test_allocator_aware_allocator_constructor_move<Tag>();
    test_allocator_aware_assignment_from_lvalue<Tag>();
    test_allocator_aware_assignment_from_rvalue_non_pocma<Tag>();
    test_allocator_aware_assignment_from_rvalue_pocma<Tag>();
    // a.swap(b) is verified in the General Container Requirements
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
    std::move_iterator<input_iterator<emplace_argument>> const i;
    c_of_emplace_constructible(i, i);
    c_of_emplace_constructible a(i, i);
});

DEFINE_TEST_SPECIALIZATION(test_sequence_range_constructor, tag_vector,
{
    // X(i, j) and X a(i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i.  For vector, if the iterator does
    // not meet the forward iterator requirements, T shall also be MoveInsertable into X.
    {
         std::move_iterator<forward_iterator<emplace_argument>> i;
         c_of_emplace_constructible(i, i);
         c_of_emplace_constructible a(i, i);
}
{
    std::move_iterator<input_iterator<emplace_argument>> i;
    c_of_ec_mi(i, i);
    c_of_ec_mi a(i, i);
}
});


DEFINE_TEST(test_sequence_initializer_list_constructor, {
    // X(il);
    // Equivalent to X(il.begin(), il.end())
    copy_insertable t = key();
    c_of_copy_insertable({t, t, t});
    c_of_copy_insertable a({t, t, t});
});


DEFINE_TEST(test_sequence_initializer_list_assignment, {
    // a = il;
    // X&
    // Requires: T is CopyInsertable into X and CopyAssignable.
    // Assigns the range [il.begin(), il.end()) into a.
    // All existing elements of a are either assigned to or destroyed.
    // Returns: *this.
    ca_ci t = key();
    c_of_ca_ci a;
    a = {t, t, t};

    STATIC_ASSERT(std::is_same_v<decltype(a = {t, t, t}), c_of_ca_ci&>);
});


DEFINE_TEST(test_sequence_emplace, {
    // a.emplace(p, args)
    // Requires: T is EmplaceConstructible into X from args.
    c_of_emplace_constructible a;
    a.emplace(a.begin(), emplace_argument());
    a.emplace(a.begin(), emplace_argument(), emplace_argument());
    a.emplace(a.begin(), emplace_argument(), emplace_argument(), emplace_argument());

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.emplace(a.begin(), emplace_argument())),
        typename c_of_emplace_constructible::iterator>);
});

DEFINE_TEST(test_sequence_emplace_deque_vector, {
    // a.emplace(p, args)
    // Requires: T is EmplaceConstructible into X from args. For vector and deque, T is also
    // MoveInsertable into X and MoveAssignable.
    c_of_ec_ma_mi a;
    a.emplace(a.begin(), emplace_argument());
    a.emplace(a.begin(), emplace_argument(), emplace_argument());
    a.emplace(a.begin(), emplace_argument(), emplace_argument(), emplace_argument());

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.emplace(a.begin(), emplace_argument())), typename c_of_ec_ma_mi::iterator>);
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

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.insert(a.begin(), t)), typename c_of_copy_insertable::iterator>);
});

DEFINE_TEST(test_sequence_copy_insert_one_deque_vector, {
    // a.insert(p, t)
    // Requires:  T shall be CopyInsertable into X.  For vector and deque, T shall also be
    // MoveAssignable.
    c_of_ca_ci a;
    ca_ci const t((key()));
    a.insert(a.begin(), t);

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.insert(a.begin(), t)), typename c_of_ca_ci::iterator>);
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
    STATIC_ASSERT(
        std::is_same_v<decltype(a.insert(a.begin(), move_insertable(key()))), typename c_of_move_insertable::iterator>);
});

DEFINE_TEST(test_sequence_move_insert_one_deque_vector, {
    // a.insert(p, rv)
    // Requires:  T shall be MoveInsertable into X.  For vector and deque, T shall also be
    // MoveAssignable.
    c_of_ma_mi a;
    a.insert(a.begin(), ma_mi(key()));

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.insert(a.begin(), ma_mi(key()))), typename c_of_ma_mi::iterator>);
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

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.insert(a.begin(), 1, t)), typename c_of_ca_ci::iterator>);
});

NOT_SUPPORTED_SPECIALIZATION(test_sequence_copy_insert_n, tag_forward_list);


DEFINE_TEST(test_sequence_range_insert, {
    // a.insert(p, i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i.
    c_of_emplace_constructible a;
    std::move_iterator<input_iterator<emplace_argument>> const i;
    a.insert(a.begin(), i, i);

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.insert(a.begin(), i, i)), typename c_of_emplace_constructible::iterator>);
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
        std::move_iterator<input_iterator<emplace_argument>> const i;
        a.insert(a.begin(), i, i);

        // Return type:  iterator
        STATIC_ASSERT(std::is_same_v<
            decltype(a.insert(a.begin(), i, i)),
            typename c_of_ec_ma_mi::iterator>);
}
});

DEFINE_TEST_SPECIALIZATION(test_sequence_range_insert, tag_deque, { test_sequence_range_insert_deque_vector<Tag>(); });
DEFINE_TEST_SPECIALIZATION(test_sequence_range_insert, tag_vector, { test_sequence_range_insert_deque_vector<Tag>(); });
NOT_SUPPORTED_SPECIALIZATION(test_sequence_range_insert, tag_forward_list);


DEFINE_TEST(test_sequence_initializer_list_insert, {
    // a.insert(p, il);
    // iterator
    // a.insert(p, il.begin(), il.end()).
    copy_insertable t = key();
    c_of_copy_insertable a;
    a.insert(a.begin(), {t, t, t});

    STATIC_ASSERT(std::is_same_v<decltype(a.insert(a.begin(), {t, t, t})), typename c_of_copy_insertable::iterator>);
});

DEFINE_TEST(test_sequence_initializer_list_insert_deque_vector, {
    // a.insert(p, il);
    // iterator
    // a.insert(p, il.begin(), il.end()).
    ci_ma t = key();
    c_of_ci_ma a;
    a.insert(a.begin(), {t, t, t});

    STATIC_ASSERT(std::is_same_v<decltype(a.insert(a.begin(), {t, t, t})), typename c_of_ci_ma::iterator>);
});

DEFINE_TEST_SPECIALIZATION(
    test_sequence_initializer_list_insert, tag_deque, { test_sequence_initializer_list_insert_deque_vector<Tag>(); });
DEFINE_TEST_SPECIALIZATION(
    test_sequence_initializer_list_insert, tag_vector, { test_sequence_initializer_list_insert_deque_vector<Tag>(); });
NOT_SUPPORTED_SPECIALIZATION(test_sequence_initializer_list_insert, tag_forward_list);


DEFINE_TEST(test_sequence_erase, {
    // a.erase(q), a.erase(q1, q2)
    // [No Requires clause]
    c_of_erasable a;
    a.erase(a.begin());
    STATIC_ASSERT(noexcept(a.erase(a.begin()))); // strengthened
    a.erase(a.begin(), a.end());
    STATIC_ASSERT(noexcept(a.erase(a.begin(), a.end()))); // strengthened

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.erase(a.begin())), typename c_of_erasable::iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.erase(a.begin(), a.end())), typename c_of_erasable::iterator>);
});

DEFINE_TEST(test_sequence_erase_deque_vector, {
    // a.erase(q), a.erase(q1, q2)
    // For vector and deque, T shall be MoveAssignable
    c_of_move_assignable a;
    a.erase(a.begin());
    STATIC_ASSERT(noexcept(a.erase(a.begin()))); // strengthened
    a.erase(a.begin(), a.end());
    STATIC_ASSERT(noexcept(a.erase(a.begin(), a.end()))); // strengthened

    // Return type:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(a.erase(a.begin())), typename c_of_move_assignable::iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.erase(a.begin(), a.end())), typename c_of_move_assignable::iterator>);
});

DEFINE_TEST_SPECIALIZATION(test_sequence_erase, tag_deque, { test_sequence_erase_deque_vector<Tag>(); });
DEFINE_TEST_SPECIALIZATION(test_sequence_erase, tag_vector, { test_sequence_erase_deque_vector<Tag>(); });

NOT_SUPPORTED_SPECIALIZATION(test_sequence_erase, tag_forward_list);


DEFINE_TEST(test_sequence_clear, {
    // a.clear()
    // [No Requires clause]
    c_of_erasable a;
    a.clear();
    STATIC_ASSERT(noexcept(a.clear()));

    // Return type:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.clear()), void>);
});


DEFINE_TEST(test_sequence_range_assign, {
    // [N4606]
    // a.assign(i, j)
    // Requires:  T shall be EmplaceConstructible into X from *i and assignable from *i.

    c_of_ec_ea a;
    std::move_iterator<input_iterator<emplace_argument>> const i;
    a.assign(i, i);

    // Return type:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.assign(i, i)), void>);
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
        std::move_iterator<forward_iterator<emplace_argument>> i;
        a.assign(i, i);

        // Return type:  void
        STATIC_ASSERT(std::is_same_v<decltype(a.assign(i, i)), void>);
}

{
    c_of_ec_ea_mi a;
    std::move_iterator<input_iterator<emplace_argument>> i;
    a.assign(i, i);

    // Return type:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.assign(i, i)), void>);
}
});


DEFINE_TEST(test_sequence_initializer_list_assign, {
    // a.assign(il)
    // void
    // a.assign(il.begin(), il.end()).
    ca_ci t = key();
    c_of_ca_ci a;
    a.assign({t, t, t});

    STATIC_ASSERT(std::is_same_v<decltype(a.assign({t, t, t})), void>);
});


DEFINE_TEST(test_sequence_assign_n, {
    // a.assign(n, t)
    // Requires:  T shall be CopyInsertable into X and CopyAssignable.
    c_of_ca_ci a;
    ca_ci const t((key()));
    a.assign(1, t);

    STATIC_ASSERT(std::is_same_v<decltype(a.assign(1, t)), void>);
});


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_sequence) != 0>
struct check_all_sequence_requirements {
    check_all_sequence_requirements() {
        test_sequence_count_constructor_copy<Tag>();
        test_sequence_range_constructor<Tag>();

        test_sequence_initializer_list_constructor<Tag>();
        test_sequence_initializer_list_assignment<Tag>();

        test_sequence_emplace<Tag>();
        test_sequence_copy_insert_one<Tag>();
        test_sequence_move_insert_one<Tag>();
        test_sequence_copy_insert_n<Tag>();
        test_sequence_range_insert<Tag>();
        test_sequence_initializer_list_insert<Tag>();

        test_sequence_erase<Tag>();
        test_sequence_clear<Tag>();

        test_sequence_range_assign<Tag>();
        test_sequence_initializer_list_assign<Tag>();
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

DEFINE_TEST(test_optional_sequence_front, {
    // a.front()
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable const b;

    (void) a.front();
    STATIC_ASSERT(noexcept(a.front())); // strengthened
    (void) b.front();
    STATIC_ASSERT(noexcept(b.front())); // strengthened

    // Returns:  reference; const_reference for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a.front()), typename c_of_erasable::reference>);
    STATIC_ASSERT(std::is_same_v<decltype(b.front()), typename c_of_erasable::const_reference>);
});


DEFINE_TEST(test_optional_sequence_back, {
    // a.back()
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable const b;

    (void) a.back();
    STATIC_ASSERT(noexcept(a.back())); // strengthened
    (void) b.back();
    STATIC_ASSERT(noexcept(b.back())); // strengthened

    // Returns:  reference; const_reference for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a.back()), typename c_of_erasable::reference>);
    STATIC_ASSERT(std::is_same_v<decltype(b.back()), typename c_of_erasable::const_reference>);
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_back, tag_forward_list);


#if _HAS_CXX17
#define IF17_ELSE14(ARG17, ARG14) ARG17
#else // _HAS_CXX17
#define IF17_ELSE14(ARG17, ARG14) ARG14
#endif // _HAS_CXX17


DEFINE_TEST(test_optional_sequence_emplace_front, {
    // a.emplace_front(args)
    // Requires:  T shall be EmplaceConstructible into X from args
    c_of_emplace_constructible a;
    a.emplace_front(emplace_argument());
    a.emplace_front(emplace_argument(), emplace_argument());
    a.emplace_front(emplace_argument(), emplace_argument(), emplace_argument());

    // (C++17) Returns:  reference
    // (C++14) Returns:  void
    using ret_type = IF17_ELSE14(typename c_of_emplace_constructible::reference, void);
    STATIC_ASSERT(std::is_same_v<decltype(a.emplace_front(emplace_argument())), ret_type>);
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_emplace_front, tag_vector);


DEFINE_TEST(test_optional_sequence_emplace_back, {
    // a.emplace_back(args)
    // Requires:  T shall be EmplaceConstructible into X from args
    c_of_emplace_constructible a;
    a.emplace_back(emplace_argument());
    a.emplace_back(emplace_argument(), emplace_argument());
    a.emplace_back(emplace_argument(), emplace_argument(), emplace_argument());

    // (C++17) Returns:  reference
    // (C++14) Returns:  void
    using ret_type = IF17_ELSE14(typename c_of_emplace_constructible::reference, void);
    STATIC_ASSERT(std::is_same_v<decltype(a.emplace_back(emplace_argument())), ret_type>);
});

DEFINE_TEST_SPECIALIZATION(test_optional_sequence_emplace_back, tag_vector, {
    // a.emplace_back(args)
    // Requires:  T shall be EmplaceConstructible into X from args.  For vector, T shall be
    // MoveInsertable into X.
    c_of_ec_mi a;
    a.emplace_back(emplace_argument());
    a.emplace_back(emplace_argument(), emplace_argument());
    a.emplace_back(emplace_argument(), emplace_argument(), emplace_argument());

    // (C++17) Returns:  reference
    // (C++14) Returns:  void
    using ret_type = IF17_ELSE14(typename c_of_ec_mi::reference, void);
    STATIC_ASSERT(std::is_same_v<decltype(a.emplace_back(emplace_argument())), ret_type>);
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

        // Returns:  void
        STATIC_ASSERT(std::is_same_v<decltype(a.push_front(t)), void>);
}

{
    // a.push_front(rv)
    // Requires:  T shall be MoveInsertable into X
    c_of_move_insertable a;
    a.push_front(move_insertable(key()));

    // Returns:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.push_front(move_insertable(key()))), void>);
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

        // Returns:  void
        STATIC_ASSERT(std::is_same_v<decltype(a.push_back(t)), void>);
}

{
    // a.push_back(rv)
    // Requires:  T shall be MoveInsertable into X
    c_of_move_insertable a;
    a.push_back(move_insertable(key()));

    // Returns:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.push_back(move_insertable(key()))), void>);
}
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_push_back, tag_forward_list);


DEFINE_TEST(test_optional_sequence_pop_front, {
    // a.pop_back()
    // [No Requires clause]
    c_of_erasable a;
    a.pop_front();
    STATIC_ASSERT(noexcept(a.pop_front())); // strengthened

    // Returns:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.pop_front()), void>);
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_pop_front, tag_vector);


DEFINE_TEST(test_optional_sequence_pop_back, {
    // a.pop_back()
    // [No Requires clause]
    c_of_erasable a;
    a.pop_back();
    STATIC_ASSERT(noexcept(a.pop_back())); // strengthened

    // Returns:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.pop_back()), void>);
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_pop_back, tag_forward_list);


DEFINE_TEST(test_optional_sequence_subscript, {
    // a[n] and a.at(n)
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable const b;

    (void) a[0];
    STATIC_ASSERT(noexcept(a[0])); // strengthened
    (void) b[0];
    STATIC_ASSERT(noexcept(b[0])); // strengthened

    (void) a.at(0);
    (void) b.at(0);

    // Returns:  reference; const_reference for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a[0]), typename c_of_erasable::reference>);
    STATIC_ASSERT(std::is_same_v<decltype(b[0]), typename c_of_erasable::const_reference>);

    STATIC_ASSERT(std::is_same_v<decltype(a.at(0)), typename c_of_erasable::reference>);
    STATIC_ASSERT(std::is_same_v<decltype(b.at(0)), typename c_of_erasable::const_reference>);
});

NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_subscript, tag_forward_list);
NOT_SUPPORTED_SPECIALIZATION(test_optional_sequence_subscript, tag_list);


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_sequence) != 0>
struct check_all_optional_sequence_requirements {
    check_all_optional_sequence_requirements() {
        test_optional_sequence_front<Tag>();
        test_optional_sequence_back<Tag>();
        test_optional_sequence_emplace_front<Tag>();
        test_optional_sequence_emplace_back<Tag>();
        test_optional_sequence_push_front<Tag>();
        test_optional_sequence_push_back<Tag>();
        test_optional_sequence_pop_front<Tag>();
        test_optional_sequence_pop_back<Tag>();
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

DEFINE_TEST(test_ordered_associative_typedefs, {
    // X::key_type is Key
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::key_type, erasable>);

    // X::key_compare is Compare
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::key_compare, faux_compare<erasable>>);

    // X::value_compare is "a binary predicate type;" let's just be sure it's defined:
    STATIC_ASSERT(!std::is_same_v<typename c_of_erasable::value_compare, void>);
});


DEFINE_TEST(test_ordered_associative_typedefs_for_maps, {
    // X::mapped_type is T
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::mapped_type, erasable>);

    // X::value_type is pair<const Key, T>
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::value_type, std::pair<erasable const, erasable>>);
});

NOT_SUPPORTED_SPECIALIZATION(test_ordered_associative_typedefs_for_maps, tag_multiset);
NOT_SUPPORTED_SPECIALIZATION(test_ordered_associative_typedefs_for_maps, tag_set);


DEFINE_TEST(test_ordered_associative_default_constructor_with_comparer,
{
    // X(c) and X a(c);
    // Requires:  key_compare is CopyConstructible
    {
        typedef typename Traits::template bind_container<
            erasable,
            std::allocator<erasable>,
            copy_constructible_compare<erasable>
        >::type container_type;

        copy_constructible_compare<erasable> const c((key()));
        (container_type(c));
        container_type a(c);

        // GH-1037 containers should be move constructible with a non-assignable comparator
        container_type b{std::move(a)};
        container_type d = std::move(b);
}

// X() and X a;
// Requires:  key_compare is DefaultConstructible
{
    typedef typename Traits::template bind_container<erasable, std::allocator<erasable>,
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

    std::move_iterator<input_iterator<typename Traits::emplace_argument_type>> i;
    typename c_of_emplace_constructible::key_compare c;
    c_of_emplace_constructible(i, i);
    c_of_emplace_constructible a1(i, i);
    c_of_emplace_constructible(i, i, c);
    c_of_emplace_constructible a2(i, i, c);

    typename c_of_copy_insertable::value_type t = Traits::template construct_value<copy_insertable>();
    c_of_copy_insertable({t, t, t});
    c_of_copy_insertable a3({t, t, t});
});


DEFINE_TEST(test_ordered_associative_initializer_list_assign, {
    // a = il
    // X&
    // Requires: value_type is CopyInsertable into X and CopyAssignable.
    // Effects: Assigns the range [il.begin(), il.end()) into a.
    // All existing elements of a are either assigned to or destroyed.
    typename c_of_ca_ci::value_type t = Traits::template construct_value<ca_ci>();
    c_of_ca_ci a;
    a = {t, t, t};

    STATIC_ASSERT(std::is_same_v<decltype(a = {t, t, t}), c_of_ca_ci&>);
});


DEFINE_TEST(test_ordered_associative_comparers, {
    // a.key_comp()
    // [No Requires clause]
    c_of_erasable const a;
    (void) a.key_comp();
    (void) a.value_comp();

    // Returns:  X::key_compare, X::value_compare
    STATIC_ASSERT(std::is_same_v<decltype(a.key_comp()), typename c_of_erasable::key_compare>);
    STATIC_ASSERT(std::is_same_v<decltype(a.value_comp()), typename c_of_erasable::value_compare>);
});


DEFINE_TEST(test_ordered_associative_emplace, {
    // a_uniq.emplace(args), a_eq.emplace(args)
    // Requires:  value_type shall be EmplaceConstructible into X from args
    c_of_emplace_constructible a;
    a.emplace(emplace_argument(), emplace_argument());

    // Returns:  pair<iterator, bool> for a_uniq; iterator for a_eq
    STATIC_ASSERT(std::is_same_v<decltype(a.emplace(emplace_argument(), emplace_argument())),
        std::conditional_t<(Traits::features & c_is_unique_associative) != 0,
            std::pair<typename c_of_emplace_constructible::iterator, bool>,
            typename c_of_emplace_constructible::iterator>>);

    // a.emplace_hint(p, args)
    // Requires:  equivalent to a.emplace(args)
    a.emplace_hint(a.begin(), emplace_argument(), emplace_argument());

    // Returns:  iterator
    typedef decltype(a.emplace_hint(a.begin(), emplace_argument(), emplace_argument())) result_type;

    STATIC_ASSERT(std::is_same_v<result_type, typename c_of_emplace_constructible::iterator>);
});


DEFINE_TEST(test_ordered_associative_insert, {
    // a_uniq.insert(t) and a_eq.insert(t)
    // Requires:  If t is a non-const rvalue expression, value_type shall be MoveInsertable into X;
    // otherwise, value_type shall be CopyInsertable into X.
    c_of_move_insertable am;
    am.insert(Traits::template construct_value<move_insertable>());

    c_of_copy_insertable ac;
    typename c_of_copy_insertable::value_type value(Traits::template construct_value<copy_insertable>());
    ac.insert(value);

    // Returns:  pair<iterator, bool> for a_uniq; iterator for a_eq
    STATIC_ASSERT(std::is_same_v<decltype(am.insert(Traits::template construct_value<move_insertable>())),
        std::conditional_t<(Traits::features & c_is_unique_associative) != 0,
            std::pair<typename c_of_move_insertable::iterator, bool>, typename c_of_move_insertable::iterator>>);

    STATIC_ASSERT(std::is_same_v<decltype(ac.insert(value)),
        std::conditional_t<(Traits::features & c_is_unique_associative) != 0,
            std::pair<typename c_of_copy_insertable::iterator, bool>, typename c_of_copy_insertable::iterator>>);

    // a.insert(p, t)
    // Requires:  If t is a non-const rvalue expression, value_type shall be MoveInsertable into X;
    // otherwise, value_type shall be CopyInsertable into X.
    am.insert(am.begin(), Traits::template construct_value<move_insertable>());
    ac.insert(ac.begin(), value);

    // Returns:  iterator
    STATIC_ASSERT(std::is_same_v<decltype(am.insert(am.begin(), Traits::template construct_value<move_insertable>())),
        typename c_of_move_insertable::iterator>);

    STATIC_ASSERT(std::is_same_v<decltype(ac.insert(ac.begin(), value)), typename c_of_copy_insertable::iterator>);

    // a.insert(i, j)
    // Requires:  value_type shall be EmplaceConstructible into X from *i
    c_of_emplace_constructible ea;
    std::move_iterator<input_iterator<typename Traits::emplace_argument_type>> i;
    ea.insert(i, i);

    STATIC_ASSERT(std::is_same_v<decltype(ea.insert(i, i)), void>);

    // a.insert(il)
    // void
    // Equivalent to a.insert(il.begin(), il.end()).
    ac.insert({value, value, value});

    STATIC_ASSERT(std::is_same_v<decltype(ac.insert({value, value, value})), void>);
});


DEFINE_TEST(test_ordered_associative_erase,
{
    // a.erase(k)
    // [No Requires clause]
    // Returns:  size_type
    {
        c_of_erasable a;
        a.erase(erasable(key()));
        STATIC_ASSERT(noexcept(a.erase(std::declval<erasable>()))); // strengthened
        STATIC_ASSERT(std::is_same_v<decltype(a.erase(erasable(key()))), typename c_of_erasable::size_type>);
}

// a.erase(q)
// [No Requires clause]
{
    c_of_erasable a;
    a.erase(a.begin());
    STATIC_ASSERT(noexcept(a.erase(a.begin()))); // strengthened
    STATIC_ASSERT(std::is_same_v<decltype(a.erase(a.begin())), typename c_of_erasable::iterator>);
}

// a.erase(q1, q2)
// [No Requires clause]
{
    c_of_erasable a;
    a.erase(a.end(), a.end());
    STATIC_ASSERT(noexcept(a.erase(a.end(), a.end()))); // strengthened
    STATIC_ASSERT(std::is_same_v<decltype(a.erase(a.end(), a.end())), typename c_of_erasable::iterator>);
}
});


DEFINE_TEST(test_ordered_associative_clear, {
    // a.clear()
    // [No Requires clause]
    // Returns:  void
    c_of_erasable a;
    a.clear();
    STATIC_ASSERT(noexcept(a.clear()));
    STATIC_ASSERT(std::is_same_v<decltype(a.clear()), void>);
});


DEFINE_TEST(test_ordered_associative_find, {
    // a.find(k), a.count(k), a.lower_bound(k), a.upper_bound(k), a.equal_range(k)
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable const b;

    (void) a.find(erasable(key()));
    (void) b.find(erasable(key()));

    (void) a.count(erasable(key()));
    (void) b.count(erasable(key()));

    (void) a.lower_bound(erasable(key()));
    (void) b.lower_bound(erasable(key()));

    (void) a.upper_bound(erasable(key()));
    (void) b.upper_bound(erasable(key()));

    (void) a.equal_range(erasable(key()));
    (void) b.equal_range(erasable(key()));

    // a.find(k):  Returns:  iterator; const_iterator for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a.find(erasable(key()))), typename c_of_erasable::iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.find(erasable(key()))), typename c_of_erasable::const_iterator>);

    // a.count(k):  Returns:  size_type
    STATIC_ASSERT(std::is_same_v<decltype(a.count(erasable(key()))), typename c_of_erasable::size_type>);
    STATIC_ASSERT(std::is_same_v<decltype(b.count(erasable(key()))), typename c_of_erasable::size_type>);

    // a.lower_bound(k):  Returns:  iterator; const_iterator for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a.lower_bound(erasable(key()))), typename c_of_erasable::iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.lower_bound(erasable(key()))), typename c_of_erasable::const_iterator>);

    // a.upper_bound(k):  Returns:  iterator; const_iterator for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a.upper_bound(erasable(key()))), typename c_of_erasable::iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.upper_bound(erasable(key()))), typename c_of_erasable::const_iterator>);

    // a.equal_range(k):  Returns pair<iterator, iterator>; pair<const_iterator, const_iterator>
    // for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a.equal_range(erasable(key()))),
        std::pair<typename c_of_erasable::iterator, typename c_of_erasable::iterator>>);

    STATIC_ASSERT(std::is_same_v<decltype(b.equal_range(erasable(key()))),
        std::pair<typename c_of_erasable::const_iterator, typename c_of_erasable::const_iterator>>);
});


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_ordered_associative) != 0>
struct check_all_ordered_associative_requirements {
    check_all_ordered_associative_requirements() {
        test_ordered_associative_typedefs<Tag>();
        test_ordered_associative_typedefs_for_maps<Tag>();
        test_ordered_associative_default_constructor_with_comparer<Tag>();
        test_ordered_associative_range_constructor_with_comparer<Tag>();
        test_ordered_associative_initializer_list_assign<Tag>();
        test_ordered_associative_comparers<Tag>();
        test_ordered_associative_emplace<Tag>();
        test_ordered_associative_insert<Tag>();
        test_ordered_associative_erase<Tag>();
        test_ordered_associative_clear<Tag>();
        test_ordered_associative_find<Tag>();
    }
};

template <container_tag Tag>
struct check_all_ordered_associative_requirements<Tag, false> {};


//
//
// UNORDERED ASSOCIATIVE CONTAINER REQUIREMENTS (23.2.5[unord.req]/Table 103)
//
//

DEFINE_TEST(test_unordered_associative_typedefs, {
    // X::key_type is Key
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::key_type, erasable>);

    // X::hasher is Hash
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::hasher, faux_hash<erasable>>);

    // X::key_equal is Pred
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::key_equal, faux_compare<erasable>>);

    // X::local_iterator is "an iterator type whose category, value type, difference type, and
    // pointer and reference types are the same as X::iterator's."
    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::iterator>::iterator_category,
        typename std::iterator_traits<typename c_of_erasable::local_iterator>::iterator_category>);

    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::iterator>::value_type,
        typename std::iterator_traits<typename c_of_erasable::local_iterator>::value_type>);

    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::iterator>::difference_type,
        typename std::iterator_traits<typename c_of_erasable::local_iterator>::difference_type>);

    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::iterator>::pointer,
        typename std::iterator_traits<typename c_of_erasable::local_iterator>::pointer>);

    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::iterator>::reference,
        typename std::iterator_traits<typename c_of_erasable::local_iterator>::reference>);

    // X::const_local_iterator is "an iterator type whose category, value type, difference type, and
    // pointer and reference types are the same as X::const_iterator's."
    STATIC_ASSERT(
        std::is_same_v<typename std::iterator_traits<typename c_of_erasable::const_iterator>::iterator_category,
            typename std::iterator_traits<typename c_of_erasable::const_local_iterator>::iterator_category>);

    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::const_iterator>::value_type,
        typename std::iterator_traits<typename c_of_erasable::const_local_iterator>::value_type>);

    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::const_iterator>::difference_type,
        typename std::iterator_traits<typename c_of_erasable::const_local_iterator>::difference_type>);

    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::const_iterator>::pointer,
        typename std::iterator_traits<typename c_of_erasable::const_local_iterator>::pointer>);

    STATIC_ASSERT(std::is_same_v<typename std::iterator_traits<typename c_of_erasable::const_iterator>::reference,
        typename std::iterator_traits<typename c_of_erasable::const_local_iterator>::reference>);
});


DEFINE_TEST(test_unordered_associative_typedefs_for_maps, {
    // X::mapped_type is T
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::mapped_type, erasable>);

    // X::value_type is pair<const Key, T>
    STATIC_ASSERT(std::is_same_v<typename c_of_erasable::value_type, std::pair<erasable const, erasable>>);
});

NOT_SUPPORTED_SPECIALIZATION(test_unordered_associative_typedefs_for_maps, tag_unordered_multiset);
NOT_SUPPORTED_SPECIALIZATION(test_unordered_associative_typedefs_for_maps, tag_unordered_set);


DEFINE_TEST(test_unordered_associative_default_constructors,
{
    // X(n, hf, eq) and X a(n, hf, eq)
    // Requires:  hasher and key_equal are CopyConstructible.
    {
        typedef typename Traits::template bind_container<
            erasable,
            std::allocator<erasable>,
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
    typedef typename Traits::template bind_container<erasable, std::allocator<erasable>,
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
    std::move_iterator<input_iterator<typename Traits::emplace_argument_type>> i;

    // X(i, j, n, hf, eq) and X a(i, j, n, hf, eq)
    // Requires:  hasher and key_equal are CopyConstructible.  value_type is EmplaceConstructible
    // into X from *i.
    {
        typedef typename Traits::template bind_container<emplace_constructible, std::allocator<emplace_constructible>,
            copy_constructible_hash<emplace_constructible>, copy_constructible_compare<emplace_constructible>>::type
            container_type;

        copy_constructible_hash<emplace_constructible> const hf((key()));
        copy_constructible_compare<emplace_constructible> const eq((key()));
        (container_type(i, i, 0, hf, eq));
        container_type a(i, i, 0, hf, eq);
    }

    // X(i, j, n, hf) and X a(i, j, n, hf)
    // Requires:  hasher is CopyConstructible and key_equal is DefaultConstructible.  value_type is
    // EmplaceConstructible into X from *i.
    {
        typedef typename Traits::template bind_container<emplace_constructible, std::allocator<emplace_constructible>,
            copy_constructible_hash<emplace_constructible>, default_constructible_compare<emplace_constructible>>::type
            container_type;

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

    // X(il)
    // Equivalent to X(il.begin(), il.end()).
    {
        typename c_of_copy_insertable::value_type t = Traits::template construct_value<copy_insertable>();
        c_of_copy_insertable({t, t, t});
        c_of_copy_insertable a2({t, t, t});
    }
});


DEFINE_TEST(test_unordered_associative_copy_constructor,
    {
        // X(b) and X a(b)
        // [Implicitly requires that the hash function and predicate are CopyConstructible.]
    });


DEFINE_TEST(test_unordered_associative_find, {
    // a.find(k), a.count(k), a.equal_range(k)
    // [No Requires clause]
    c_of_erasable a;
    c_of_erasable const b;

    (void) a.find(erasable(key()));
    (void) b.find(erasable(key()));

    (void) a.count(erasable(key()));
    (void) b.count(erasable(key()));

    (void) a.equal_range(erasable(key()));
    (void) b.equal_range(erasable(key()));

    // a.find(k):  Returns:  iterator; const_iterator for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a.find(erasable(key()))), typename c_of_erasable::iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.find(erasable(key()))), typename c_of_erasable::const_iterator>);

    // a.count(k):  Returns:  size_type
    STATIC_ASSERT(std::is_same_v<decltype(a.count(erasable(key()))), typename c_of_erasable::size_type>);
    STATIC_ASSERT(std::is_same_v<decltype(b.count(erasable(key()))), typename c_of_erasable::size_type>);

    // a.equal_range(k):  Returns pair<iterator, iterator>; pair<const_iterator, const_iterator>
    // for constant a
    STATIC_ASSERT(std::is_same_v<decltype(a.equal_range(erasable(key()))),
        std::pair<typename c_of_erasable::iterator, typename c_of_erasable::iterator>>);

    STATIC_ASSERT(std::is_same_v<decltype(b.equal_range(erasable(key()))),
        std::pair<typename c_of_erasable::const_iterator, typename c_of_erasable::const_iterator>>);
});


DEFINE_TEST(test_unordered_associative_buckets, {
    c_of_erasable const b;

    // b.bucket_count()
    // [No Requires clause]
    // Returns:  size_type
    (void) b.bucket_count();
    STATIC_ASSERT(std::is_same_v<decltype(b.bucket_count()), typename c_of_erasable::size_type>);

    // b.max_bucket_count()
    // [No Requires clause]
    // Returns:  size_type
    (void) b.max_bucket_count();
    STATIC_ASSERT(std::is_same_v<decltype(b.max_bucket_count()), typename c_of_erasable::size_type>);

    // b.bucket(k)
    // [No Requires clause]
    // Returns:  size_type
    (void) b.bucket(erasable(key()));
    STATIC_ASSERT(std::is_same_v<decltype(b.bucket(erasable(key()))), typename c_of_erasable::size_type>);

    // b.bucket_size(n)
    // [No Requires clause]
    // Returns:  size_type
    (void) b.bucket_size(0);
    STATIC_ASSERT(std::is_same_v<decltype(b.bucket_size(0)), typename c_of_erasable::size_type>);
});


DEFINE_TEST(test_unordered_associative_bucket_begin_end, {
    // b.begin(n), b.end(n), b.cbegin(n), b.cend(n)
    // [No Requires clause]
    c_of_erasable a;
    (void) a.begin(0);
    (void) a.end(0);
    (void) a.cbegin(0);
    (void) a.cend(0);

    c_of_erasable const b;
    (void) b.begin(0);
    (void) b.end(0);
    (void) b.cbegin(0);
    (void) b.cend(0);

    // Returns:  local_iterator; const_local_iterator for const b and c{begin,end}()
    STATIC_ASSERT(std::is_same_v<decltype(a.begin()), typename c_of_erasable::local_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.end()), typename c_of_erasable::local_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.cbegin()), typename c_of_erasable::const_local_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(a.cend()), typename c_of_erasable::const_local_iterator>);

    STATIC_ASSERT(std::is_same_v<decltype(b.begin()), typename c_of_erasable::const_local_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.end()), typename c_of_erasable::const_local_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.cbegin()), typename c_of_erasable::const_local_iterator>);
    STATIC_ASSERT(std::is_same_v<decltype(b.cend()), typename c_of_erasable::const_local_iterator>);
});


DEFINE_TEST(test_unordered_associative_load_factor, {
    c_of_erasable a;
    c_of_erasable const b;

    // b.load_factor()
    // [No Requires clause]
    (void) b.load_factor();

    // Returns:  float
    STATIC_ASSERT(std::is_same_v<decltype(b.load_factor()), float>);

    // b.max_load_factor()
    // [No Requires clause]
    (void) b.max_load_factor();

    // Returns:  float
    STATIC_ASSERT(std::is_same_v<decltype(b.max_load_factor()), float>);

    // a.max_load_factor(z)
    // [No Requires clause]
    a.max_load_factor(1.0);

    // Returns:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.max_load_factor(1.0)), void>);
});


DEFINE_TEST(test_unordered_associative_rehash, {
    c_of_erasable a;

    // a.rehash(n)
    // [No Requires clause]
    a.rehash(0);

    // Returns:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.rehash(0)), void>);

    // a.reserve(n)
    // [No Requires clause]
    a.reserve(0);

    // Returns:  void
    STATIC_ASSERT(std::is_same_v<decltype(a.reserve(0)), void>);
});


template <container_tag Tag, bool = (container_traits<Tag>::features & c_is_unordered_associative) != 0>
struct check_all_unordered_associative_requirements {
    check_all_unordered_associative_requirements() {
        test_unordered_associative_typedefs<Tag>();
        test_unordered_associative_typedefs_for_maps<Tag>();
        test_unordered_associative_default_constructors<Tag>();
        test_unordered_associative_range_constructors<Tag>();
        test_unordered_associative_copy_constructor<Tag>();

        // These operations are the same as their ordered counterparts
        test_ordered_associative_initializer_list_assign<Tag>();
        test_ordered_associative_emplace<Tag>();
        test_ordered_associative_insert<Tag>();
        test_ordered_associative_erase<Tag>();
        test_ordered_associative_clear<Tag>();

        test_unordered_associative_find<Tag>();
        test_unordered_associative_buckets<Tag>();
        test_unordered_associative_bucket_begin_end<Tag>();
        test_unordered_associative_load_factor<Tag>();
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
    c_of_erasable a((std::allocator<erasable>()));
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
    c_of_copy_insertable a(1, copy_insertable(key()), std::allocator<copy_insertable>());
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
    std::move_iterator<input_iterator<emplace_argument>> const i;
    c_of_emplace_constructible a(i, i, std::allocator<emplace_constructible>());
});

DEFINE_TEST_SPECIALIZATION(test_specific_sequence_range_constructor, tag_vector,
{
    // X(InputIterator first, InputIterator last, const Allocator& = Allocator());
    // [See the primary test for comments.]
    //
    // Requires:  T shall be EmplaceConstructible into X from *i.  For vector, if the iterator does
    // not meet the forward iterator requirements, T shall also be MoveInsertable into X.
    {
         std::move_iterator<forward_iterator<emplace_argument>> i;
         c_of_emplace_constructible(i, i, std::allocator<emplace_constructible>());
         c_of_emplace_constructible a(i, i, std::allocator<emplace_constructible>());
}
{
    std::move_iterator<input_iterator<emplace_argument>> i;
    c_of_ec_mi(i, i, std::allocator<ec_mi>());
    c_of_ec_mi a(i, i, std::allocator<ec_mi>());
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

        // (C++20) Returns:  size_type
        // (C++17) Returns:  void
        using ret_type_a = std::conditional_t<_HAS_CXX20, typename c_of_equality_comparable::size_type, void>;
        STATIC_ASSERT(std::is_same_v<decltype(a.remove(equality_comparable(key()))), ret_type_a>);

        // (C++20) Returns:  size_type
        // (C++17) Returns:  void
        using ret_type_b = std::conditional_t<_HAS_CXX20, typename c_of_erasable::size_type, void>;
        STATIC_ASSERT(std::is_same_v<decltype(b.remove_if(faux_predicate<erasable>())), ret_type_b>);
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

    // (C++20) Returns:  size_type
    // (C++17) Returns:  void
    using ret_type_a = std::conditional_t<_HAS_CXX20, typename c_of_equality_comparable::size_type, void>;
    STATIC_ASSERT(std::is_same_v<decltype(a.unique()), ret_type_a>);

    // (C++20) Returns:  size_type
    // (C++17) Returns:  void
    using ret_type_b = std::conditional_t<_HAS_CXX20, typename c_of_erasable::size_type, void>;
    STATIC_ASSERT(std::is_same_v<decltype(b.unique(faux_compare<erasable>())), ret_type_b>);
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

    STATIC_ASSERT(std::is_same_v<decltype(a.merge(a)), void>);
    STATIC_ASSERT(std::is_same_v<decltype(a.merge(c_of_less_comparable())), void>);

    STATIC_ASSERT(std::is_same_v<decltype(b.merge(b, faux_compare<erasable>())), void>);
    STATIC_ASSERT(std::is_same_v<decltype(b.merge(c_of_erasable(), faux_compare<erasable>())), void>);
}

// void sort();
// void sort(Compare comp);
// [No relevant Requires clause]
{
    c_of_less_comparable a;
    c_of_erasable b;
    a.sort();
    b.sort(faux_compare<erasable>());

    STATIC_ASSERT(std::is_same_v<decltype(a.sort()), void>);
    STATIC_ASSERT(std::is_same_v<decltype(b.sort(faux_compare<erasable>())), void>);
}

// void reverse();
// [No Requires clause]
{
    c_of_erasable a;
    a.reverse();

    STATIC_ASSERT(std::is_same_v<decltype(a.reverse()), void>);
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
            std::allocator<erasable>,
            copy_constructible_compare<erasable>
        >::type container_type;

        copy_constructible_compare<erasable> const c((key()));
        container_type a(c, std::allocator<typename Traits::template bind_value<erasable>::type>());
}

// X(i, j, c) and X a(i, j, c);
// [No Requires clause]
//
// SPEC:  The Requires clause should be the same as the Requires clause for the equivalent
// constructor in the Associative requirements (sans-allocator).  That states:  "Requires:
// key_compare is CopyConstructible.  value_type is EmplaceConstructible into X from *i."

{
    typedef typename Traits::template bind_container<emplace_constructible, std::allocator<emplace_constructible>,
        copy_constructible_compare<emplace_constructible>>::type container_type;

    copy_constructible_compare<emplace_constructible> const c((key()));
    std::move_iterator<input_iterator<typename Traits::emplace_argument_type>> i;
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
            std::allocator<erasable>,
            copy_constructible_hash<erasable>,
            copy_constructible_compare<erasable>
        >::type container_type;

        copy_constructible_hash<erasable> const hf((key()));
        copy_constructible_compare<erasable> const eq((key()));
        (container_type(0, hf, eq, std::allocator<typename Traits::template bind_value<erasable>::type>()));
        container_type a(0, hf, eq, std::allocator<typename Traits::template bind_value<erasable>::type>());
}

// X(i, j, n, hf, eq, allocator)
//
// SPEC:  The Requires clause should be the same as the Requires clause for the equivalent
// constructor in the Unordered Associative requirements (sans-allocator).  That states:
// "Requires:  hasher and key_equal are CopyConstructible.  value_type is EmplaceConstructible
// into X from *i."
{
    typedef typename Traits::template bind_container<emplace_constructible, std::allocator<emplace_constructible>,
        copy_constructible_hash<emplace_constructible>, copy_constructible_compare<emplace_constructible>>::type
        container_type;
    std::move_iterator<input_iterator<typename Traits::emplace_argument_type>> i;

    copy_constructible_hash<emplace_constructible> const hf((key()));
    copy_constructible_compare<emplace_constructible> const eq((key()));
    (container_type(
        i, i, 0, hf, eq, std::allocator<typename Traits::template bind_value<emplace_constructible>::type>()));
    container_type a(
        i, i, 0, hf, eq, std::allocator<typename Traits::template bind_value<emplace_constructible>::type>());
}
});


DEFINE_TEST(check_all_specific_requirements, {});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_deque, {
    test_specific_sequence_count_constructor_default<Tag>();
    test_specific_sequence_count_constructor_copy<Tag>();
    test_specific_sequence_range_constructor<Tag>();
    test_specific_sequence_range_constructor<Tag>();

    // assign() is verified in the Sequence Container requirements

    // void resize(size_type sz);
    // Requires:  T shall be MoveInsertable and DefaultInsertable into *this
    {
        c_of_dc_mi a;
        a.resize(1);

        STATIC_ASSERT(std::is_same_v<decltype(a.resize(1)), void>);
    }

    // void resize(size_type sz, const T& c);
    // Requires:  T shall be MoveInsertable and CopyInsertable into *this
    {
        c_of_copy_insertable a;
        a.resize(1, copy_insertable(key()));

        STATIC_ASSERT(std::is_same_v<decltype(a.resize(1, copy_insertable(key()))), void>);
    }

    // void shrink_to_fit();
    // Requires:  T shall be MoveInsertable into *this
    {
        c_of_move_insertable a;
        a.shrink_to_fit();

        STATIC_ASSERT(std::is_same_v<decltype(a.shrink_to_fit()), void>);
    }
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_forward_list, {
    test_specific_sequence_count_constructor_default<Tag>();
    test_specific_sequence_count_constructor_copy<Tag>();
    test_specific_sequence_range_constructor<Tag>();
    test_specific_sequence_range_constructor<Tag>();

    // iterator before_begin();
    // const_iterator before_begin() const;
    // const_iterator cbefore_begin() const;
    // [No Requires clause]
    {
        c_of_erasable a;
        c_of_erasable const b;

        (void) a.before_begin();
        (void) b.before_begin();

        (void) a.cbefore_begin();
        (void) b.cbefore_begin();

        STATIC_ASSERT(std::is_same_v<decltype(a.before_begin()), typename c_of_erasable::iterator>);
        STATIC_ASSERT(std::is_same_v<decltype(b.before_begin()), typename c_of_erasable::const_iterator>);

        STATIC_ASSERT(std::is_same_v<decltype(a.cbefore_begin()), typename c_of_erasable::const_iterator>);
        STATIC_ASSERT(std::is_same_v<decltype(b.cbefore_begin()), typename c_of_erasable::const_iterator>);
    }

    // iterator insert_after(const_iterator position, const T& x);
    // iterator insert_after(const_iterator position, T&& x);
    // [No relevant Requires clause]
    //
    // SPEC: Should require T:CopyInsertable for lvalue, T:MoveInsertable for rvalue
    {
        c_of_copy_insertable a;
        a.insert_after(a.cbefore_begin(), copy_insertable(key()));

        STATIC_ASSERT(std::is_same_v<decltype(a.insert_after(a.cbefore_begin(), copy_insertable(key()))),
            typename c_of_copy_insertable::iterator>);

        c_of_move_insertable b;
        b.insert_after(b.cbefore_begin(), move_insertable(key()));

        STATIC_ASSERT(std::is_same_v<decltype(b.insert_after(b.cbefore_begin(), move_insertable(key()))),
            typename c_of_move_insertable::iterator>);
    }

    // iterator insert_after(const_iterator position, size_type n, const T& x);
    // [No relevant Requires clause]
    //
    // SPEC:  Should require T:CopyInsertable
    {
        c_of_copy_insertable a;
        a.insert_after(a.cbefore_begin(), 1, copy_insertable(key()));

        STATIC_ASSERT(std::is_same_v<decltype(a.insert_after(a.cbefore_begin(), 1, copy_insertable(key()))),
            typename c_of_copy_insertable::iterator>);
    }

    // iterator insert_after(const_iterator position, InputIterator first, InputIterator last);
    // [No relevant Requires clause]
    //
    // SPEC:  Should require T:EmplaceConstructible into *this from *first.
    {
        std::move_iterator<input_iterator<emplace_argument>> i;
        c_of_emplace_constructible a;
        a.insert_after(a.cbefore_begin(), i, i);

        STATIC_ASSERT(std::is_same_v<decltype(a.insert_after(a.cbefore_begin(), i, i)),
            typename c_of_emplace_constructible::iterator>);
    }

    // iterator insert_after(const_iterator position, initializer_list<T> il);
    // [No relevant Requires clause]
    //
    // SPEC:  Should require T:CopyInsertable
    {
        copy_insertable t = key();
        c_of_copy_insertable a;
        a.insert_after(a.cbefore_begin(), {t, t, t});

        STATIC_ASSERT(std::is_same_v<decltype(a.insert_after(a.cbefore_begin(), {t, t, t})),
            typename c_of_copy_insertable::iterator>);
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

        STATIC_ASSERT(std::is_same_v<decltype(a.emplace_after(a.cbefore_begin(), emplace_argument())),
            typename c_of_emplace_constructible::iterator>);
    }

    // iterator erase_after(const_iterator position);
    // [No relevant Requires clause]
    {
        c_of_erasable a;
        a.erase_after(a.cbefore_begin());
        STATIC_ASSERT(noexcept(a.erase_after(a.cbefore_begin()))); // strengthened

        STATIC_ASSERT(std::is_same_v<decltype(a.erase_after(a.cbefore_begin())), typename c_of_erasable::iterator>);
    }

    // iterator erase_after(const_iterator position, const_iterator last);
    // [No relevant Requires clause]
    {
        c_of_erasable a;
        a.erase_after(a.cbefore_begin(), a.cbefore_begin());
        STATIC_ASSERT(noexcept(a.erase_after(a.cbefore_begin(), a.cbefore_begin()))); // strengthened

        STATIC_ASSERT(std::is_same_v<decltype(a.erase_after(a.cbefore_begin(), a.cbefore_begin())),
            typename c_of_erasable::iterator>);
    }

    // void resize(size_type sz);
    // Requires:  T shall be DefaultInsertable into *this
    {
        c_of_default_constructible a;
        a.resize(1);

        STATIC_ASSERT(std::is_same_v<decltype(a.resize(1)), void>);
    }

    // void resize(size_type sz, const value_type& c);
    // Requires:  T shall be CopyInsertable into *this
    {
        c_of_copy_insertable a;
        a.resize(1, copy_insertable(key()));

        STATIC_ASSERT(std::is_same_v<decltype(a.resize(1, copy_insertable(key()))), void>);
    }

    // void clear()
    // [No Requires clause]
    {
        c_of_erasable a;
        a.clear();
        STATIC_ASSERT(noexcept(a.clear()));

        STATIC_ASSERT(std::is_same_v<decltype(a.clear()), void>);
    }

    // void splice_after(const_iterator position, forward_list& x);
    // void splice_after(const_iterator position, forward_list&& x);
    // [No Requires clause]
    {
        c_of_erasable a;
        a.splice_after(a.cbefore_begin(), a);
        STATIC_ASSERT(noexcept(a.splice_after(a.cbefore_begin(), a))); // strengthened
        a.splice_after(a.cbefore_begin(), std::move(a));
        STATIC_ASSERT(noexcept(a.splice_after(a.cbefore_begin(), std::move(a)))); // strengthened

        STATIC_ASSERT(std::is_same_v<decltype(a.splice_after(a.cbefore_begin(), a)), void>);

        STATIC_ASSERT(std::is_same_v<decltype(a.splice_after(a.cbefore_begin(), std::move(a))), void>);
    }

    // void splice_after(const_iterator position, forward_list& x, const_iterator i);
    // void splice_after(const_iterator position, forward_list&& x, const_iterator i);
    // [No relevant Requires clause]
    {
        c_of_erasable a;
        a.splice_after(a.cbefore_begin(), a, a.cbefore_begin());
        STATIC_ASSERT(noexcept(a.splice_after(a.cbefore_begin(), a, a.cbefore_begin()))); // strengthened
        a.splice_after(a.cbefore_begin(), std::move(a), a.cbefore_begin());
        STATIC_ASSERT(noexcept(a.splice_after(a.cbefore_begin(), std::move(a), a.cbefore_begin()))); // strengthened

        STATIC_ASSERT(std::is_same_v<decltype(a.splice_after(a.cbefore_begin(), a, a.cbefore_begin())), void>);

        STATIC_ASSERT(
            std::is_same_v<decltype(a.splice_after(a.cbefore_begin(), std::move(a), a.cbefore_begin())), void>);
    }

    // void splice_after(const_iterator position, forward_list& x, const_iterator first, const_iterator last);
    // void splice_after(const_iterator position, forward_list&& x, const_iterator first, const_iterator last);
    // [No relevant Requires clause]
    {
        c_of_erasable a;
        a.splice_after(a.cbefore_begin(), a, a.cbefore_begin(), a.cbefore_begin());
        STATIC_ASSERT(
            noexcept(a.splice_after(a.cbefore_begin(), a, a.cbefore_begin(), a.cbefore_begin()))); // strengthened
        a.splice_after(a.cbefore_begin(), std::move(a), a.cbefore_begin(), a.cbefore_begin());
        STATIC_ASSERT(noexcept(
            a.splice_after(a.cbefore_begin(), std::move(a), a.cbefore_begin(), a.cbefore_begin()))); // strengthened

        STATIC_ASSERT(
            std::is_same_v<decltype(a.splice_after(a.cbefore_begin(), a, a.cbefore_begin(), a.cbefore_begin())), void>);

        STATIC_ASSERT(std::is_same_v<
            decltype(a.splice_after(a.cbefore_begin(), std::move(a), a.cbefore_begin(), a.cbefore_begin())), void>);
    }

    test_specific_common_list_operations<Tag>();
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_list, {
    test_specific_sequence_count_constructor_default<Tag>();
    test_specific_sequence_count_constructor_copy<Tag>();
    test_specific_sequence_range_constructor<Tag>();
    test_specific_sequence_range_constructor<Tag>();

    // assign() is verified in the Sequence Container requirements

    // void resize(size_type sz);
    // Requires:  T shall be DefaultInsertable into *this

    // void resize(size_type sz, const T& c);
    // Requires:  T shall be CopyInsertable into *this

    // void splice(const_iterator position, list& x);
    // void splice(const_iterator position, list&& x);
    // [No Requires clause]
    {
        c_of_erasable a;
        a.splice(a.begin(), a);
        STATIC_ASSERT(!noexcept(a.splice(a.begin(), a))); // throws length_error
        a.splice(a.begin(), std::move(a));
        STATIC_ASSERT(!noexcept(a.splice(a.begin(), std::move(a)))); // throws length_error
    }

    // void splice(const_iterator position, list& x, const_iterator i);
    // void splice(const_iterator position, list&& x, const_iterator i);
    // [No relevant Requires clause]
    {
        c_of_erasable a;
        a.splice(a.begin(), a, a.begin());
        STATIC_ASSERT(!noexcept(a.splice(a.begin(), a, a.begin()))); // throws length_error
        a.splice(a.begin(), std::move(a), a.begin());
        STATIC_ASSERT(!noexcept(a.splice(a.begin(), std::move(a), a.begin()))); // throws length_error
    }

    // void splice(const_iterator position, list& x, const_iterator first, const_iterator last);
    // void splice(const_iterator position, list&& x, const_iterator first, const_iterator last);
    // [No relevant Requires clause]
    {
        c_of_erasable a;
        a.splice(a.begin(), a, a.begin(), a.begin());
        STATIC_ASSERT(!noexcept(a.splice(a.begin(), a, a.begin(), a.begin()))); // throws length_error
        a.splice(a.begin(), std::move(a), a.begin(), a.begin());
        STATIC_ASSERT(!noexcept(a.splice(a.begin(), std::move(a), a.begin(), a.begin()))); // throws length_error
    }

    test_specific_common_list_operations<Tag>();
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_vector, {
    test_specific_sequence_count_constructor_default<Tag>();
    test_specific_sequence_count_constructor_copy<Tag>();
    test_specific_sequence_range_constructor<Tag>();
    test_specific_sequence_range_constructor<Tag>();

    // assign() is verified in the Sequence Container requirements

    // size_type capacity() const;
    // [No Requires clause]
    {
        c_of_erasable const a;
        (void) a.capacity();

        STATIC_ASSERT(std::is_same_v<decltype(a.capacity()), typename c_of_erasable::size_type>);
    }

    // void reserve(size_type n); and void shrink_to_fit();
    // Requires:  T shall be MoveInsertable into *this
    {
        c_of_move_insertable a;
        a.reserve(1);
        a.shrink_to_fit();

        STATIC_ASSERT(std::is_same_v<decltype(a.reserve(1)), void>);
        STATIC_ASSERT(std::is_same_v<decltype(a.shrink_to_fit()), void>);
    }

    // void resize(size_type sz);
    // Requires:  T shall be MoveInsertable and DefaultInsertable into *this
    {
        c_of_dc_mi a;
        a.resize(1);

        STATIC_ASSERT(std::is_same_v<decltype(a.resize(1)), void>);
    }

    // void resize(size_type sz, const T& c);
    // Requires:  T shall be MoveInsertable into *this and CopyInsertable into *this
    {
        c_of_copy_insertable a;
        a.resize(1, copy_insertable(key()));

        STATIC_ASSERT(std::is_same_v<decltype(a.resize(1, copy_insertable(key()))), void>);
    }

    // T* data(); and const T* data() const;
    // [No Requires clause]
    {
        c_of_erasable a;
        c_of_erasable const b;

        (void) a.data();
        (void) b.data();

        STATIC_ASSERT(std::is_same_v<decltype(a.data()), erasable*>);
        STATIC_ASSERT(std::is_same_v<decltype(b.data()), erasable const*>);
    }
});

DEFINE_TEST_SPECIALIZATION(check_all_specific_requirements, tag_map, {
    test_specific_ordered_associative_constructors<Tag>();

    // T& operator[](const key_type& x);
    // Requires:  key_type shall be CopyInsertable and mapped_type shall be DefaultInsertable into
    // *this
    {
        std::map<copy_insertable, default_constructible, faux_compare<copy_insertable>> a;

        copy_insertable k((key()));
        a[k];

        STATIC_ASSERT(std::is_same_v<decltype(a[k]), default_constructible&>);
    }

    // T& operator[](key_type& x);
    // Requires:  mapped_type shall be DefaultInsertable into *this
    //
    // SPEC:  Presumably this should also say "key_type shall be MoveInsertable into *this," given
    // the Effects clause.
    {
        std::map<move_insertable, default_constructible, faux_compare<move_insertable>> a;

        a[move_insertable(key())];

        STATIC_ASSERT(std::is_same_v<decltype(a[move_insertable(key())]), default_constructible&>);
    }

    // T& at(const key_type& x);
    // const T& at(const key_type& x) const;
    // [No Requires clause]
    {
        c_of_erasable a;
        c_of_erasable const b;

        (void) a.at(erasable(key()));
        (void) b.at(erasable(key()));

        STATIC_ASSERT(std::is_same_v<decltype(a.at(erasable(key()))), erasable&>);
        STATIC_ASSERT(std::is_same_v<decltype(b.at(erasable(key()))), erasable const&>);
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
            faux_compare<copy_insertable>>
            a;

        copy_insertable k((key()));
        a[k];

        STATIC_ASSERT(std::is_same_v<decltype(a[k]), default_constructible&>);

        std::unordered_map<move_insertable, default_constructible, faux_hash<move_insertable>,
            faux_compare<move_insertable>>
            b;

        b[move_insertable(key())];

        STATIC_ASSERT(std::is_same_v<decltype(b[move_insertable(key())]), default_constructible&>);
    }

    // mapped_type& at(const key_type& k);
    // const mapped_type& at(const key_type& k) const;
    // [No Requires clause]
    {
        c_of_erasable a;
        c_of_erasable const b;

        (void) a.at(erasable(key()));
        (void) b.at(erasable(key()));

        STATIC_ASSERT(std::is_same_v<decltype(a.at(erasable(key()))), erasable&>);
        STATIC_ASSERT(std::is_same_v<decltype(b.at(erasable(key()))), erasable const&>);
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

// Ad hoc tests for exception specifications of std::vector<bool, Alloc> (LWG-3778)
template <class Alloc>
void assert_vector_bool_noexcept_impl() {
    using vec_bool = std::vector<bool, Alloc>;

    constexpr bool nothrow_on_pocma = std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value
                                   || std::allocator_traits<Alloc>::is_always_equal::value;

    STATIC_ASSERT(std::is_nothrow_default_constructible_v<vec_bool> == std::is_nothrow_default_constructible_v<Alloc>);
    STATIC_ASSERT(std::is_nothrow_constructible_v<vec_bool, const Alloc&>);
    STATIC_ASSERT(std::is_nothrow_move_constructible_v<vec_bool>);
    STATIC_ASSERT(std::is_nothrow_move_assignable_v<vec_bool> == nothrow_on_pocma);
    STATIC_ASSERT(std::is_nothrow_destructible_v<vec_bool>);

    STATIC_ASSERT(noexcept(std::declval<vec_bool&>().swap(std::declval<vec_bool&>()))); // strengthened
    STATIC_ASSERT(noexcept(std::swap(std::declval<vec_bool&>(), std::declval<vec_bool&>()))); // strengthened
#if _HAS_CXX17
    STATIC_ASSERT(std::is_nothrow_swappable_v<vec_bool>); // strengthened
#endif // _HAS_CXX17
}

void assert_vector_bool_noexcept() {
    assert_vector_bool_noexcept_impl<std::allocator<bool>>();
    assert_vector_bool_noexcept_impl<pocma_allocator<bool>>();
    assert_vector_bool_noexcept_impl<non_pocma_allocator<bool>>();
}

template <container_tag Tag>
void assert_container() {
    check_all_container_requirements<Tag>();
    check_all_reversible_requirements<Tag>();
    check_all_optional_container_requirements<Tag>();
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
