// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <iterator>
#include <numeric>
#include <random>
#include <type_traits>

#include "adapterator.hpp"

using namespace std;

namespace {
    constexpr unsigned int Pop = 1000;

    array<int, Pop> source;
    array<int, Pop> dest0;
    array<int, Pop> dest1;

    void clear() {
        dest0.fill(0);
        dest1.fill(0);
    }

    template <class SourceCategory, class DestCategory, class URNG>
    void test_source_dest_size(URNG& urng, unsigned int n) {
        using SI = adapterator<decltype(cbegin(source)), SourceCategory>;
        using DI = adapterator<decltype(begin(dest0)), DestCategory>;
        clear();
        auto const result = static_cast<ptrdiff_t>(min(n, Pop));

        auto cpy = urng;
        assert(
            sample(SI{cbegin(source)}, SI{cend(source)}, DI{begin(dest0)}, n, urng).base() == cbegin(dest0) + result);

        // Verify repeatability
        assert(sample(SI{cbegin(source)}, SI{cend(source)}, DI{begin(dest1)}, n, cpy).base() == cbegin(dest1) + result);
        assert(equal(cbegin(dest0), cbegin(dest0) + result, cbegin(dest1), cbegin(dest1) + result));

        if (is_base_of_v<forward_iterator_tag, SourceCategory>) {
            // Verify stability
            assert(is_sorted(cbegin(dest0), cbegin(dest0) + result));
        } else {
            // Ensure ordering for set_difference
            sort(begin(dest0), begin(dest0) + result);
        }

        // Verify sample is a subset (i.e., sample - population is the empty set)
        assert(set_difference(cbegin(dest0), cbegin(dest0) + result, cbegin(source), cend(source), begin(dest1))
               == begin(dest1));
    }

    constexpr unsigned int div_ceil(unsigned int dividend, unsigned int divisor) {
        return (dividend + divisor - 1) / divisor;
    }

    template <class SourceCategory, class DestCategory, class URNG>
    void test_source_dest(URNG& gen) {
        for (auto n : {div_ceil(Pop, 100), div_ceil(Pop, 2), Pop, 2 * Pop}) {
            test_source_dest_size<SourceCategory, DestCategory>(gen, n);
        }
    }

    template <class SourceCategory, class URNG>
    void test_source(URNG& gen) {
        test_source_dest<SourceCategory, output_iterator_tag>(gen);
        test_source_dest<SourceCategory, input_iterator_tag>(gen);
        test_source_dest<SourceCategory, forward_iterator_tag>(gen);
        test_source_dest<SourceCategory, bidirectional_iterator_tag>(gen);
        test_source_dest<SourceCategory, random_access_iterator_tag>(gen);
    }
} // unnamed namespace

int main() {
    iota(begin(source), end(source), 0);

    const unsigned int seed = random_device{}();
    printf("Using seed: %u\n", seed);

    mt19937 gen{seed};

    test_source_dest<input_iterator_tag, random_access_iterator_tag>(gen);
    test_source<forward_iterator_tag>(gen);
    test_source<bidirectional_iterator_tag>(gen);
    test_source<random_access_iterator_tag>(gen);
}
