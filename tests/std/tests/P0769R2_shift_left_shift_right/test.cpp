// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

using namespace std;

template <bool HasSwap>
struct MoveOnly {
    ptrdiff_t value;

    explicit MoveOnly(const ptrdiff_t v) : value(v) {}

    // Only here to allow vector::emplace_back to work
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&)      = default;

    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly& operator=(MoveOnly&&)      = default;

    MoveOnly& operator=(const ptrdiff_t x) {
        value = x;
        return *this;
    }

    MoveOnly& operator++() {
        ++value;
        return *this;
    }

    friend bool operator<(const MoveOnly& x, const MoveOnly& y) {
        return x.value < y.value;
    }
    friend bool operator==(const MoveOnly& x, const MoveOnly& y) {
        return x.value == y.value;
    }
    friend bool operator!=(const MoveOnly& x, const MoveOnly& y) {
        return x.value != y.value;
    }
};

template <bool HasSwap>
void swap(MoveOnly<HasSwap>& x, MoveOnly<HasSwap>& y) {
    static_assert(HasSwap);
    swap(x.value, y.value);
}

template <typename T>
void fill_iota(forward_list<T>& f, const ptrdiff_t count) {
    f.clear();
    for (ptrdiff_t i = count; i > 0; --i) {
        f.emplace_front(i);
    }
}

template <typename T>
void fill_iota(list<T>& l, const ptrdiff_t count) {
    l.clear();
    for (ptrdiff_t i = 1; i <= count; ++i) {
        l.emplace_back(i);
    }
}

template <typename T>
void fill_iota(vector<T>& v, const ptrdiff_t count) {
    v.clear();
    for (ptrdiff_t i = 1; i <= count; ++i) {
        v.emplace_back(i);
    }
}

template <typename FwdIt>
void test_iota(FwdIt first, const FwdIt last, const ptrdiff_t min_exp, const ptrdiff_t max_exp) {
    typename iterator_traits<FwdIt>::value_type min_val{min_exp};
    const typename iterator_traits<FwdIt>::value_type max_val{max_exp};

    if (max_val < min_val) {
        assert(first == last);
        return;
    }

    for (;; ++min_val) {
        assert(first != last);
        assert(*first == min_val);
        ++first;

        if (min_val == max_val) {
            assert(first == last);
            break;
        }
    }
}

template <typename Container>
void test_case_shift_left(const ptrdiff_t tmpSize) {
    Container tmp;
    fill_iota(tmp, tmpSize);

    for (ptrdiff_t pos_to_shift = 0; pos_to_shift < tmpSize; ++pos_to_shift) {
        fill_iota(tmp, tmpSize);
        test_iota(tmp.begin(), shift_left(tmp.begin(), tmp.end(), pos_to_shift), pos_to_shift + 1, tmpSize);

#if __cpp_lib_shift >= 202202L
        {
            fill_iota(tmp, tmpSize);
            auto [first, last] = ranges::shift_left(tmp.begin(), tmp.end(), pos_to_shift);
            assert(first == tmp.begin());
            test_iota(first, last, pos_to_shift + 1, tmpSize);
        }

        {
            fill_iota(tmp, tmpSize);
            auto [first, last] = ranges::shift_left(tmp, pos_to_shift);
            assert(first == tmp.begin());
            test_iota(first, last, pos_to_shift + 1, tmpSize);
        }
#endif // __cpp_lib_shift >= 202202L
    }

    fill_iota(tmp, tmpSize);
    for (int i = 0; i < 3; ++i) {
        test_iota(shift_left(tmp.begin(), tmp.end(), tmpSize + i), tmp.end(), 1, tmpSize);
    }

#if __cpp_lib_shift >= 202202L
    fill_iota(tmp, tmpSize);
    for (int i = 0; i < 3; ++i) {
        auto [first, last] = ranges::shift_left(tmp.begin(), tmp.end(), tmpSize + i);
        assert(first == tmp.begin());
        test_iota(last, tmp.end(), 1, tmpSize);
    }

    fill_iota(tmp, tmpSize);
    for (int i = 0; i < 3; ++i) {
        auto [first, last] = ranges::shift_left(tmp, tmpSize + i);
        assert(first == tmp.begin());
        test_iota(last, tmp.end(), 1, tmpSize);
    }
#endif // __cpp_lib_shift >= 202202L
}

template <typename Container>
void test_case_shift_right(const ptrdiff_t tmpSize) {
    Container tmp;
    fill_iota(tmp, tmpSize);

    for (ptrdiff_t pos_to_shift = 0; pos_to_shift < tmpSize; ++pos_to_shift) {
        fill_iota(tmp, tmpSize);
        test_iota(shift_right(tmp.begin(), tmp.end(), pos_to_shift), tmp.end(), 1, tmpSize - pos_to_shift);

#if __cpp_lib_shift >= 202202L
        {
            fill_iota(tmp, tmpSize);
            auto [first, last] = ranges::shift_right(tmp.begin(), tmp.end(), pos_to_shift);
            assert(last == tmp.end());
            test_iota(first, last, 1, tmpSize - pos_to_shift);
        }

        {
            fill_iota(tmp, tmpSize);
            auto [first, last] = ranges::shift_right(tmp, pos_to_shift);
            assert(last == tmp.end());
            test_iota(first, last, 1, tmpSize - pos_to_shift);
        }
#endif // __cpp_lib_shift >= 202202L
    }

    fill_iota(tmp, tmpSize);
    for (int i = 0; i < 3; ++i) {
        test_iota(tmp.begin(), shift_right(tmp.begin(), tmp.end(), tmpSize + i), 1, tmpSize);
    }

#if __cpp_lib_shift >= 202202L
    fill_iota(tmp, tmpSize);
    for (int i = 0; i < 3; ++i) {
        auto [first, last] = ranges::shift_right(tmp.begin(), tmp.end(), tmpSize + i);
        assert(last == tmp.end());
        test_iota(tmp.begin(), first, 1, tmpSize);
    }

    fill_iota(tmp, tmpSize);
    for (int i = 0; i < 3; ++i) {
        auto [first, last] = ranges::shift_right(tmp, tmpSize + i);
        assert(last == tmp.end());
        test_iota(tmp.begin(), first, 1, tmpSize);
    }
#endif // __cpp_lib_shift >= 202202L
}

int main() {
    for (auto sz = 0; sz != 10; ++sz) {
        test_case_shift_left<forward_list<MoveOnly<false>>>(sz);
        test_case_shift_left<list<MoveOnly<false>>>(sz);
        test_case_shift_left<vector<MoveOnly<false>>>(sz);

        test_case_shift_right<forward_list<MoveOnly<true>>>(sz);
        test_case_shift_right<list<MoveOnly<false>>>(sz);
        test_case_shift_right<vector<MoveOnly<false>>>(sz);
    }
}
