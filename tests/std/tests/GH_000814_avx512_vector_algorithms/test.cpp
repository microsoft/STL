#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <xutility>

#if !defined(_USE_STD_VECTOR_ALGORITHMS) || !_USE_STD_VECTOR_ALGORITHMS

int main() {
    assert(false);
    return EXIT_FAILURE;
}

#else // defined(_USE_STD_VECTOR_ALGORITHMS) && _USE_STD_VECTOR_ALGORITHMS

#include <vector>

extern "C" long __isa_enabled;

namespace {

    void _Test_swap() {
        ::std::vector<unsigned char> count_up{/* */};
        ::std::vector<unsigned char> count_invert{/* */};

        constexpr ::std::size_t num_of_samples = 256;
        count_up.resize(num_of_samples);
        count_invert.resize(num_of_samples);

        for (::std::size_t i = 0; i < num_of_samples; ++i) {
            count_up[i]     = static_cast<unsigned char>(i);
            count_invert[i] = static_cast<unsigned char>(~count_up[i]);
        }

        const ::std::vector<unsigned char> count_up_original{count_up};
        const ::std::vector<unsigned char> count_invert_original{count_invert};

        assert(count_up != count_invert);

        __std_swap_ranges_trivially_swappable_noalias(
            count_up.data(), count_up.data() + count_up.size(), count_invert.data());

        assert(count_up != count_invert);
        assert(count_up == count_invert_original);
        assert(count_invert == count_up_original);
    }

    ::std::size_t __Sum_squares(::std::size_t min_size) {
        constexpr ::std::size_t limit = 128;
        ::std::size_t squared_sum     = limit;
        while (min_size < 128) {
            squared_sum += min_size;
            min_size *= 2;
        }
        return squared_sum;
    }

    template <class _BidIt>
    void _Test_reverse_oracle(_BidIt _First, _BidIt _Last) noexcept {
        for (; _First != _Last && _First != --_Last; ++_First) {
            const auto _Temp = *_First;
            *_First          = *_Last;
            *_Last           = _Temp;
        }
    }

    template <typename _Integer>
    void _Test_reverse(void (*swap_fn)(void*, void*)) {
        ::std::vector<unsigned char> test_vector{/* */};

        ::std::size_t num_of_samples = __Sum_squares(sizeof(_Integer));
        test_vector.resize(num_of_samples);

        for (::std::size_t i = 0; i < num_of_samples; ++i) {
            test_vector[i] = static_cast<unsigned char>(i);
        }

        ::std::vector<unsigned char> test_oracle{test_vector};
        _Test_reverse_oracle(reinterpret_cast<_Integer*>(test_oracle.data()),
            reinterpret_cast<_Integer*>(test_oracle.data() + test_oracle.size()));

        assert(test_oracle != test_vector);

        swap_fn(test_vector.data(), test_vector.data() + test_vector.size());

        assert(test_oracle == test_vector);
    }

    template <typename _Integer>
    void _Test_reverse_copy(void (*swap_fn)(const void*, const void*, void*)) {
        ::std::vector<unsigned char> test_source{/* */};

        ::std::size_t num_of_samples = __Sum_squares(sizeof(_Integer));
        test_source.resize(num_of_samples);

        for (::std::size_t i = 0; i < num_of_samples; ++i) {
            test_source[i] = static_cast<unsigned char>(i);
        }

        ::std::vector<unsigned char> test_oracle{test_source};
        _Test_reverse_oracle(reinterpret_cast<_Integer*>(test_oracle.data()),
            reinterpret_cast<_Integer*>(test_oracle.data() + test_oracle.size()));

        ::std::vector<unsigned char> test_vector{/* */};
        test_vector.resize(num_of_samples);

        assert(test_oracle != test_vector);

        swap_fn(test_source.data(), test_source.data() + test_source.size(), test_vector.data());

        assert(test_oracle == test_vector);
    }

} // namespace

int main() {
    if (!_bittest(&__isa_enabled, 6 /* __ISA_AVAILABLE_AVX512 */)) {
        assert(false);
        return EXIT_FAILURE;
    }

    _Test_swap();

    _Test_reverse<unsigned char>(__std_reverse_trivially_swappable_1);
    _Test_reverse<unsigned short>(__std_reverse_trivially_swappable_2);
    _Test_reverse<unsigned int>(__std_reverse_trivially_swappable_4);
    _Test_reverse<unsigned long long>(__std_reverse_trivially_swappable_8);

    _Test_reverse_copy<unsigned char>(__std_reverse_copy_trivially_copyable_1);
    _Test_reverse_copy<unsigned short>(__std_reverse_copy_trivially_copyable_2);
    _Test_reverse_copy<unsigned int>(__std_reverse_copy_trivially_copyable_4);
    _Test_reverse_copy<unsigned long long>(__std_reverse_copy_trivially_copyable_8);

    return EXIT_SUCCESS;
}

#endif // defined(_USE_STD_VECTOR_ALGORITHMS) && _USE_STD_VECTOR_ALGORITHMS
