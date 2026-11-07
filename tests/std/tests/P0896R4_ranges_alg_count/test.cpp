// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;
using P = pair<int, int>;

struct instantiator {
    static constexpr P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::count;
        { // Validate iterator + sentinel overload
            Read wrapped_input{input};

            auto result = count(wrapped_input.begin(), wrapped_input.end(), 47, get_second);
            static_assert(same_as<decltype(result), ranges::range_difference_t<Read>>);
            assert(result == 2);
        }
        { // Validate range overload
            Read wrapped_input{input};

            auto result = count(wrapped_input, 99, get_second);
            static_assert(same_as<decltype(result), ranges::range_difference_t<Read>>);
            assert(result == 3);
        }
    }
};

// LWG-4490: count computes the end from the size instead of walking to the sentinel for sized random access ranges
struct counting_sentinel {
    const int* last;
    int* comparisons;

    friend constexpr bool operator==(const int* const current, const counting_sentinel sentinel) {
        ++*sentinel.comparisons;
        return current == sentinel.last;
    }
};

class sized_range_unsized_sentinel {
public:
    constexpr sized_range_unsized_sentinel(const int* const first, const size_t count, int* const comparisons)
        : first_{first}, count_{count}, comparisons_{comparisons} {}

    constexpr const int* begin() const {
        return first_;
    }

    constexpr counting_sentinel end() const {
        return {first_ + count_, comparisons_};
    }

    constexpr size_t size() const {
        return count_;
    }

private:
    const int* first_;
    size_t count_;
    int* comparisons_;
};

static_assert(ranges::random_access_range<sized_range_unsized_sentinel>);
static_assert(ranges::sized_range<sized_range_unsized_sentinel>);
static_assert(!ranges::common_range<sized_range_unsized_sentinel>);
static_assert(!sized_sentinel_for<counting_sentinel, const int*>);

constexpr bool test_sized_range_unsized_sentinel() {
    const int input[] = {1, 0, 1, 2, 1, 0, 3};
    int comparisons   = 0;

    assert(ranges::count(sized_range_unsized_sentinel{input, 7, &comparisons}, 1) == 3);
    assert(ranges::count(sized_range_unsized_sentinel{input, 7, &comparisons}, 9) == 0);
    assert(ranges::count(sized_range_unsized_sentinel{input, 0, &comparisons}, 1) == 0);
    assert(
        ranges::count(sized_range_unsized_sentinel{input, 7, &comparisons}, 1, [](const int x) { return x + 1; }) == 2);
    assert(comparisons == 0);

    return true;
}

void test_sized_range_unsized_sentinel_vectorized() {

    vector<int> input(200);
    for (size_t i = 0; i != input.size(); ++i) {
        input[i] = static_cast<int>(i % 3 == 0);
    }

    int comparisons = 0;
    for (size_t length = 0; length != input.size(); ++length) {
        const auto expected = static_cast<ptrdiff_t>((length + 2) / 3);
        assert(ranges::count(sized_range_unsized_sentinel{input.data(), length, &comparisons}, 1) == expected);
    }
    assert(comparisons == 0);
}

int main() {
    static_assert((test_in<instantiator, P const>(), true));
    test_in<instantiator, P const>();

    static_assert(test_sized_range_unsized_sentinel());
    assert(test_sized_range_unsized_sentinel());
    test_sized_range_unsized_sentinel_vectorized();

}
