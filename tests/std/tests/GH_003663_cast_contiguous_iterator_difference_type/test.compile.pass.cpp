// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <iterator>

class ContiguousIterator {
public:
    using value_type        = int;
    using difference_type   = int;
    using iterator_category = std::contiguous_iterator_tag;

    value_type* operator->() const;
    value_type& operator*() const;
    value_type& operator[](difference_type) const;
    ContiguousIterator& operator++();
    ContiguousIterator operator++(int);
    ContiguousIterator& operator--();
    ContiguousIterator operator--(int);
    ContiguousIterator& operator+=(difference_type);
    ContiguousIterator& operator-=(difference_type);
    friend auto operator<=>(ContiguousIterator, ContiguousIterator) = default;
    friend difference_type operator-(ContiguousIterator, ContiguousIterator);
    friend ContiguousIterator operator+(ContiguousIterator, difference_type);
    friend ContiguousIterator operator-(ContiguousIterator, difference_type);
    friend ContiguousIterator operator+(difference_type, ContiguousIterator);
};

static_assert(std::contiguous_iterator<ContiguousIterator>);

// GH-3663 <algorithm>/<iterator>: contiguous iterators with non-ptrdiff_t difference types
void test() {
    int* p = nullptr;
    std::copy(p, p, ContiguousIterator{});
    std::sort(ContiguousIterator{}, ContiguousIterator{});
}
