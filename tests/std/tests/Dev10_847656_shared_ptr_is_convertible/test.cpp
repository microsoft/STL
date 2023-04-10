// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>

using namespace std;

class Cat {
public:
    Cat() {}
    virtual int meow() const = 0;
    virtual ~Cat() {}

private:
    Cat(const Cat&);
    Cat& operator=(const Cat&);
};

struct Lion : public Cat {
    virtual int meow() const {
        return 6;
    }
};

struct Tiger : public Cat {
    virtual int meow() const {
        return 7;
    }
};


class Planet {
public:
    Planet() {}
    virtual int orbit() const = 0;
    virtual ~Planet() {}

private:
    Planet(const Planet&);
    Planet& operator=(const Planet&);
};

struct Jupiter : public Planet {
    virtual int orbit() const {
        return 8;
    }
};

struct Saturn : public Planet {
    virtual int orbit() const {
        return 9;
    }
};


int function_taking_shared(const shared_ptr<Cat>& p) {
    return p->meow() * 10 + 1;
}

int function_taking_shared(const shared_ptr<Planet>& p) {
    return p->orbit() * 10 + 2;
}


int function_taking_weak(const weak_ptr<Cat>& p) {
    return p.lock()->meow() * 10 + 3;
}

int function_taking_weak(const weak_ptr<Planet>& p) {
    return p.lock()->orbit() * 10 + 4;
}


int main() {
    shared_ptr<Lion> sp1(new Lion);
    shared_ptr<Tiger> sp2(new Tiger);
    shared_ptr<Jupiter> sp3(new Jupiter);
    shared_ptr<Saturn> sp4(new Saturn);

    // Test shared_ptr(const shared_ptr<Y>&)
    assert(function_taking_shared(sp1) == 61);
    assert(function_taking_shared(sp2) == 71);
    assert(function_taking_shared(sp3) == 82);
    assert(function_taking_shared(sp4) == 92);

    // Test shared_ptr(shared_ptr<Y>&&)
    assert(function_taking_shared(make_shared<Lion>()) == 61);
    assert(function_taking_shared(make_shared<Tiger>()) == 71);
    assert(function_taking_shared(make_shared<Jupiter>()) == 82);
    assert(function_taking_shared(make_shared<Saturn>()) == 92);

    // Test shared_ptr(unique_ptr<Y, D>&&)
    assert(function_taking_shared(make_unique<Lion>()) == 61);
    assert(function_taking_shared(make_unique<Tiger>()) == 71);
    assert(function_taking_shared(make_unique<Jupiter>()) == 82);
    assert(function_taking_shared(make_unique<Saturn>()) == 92);

    // Test weak_ptr(const shared_ptr<Y>&)
    assert(function_taking_weak(sp1) == 63);
    assert(function_taking_weak(sp2) == 73);
    assert(function_taking_weak(sp3) == 84);
    assert(function_taking_weak(sp4) == 94);

    weak_ptr<Lion> wp1    = sp1;
    weak_ptr<Tiger> wp2   = sp2;
    weak_ptr<Jupiter> wp3 = sp3;
    weak_ptr<Saturn> wp4  = sp4;

    // Test weak_ptr(const weak_ptr<Y>&)
    assert(function_taking_weak(wp1) == 63);
    assert(function_taking_weak(wp2) == 73);
    assert(function_taking_weak(wp3) == 84);
    assert(function_taking_weak(wp4) == 94);

    // Test weak_ptr(weak_ptr<Y>&&)
    assert(function_taking_weak(weak_ptr<Lion>(sp1)) == 63);
    assert(function_taking_weak(weak_ptr<Tiger>(sp2)) == 73);
    assert(function_taking_weak(weak_ptr<Jupiter>(sp3)) == 84);
    assert(function_taking_weak(weak_ptr<Saturn>(sp4)) == 94);
}
