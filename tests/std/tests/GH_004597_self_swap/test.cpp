// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <utility>

struct swap_counter {
    unsigned int* pcnt_ = nullptr;

    friend _CONSTEXPR20 void swap(swap_counter& lhs, swap_counter& rhs) noexcept {
        std::swap(lhs.pcnt_, rhs.pcnt_);
        if (lhs.pcnt_ != nullptr) {
            ++*lhs.pcnt_;
        }

        if (rhs.pcnt_ != nullptr) {
            ++*rhs.pcnt_;
        }
    }

    _CONSTEXPR20 bool operator==(unsigned int x) const {
        return *pcnt_ == x;
    }
};


// Test GH-4597 "<utility>: Side effects in self-swaps of pair are skipped"
_CONSTEXPR20 bool test_gh_4597() {
    {
        unsigned int cnt{};
        std::pair<swap_counter, int> pr{swap_counter{&cnt}, 0};
        pr.swap(pr);
        assert(cnt == 2u);
        assert(pr.first == 2u);
        assert(pr.second == 0);
    }

    {
        unsigned int cnt{};
        std::pair<swap_counter, int> p1{swap_counter{&cnt}, 0};
        std::pair<swap_counter, int> p2{swap_counter{&cnt}, 1};
        p1.swap(p2);
        assert(cnt == 2u);
        assert(p1.first == 2u);
        assert(p1.second == 1);
        assert(p2.first == 2u);
        assert(p2.second == 0);
    }

    {
        unsigned int c1{};
        unsigned int c2{2};
        std::pair<swap_counter, int> p1{swap_counter{&c1}, 1};
        std::pair<swap_counter, int> p2{swap_counter{&c2}, 3};
        p1.swap(p2);
        assert(c1 == 1u);
        assert(c2 == 3u);
        assert(p1.first == 3u);
        assert(p1.second == 3);
        assert(p2.first == 1u);
        assert(p2.second == 1);
    }

#if _HAS_CXX23
    {
        unsigned int c1{};
        unsigned int c2{2};
        int i1 = 1;
        int i2 = 3;
        swap_counter s1{&c1};
        swap_counter s2{&c2};
        const std::pair<swap_counter&, int&> p1{s1, i1};
        const std::pair<swap_counter&, int&> p2{s2, i2};
        p1.swap(p2);
        assert(c1 == 1u);
        assert(c2 == 3u);
        assert(p1.first == 3u);
        assert(p1.second == 3);
        assert(p2.first == 1u);
        assert(p2.second == 1);
    }
#endif // _HAS_CXX23

    return true;
}

int main() {
#if _HAS_CXX20
    static_assert(test_gh_4597());
#endif // _HAS_CXX20
    assert(test_gh_4597());
}
