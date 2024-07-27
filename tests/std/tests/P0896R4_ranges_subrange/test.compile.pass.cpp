// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers ranges::view_interface and ranges::subrange

#include <cassert>
#include <forward_list>
#include <istream>
#include <list>
#include <ranges>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

#define ASSERT(...) assert((__VA_ARGS__))

using std::output_iterator_tag, std::input_iterator_tag, std::forward_iterator_tag, std::bidirectional_iterator_tag,
    std::random_access_iterator_tag, std::contiguous_iterator_tag;

void test_LWG_3470() {
    // LWG-3470 relaxed the "convertible-to-non-slicing" requirements to allow this non-slicing case
    int a[]                 = {1, 2, 3};
    int* b[]                = {&a[2], &a[0], &a[1]};
    [[maybe_unused]] auto c = std::ranges::subrange<const int* const*>(b);
}

struct empty {};

namespace test_view_interface {
    template <class T>
    concept CanViewInterface = requires { typename ranges::view_interface<T>; };

    template <class T>
    constexpr bool test_template_id() {
        // view_interface<T> is a valid template-id only if T is a cv-unqualified class type
        static_assert(std::is_same_v<T, std::remove_cvref_t<T>>);
        static_assert(CanViewInterface<T> == std::is_class_v<T>);
        if constexpr (!std::is_function_v<T>) {
            static_assert(!CanViewInterface<T const>);
            static_assert(!CanViewInterface<T volatile>);
            static_assert(!CanViewInterface<T const volatile>);
        }

        if constexpr (!std::is_void_v<T>) {
            static_assert(!CanViewInterface<T&>);
            static_assert(!CanViewInterface<T&&>);
            if constexpr (!std::is_function_v<T>) {
                static_assert(!CanViewInterface<T const&>);
                static_assert(!CanViewInterface<T volatile&>);
                static_assert(!CanViewInterface<T const volatile&>);
                static_assert(!CanViewInterface<T const&&>);
                static_assert(!CanViewInterface<T volatile&&>);
                static_assert(!CanViewInterface<T const volatile&&>);
            }
        }
        return true;
    }

    static_assert(test_template_id<int>());
    static_assert(test_template_id<void>());
    static_assert(test_template_id<int()>());
    static_assert(test_template_id<empty>());

    using test::CanCompare, test::CanDifference, test::Common, test::ProxyRef, test::to_bool;
    enum class ConstRange : bool { no, yes };

    template <class Cat, Common IsCommon, CanDifference Diff, ConstRange HasConstRange>
    struct fake_view : ranges::view_interface<fake_view<Cat, IsCommon, Diff, HasConstRange>> {
        using I = test::iterator<Cat, int, Diff, CanCompare::yes, ProxyRef::no>;
        using S = std::conditional_t<to_bool(IsCommon), I, test::sentinel<int>>;

        I begin();
        I begin() const
            requires (to_bool(HasConstRange));

        S end();
        S end() const
            requires (to_bool(HasConstRange));

        unsigned int size()
            requires (to_bool(Diff) && !std::derived_from<Cat, forward_iterator_tag>);
        unsigned int size() const
            requires (to_bool(HasConstRange) && to_bool(Diff) && !std::derived_from<Cat, forward_iterator_tag>);
    };

    namespace output_unsized_onlymutable {
        using V = fake_view<output_iterator_tag, Common::no, CanDifference::no, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(!CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(!CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(!CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(!CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(!CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace output_unsized_onlymutable

    namespace output_unsized_allowconst {
        using V = fake_view<output_iterator_tag, Common::no, CanDifference::no, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(!CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(!CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(!CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(!CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(!CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace output_unsized_allowconst

    namespace output_sized_onlymutable {
        using V = fake_view<output_iterator_tag, Common::no, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(!CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(!CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(!CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace output_sized_onlymutable

    namespace output_sized_allowconst {
        using V = fake_view<output_iterator_tag, Common::no, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(!CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(!CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(!CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace output_sized_allowconst

    namespace input_unsized_onlymutable {
        using V = fake_view<input_iterator_tag, Common::no, CanDifference::no, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(!CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(!CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(!CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace input_unsized_onlymutable

    namespace input_unsized_allowconst {
        using V = fake_view<input_iterator_tag, Common::no, CanDifference::no, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(!CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(!CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(!CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace input_unsized_allowconst

    namespace input_sized_onlymutable {
        using V = fake_view<input_iterator_tag, Common::no, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(!CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace input_sized_onlymutable

    namespace input_sized_allowconst {
        using V = fake_view<input_iterator_tag, Common::no, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(!CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace input_sized_allowconst

    namespace forward_uncommon_unsized_onlymutable {
        using V = fake_view<forward_iterator_tag, Common::no, CanDifference::no, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace forward_uncommon_unsized_onlymutable

    namespace forward_uncommon_unsized_allowconst {
        using V = fake_view<forward_iterator_tag, Common::no, CanDifference::no, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace forward_uncommon_unsized_allowconst

    namespace forward_uncommon_sized_onlymutable {
        using V = fake_view<forward_iterator_tag, Common::no, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace forward_uncommon_sized_onlymutable

    namespace forward_uncommon_sized_allowconst {
        using V = fake_view<forward_iterator_tag, Common::no, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace forward_uncommon_sized_allowconst

    namespace forward_common_unsized_onlymutable {
        using V = fake_view<forward_iterator_tag, Common::yes, CanDifference::no, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace forward_common_unsized_onlymutable

    namespace forward_common_unsized_allowconst {
        using V = fake_view<forward_iterator_tag, Common::yes, CanDifference::no, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace forward_common_unsized_allowconst

    namespace forward_common_sized_onlymutable {
        using V = fake_view<forward_iterator_tag, Common::yes, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace forward_common_sized_onlymutable

    namespace forward_common_sized_allowconst {
        using V = fake_view<forward_iterator_tag, Common::yes, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace forward_common_sized_allowconst

    namespace bidi_uncommon_unsized_onlymutable {
        using V = fake_view<bidirectional_iterator_tag, Common::no, CanDifference::no, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace bidi_uncommon_unsized_onlymutable

    namespace bidi_uncommon_unsized_allowconst {
        using V = fake_view<bidirectional_iterator_tag, Common::no, CanDifference::no, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace bidi_uncommon_unsized_allowconst

    namespace bidi_uncommon_sized_onlymutable {
        using V = fake_view<bidirectional_iterator_tag, Common::no, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace bidi_uncommon_sized_onlymutable

    namespace bidi_uncommon_sized_allowconst {
        using V = fake_view<bidirectional_iterator_tag, Common::no, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace bidi_uncommon_sized_allowconst

    namespace bidi_common_unsized_onlymutable {
        using V = fake_view<bidirectional_iterator_tag, Common::yes, CanDifference::no, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace bidi_common_unsized_onlymutable

    namespace bidi_common_unsized_allowconst {
        using V = fake_view<bidirectional_iterator_tag, Common::yes, CanDifference::no, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(!CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(CanMemberBack<V&>);
        static_assert(CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace bidi_common_unsized_allowconst

    namespace bidi_common_sized_onlymutable {
        using V = fake_view<bidirectional_iterator_tag, Common::yes, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace bidi_common_sized_onlymutable

    namespace bidi_common_sized_allowconst {
        using V = fake_view<bidirectional_iterator_tag, Common::yes, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(CanMemberBack<V&>);
        static_assert(CanMemberBack<V const&>);
        static_assert(!CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace bidi_common_sized_allowconst

    namespace random_uncommon_sized_onlymutable {
        using V = fake_view<random_access_iterator_tag, Common::no, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace random_uncommon_sized_onlymutable

    namespace random_uncommon_sized_allowconst {
        using V = fake_view<random_access_iterator_tag, Common::no, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(CanIndex<V&>);
        static_assert(CanIndex<V const&>);
    } // namespace random_uncommon_sized_allowconst

    namespace random_common_sized_onlymutable {
        using V = fake_view<random_access_iterator_tag, Common::yes, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace random_common_sized_onlymutable

    namespace random_common_sized_allowconst {
        using V = fake_view<random_access_iterator_tag, Common::yes, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(!CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(CanMemberBack<V&>);
        static_assert(CanMemberBack<V const&>);
        static_assert(CanIndex<V&>);
        static_assert(CanIndex<V const&>);
    } // namespace random_common_sized_allowconst

    namespace contiguous_uncommon_sized_onlymutable {
        using V = fake_view<contiguous_iterator_tag, Common::no, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace contiguous_uncommon_sized_onlymutable

    namespace contiguous_uncommon_sized_allowconst {
        using V = fake_view<contiguous_iterator_tag, Common::no, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(CanData<V&>);
        static_assert(CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(!CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(CanIndex<V&>);
        static_assert(CanIndex<V const&>);
    } // namespace contiguous_uncommon_sized_allowconst

    namespace contiguous_common_sized_onlymutable {
        using V = fake_view<contiguous_iterator_tag, Common::yes, CanDifference::yes, ConstRange::no>;
        static_assert(ranges::range<V>);
        static_assert(!ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(!CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(!CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(!CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(!CanBool<V const&>);
        static_assert(CanData<V&>);
        static_assert(!CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(!CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(!CanMemberFront<V const&>);
        static_assert(CanMemberBack<V&>);
        static_assert(!CanMemberBack<V const&>);
        static_assert(CanIndex<V&>);
        static_assert(!CanIndex<V const&>);
    } // namespace contiguous_common_sized_onlymutable

    namespace contiguous_common_sized_allowconst {
        using V = fake_view<contiguous_iterator_tag, Common::yes, CanDifference::yes, ConstRange::yes>;
        static_assert(ranges::range<V>);
        static_assert(ranges::range<V const>);
        static_assert(ranges::view<V>);
        static_assert(CanEmpty<V&>);
        static_assert(CanEmpty<V const&>);
#if _HAS_CXX23
        static_assert(CanMemberCBegin<V&>);
        static_assert(CanMemberCBegin<V const&>);
        static_assert(CanMemberCEnd<V&>);
        static_assert(CanMemberCEnd<V const&>);
#endif // _HAS_CXX23
        static_assert(CanBool<V&>);
        static_assert(CanBool<V const&>);
        static_assert(CanData<V&>);
        static_assert(CanData<V const&>);
        static_assert(CanSize<V&>);
        static_assert(CanSize<V const&>);
        static_assert(CanMemberFront<V&>);
        static_assert(CanMemberFront<V const&>);
        static_assert(CanMemberBack<V&>);
        static_assert(CanMemberBack<V const&>);
        static_assert(CanIndex<V&>);
        static_assert(CanIndex<V const&>);
    } // namespace contiguous_common_sized_allowconst
} // namespace test_view_interface

namespace test_subrange {
    using ranges::borrowed_range, ranges::range, ranges::sized_range, ranges::subrange, ranges::subrange_kind;
    using std::constructible_from, std::copyable, std::default_initializable, std::movable, std::same_as,
        std::sized_sentinel_for;

    // * template-id: subrange<I, S, K> is a valid template-id iff I models input_or_output_iterator, S models
    // sentinel_for<I>, and sized_sentinel_for<S, I> implies K == sized.
    template <class I, class S, ranges::subrange_kind K>
    concept CanSubrange = requires { typename subrange<I, S, K>; };
    static_assert(CanSubrange<int*, int*, subrange_kind::sized>);
    static_assert(!CanSubrange<int*, int*, subrange_kind::unsized>);
    static_assert(!CanSubrange<int*, void, subrange_kind::unsized>);
    static_assert(!CanSubrange<void, int*, subrange_kind::unsized>);
    static_assert(CanSubrange<int*, std::unreachable_sentinel_t, subrange_kind::unsized>);
    static_assert(!CanSubrange<std::unreachable_sentinel_t, int*, subrange_kind::unsized>);
    static_assert(CanSubrange<int*, std::unreachable_sentinel_t, subrange_kind::sized>);

    template <class R>
    concept HasMemberEmpty = requires(std::remove_reference_t<R> const r) {
        { r.empty() } -> same_as<bool>;
    };

    template <class R>
    concept HasMemberSize = requires(std::remove_reference_t<R> const r) {
        { r.size() } -> std::integral;
    };

    // Validate default template arguments: second defaults to first, and third defaults to subrange_kind::sized iff
    // sized_sentinel_for<second, first>.
    static_assert(same_as<subrange<int*>, subrange<int*, int*, subrange_kind::sized>>);
    static_assert(same_as<subrange<int*, std::unreachable_sentinel_t>,
        subrange<int*, std::unreachable_sentinel_t, subrange_kind::unsized>>);
    static_assert(same_as<subrange<std::forward_list<int>::iterator>,
        subrange<std::forward_list<int>::iterator, std::forward_list<int>::iterator, subrange_kind::unsized>>);

    // Validate many properties of a specialization of subrange
    template <class>
    constexpr bool is_subrange = false;
    template <class I, class S, subrange_kind K>
    constexpr bool is_subrange<subrange<I, S, K>> = true;

    template <class T>
    struct illformed {
        static_assert(false);
    };

    template <class T>
    constexpr auto kind_of = illformed<T>{};
    template <class I, class S, subrange_kind K>
    constexpr auto kind_of<subrange<I, S, K>> = K;

    template <class Subrange, class Rng>
    constexpr bool test_subrange() {
        static_assert(same_as<Subrange, std::remove_cvref_t<Subrange>>);
        static_assert(is_subrange<Subrange>);

        using I              = ranges::iterator_t<Subrange>;
        using S              = ranges::sentinel_t<Subrange>;
        constexpr bool sized = kind_of<Subrange> == subrange_kind::sized;
        static_assert(
            std::integral<std::iter_difference_t<I>>, "make_unsigned_t below needs to be make-unsigned-like-t");
        using size_type = std::make_unsigned_t<std::iter_difference_t<I>>;

        // Validate SMFs
        static_assert(default_initializable<Subrange> == default_initializable<I>);
        static_assert(movable<Subrange>);
        static_assert(!copyable<I> || copyable<Subrange>);

        static_assert(constructible_from<Subrange, I, S> == (!sized || sized_sentinel_for<S, I>) );
        static_assert(
            constructible_from<Subrange, I const&, S const&> == (copyable<I> && (!sized || sized_sentinel_for<S, I>) ));
        static_assert(constructible_from<Subrange, I, S, size_type> == sized);
        static_assert(constructible_from<Subrange, I const&, S const&, size_type> == (copyable<I> && sized));

        static_assert(constructible_from<Subrange, Rng&> == (!sized || sized_range<Rng> || sized_sentinel_for<S, I>) );
        static_assert(constructible_from<Subrange, Rng&, size_type> == sized);
        static_assert(constructible_from<Subrange, Rng>
                      == (borrowed_range<Rng> && (!sized || sized_range<Rng> || sized_sentinel_for<S, I>) ));
        static_assert(constructible_from<Subrange, Rng, size_type> == (sized && borrowed_range<Rng>) );

        // Validate begin/end/empty
        static_assert(range<Subrange>);
        static_assert(HasMemberEmpty<Subrange const>);
        static_assert(!copyable<I> || range<Subrange const&>);

#if _HAS_CXX23 // Validate cbegin/cend
        if constexpr (ranges::input_range<Subrange>) {
            static_assert(CanMemberCBegin<Subrange>);
            static_assert(CanMemberCBegin<const Subrange> == ranges::input_range<const Subrange&>);
            static_assert(CanMemberCEnd<Subrange>);
            static_assert(CanMemberCEnd<const Subrange> == ranges::input_range<const Subrange&>);
        }
#endif // _HAS_CXX23

        // Validate size
        static_assert(sized == HasMemberSize<Subrange>);

        return true;
    }

    template <class Rng>
    constexpr bool test_construction() {
        static_assert(same_as<Rng, std::remove_cvref_t<Rng>>);

        using I = ranges::iterator_t<Rng>;
        using S = ranges::sentinel_t<Rng>;

        static_assert(test_subrange<subrange<I, S, subrange_kind::sized>, Rng>());
        if constexpr (!sized_sentinel_for<S, I>) {
            static_assert(test_subrange<subrange<I, S, subrange_kind::unsized>, Rng>());
        }

        return true;
    }

    using test::CanCompare, test::CanDifference, test::Common, test::ProxyRef, test::Sized;

    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no,
            Common::no, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no,
            Common::no, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no,
            Common::yes, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no,
            Common::yes, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no,
            Common::no, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no,
            Common::no, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no,
            Common::yes, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no,
            Common::yes, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_construction<test::range<random_access_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<random_access_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<random_access_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<random_access_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_construction<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_construction<test::range<contiguous_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<contiguous_iterator_tag, int, Sized::no, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<contiguous_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::no, CanCompare::yes, ProxyRef::no>>());
    static_assert(test_construction<test::range<contiguous_iterator_tag, int, Sized::yes, CanDifference::yes,
            Common::yes, CanCompare::yes, ProxyRef::no>>());

    static_assert(test_construction<std::forward_list<int>>());
    static_assert(test_construction<std::list<int>>());
    static_assert(test_construction<std::vector<int>>());
    static_assert(test_construction<std::string_view>());

    // Validate that slicing conversions are forbidden
    struct Base {};
    struct Derived : Base {};
    static_assert(!std::constructible_from<subrange<Base*>, Derived*, Derived*>);
    static_assert(!std::constructible_from<subrange<Base*>, Derived*, Derived*, std::make_unsigned_t<std::ptrdiff_t>>);
    static_assert(!std::constructible_from<subrange<Base*>, subrange<Derived*>>);
    static_assert(!std::constructible_from<subrange<Base*>, subrange<Derived*>, std::make_unsigned_t<std::ptrdiff_t>>);

    struct with_converting_iterators {
        template <bool IsConst>
        struct iterator {
            using iterator_concept = input_iterator_tag;
            using value_type       = int;
            using difference_type  = int;
            using pointer          = void;
            using reference        = int;

            iterator() = default;
            iterator(iterator<!IsConst>)
                requires IsConst;

            iterator(iterator&&)            = default;
            iterator& operator=(iterator&&) = default;

            int operator*() const;
            iterator& operator++();
            void operator++(int);
        };

        template <bool IsConst>
        struct sentinel {
            sentinel() = default;
            sentinel(sentinel<!IsConst>)
                requires IsConst;

            bool operator==(iterator<IsConst> const&) const;
        };

        iterator<false> begin();
        sentinel<false> end();
        iterator<true> begin() const;
        sentinel<true> end() const;
    };

    void test_non_slicing_conversions() {
        // and non-slicing conversions are ok
        using LI = std::list<int>::iterator;
        static_assert(std::constructible_from<subrange<std::list<int>::const_iterator>, LI const&, LI const&>);
        static_assert(std::constructible_from<subrange<std::list<int>::const_iterator>, LI, LI>);

        using I  = ranges::iterator_t<with_converting_iterators>;
        using S  = ranges::sentinel_t<with_converting_iterators>;
        using CI = ranges::iterator_t<with_converting_iterators const>;
        using CS = ranges::sentinel_t<with_converting_iterators const>;

        using SizedSubrange = subrange<CI, CS, subrange_kind::sized>;
        static_assert(test_subrange<SizedSubrange, with_converting_iterators>());

        static_assert(!constructible_from<SizedSubrange, I&, S&>); // lvalues are not convertible
        static_assert(!constructible_from<SizedSubrange, I&, S&, unsigned int>); // ditto
        static_assert(!constructible_from<SizedSubrange, I, S>); // missing size
        static_assert(constructible_from<SizedSubrange, I, S, unsigned int>);

        using UnsizedSubrange = subrange<CI, CS, subrange_kind::unsized>;
        static_assert(test_subrange<UnsizedSubrange, with_converting_iterators>());

        static_assert(!constructible_from<UnsizedSubrange, I&, S&>); // lvalues are not convertible
        static_assert(!constructible_from<UnsizedSubrange, I&, S&, unsigned int>); // ditto
        static_assert(constructible_from<UnsizedSubrange, I, S>); // but rvalues are
        static_assert(!constructible_from<UnsizedSubrange, I, S, unsigned int>); // !sized
    }

    // Validate deduction guides
    template <class Rng>
    constexpr bool test_ctad() {
        using I             = ranges::iterator_t<Rng>;
        using S             = ranges::sentinel_t<Rng>;
        constexpr bool diff = sized_sentinel_for<S, I>;

        {
            using T = decltype(subrange(std::declval<I>(), std::declval<S>()));
            static_assert(same_as<T, subrange<I, S, subrange_kind{diff}>>);

            static_assert(range<T>);
            static_assert(!copyable<I> || range<T const&>);
        }

        static_assert(
            std::integral<std::iter_difference_t<I>>, "make_unsigned_t below needs to be make-unsigned-like-t");
        using size_type = std::make_unsigned_t<std::iter_difference_t<I>>;
        {
            using T = decltype(subrange(std::declval<I>(), std::declval<S>(), std::declval<size_type>()));
            static_assert(same_as<T, subrange<I, S, subrange_kind::sized>>);

            static_assert(range<T>);
            static_assert(!copyable<I> || range<T const&>);
        }

        constexpr bool is_sized = diff | sized_range<Rng>;
        {
            using T = decltype(subrange(std::declval<Rng&>()));
            static_assert(same_as<T, subrange<I, S, subrange_kind{is_sized}>>);

            static_assert(range<T>);
            static_assert(!copyable<I> || range<T const&>);

            if constexpr (borrowed_range<Rng>) {
                using U = decltype(subrange(std::declval<Rng>()));
                static_assert(same_as<U, T>);
            }
        }

        {
            using T = decltype(subrange(std::declval<Rng&>(), std::declval<size_type>()));
            static_assert(same_as<T, subrange<I, S, subrange_kind::sized>>);

            static_assert(range<T>);
            static_assert(!copyable<I> || range<T const&>);

            if constexpr (borrowed_range<Rng>) {
                using U = decltype(subrange(std::declval<Rng>(), std::declval<size_type>()));
                static_assert(same_as<U, T>);
            }
        }

        return true;
    }

    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::no, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::yes>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::yes>>());

    static_assert(test_ctad<test::range<contiguous_iterator_tag, int, Sized::no, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<contiguous_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<contiguous_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no,
            CanCompare::yes, ProxyRef::no>>());
    static_assert(test_ctad<test::range<contiguous_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes,
            CanCompare::yes, ProxyRef::no>>());

    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::no, CanDifference::no>>());
    static_assert(test_ctad<test::range<output_iterator_tag, int, Sized::yes, CanDifference::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::no, CanDifference::no>>());
    static_assert(test_ctad<test::range<input_iterator_tag, int, Sized::yes, CanDifference::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::no, CanDifference::yes>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::no>>());
    static_assert(test_ctad<test::range<forward_iterator_tag, int, Sized::yes, CanDifference::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no>>());
    static_assert(test_ctad<test::range<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::no>>());
    static_assert(test_ctad<test::range<random_access_iterator_tag, int, Sized::yes, CanDifference::yes>>());
    static_assert(test_ctad<test::range<contiguous_iterator_tag, int, Sized::yes, CanDifference::no>>());
    static_assert(test_ctad<test::range<contiguous_iterator_tag, int, Sized::yes, CanDifference::yes>>());

    static_assert(test_ctad<std::forward_list<int>>());
    static_assert(test_ctad<std::list<int>>());
    static_assert(test_ctad<std::vector<int>>());
    static_assert(test_ctad<std::string_view>());

    // Validate conversion to PairLike
    static_assert(std::is_convertible_v<subrange<int*>, std::pair<int*, int*>>);
    static_assert(!std::is_convertible_v<subrange<int const*>, std::pair<int*, int*>>);
    static_assert(std::is_convertible_v<subrange<int*>, std::pair<int const*, int const*>>);
    static_assert(std::is_convertible_v<subrange<int const*>, std::pair<int const*, int const*>>);
    static_assert(std::is_convertible_v<subrange<int*>, std::tuple<int*, int*>>);
    static_assert(!std::is_convertible_v<subrange<int const*>, std::tuple<int*, int*>>);
    static_assert(std::is_convertible_v<subrange<int*>, std::tuple<int const*, int const*>>);
    static_assert(std::is_convertible_v<subrange<int const*>, std::tuple<int const*, int const*>>);

    constexpr bool test_advance_next_prev() {
        // Validate advance/next/prev
        int const some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        auto const first      = ranges::begin(some_ints);
        auto const last       = ranges::end(some_ints);

        ASSERT(subrange{some_ints}.begin() == first);
        ASSERT(subrange{some_ints}.end() == last);

        ASSERT(subrange{some_ints}.next().begin() == first + 1);
        ASSERT(subrange{some_ints}.next().end() == last);

        ASSERT(subrange{first + 1, last}.prev().begin() == first);
        ASSERT(subrange{first + 1, last}.prev().end() == last);

        for (std::ptrdiff_t i = 0; i <= last - first; ++i) {
            ASSERT(subrange{some_ints}.next(i).begin() == first + i);
            ASSERT(subrange{some_ints}.next(i).end() == last);

            subrange s0{some_ints};
            s0.advance(i);
            ASSERT(s0.begin() == first + i);
            ASSERT(s0.end() == last);

            ASSERT(subrange{first + i, last}.prev(i).begin() == first);
            ASSERT(subrange{first + i, last}.prev(i).end() == last);

            subrange s1{first + i, last};
            s1.advance(-i);
            ASSERT(s1.begin() == first);
            ASSERT(s1.end() == last);
        }

        return true;
    }
    static_assert(test_advance_next_prev());

    // Validate tuple interface
    template <class Subrange>
    constexpr bool test_tuple() {
        using std::declval, std::tuple_element_t, std::tuple_size_v;

        using I = ranges::iterator_t<Subrange>;
        using S = ranges::sentinel_t<Subrange>;

        static_assert(tuple_size_v<Subrange> == 2);
        static_assert(tuple_size_v<Subrange const> == 2);

        static_assert(same_as<tuple_element_t<0, Subrange>, I>);
        static_assert(same_as<tuple_element_t<0, Subrange const>, I>);
        static_assert(same_as<tuple_element_t<1, Subrange>, S>);
        static_assert(same_as<tuple_element_t<1, Subrange const>, S>);

        static_assert(same_as<decltype(ranges::get<0>(declval<Subrange>())), I>);
        static_assert(same_as<decltype(ranges::get<0>(declval<Subrange&>())), I>);
        static_assert(same_as<decltype(ranges::get<0>(declval<Subrange const>())), I>);
        static_assert(same_as<decltype(ranges::get<0>(declval<Subrange const&>())), I>);
        static_assert(same_as<decltype(std::get<0>(declval<Subrange>())), I>);
        static_assert(same_as<decltype(std::get<0>(declval<Subrange&>())), I>);
        static_assert(same_as<decltype(std::get<0>(declval<Subrange const>())), I>);
        static_assert(same_as<decltype(std::get<0>(declval<Subrange const&>())), I>);

        static_assert(same_as<decltype(ranges::get<1>(declval<Subrange>())), S>);
        static_assert(same_as<decltype(ranges::get<1>(declval<Subrange&>())), S>);
        static_assert(same_as<decltype(ranges::get<1>(declval<Subrange const>())), S>);
        static_assert(same_as<decltype(ranges::get<1>(declval<Subrange const&>())), S>);
        static_assert(same_as<decltype(std::get<1>(declval<Subrange>())), S>);
        static_assert(same_as<decltype(std::get<1>(declval<Subrange&>())), S>);
        static_assert(same_as<decltype(std::get<1>(declval<Subrange const>())), S>);
        static_assert(same_as<decltype(std::get<1>(declval<Subrange const&>())), S>);

        return true;
    }
    static_assert(test_tuple<subrange<int*>>());
    static_assert(test_tuple<subrange<int*, std::unreachable_sentinel_t, subrange_kind::sized>>());
    static_assert(test_tuple<subrange<int*, std::unreachable_sentinel_t, subrange_kind::unsized>>());
} // namespace test_subrange

namespace test_lwg_3589 {
    // LWG-3589 added a Constraint to std::get<0>(const subrange&) to require the iterator type to be copyable
    template <class T, size_t I>
    concept CanGet = requires { std::get<I>(std::declval<T>()); };

    template <class T, size_t I>
    concept CanRangesGet = requires { ranges::get<I>(std::declval<T>()); };

    template <class I, class S>
    constexpr bool test() {
        using ranges::subrange;

        static_assert(std::input_iterator<I>);
        static_assert(std::sentinel_for<S, I>);

        static_assert(CanGet<const subrange<I, S>&, 0> == std::copyable<I>);
        static_assert(CanGet<const subrange<I, S>&, 1>);
        static_assert(!CanGet<const subrange<I, S>&, 2>);
        static_assert(CanGet<subrange<I, S>, 0>);
        static_assert(CanGet<subrange<I, S>, 1>);
        static_assert(!CanGet<subrange<I, S>, 2>);

        static_assert(CanRangesGet<const subrange<I, S>&, 0> == std::copyable<I>);
        static_assert(CanRangesGet<const subrange<I, S>&, 1>);
        static_assert(!CanRangesGet<const subrange<I, S>&, 2>);
        static_assert(CanRangesGet<subrange<I, S>, 0>);
        static_assert(CanRangesGet<subrange<I, S>, 1>);
        static_assert(!CanRangesGet<subrange<I, S>, 2>);

        return true;
    }

    // Validate with a copyable iterator type, and with a move-only iterator type
    static_assert(test<int*, int*>());
    static_assert(test<ranges::iterator_t<ranges::istream_view<int>>, ranges::sentinel_t<ranges::istream_view<int>>>());
} // namespace test_lwg_3589
