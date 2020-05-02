// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers ranges::view_interface and ranges::subrange

#include <cassert>
#include <concepts>
#include <forward_list>
#include <list>
#include <ranges>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
//
#include "range_algorithm_support.hpp"

#define ASSERT(...) assert((__VA_ARGS__))

using std::output_iterator_tag, std::input_iterator_tag, std::forward_iterator_tag, std::bidirectional_iterator_tag,
    std::random_access_iterator_tag, std::contiguous_iterator_tag;

int main() {} // COMPILE-ONLY

struct empty {};

namespace test_view_interface {
    template <class T>
    concept CanViewInterface = requires {
        typename ranges::view_interface<T>;
    };

    template <class T>
    constexpr bool test_template_id() {
        // view_interface<T> is a valid template-id only if T is a cv-unqualified class type
        STATIC_ASSERT(std::is_same_v<T, std::remove_cvref_t<T>>);
        STATIC_ASSERT(CanViewInterface<T> == std::is_class_v<T>);
        if constexpr (!std::is_function_v<T>) {
            STATIC_ASSERT(!CanViewInterface<T const>);
            STATIC_ASSERT(!CanViewInterface<T volatile>);
            STATIC_ASSERT(!CanViewInterface<T const volatile>);
        }

        if constexpr (!std::is_void_v<T>) {
            STATIC_ASSERT(!CanViewInterface<T&>);
            STATIC_ASSERT(!CanViewInterface<T&&>);
            if constexpr (!std::is_function_v<T>) {
                STATIC_ASSERT(!CanViewInterface<T const&>);
                STATIC_ASSERT(!CanViewInterface<T volatile&>);
                STATIC_ASSERT(!CanViewInterface<T const volatile&>);
                STATIC_ASSERT(!CanViewInterface<T const&&>);
                STATIC_ASSERT(!CanViewInterface<T volatile&&>);
                STATIC_ASSERT(!CanViewInterface<T const volatile&&>);
            }
        }
        return true;
    }

    STATIC_ASSERT(test_template_id<int>());
    STATIC_ASSERT(test_template_id<void>());
    STATIC_ASSERT(test_template_id<int()>());
    STATIC_ASSERT(test_template_id<empty>());

    template <class T>
    concept CanEmpty = requires(T& t) {
        ranges::empty(t);
    };
    template <class T>
    concept CanBool = requires(T& t) {
        static_cast<bool>(t);
    };
    template <class T>
    concept CanData = requires(T& t) {
        ranges::data(t);
    };
    template <class T>
    concept CanSize = requires(T& t) {
        ranges::size(t);
    };
    template <class T>
    concept CanFront = requires(T& t) {
        t.front();
    };
    template <class T>
    concept CanBack = requires(T& t) {
        t.back();
    };
    template <class T>
    concept CanIndex = requires(T& t) {
        t[42];
    };

    template <class Cat, bool Common, bool SizedSentinel, bool ConstRange>
    struct fake_view : ranges::view_interface<fake_view<Cat, Common, SizedSentinel, ConstRange>> {
        using iterator = test_iterator<Cat, int, SizedSentinel>;
        using sentinel = std::conditional_t<Common, iterator, std::default_sentinel_t>;

        iterator begin();
        iterator begin() const requires ConstRange;

        sentinel end();
        sentinel end() const requires ConstRange;
    };

    namespace output_unsized_onlymutable {
        using V = fake_view<output_iterator_tag, false, false, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(!CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(!CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(!CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace output_unsized_onlymutable

    namespace output_unsized_allowconst {
        using V = fake_view<output_iterator_tag, false, false, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(!CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(!CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(!CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace output_unsized_allowconst

    namespace output_sized_onlymutable {
        using V = fake_view<output_iterator_tag, false, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(!CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(!CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(!CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace output_sized_onlymutable

    namespace output_sized_allowconst {
        using V = fake_view<output_iterator_tag, false, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(!CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(!CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(!CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace output_sized_allowconst

    namespace input_unsized_onlymutable {
        using V = fake_view<input_iterator_tag, false, false, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(!CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(!CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(!CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace input_unsized_onlymutable

    namespace input_unsized_allowconst {
        using V = fake_view<input_iterator_tag, false, false, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(!CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(!CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(!CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace input_unsized_allowconst

    namespace input_sized_onlymutable {
        using V = fake_view<input_iterator_tag, false, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(!CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(!CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(!CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace input_sized_onlymutable

    namespace input_sized_allowconst {
        using V = fake_view<input_iterator_tag, false, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(!CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(!CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(!CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace input_sized_allowconst

    namespace forward_uncommon_unsized_onlymutable {
        using V = fake_view<forward_iterator_tag, false, false, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace forward_uncommon_unsized_onlymutable

    namespace forward_uncommon_unsized_allowconst {
        using V = fake_view<forward_iterator_tag, false, false, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace forward_uncommon_unsized_allowconst

    namespace forward_uncommon_sized_onlymutable {
        using V = fake_view<forward_iterator_tag, false, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace forward_uncommon_sized_onlymutable

    namespace forward_uncommon_sized_allowconst {
        using V = fake_view<forward_iterator_tag, false, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace forward_uncommon_sized_allowconst

    namespace forward_common_unsized_onlymutable {
        using V = fake_view<forward_iterator_tag, true, false, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace forward_common_unsized_onlymutable

    namespace forward_common_unsized_allowconst {
        using V = fake_view<forward_iterator_tag, true, false, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace forward_common_unsized_allowconst

    namespace forward_common_sized_onlymutable {
        using V = fake_view<forward_iterator_tag, true, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace forward_common_sized_onlymutable

    namespace forward_common_sized_allowconst {
        using V = fake_view<forward_iterator_tag, true, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace forward_common_sized_allowconst

    namespace bidi_uncommon_unsized_onlymutable {
        using V = fake_view<bidirectional_iterator_tag, false, false, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace bidi_uncommon_unsized_onlymutable

    namespace bidi_uncommon_unsized_allowconst {
        using V = fake_view<bidirectional_iterator_tag, false, false, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace bidi_uncommon_unsized_allowconst

    namespace bidi_uncommon_sized_onlymutable {
        using V = fake_view<bidirectional_iterator_tag, false, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace bidi_uncommon_sized_onlymutable

    namespace bidi_uncommon_sized_allowconst {
        using V = fake_view<bidirectional_iterator_tag, false, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace bidi_uncommon_sized_allowconst

    namespace bidi_common_unsized_onlymutable {
        using V = fake_view<bidirectional_iterator_tag, true, false, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace bidi_common_unsized_onlymutable

    namespace bidi_common_unsized_allowconst {
        using V = fake_view<bidirectional_iterator_tag, true, false, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(CanBack<V>);
        STATIC_ASSERT(CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace bidi_common_unsized_allowconst

    namespace bidi_common_sized_onlymutable {
        using V = fake_view<bidirectional_iterator_tag, true, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace bidi_common_sized_onlymutable

    namespace bidi_common_sized_allowconst {
        using V = fake_view<bidirectional_iterator_tag, true, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(CanBack<V>);
        STATIC_ASSERT(CanBack<V const>);
        STATIC_ASSERT(!CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace bidi_common_sized_allowconst

    namespace random_uncommon_unsized_onlymutable {
        using V = fake_view<random_access_iterator_tag, false, false, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace random_uncommon_unsized_onlymutable

    namespace random_uncommon_unsized_allowconst {
        using V = fake_view<random_access_iterator_tag, false, false, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(CanIndex<V const>);
    } // namespace random_uncommon_unsized_allowconst

    namespace random_uncommon_sized_onlymutable {
        using V = fake_view<random_access_iterator_tag, false, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace random_uncommon_sized_onlymutable

    namespace random_uncommon_sized_allowconst {
        using V = fake_view<random_access_iterator_tag, false, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(CanIndex<V const>);
    } // namespace random_uncommon_sized_allowconst

    namespace random_common_sized_onlymutable {
        using V = fake_view<random_access_iterator_tag, true, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace random_common_sized_onlymutable

    namespace random_common_sized_allowconst {
        using V = fake_view<random_access_iterator_tag, true, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(!CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(CanBack<V>);
        STATIC_ASSERT(CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(CanIndex<V const>);
    } // namespace random_common_sized_allowconst

    namespace contiguous_uncommon_unsized_onlymutable {
        using V = fake_view<contiguous_iterator_tag, false, false, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace contiguous_uncommon_unsized_onlymutable

    namespace contiguous_uncommon_unsized_allowconst {
        using V = fake_view<contiguous_iterator_tag, false, false, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(CanData<V>);
        STATIC_ASSERT(CanData<V const>);
        STATIC_ASSERT(!CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(CanIndex<V const>);
    } // namespace contiguous_uncommon_unsized_allowconst

    namespace contiguous_uncommon_sized_onlymutable {
        using V = fake_view<contiguous_iterator_tag, false, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace contiguous_uncommon_sized_onlymutable

    namespace contiguous_uncommon_sized_allowconst {
        using V = fake_view<contiguous_iterator_tag, false, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(CanData<V>);
        STATIC_ASSERT(CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(!CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(CanIndex<V const>);
    } // namespace contiguous_uncommon_sized_allowconst

    namespace contiguous_common_sized_onlymutable {
        using V = fake_view<contiguous_iterator_tag, true, true, false>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(!ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(!CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(!CanBool<V const>);
        STATIC_ASSERT(CanData<V>);
        STATIC_ASSERT(!CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(!CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(!CanFront<V const>);
        STATIC_ASSERT(CanBack<V>);
        STATIC_ASSERT(!CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(!CanIndex<V const>);
    } // namespace contiguous_common_sized_onlymutable

    namespace contiguous_common_sized_allowconst {
        using V = fake_view<contiguous_iterator_tag, true, true, true>;
        STATIC_ASSERT(ranges::range<V>);
        STATIC_ASSERT(ranges::range<V const>);
        STATIC_ASSERT(ranges::view<V>);
        STATIC_ASSERT(CanEmpty<V>);
        STATIC_ASSERT(CanEmpty<V const>);
        STATIC_ASSERT(CanBool<V>);
        STATIC_ASSERT(CanBool<V const>);
        STATIC_ASSERT(CanData<V>);
        STATIC_ASSERT(CanData<V const>);
        STATIC_ASSERT(CanSize<V>);
        STATIC_ASSERT(CanSize<V const>);
        STATIC_ASSERT(CanFront<V>);
        STATIC_ASSERT(CanFront<V const>);
        STATIC_ASSERT(CanBack<V>);
        STATIC_ASSERT(CanBack<V const>);
        STATIC_ASSERT(CanIndex<V>);
        STATIC_ASSERT(CanIndex<V const>);
    } // namespace contiguous_common_sized_allowconst
} // namespace test_view_interface

namespace test_subrange {
    using ranges::borrowed_range, ranges::range, ranges::sized_range, ranges::subrange, ranges::subrange_kind;
    using std::constructible_from, std::copyable, std::default_initializable, std::movable, std::same_as,
        std::sized_sentinel_for;

    // * template-id: subrange<I, S, K> is a valid template-id iff I models input_or_output_iterator, S models
    // sentinel_for<I>, and sized_sentinel_for<S, I> implies K == sized.
    template <class I, class S, ranges::subrange_kind K>
    concept CanSubrange = requires {
        typename subrange<I, S, K>;
    };
    STATIC_ASSERT(CanSubrange<int*, int*, subrange_kind::sized>);
    STATIC_ASSERT(!CanSubrange<int*, int*, subrange_kind::unsized>);
    STATIC_ASSERT(!CanSubrange<int*, void, subrange_kind::unsized>);
    STATIC_ASSERT(!CanSubrange<void, int*, subrange_kind::unsized>);
    STATIC_ASSERT(CanSubrange<int*, std::unreachable_sentinel_t, subrange_kind::unsized>);
    STATIC_ASSERT(!CanSubrange<std::unreachable_sentinel_t, int*, subrange_kind::unsized>);
    STATIC_ASSERT(CanSubrange<int*, std::unreachable_sentinel_t, subrange_kind::sized>);

    // clang-format off
    template <class R>
    concept HasMemberEmpty = requires(std::remove_reference_t<R> const r) {
        { r.empty() } -> same_as<bool>;
    };

    template <class R>
    concept HasMemberSize = requires(std::remove_reference_t<R> const r) {
        { r.size() } -> std::integral;
    };
    // clang-format on

    // Validate default template arguments: second defaults to first, and third defaults to subrange_kind::sized iff
    // sized_sentinel_for<second, first>.
    STATIC_ASSERT(same_as<subrange<int*>, subrange<int*, int*, subrange_kind::sized>>);
    STATIC_ASSERT(same_as<subrange<int*, std::unreachable_sentinel_t>,
        subrange<int*, std::unreachable_sentinel_t, subrange_kind::unsized>>);
    STATIC_ASSERT(same_as<subrange<test_iterator<forward_iterator_tag, int>>,
        subrange<test_iterator<forward_iterator_tag, int>, test_iterator<forward_iterator_tag, int>,
            subrange_kind::unsized>>);

    // Validate many properties of a specialization of subrange
    template <class>
    inline constexpr bool is_subrange = false;
    template <class I, class S, subrange_kind K>
    inline constexpr bool is_subrange<subrange<I, S, K>> = true;

    template <class T>
    inline constexpr auto kind_of = illformed<T>();
    template <class I, class S, subrange_kind K>
    inline constexpr auto kind_of<subrange<I, S, K>> = K;

    template <class Subrange, class Rng>
    constexpr bool test_subrange() {
        STATIC_ASSERT(same_as<Subrange, std::remove_cvref_t<Subrange>>);
        STATIC_ASSERT(is_subrange<Subrange>);

        using I              = ranges::iterator_t<Subrange>;
        using S              = ranges::sentinel_t<Subrange>;
        constexpr bool sized = kind_of<Subrange> == subrange_kind::sized;
        static_assert(
            std::integral<std::iter_difference_t<I>>, "make_unsigned_t below needs to be make-unsigned-like-t");
        using size_type = std::make_unsigned_t<std::iter_difference_t<I>>;

        // Validate SMFs
        STATIC_ASSERT(default_initializable<Subrange>);
        STATIC_ASSERT(movable<Subrange>);
        STATIC_ASSERT(!copyable<I> || copyable<Subrange>);

        STATIC_ASSERT(constructible_from<Subrange, I, S> == (!sized || sized_sentinel_for<S, I>) );
        STATIC_ASSERT(
            constructible_from<Subrange, I const&, S const&> == (copyable<I> && (!sized || sized_sentinel_for<S, I>) ));
        STATIC_ASSERT(constructible_from<Subrange, I, S, size_type> == sized);
        STATIC_ASSERT(constructible_from<Subrange, I const&, S const&, size_type> == (copyable<I> && sized));

        STATIC_ASSERT(constructible_from<Subrange, Rng&> == (!sized || sized_range<Rng> || sized_sentinel_for<S, I>) );
        STATIC_ASSERT(constructible_from<Subrange, Rng&, size_type> == sized);
        STATIC_ASSERT(constructible_from<Subrange,
                          Rng> == (borrowed_range<Rng> && (!sized || sized_range<Rng> || sized_sentinel_for<S, I>) ));
        STATIC_ASSERT(constructible_from<Subrange, Rng, size_type> == (sized && borrowed_range<Rng>) );

        // Validate begin/end/empty
        STATIC_ASSERT(range<Subrange>);
        STATIC_ASSERT(HasMemberEmpty<Subrange const>);
        STATIC_ASSERT(!copyable<I> || range<Subrange const&>);

        // Validate size
        STATIC_ASSERT(sized == HasMemberSize<Subrange>);

        return true;
    }

    template <class Rng>
    constexpr bool test_construction() {
        STATIC_ASSERT(same_as<Rng, std::remove_cvref_t<Rng>>);

        using I = ranges::iterator_t<Rng>;
        using S = ranges::sentinel_t<Rng>;

        STATIC_ASSERT(test_subrange<subrange<I, S, subrange_kind::sized>, Rng>());
        if constexpr (!sized_sentinel_for<S, I>) {
            STATIC_ASSERT(test_subrange<subrange<I, S, subrange_kind::unsized>, Rng>());
        }

        return true;
    }
    STATIC_ASSERT(test_construction<test_range<output_iterator_tag, int, false, false>>());
    STATIC_ASSERT(test_construction<test_range<output_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_construction<test_range<input_iterator_tag, int, false, false>>());
    STATIC_ASSERT(test_construction<test_range<input_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_construction<test_range<forward_iterator_tag, int, false, false>>());
    STATIC_ASSERT(test_construction<test_range<forward_iterator_tag, int, false, true>>());
    STATIC_ASSERT(test_construction<test_range<forward_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_construction<test_range<forward_iterator_tag, int, true, true>>());
    STATIC_ASSERT(test_construction<test_range<bidirectional_iterator_tag, int, false, false>>());
    STATIC_ASSERT(test_construction<test_range<bidirectional_iterator_tag, int, false, true>>());
    STATIC_ASSERT(test_construction<test_range<bidirectional_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_construction<test_range<bidirectional_iterator_tag, int, true, true>>());
    STATIC_ASSERT(test_construction<test_range<random_access_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_construction<test_range<random_access_iterator_tag, int, true, true>>());
    STATIC_ASSERT(test_construction<test_range<contiguous_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_construction<test_range<contiguous_iterator_tag, int, true, true>>());

    STATIC_ASSERT(test_construction<std::forward_list<int>>());
    STATIC_ASSERT(test_construction<std::list<int>>());
    STATIC_ASSERT(test_construction<std::vector<int>>());
    STATIC_ASSERT(test_construction<std::string_view>());

    // Validate that slicing conversions are forbidden
    struct Base {};
    struct Derived : Base {};
    STATIC_ASSERT(!std::constructible_from<subrange<Base*>, Derived*, Derived*>);
    STATIC_ASSERT(!std::constructible_from<subrange<Base*>, Derived*, Derived*, std::make_unsigned_t<std::ptrdiff_t>>);
    STATIC_ASSERT(!std::constructible_from<subrange<Base*>, subrange<Derived*>>);
    STATIC_ASSERT(!std::constructible_from<subrange<Base*>, subrange<Derived*>, std::make_unsigned_t<std::ptrdiff_t>>);

    struct with_converting_iterators {
        template <bool IsConst>
        struct iterator {
            using iterator_concept  = input_iterator_tag;
            using iterator_category = void; // TRANSITION, LWG-3289
            using value_type        = int;
            using difference_type   = int;
            using pointer           = void;
            using reference         = int;

            iterator() = default;
            iterator(iterator<!IsConst>) requires IsConst;

            iterator(iterator&&) = default;
            iterator& operator=(iterator&&) = default;

            int operator*() const;
            iterator& operator++();
            void operator++(int);
        };

        template <bool IsConst>
        struct sentinel {
            sentinel() = default;
            sentinel(sentinel<!IsConst>) requires IsConst;

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
        STATIC_ASSERT(std::constructible_from<subrange<std::list<int>::const_iterator>, LI const&, LI const&>);
        STATIC_ASSERT(std::constructible_from<subrange<std::list<int>::const_iterator>, LI, LI>);

        using I  = ranges::iterator_t<with_converting_iterators>;
        using S  = ranges::sentinel_t<with_converting_iterators>;
        using CI = ranges::iterator_t<with_converting_iterators const>;
        using CS = ranges::sentinel_t<with_converting_iterators const>;

        using SizedSubrange = subrange<CI, CS, subrange_kind::sized>;
        STATIC_ASSERT(test_subrange<SizedSubrange, with_converting_iterators>());

        STATIC_ASSERT(!constructible_from<SizedSubrange, I&, S&>); // lvalues are not convertible
        STATIC_ASSERT(!constructible_from<SizedSubrange, I&, S&, unsigned int>); // ditto
        STATIC_ASSERT(!constructible_from<SizedSubrange, I, S>); // missing size
        STATIC_ASSERT(constructible_from<SizedSubrange, I, S, unsigned int>);

        using UnsizedSubrange = subrange<CI, CS, subrange_kind::unsized>;
        STATIC_ASSERT(test_subrange<UnsizedSubrange, with_converting_iterators>());

        STATIC_ASSERT(!constructible_from<UnsizedSubrange, I&, S&>); // lvalues are not convertible
        STATIC_ASSERT(!constructible_from<UnsizedSubrange, I&, S&, unsigned int>); // ditto
        STATIC_ASSERT(constructible_from<UnsizedSubrange, I, S>); // but rvalues are
        STATIC_ASSERT(!constructible_from<UnsizedSubrange, I, S, unsigned int>); // !sized
    }

    // Validate deduction guides
    template <class Rng>
    constexpr bool test_ctad() {
        using I             = ranges::iterator_t<Rng>;
        using S             = ranges::sentinel_t<Rng>;
        constexpr bool diff = sized_sentinel_for<S, I>;

        {
            using T = decltype(subrange(std::declval<I>(), std::declval<S>()));
            STATIC_ASSERT(same_as<T, subrange<I, S, subrange_kind{diff}>>);

            STATIC_ASSERT(range<T>);
            STATIC_ASSERT(!copyable<I> || range<T const&>);
        }

        static_assert(
            std::integral<std::iter_difference_t<I>>, "make_unsigned_t below needs to be make-unsigned-like-t");
        using size_type = std::make_unsigned_t<std::iter_difference_t<I>>;
        {
            using T = decltype(subrange(std::declval<I>(), std::declval<S>(), std::declval<size_type>()));
            STATIC_ASSERT(same_as<T, subrange<I, S, subrange_kind::sized>>);

            STATIC_ASSERT(range<T>);
            STATIC_ASSERT(!copyable<I> || range<T const&>);
        }

        constexpr bool is_sized = diff | sized_range<Rng>;
        {
            using T = decltype(subrange(std::declval<Rng&>()));
            STATIC_ASSERT(same_as<T, subrange<I, S, subrange_kind{is_sized}>>);

            STATIC_ASSERT(range<T>);
            STATIC_ASSERT(!copyable<I> || range<T const&>);

            if constexpr (borrowed_range<Rng>) {
                using U = decltype(subrange(std::declval<Rng>()));
                STATIC_ASSERT(same_as<U, T>);
            }
        }

        {
            using T = decltype(subrange(std::declval<Rng&>(), std::declval<size_type>()));
            STATIC_ASSERT(same_as<T, subrange<I, S, subrange_kind::sized>>);

            STATIC_ASSERT(range<T>);
            STATIC_ASSERT(!copyable<I> || range<T const&>);

            if constexpr (borrowed_range<Rng>) {
                using U = decltype(subrange(std::declval<Rng>(), std::declval<size_type>()));
                STATIC_ASSERT(same_as<U, T>);
            }
        }

        return true;
    }
    STATIC_ASSERT(test_ctad<test_range<output_iterator_tag, int, false, false>>());
    STATIC_ASSERT(test_ctad<test_range<output_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_ctad<test_range<input_iterator_tag, int, false, false>>());
    STATIC_ASSERT(test_ctad<test_range<input_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_ctad<test_range<forward_iterator_tag, int, false, false>>());
    STATIC_ASSERT(test_ctad<test_range<forward_iterator_tag, int, false, true>>());
    STATIC_ASSERT(test_ctad<test_range<forward_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_ctad<test_range<forward_iterator_tag, int, true, true>>());
    STATIC_ASSERT(test_ctad<test_range<bidirectional_iterator_tag, int, false, false>>());
    STATIC_ASSERT(test_ctad<test_range<bidirectional_iterator_tag, int, false, true>>());
    STATIC_ASSERT(test_ctad<test_range<bidirectional_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_ctad<test_range<bidirectional_iterator_tag, int, true, true>>());
    STATIC_ASSERT(test_ctad<test_range<random_access_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_ctad<test_range<random_access_iterator_tag, int, true, true>>());
    STATIC_ASSERT(test_ctad<test_range<contiguous_iterator_tag, int, true, false>>());
    STATIC_ASSERT(test_ctad<test_range<contiguous_iterator_tag, int, true, true>>());

    STATIC_ASSERT(test_ctad<std::forward_list<int>>());
    STATIC_ASSERT(test_ctad<std::list<int>>());
    STATIC_ASSERT(test_ctad<std::vector<int>>());
    STATIC_ASSERT(test_ctad<std::string_view>());

    // Validate conversion to PairLike
    STATIC_ASSERT(std::is_convertible_v<subrange<int*>, std::pair<int*, int*>>);
    STATIC_ASSERT(!std::is_convertible_v<subrange<int const*>, std::pair<int*, int*>>);
    STATIC_ASSERT(std::is_convertible_v<subrange<int*>, std::pair<int const*, int const*>>);
    STATIC_ASSERT(std::is_convertible_v<subrange<int const*>, std::pair<int const*, int const*>>);
    STATIC_ASSERT(std::is_convertible_v<subrange<int*>, std::tuple<int*, int*>>);
    STATIC_ASSERT(!std::is_convertible_v<subrange<int const*>, std::tuple<int*, int*>>);
    STATIC_ASSERT(std::is_convertible_v<subrange<int*>, std::tuple<int const*, int const*>>);
    STATIC_ASSERT(std::is_convertible_v<subrange<int const*>, std::tuple<int const*, int const*>>);

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
    STATIC_ASSERT(test_advance_next_prev());

    // Validate tuple interface
    template <class Subrange>
    constexpr bool test_tuple() {
        using std::declval, std::tuple_element_t, std::tuple_size_v;

        using I = ranges::iterator_t<Subrange>;
        using S = ranges::sentinel_t<Subrange>;

        STATIC_ASSERT(tuple_size_v<Subrange> == 2);
        STATIC_ASSERT(tuple_size_v<Subrange const> == 2);

        STATIC_ASSERT(same_as<tuple_element_t<0, Subrange>, I>);
        STATIC_ASSERT(same_as<tuple_element_t<0, Subrange const>, I>);
        STATIC_ASSERT(same_as<tuple_element_t<1, Subrange>, S>);
        STATIC_ASSERT(same_as<tuple_element_t<1, Subrange const>, S>);

        STATIC_ASSERT(same_as<decltype(ranges::get<0>(declval<Subrange>())), I>);
        STATIC_ASSERT(same_as<decltype(ranges::get<0>(declval<Subrange&>())), I>);
        STATIC_ASSERT(same_as<decltype(ranges::get<0>(declval<Subrange const>())), I>);
        STATIC_ASSERT(same_as<decltype(ranges::get<0>(declval<Subrange const&>())), I>);
        STATIC_ASSERT(same_as<decltype(std::get<0>(declval<Subrange>())), I>);
        STATIC_ASSERT(same_as<decltype(std::get<0>(declval<Subrange&>())), I>);
        STATIC_ASSERT(same_as<decltype(std::get<0>(declval<Subrange const>())), I>);
        STATIC_ASSERT(same_as<decltype(std::get<0>(declval<Subrange const&>())), I>);

        STATIC_ASSERT(same_as<decltype(ranges::get<1>(declval<Subrange>())), S>);
        STATIC_ASSERT(same_as<decltype(ranges::get<1>(declval<Subrange&>())), S>);
        STATIC_ASSERT(same_as<decltype(ranges::get<1>(declval<Subrange const>())), S>);
        STATIC_ASSERT(same_as<decltype(ranges::get<1>(declval<Subrange const&>())), S>);
        STATIC_ASSERT(same_as<decltype(std::get<1>(declval<Subrange>())), S>);
        STATIC_ASSERT(same_as<decltype(std::get<1>(declval<Subrange&>())), S>);
        STATIC_ASSERT(same_as<decltype(std::get<1>(declval<Subrange const>())), S>);
        STATIC_ASSERT(same_as<decltype(std::get<1>(declval<Subrange const&>())), S>);

        return true;
    }
    STATIC_ASSERT(test_tuple<subrange<int*>>());
    STATIC_ASSERT(test_tuple<subrange<int*, std::unreachable_sentinel_t, subrange_kind::sized>>());
    STATIC_ASSERT(test_tuple<subrange<int*, std::unreachable_sentinel_t, subrange_kind::unsized>>());
} // namespace test_subrange
