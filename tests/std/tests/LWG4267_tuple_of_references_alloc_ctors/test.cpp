// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// LWG-4267 Uses-allocator construction is meaningless for tuple of references

#include <array>
#include <cassert>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

using Alloc = allocator<int>;

// Uses the leading allocator convention: T(allocator_arg_t, const Alloc&, args...)
struct LeadingAlloc {
    using allocator_type = Alloc;

    bool got_alloc = false;

    LeadingAlloc()                    = default;
    LeadingAlloc(const LeadingAlloc&) = default;
    constexpr LeadingAlloc(allocator_arg_t, const Alloc&) : got_alloc{true} {}
    constexpr LeadingAlloc(allocator_arg_t, const Alloc&, const LeadingAlloc&) : got_alloc{true} {}
};

// Uses the trailing allocator convention: T(args..., const Alloc&)
struct TrailingAlloc {
    using allocator_type = Alloc;

    bool got_alloc = false;

    TrailingAlloc()                     = default;
    TrailingAlloc(const TrailingAlloc&) = default;
    constexpr explicit TrailingAlloc(const Alloc&) : got_alloc{true} {}
    constexpr TrailingAlloc(const TrailingAlloc&, const Alloc&) : got_alloc{true} {}
};

STATIC_ASSERT(uses_allocator_v<LeadingAlloc, Alloc>);
STATIC_ASSERT(uses_allocator_v<TrailingAlloc, Alloc>);

// A reference type is never a program-defined type, so uses_allocator is always false for it.
STATIC_ASSERT(!uses_allocator_v<LeadingAlloc&, Alloc>);
STATIC_ASSERT(!uses_allocator_v<const LeadingAlloc&, Alloc>);
STATIC_ASSERT(!uses_allocator_v<LeadingAlloc&&, Alloc>);
STATIC_ASSERT(!uses_allocator_v<TrailingAlloc&, Alloc>);

// References can't be default-constructed, with or without an allocator.
STATIC_ASSERT(!is_constructible_v<tuple<LeadingAlloc&>, allocator_arg_t, const Alloc&>);
STATIC_ASSERT(!is_constructible_v<tuple<TrailingAlloc, TrailingAlloc&>, allocator_arg_t, const Alloc&>);

// N5014 [tuple.cnstr]/33 as modified by LWG-4267: "each element of non-reference type is constructed with
// uses-allocator construction". Reference elements must be bound directly to the original objects.
template <class T>
_CONSTEXPR20 bool test_tuple_of_references() {
    const Alloc al{};
    T obj{};
    const T& cref = obj;

    { // tuple(allocator_arg_t, const Alloc&, UTypes&&...)
        tuple<T&> lref{allocator_arg, al, obj};
        assert(&get<0>(lref) == &obj);

        tuple<const T&> clref{allocator_arg, al, obj};
        assert(&get<0>(clref) == &obj);

        tuple<T&&> rref{allocator_arg, al, move(obj)};
        assert(&get<0>(rref) == &obj);

        tuple<T, T&> mixed{allocator_arg, al, obj, obj};
        assert(get<0>(mixed).got_alloc);
        assert(&get<1>(mixed) == &obj);
    }

    { // tuple(allocator_arg_t, const Alloc&, const Types&...), chosen because {} can't deduce UTypes
        tuple<T, const T&> mixed{allocator_arg, al, {}, cref};
        assert(get<0>(mixed).got_alloc);
        assert(&get<1>(mixed) == &obj);
    }

    { // allocator-extended copy and move constructors
        tuple<T, T&> src{obj, obj};
        const auto& csrc = src;

        tuple<T, T&> copied{allocator_arg, al, csrc};
        assert(get<0>(copied).got_alloc);
        assert(&get<1>(copied) == &obj);

        tuple<T, T&> moved{allocator_arg, al, move(src)};
        assert(get<0>(moved).got_alloc);
        assert(&get<1>(moved) == &obj);
    }

    { // allocator-extended converting constructors from tuple<UTypes...>
        tuple<T&, T&> src{obj, obj};
        const auto& csrc = src;

        tuple<T, T&> from_lvalue{allocator_arg, al, src};
        assert(get<0>(from_lvalue).got_alloc);
        assert(&get<1>(from_lvalue) == &obj);

        tuple<T, T&> from_const_lvalue{allocator_arg, al, csrc};
        assert(get<0>(from_const_lvalue).got_alloc);
        assert(&get<1>(from_const_lvalue) == &obj);

        tuple<T, T&> from_const_rvalue{allocator_arg, al, move(csrc)};
        assert(get<0>(from_const_rvalue).got_alloc);
        assert(&get<1>(from_const_rvalue) == &obj);

        tuple<T, T&> from_rvalue{allocator_arg, al, move(src)};
        assert(get<0>(from_rvalue).got_alloc);
        assert(&get<1>(from_rvalue) == &obj);
    }

    { // allocator-extended converting constructors from pair<U1, U2>
        pair<T&, T&> src{obj, obj};
        const auto& csrc = src;

        tuple<T, T&> from_lvalue{allocator_arg, al, src};
        assert(get<0>(from_lvalue).got_alloc);
        assert(&get<1>(from_lvalue) == &obj);

        tuple<T, T&> from_const_lvalue{allocator_arg, al, csrc};
        assert(get<0>(from_const_lvalue).got_alloc);
        assert(&get<1>(from_const_lvalue) == &obj);

        tuple<T, T&> from_const_rvalue{allocator_arg, al, move(csrc)};
        assert(get<0>(from_const_rvalue).got_alloc);
        assert(&get<1>(from_const_rvalue) == &obj);

        tuple<T, T&> from_rvalue{allocator_arg, al, move(src)};
        assert(get<0>(from_rvalue).got_alloc);
        assert(&get<1>(from_rvalue) == &obj);
    }

#if _HAS_CXX20
    { // uses-allocator construction of the tuple itself, as performed by containers like pmr::vector
        auto made = make_obj_using_allocator<tuple<T, T&>>(al, obj, obj);
        assert(get<0>(made).got_alloc);
        assert(&get<1>(made) == &obj);
    }
#endif // _HAS_CXX20

#if _HAS_CXX23
    { // allocator-extended converting constructor from tuple-like types
        array<T, 2> src{};

        tuple<T, T&> from_tuple_like{allocator_arg, al, src};
        assert(get<0>(from_tuple_like).got_alloc);
        assert(&get<1>(from_tuple_like) == &src[1]);
    }
#endif // _HAS_CXX23

    assert(!obj.got_alloc); // the referenced object was never touched
    return true;
}

int main() {
    assert(test_tuple_of_references<LeadingAlloc>());
    assert(test_tuple_of_references<TrailingAlloc>());

#if _HAS_CXX20
    STATIC_ASSERT(test_tuple_of_references<LeadingAlloc>());
    STATIC_ASSERT(test_tuple_of_references<TrailingAlloc>());
#endif // _HAS_CXX20
}
