// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <mdspan>
#include <type_traits>

using namespace std;

struct layout_packed_upper { // LAPACK packed storage, VERY INCOMPLETE
    template <class Extents>
    class mapping {
    public:
        using extents_type = Extents;
        using index_type   = extents_type::index_type;
        using size_type    = extents_type::size_type;
        using rank_type    = extents_type::rank_type;
        using layout_type  = layout_packed_upper;

        constexpr mapping() noexcept = default;
        constexpr mapping(const extents_type& ex_) : ex{ex_} {}

        constexpr const extents_type& extents() const {
            return ex;
        }

        constexpr index_type required_span_size() const {
            if (dim % 2 == 0) {
                return (dim / 2) * (dim + 1);
            } else {
                return ((dim + 1) / 2) * dim;
            }
        }

    private:
        extents_type ex = extents_type();
        index_type dim  = ex.extent(0);
    };
};

int main() {
    constexpr int32_t dim        = 47'000; // sqrt(2^32) > dim > sqrt(2^31), dim assumed even below
    constexpr auto expected_size = [] {
        int32_t unused;
        bool overflow = _Mul_overflow(dim, dim, unused);
        assert(overflow);

        using UType     = remove_cv_t<make_unsigned_t<decltype(dim)>>;
        const auto udim = static_cast<UType>(dim);
        UType result{};
        overflow = _Mul_overflow(udim, udim, result);
        assert(!overflow);
        return result;
    }();

    constexpr auto expected_req_size = [] {
        int32_t result{};
        const bool overflow = _Mul_overflow(dim / 2, dim + 1, result);
        assert(!overflow);
        return result;
    }();

    {
        using E = extents<int32_t, dim, dim>;
        constexpr mdspan<double, E, layout_packed_upper> m(nullptr);
        static_assert(m.size() == expected_size);
        static_assert(m.mapping().required_span_size() == expected_req_size);
    }

    {
        using E = dextents<int32_t, 2>;
        constexpr mdspan<double, E, layout_packed_upper> m(nullptr, dim, dim);
        static_assert(m.size() == expected_size);
        static_assert(m.mapping().required_span_size() == expected_req_size);
    }

    return 0;
}
