// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <string>
#include <utility>

using namespace std;

class Base {
public:
    Base() {}
    virtual ~Base() {}

    virtual string str() const {
        return "Base";
    }

private:
    Base(const Base&);
    Base& operator=(const Base&);
};

class Derived : public Base {
public:
    virtual string str() const {
        return "Derived";
    }
};

int main() {
    {
        // move ctor

        shared_ptr<int> src(new int(1729));

        assert(src && *src == 1729);

        shared_ptr<int> dest(move(src));

        assert(!src);

        assert(dest && *dest == 1729);
    }

    {
        // move assign

        shared_ptr<int> src(new int(123));

        shared_ptr<int> dest(new int(888));

        assert(src && *src == 123);

        assert(dest && *dest == 888);

        dest = move(src);

        assert(!src);

        assert(dest && *dest == 123);
    }

    {
        // template move ctor

        shared_ptr<Derived> src(new Derived);

        assert(src && src->str() == "Derived");

        shared_ptr<Base> dest(move(src));

        assert(!src);

        assert(dest && dest->str() == "Derived");
    }

    {
        // template move assign

        shared_ptr<Derived> src(new Derived);

        shared_ptr<Base> dest(new Base);

        assert(src && src->str() == "Derived");

        assert(dest && dest->str() == "Base");

        dest = move(src);

        assert(!src);

        assert(dest && dest->str() == "Derived");
    }
}
