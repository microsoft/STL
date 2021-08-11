#include <cstdlib>
#include <xutility>

#if defined(_M_ARM64EC) || !defined(_USE_STD_VECTOR_ALGORITHMS) || !_USE_STD_VECTOR_ALGORITHMS

int main() {
    return EXIT_SUCCESS;
}

#else // defined(_M_ARM64EC) || !defined(_USE_STD_VECTOR_ALGORITHMS) || !_USE_STD_VECTOR_ALGORITHMS

#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

extern "C" long __isa_enabled;

namespace {

    void test_swap() {
        constexpr size_t num_of_samples = 256;
        vector<unsigned char> count_up(num_of_samples);
        vector<unsigned char> count_invert(num_of_samples);

        for (size_t i = 0; i < num_of_samples; ++i) {
            count_up[i]     = static_cast<unsigned char>(i);
            count_invert[i] = static_cast<unsigned char>(~count_up[i]);
        }

        const vector<unsigned char> count_up_original{count_up};
        const vector<unsigned char> count_invert_original{count_invert};

        assert(count_up != count_invert);

        __std_swap_ranges_trivially_swappable_noalias(
            count_up.data(), count_up.data() + count_up.size(), count_invert.data());

        assert(count_up != count_invert);
        assert(count_up == count_invert_original);
        assert(count_invert == count_up_original);
    }

    size_t sum_squares(size_t min_size) {
        constexpr size_t limit = 128;
        size_t squared_sum     = limit;
        while (min_size < 128) {
            squared_sum += min_size;
            min_size *= 2;
        }
        return squared_sum;
    }

    template <class _iterator_t>
    void test_reverse_oracle(_iterator_t first, _iterator_t last) noexcept {
        for (; first != last && first != --last; ++first) {
            const auto temp = *first;
            *first          = *last;
            *last           = temp;
        }
    }

    template <typename _integer_t>
    void test_reverse(void (*swap_fn)(void*, void*)) {
        vector<unsigned char> test_vector(sum_squares(sizeof(_integer_t)));

        for (size_t i = 0; i < test_vector.size(); ++i) {
            test_vector[i] = static_cast<unsigned char>(i);
        }

        vector<unsigned char> test_oracle{test_vector};
        test_reverse_oracle(reinterpret_cast<_integer_t*>(test_oracle.data()),
            reinterpret_cast<_integer_t*>(test_oracle.data() + test_oracle.size()));

        assert(test_oracle != test_vector);

        swap_fn(test_vector.data(), test_vector.data() + test_vector.size());

        assert(test_oracle == test_vector);
    }

    template <typename _integer_t>
    void test_reverse_copy(void (*swap_fn)(const void*, const void*, void*)) {
        vector<unsigned char> test_source(sum_squares(sizeof(_integer_t)));

        for (size_t i = 0; i < test_source.size(); ++i) {
            test_source[i] = static_cast<unsigned char>(i);
        }

        vector<unsigned char> test_oracle{test_source};
        test_reverse_oracle(reinterpret_cast<_integer_t*>(test_oracle.data()),
            reinterpret_cast<_integer_t*>(test_oracle.data() + test_oracle.size()));

        vector<unsigned char> test_vector(test_source.size());

        assert(test_oracle != test_vector);

        swap_fn(test_source.data(), test_source.data() + test_source.size(), test_vector.data());

        assert(test_oracle == test_vector);
    }

} // namespace

int main() {
    if (!_bittest(&__isa_enabled, 6 /* __ISA_AVAILABLE_AVX512 */)) {
        return EXIT_SUCCESS;
    }

    test_swap();

    test_reverse<unsigned char>(::__std_reverse_trivially_swappable_1);
    test_reverse<unsigned short>(::__std_reverse_trivially_swappable_2);
    test_reverse<unsigned int>(::__std_reverse_trivially_swappable_4);
    test_reverse<unsigned long long>(__std_reverse_trivially_swappable_8);

    test_reverse_copy<unsigned char>(::__std_reverse_copy_trivially_copyable_1);
    test_reverse_copy<unsigned short>(::__std_reverse_copy_trivially_copyable_2);
    test_reverse_copy<unsigned int>(::__std_reverse_copy_trivially_copyable_4);
    test_reverse_copy<unsigned long long>(::__std_reverse_copy_trivially_copyable_8);

    return EXIT_SUCCESS;
}

#endif // defined(_M_ARM64EC) || !defined(_USE_STD_VECTOR_ALGORITHMS) || !_USE_STD_VECTOR_ALGORITHMS
