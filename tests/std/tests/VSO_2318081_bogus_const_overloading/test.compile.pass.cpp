// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <iterator>
#include <memory>
using namespace std;

// VSO-2318081 "[RWC][prod/fe][Regression] 4 projects failed with error C2440:
// 'initializing': cannot convert from 'const llvm::Value *' to '_Ty'"

struct Thing {};

struct BogusInIt {
    using iterator_category = input_iterator_tag;
    using value_type        = Thing*;
    using difference_type   = ptrdiff_t;
    using pointer           = void;
    using reference         = Thing*;

    BogusInIt& operator++();
    BogusInIt operator++(int);
    friend bool operator==(const BogusInIt&, const BogusInIt&);
    friend bool operator!=(const BogusInIt&, const BogusInIt&);

    // This single overload would be conforming:
    // Thing* operator*() const;

    // N4993 [iterator.cpp17.general]/1 and [tab:inputiterator] forbid overloading operator*()
    // with varying return types, but uninitialized_meow() tolerated this before GH-5135.

    // See LLVM-119084, reported on 2024-12-07. After that has been fixed and propagated throughout the ecosystem,
    // we should consider making the STL strictly reject such bogus iterators and removing this test coverage.
    Thing* operator*();
    const Thing* operator*() const;
};

void test() {
    BogusInIt src{};
    Thing** dest{nullptr};

    uninitialized_copy(src, src, dest);
    uninitialized_copy_n(src, 0, dest);
#if _HAS_CXX17
    uninitialized_move(src, src, dest);
    uninitialized_move_n(src, 0, dest);
#endif // _HAS_CXX17
}
