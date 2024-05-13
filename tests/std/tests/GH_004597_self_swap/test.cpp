// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <utility>

// Test GH-4597 "<utility>: Side effects in self-swaps of pair are skipped"
struct swap_counter {
    unsigned int* pcnt_ = nullptr;

    friend constexpr void swap(swap_counter& lhs, swap_counter& rhs) noexcept {
        std::swap(lhs.pcnt_, rhs.pcnt_);
        if (lhs.pcnt_ != nullptr) {
            ++(*lhs.pcnt_);
        }
        if (rhs.pcnt_ != nullptr) {
            ++(*rhs.pcnt_);
        }
    }

    constexpr bool operator==(unsigned int x) const {
        return *pcnt_ == x;
    }
};


void test_gh_4595() {
    {
        static_assert([] {
            unsigned int cnt{};
            std::pair<swap_counter, int> pr{swap_counter{&cnt}, 0};
            pr.swap(pr);
            return (cnt == 2u) && (pr.first == 2u) && (pr.second == 0);
        }());
    }

    {
        static_assert([] {
            unsigned int cnt{};
            std::pair<swap_counter, int> p1{swap_counter{&cnt}, 0};
            std::pair<swap_counter, int> p2{swap_counter{&cnt}, 1};
            p1.swap(p2);
            return (cnt == 2u) && (p1.first == 2u) && (p1.second == 1) && (p2.first == 2u) && (p2.second == 0);
        }());
    }

    {
        static_assert([] {
            unsigned int c1{};
            unsigned int c2{2};
            std::pair<swap_counter, int> p1{swap_counter{&c1}, 1};
            std::pair<swap_counter, int> p2{swap_counter{&c2}, 3};
            p1.swap(p2);
            return (c1 == 1u) && (c2 == 3u) && (p1.first == 3u) && (p1.second == 3) && (p2.first == 1u)
                && (p2.second == 1);
        }());
    }
}

int main() {
    test_gh_4595();
}
