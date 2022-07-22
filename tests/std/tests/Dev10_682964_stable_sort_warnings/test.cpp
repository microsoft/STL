// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

using namespace std;

#if _HAS_CXX17 && !defined(_M_CEE)
#define HAS_PARALLEL_ALGORITHMS 1
#else
#define HAS_PARALLEL_ALGORITHMS 0
#endif // _HAS_CXX17 && !defined(_M_CEE)

#if HAS_PARALLEL_ALGORITHMS
#include <execution>
#endif // HAS_PARALLEL_ALGORITHMS

int main() {
    {
        vector<string> v = {"cute", "fluffy", "kittens", "ugly", "stupid", "puppies"};
#if HAS_PARALLEL_ALGORITHMS
        auto p = v;
#endif

        stable_sort(v.begin(), v.end());

        const vector<string> correct = {"cute", "fluffy", "kittens", "puppies", "stupid", "ugly"};

        assert(v == correct);

#if HAS_PARALLEL_ALGORITHMS
        stable_sort(execution::par, p.begin(), p.end());

        assert(p == correct);
#endif
    }

    {
        vector<string> v = {"cute", "fluffy", "kittens", "ugly", "stupid", "puppies"};
#if HAS_PARALLEL_ALGORITHMS
        auto p = v;
#endif

        const auto cmp = [](const string& l, const string& r) { return l.size() < r.size(); };
        stable_sort(v.begin(), v.end(), cmp);

        const vector<string> correct = {"cute", "ugly", "fluffy", "stupid", "kittens", "puppies"};

        assert(v == correct);

#if HAS_PARALLEL_ALGORITHMS
        stable_sort(execution::par, p.begin(), p.end(), cmp);

        assert(p == correct);
#endif
    }


    {
        // Also test DevDiv-957501 "<algorithm>: stable_sort calls self-move-assignment operator".

        class NoSelfMove {
        public:
            explicit NoSelfMove(unsigned int n) : m_n(n) {}

            NoSelfMove(const NoSelfMove&) = default;

            NoSelfMove& operator=(const NoSelfMove&) = default;

            NoSelfMove(NoSelfMove&& other) : m_n(other.m_n) {}

            NoSelfMove& operator=(NoSelfMove&& other) {
                assert(this != &other);
                m_n = other.m_n;
                return *this;
            }

            bool operator<(const NoSelfMove& other) const {
                return m_n < other.m_n;
            }

        private:
            unsigned int m_n;
        };

        mt19937 urng(1729);

        for (int k = 0; k < 2000; ++k) {
            vector<NoSelfMove> v;
            for (int i = 0; i < k; ++i) {
                v.emplace_back(urng());
            }

#if HAS_PARALLEL_ALGORITHMS
            auto p = v;
#endif

            stable_sort(v.begin(), v.end());
            assert(is_sorted(v.begin(), v.end()));

#if HAS_PARALLEL_ALGORITHMS
            stable_sort(execution::par, p.begin(), p.end());
            assert(is_sorted(p.begin(), p.end()));
#endif
        }
    }
}
