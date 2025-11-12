// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <bitset>
#include <cassert>
#include <type_traits>
#include <vector>

using namespace std;

static const auto is_true  = [](bool b) { return b; };
static const auto is_false = [](bool b) { return !b; };

void check_values_match() {
    vector<bool> x(100, false);
    vector<bool> y(100, true);

    assert(all_of(x.begin(), x.end(), is_false));
    assert(all_of(y.begin(), y.end(), is_true));

    swap(x[12], y[34]);

    assert(all_of(x.begin(), x.begin() + 12, is_false));
    assert(x[12]);
    assert(all_of(x.begin() + 13, x.end(), is_false));

    assert(all_of(y.begin(), y.begin() + 34, is_true));
    assert(!y[34]);
    assert(all_of(y.begin() + 35, y.end(), is_true));
}

template <typename T>
void check_P3612(T& collection) {

    auto ref0       = collection[0];
    auto const ref1 = collection[1];

    // assignments from bool
    ref0 = true;
    ref1 = false;
    // assignments from reference
    ref0 = ref1;
    ref1 = ref0;

    collection[0] = true;
    collection[1] = false;

    swap(collection[0], collection[1]); // swap(reference, reference)
    assert(!collection[0]);

    bool b = true;
    swap(collection[0], b); // swap(reference, bool)
    assert(collection[0]);

    swap(b, collection[0]); // swap(bool, reference)
    assert(!collection[0]);
}

template <typename T>
constexpr bool has_noexcept_copy_ctor = noexcept(T(std::declval<const T&>()));

int main() {
    check_values_match();


    constexpr size_t N = 10;
    vector<bool> vector(N);
    bitset<N> bitset(0);

    check_P3612(vector);
    check_P3612(bitset);

    static_assert(std::is_nothrow_copy_constructible_v<decltype(vector)::reference>, "");
    static_assert(std::is_nothrow_copy_constructible_v<decltype(bitset)::reference>, "");
    static_assert(has_noexcept_copy_ctor<decltype(vector)::reference>, "");
    static_assert(has_noexcept_copy_ctor<decltype(bitset)::reference>, "");
}
