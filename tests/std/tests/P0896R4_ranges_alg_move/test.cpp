#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <iostream>
#include <utility>
#include <range_algorithm_support.hpp>

void smoke_test() {
    using ranges::move, ranges::move_result, ranges::iterator_t;
    using std::same_as;

    // Validate that copy_result aliases in_out_result
    STATIC_ASSERT(same_as<move_result<int, double>, ranges::in_out_result<int, double>>);

    // Validate dangling story
    STATIC_ASSERT(same_as<decltype(move(borrowed<false>{}, static_cast<int*>(nullptr))), move_result<ranges::dangling, int*>>);
    STATIC_ASSERT(same_as<decltype(move(borrowed<true>{}, static_cast<int*>(nullptr))), move_result<int*, int*>>);

    int const input[] = {13, 53, 12435};
    {
        int output[] = {-1, -1, -1};
        auto result = move(move_only_range{input}, move_only_range{output}.begin());
        STATIC_ASSERT(same_as<decltype(result), 
            move_result<iterator_t<move_only_range<int const>>, iterator_t<move_only_range<int>>>>);
        std::cout << std::to_address(result.in.base()) << ' ' << std::to_address(move_only_range{input}.end().base()) << std::endl;
        assert(result.in == move_only_range{input}.end());
        assert(result.out == move_only_range{output}.end());
        assert(ranges::equal(output, input));
    }
    {
        int output[] = {-1, -1, -1};
        move_only_range wrapped_input{input};
        auto result = move(wrapped_input.begin(), wrapped_input.end(), move_only_range{output}.begin());
        STATIC_ASSERT(same_as<decltype(result), move_result<iterator_t<move_only_range<int const>>, iterator_t<move_only_range<int>>>>);
        assert(result.in == wrapped_input.end());
        assert(result.out == move_only_range{output}.end());
        assert(ranges::equal(output, input));
    }
}

int main() {
    // STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In, class Out>
    static void call(In&& in = {}, Out out = {}) {
        (void) ranges::move(in, std::move(out));
        (void) ranges::move(ranges::begin(in), ranges::end(in), std::move(out)); // what is this
    }
};

template void test_in_out<instantiator>();