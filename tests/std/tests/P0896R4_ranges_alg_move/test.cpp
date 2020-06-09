#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <iostream>
#include <utility>
#include <range_algorithm_support.hpp>

struct int_wrapper {
    int val = 10;
    int_wrapper() = default;
    int_wrapper(int x) : val{x} {}
    int_wrapper(int_wrapper&& that) : val{std::exchange(that.val, -1)} {}
    int_wrapper& operator=(int_wrapper&& that) {
        val = std::exchange(that.val, -1);
        return *this;
    }
};

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
    {
        int_wrapper input1[3];
        input1[0] = int_wrapper(13);
        input1[1] = int_wrapper(55);
        input1[2] = int_wrapper(1234);
        int_wrapper expected_output[3];
        expected_output[0] = int_wrapper(13);
        expected_output[1] = int_wrapper(55);
        expected_output[2] = int_wrapper(1234);
        int_wrapper actual_output[3];
        for (int i = 0; i < 3; i++) {
            actual_output[i] = int_wrapper(-1);
        }
        move_only_range wrapped_input{input1};
        auto result = move(wrapped_input.begin(), wrapped_input.end(), move_only_range{actual_output}.begin());
        assert(result.in == wrapped_input.end());
        assert(result.out == move_only_range{actual_output}.end());
        for (int i = 0; i < 3; i++) {
             assert(input1[i].val == -1);
             assert(actual_output[i].val == expected_output[i].val);
        }
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