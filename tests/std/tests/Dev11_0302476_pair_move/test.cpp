// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

template <int A>
struct Cat {
    Cat() {}

    Cat(const Cat&) {}
    Cat& operator=(const Cat&) {
        return *this;
    }
    template <int B>
    Cat(const Cat<B>&) {}
    template <int B>
    Cat& operator=(const Cat<B>&) {
        return *this;
    }

    Cat(Cat&&) {
        abort();
    }

    Cat& operator=(Cat&&) {
        abort();
    }

    template <int B>
    Cat(Cat<B>&&) {
        abort();
    }

    template <int B>
    Cat& operator=(Cat<B>&&) {
        abort();
    }

    ~Cat() {}
};

template <typename Src, typename Dst>
void test() {
    remove_reference_t<tuple_element_t<0, Src>> s0;
    remove_reference_t<tuple_element_t<1, Src>> s1;

    Src src(s0, s1);
    Dst dst = move(src);
    dst     = move(src);
}

int main() {
    test<pair<Cat<1>&, Cat<2>&>, pair<Cat<1>&, Cat<2>&>>();
    test<pair<Cat<3>&, Cat<4>&>, pair<Cat<5>, Cat<6>>>();
    test<tuple<Cat<1>&, Cat<2>&>, tuple<Cat<1>&, Cat<2>&>>();
    test<tuple<Cat<3>&, Cat<4>&>, tuple<Cat<5>, Cat<6>>>();
    test<pair<Cat<3>&, Cat<4>&>, tuple<Cat<5>, Cat<6>>>();
}
