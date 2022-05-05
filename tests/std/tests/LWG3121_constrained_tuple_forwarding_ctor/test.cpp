// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <tuple>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#if _HAS_CXX20
#define CONSTEXPR_VAR_20 constexpr
#else
#define CONSTEXPR_VAR_20 const
#endif

using namespace std;

class pseudo_any {
private:
    bool has_value_ = false;

public:
    pseudo_any() = default;
    template <class ValT,
        enable_if_t<conjunction_v<negation<is_same<decay_t<ValT>, pseudo_any>>, is_copy_constructible<decay_t<ValT>>>,
            int> = 0>
    constexpr pseudo_any(ValT&&) noexcept : has_value_{true} {}

    constexpr bool has_value() const noexcept {
        return has_value_;
    }
};

STATIC_ASSERT(is_copy_constructible_v<tuple<pseudo_any>>);

using TA2 = tuple<pseudo_any, pseudo_any>;
using TA3 = tuple<pseudo_any, pseudo_any, pseudo_any>;

CONSTEXPR_VAR_20 TA2 t2{allocator_arg, allocator<int>{}};
CONSTEXPR_VAR_20 TA3 t3{allocator_arg, allocator<int>{}, tuple<pseudo_any, pseudo_any, int>{}};

#if _HAS_CXX20
static_assert(!get<0>(t2).has_value());
static_assert(!get<1>(t2).has_value());
static_assert(!get<0>(t3).has_value());
static_assert(!get<1>(t3).has_value());
static_assert(get<2>(t3).has_value());
#endif // _HAS_CXX20

int main() {
    assert(!get<0>(t2).has_value());
    assert(!get<1>(t2).has_value());
    assert(!get<0>(t3).has_value());
    assert(!get<1>(t3).has_value());
    assert(get<2>(t3).has_value());
}
