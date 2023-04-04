// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <functional>
#include <type_traits>

using namespace std;

struct incomplete;

template <class T>
void touch_reference_wrapper_members(reference_wrapper<T> r) {
    static_assert(is_same_v<T, typename reference_wrapper<T>::type>);
    [[maybe_unused]] auto cpy = r;
    cpy                       = r;
    (void) r.get();
    (void) static_cast<T&>(r);
}

void example(incomplete* i) {
    const reference_wrapper a(*i);
    touch_reference_wrapper_members(a);
    touch_reference_wrapper_members(ref(*i));
    touch_reference_wrapper_members(cref(*i));
    touch_reference_wrapper_members(ref(a));
    touch_reference_wrapper_members(cref(a));
}
