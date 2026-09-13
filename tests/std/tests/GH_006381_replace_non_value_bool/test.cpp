// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iterator>

using namespace std;

int main() {
    // Test replace() and replace_copy() against attempts to replace bool values with non-value representations.
    // Compare with direct memcmp() instead of algorithms or loops to make any failures as deterministic as possible.

    // We're not trying to actually engage any vectorization, we only need to dispatch to the separately compiled code,
    // so it doesn't matter if the array size is below the vectorization thresholds in the separately compiled code.

    static constexpr bool source[]            = {true, true, false, true, false, false, false};
    static constexpr bool expected_all_true[] = {true, true, true, true, true, true, true};

    constexpr size_t source_len = size(source);
    static_assert(size(expected_all_true) == source_len, "sizes should match");

    {
        bool dest_false_replaced_inplace[source_len]{};
        copy(begin(source), end(source), begin(dest_false_replaced_inplace));
        replace(begin(dest_false_replaced_inplace), end(dest_false_replaced_inplace), 0, 2);
        assert(memcmp(dest_false_replaced_inplace, expected_all_true, source_len) == 0);
    }
    {
        bool dest_true_replaced_inplace[source_len]{};
        copy(begin(source), end(source), begin(dest_true_replaced_inplace));
        replace(begin(dest_true_replaced_inplace), end(dest_true_replaced_inplace), 1, 4);
        assert(memcmp(dest_true_replaced_inplace, source, source_len) == 0);
    }
    {
        bool dest_false_replaced[source_len]{};
        replace_copy(begin(source), end(source), begin(dest_false_replaced), 0, 2);
        assert(memcmp(dest_false_replaced, expected_all_true, source_len) == 0);
    }
    {
        bool dest_true_replaced[source_len]{};
        replace_copy(begin(source), end(source), begin(dest_true_replaced), 1, 4);
        assert(memcmp(dest_true_replaced, source, source_len) == 0);
    }
#if _HAS_CXX20
    {
        bool ranges_dest_false_replaced_inplace[source_len]{};
        ranges::copy(source, ranges_dest_false_replaced_inplace);
        ranges::replace(ranges_dest_false_replaced_inplace, 0, 2);
        assert(memcmp(ranges_dest_false_replaced_inplace, expected_all_true, source_len) == 0);
    }
    {
        bool ranges_dest_true_replaced_inplace[source_len]{};
        ranges::copy(source, ranges_dest_true_replaced_inplace);
        ranges::replace(ranges_dest_true_replaced_inplace, 1, 4);
        assert(memcmp(ranges_dest_true_replaced_inplace, source, source_len) == 0);
    }
    {
        bool ranges_dest_false_replaced[source_len]{};
        ranges::replace_copy(source, begin(ranges_dest_false_replaced), 0, 2);
        assert(memcmp(ranges_dest_false_replaced, expected_all_true, source_len) == 0);
    }
    {
        bool ranges_dest_true_replaced[source_len]{};
        ranges::replace_copy(source, begin(ranges_dest_true_replaced), 1, 4);
        assert(memcmp(ranges_dest_true_replaced, source, source_len) == 0);
    }
#endif // _HAS_CXX20
}
