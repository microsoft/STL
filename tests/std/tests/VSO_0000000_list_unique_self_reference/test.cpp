// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <forward_list>
#include <initializer_list>
#include <list>
#include <set>

using namespace std;

template <class Container>
void assert_equal(const Container& c, initializer_list<typename Container::value_type> expected) {
    assert(equal(c.begin(), c.end(), expected.begin(), expected.end()));
}

static set<const void*> g_alive_noisy;

struct noisy_int {
    int i;
    noisy_int() : i() {
        g_alive_noisy.insert(this);
    }
    /* implicit */ noisy_int(int i_) : i(i_) {
        g_alive_noisy.insert(this);
    }
    noisy_int(const noisy_int& other) : i(other.i) {
        g_alive_noisy.insert(this);
    }
    noisy_int& operator=(const noisy_int&) = delete;
    ~noisy_int() {
        g_alive_noisy.erase(this);
    }

    friend bool operator==(const noisy_int& lhs, const noisy_int& rhs) {
        return lhs.i == rhs.i;
    }

    friend bool operator!=(const noisy_int& lhs, const noisy_int& rhs) {
        return lhs.i != rhs.i;
    }
};

struct modal_compare {
    noisy_int* mode;
    explicit modal_compare(noisy_int* targetMode) : mode(targetMode) {}

    bool operator()(const noisy_int& lhs, const noisy_int& rhs) const noexcept {
        assert(g_alive_noisy.find(&lhs) != g_alive_noisy.end());
        assert(g_alive_noisy.find(&rhs) != g_alive_noisy.end());
        if (mode->i == 1) {
            return lhs == rhs;
        } else {
            return lhs != rhs;
        }
    }
};

template <class Container>
void test() {
    Container l{1, 1, 3, 4, 5, 6};
    auto it = l.begin();
    ++it;
    noisy_int* firstMode = &*it;
    ++it;
    noisy_int* secondMode = &*it;

    l.unique(modal_compare(firstMode));
    assert_equal(l, {1, 3, 4, 5, 6});

    l.unique(modal_compare(secondMode));
    assert_equal(l, {1});
}

int main() {
    test<forward_list<noisy_int>>();
    test<list<noisy_int>>();
    assert(g_alive_noisy.empty());
}
