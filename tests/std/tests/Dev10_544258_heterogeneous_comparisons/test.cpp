// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Dev10-544258 "STL/xutility: Debug validation of predicate classes fail on bound predicates"
// DevDiv-813065 "<algorithm>: C++ stdlib comparator debugging breaks compile of custom Comparators in std::equal_range
// family of functions"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

using namespace std;

struct Cat {
    int c;

    explicit Cat(int x) : c(x) {}
};

struct Dog {
    int d;

    explicit Dog(int x) : d(x) {}
};

struct AssignableEither {
    AssignableEither& operator=(const Cat&) {
        return *this;
    }
    AssignableEither& operator=(const Dog&) {
        return *this;
    }
};

struct CatLessDog {
    bool operator()(const Cat& cat, const Dog& dog) const {
        return cat.c < dog.d;
    }
};

struct DogLessCat {
    bool operator()(const Dog& dog, const Cat& cat) const {
        return dog.d < cat.c;
    }
};

struct Both {
    bool operator()(const Cat& cat, const Dog& dog) const {
        return cat.c < dog.d;
    }

    bool operator()(const Dog& dog, const Cat& cat) const {
        return dog.d < cat.c;
    }
};

int main() {
    const Cat cats[] = {
        Cat(10), Cat(20), Cat(30), Cat(40), Cat(50), Cat(50), Cat(50), Cat(60), Cat(70), Cat(80), Cat(90)};

    assert(lower_bound(begin(cats), end(cats), Dog(0), CatLessDog()) == begin(cats));
    assert(lower_bound(begin(cats), end(cats), Dog(33), CatLessDog()) == begin(cats) + 3);
    assert(lower_bound(begin(cats), end(cats), Dog(50), CatLessDog()) == begin(cats) + 4);
    assert(lower_bound(begin(cats), end(cats), Dog(77), CatLessDog()) == begin(cats) + 9);
    assert(lower_bound(begin(cats), end(cats), Dog(99), CatLessDog()) == end(cats));

    assert(upper_bound(begin(cats), end(cats), Dog(0), DogLessCat()) == begin(cats));
    assert(upper_bound(begin(cats), end(cats), Dog(33), DogLessCat()) == begin(cats) + 3);
    assert(upper_bound(begin(cats), end(cats), Dog(50), DogLessCat()) == begin(cats) + 7);
    assert(upper_bound(begin(cats), end(cats), Dog(77), DogLessCat()) == begin(cats) + 9);
    assert(upper_bound(begin(cats), end(cats), Dog(99), DogLessCat()) == end(cats));

    assert(equal_range(begin(cats), end(cats), Dog(0), Both()) == make_pair(begin(cats), begin(cats)));
    assert(equal_range(begin(cats), end(cats), Dog(33), Both()) == make_pair(begin(cats) + 3, begin(cats) + 3));
    assert(equal_range(begin(cats), end(cats), Dog(50), Both()) == make_pair(begin(cats) + 4, begin(cats) + 7));
    assert(equal_range(begin(cats), end(cats), Dog(77), Both()) == make_pair(begin(cats) + 9, begin(cats) + 9));
    assert(equal_range(begin(cats), end(cats), Dog(99), Both()) == make_pair(end(cats), end(cats)));

    assert(!binary_search(begin(cats), end(cats), Dog(0), Both()));
    assert(!binary_search(begin(cats), end(cats), Dog(33), Both()));
    assert(binary_search(begin(cats), end(cats), Dog(50), Both()));
    assert(!binary_search(begin(cats), end(cats), Dog(77), Both()));
    assert(binary_search(begin(cats), end(cats), Dog(80), Both()));
    assert(!binary_search(begin(cats), end(cats), Dog(99), Both()));

    // also test VSO-488479 "std::includes unable to be compiled with /MDd"
    const Dog dogs[] = {Dog(1729)};
    AssignableEither results[size(cats) + size(dogs)];
    merge(begin(cats), end(cats), begin(dogs), end(dogs), results, Both());
    (void) includes(begin(cats), end(cats), begin(dogs), end(dogs), Both());
    set_union(begin(cats), end(cats), begin(dogs), end(dogs), results, Both());
    set_intersection(begin(cats), end(cats), begin(dogs), end(dogs), results, Both());
    set_difference(begin(cats), end(cats), begin(dogs), end(dogs), results, Both());
    set_symmetric_difference(begin(cats), end(cats), begin(dogs), end(dogs), results, Both());
}
