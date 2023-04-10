// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_AUTO_PTR_ETC 1
#include <cassert>
#include <memory>
#include <utility>

using namespace std;

class A : public enable_shared_from_this<A> {
public:
    explicit A(const int n) : m_n(n) {}

    int num() const {
        return m_n;
    }

private:
    int m_n;
};

int main() {
    {
        auto_ptr<A> a(new A(4));
        shared_ptr<A> s(move(a));
        shared_ptr<A> t = s->shared_from_this();
        assert(t->num() == 4);
    }

    {
        auto_ptr<A> a(new A(7));
        shared_ptr<A> s;
        s               = move(a);
        shared_ptr<A> t = s->shared_from_this();
        assert(t->num() == 7);
    }
}
