// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iterator>

#include "shared_test.hpp"

struct minimal_container {
    void begin() const {}
    void end() const {}
    void cbegin() const {}
    void cend() const {}
    void crbegin() const {}
    void crend() const {}
    void rbegin() const {}
    void rend() const {}
    size_t size() const {
        return 0;
    }
    void empty() const {}
    void data() const {}
};

void test_iterator() {
    minimal_container container;
    shared_test(container);
}
