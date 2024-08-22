// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <iterator>

class ContiguousIterator {
public:
    using value_type        = int;
    using difference_type   = int;
    using element_type      = int;
    using iterator_category = std::contiguous_iterator_tag;
    int* operator->() const;
    int& operator*() const;
    int& operator[](int) const;
    ContiguousIterator& operator++();
    ContiguousIterator operator++(int);
    ContiguousIterator& operator--();
    ContiguousIterator operator--(int);
    ContiguousIterator& operator+=(int);
    ContiguousIterator& operator-=(int);
    friend auto operator<=>(ContiguousIterator, ContiguousIterator) = default;
    friend int operator-(ContiguousIterator, ContiguousIterator);
    friend ContiguousIterator operator+(ContiguousIterator, int);
    friend ContiguousIterator operator-(ContiguousIterator, int);
    friend ContiguousIterator operator+(int, ContiguousIterator);
};

static_assert(std::contiguous_iterator<ContiguousIterator>);

// GH-3663 <algorithm>/<iterator>: contiguous iterators with non-ptrdiff_t difference types
void test() {
    int* p = nullptr;
    std::copy(p, p, ContiguousIterator{});
    std::sort(ContiguousIterator{}, ContiguousIterator{});
}
