// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace std;

vector<pair<string, int>> results;

class Cat {
public:
    explicit Cat(int n);
    void meow();
    void vanish0();
    void vanish1();
    void vanish2();
    void vanish3();
    ~Cat();

private:
    Cat(const Cat&);
    Cat& operator=(const Cat&);

    int m_n;
    shared_ptr<Cat> m_p;
};

void deleter(Cat* p) {
    delete p;
}

Cat::Cat(const int n) : m_n(n), m_p() {
    results.emplace_back("Cat::Cat()", m_n);
}

void Cat::meow() {
    results.emplace_back("Cat::meow()", m_n);

    m_p.reset(this);
}

void Cat::vanish0() {
    results.emplace_back("Cat::vanish0()", m_n);

    m_p.reset();
}

void Cat::vanish1() {
    results.emplace_back("Cat::vanish1()", m_n);

    m_p.reset(new Cat(11));
}

void Cat::vanish2() {
    results.emplace_back("Cat::vanish2()", m_n);

    m_p.reset(new Cat(22), deleter);
}

void Cat::vanish3() {
    results.emplace_back("Cat::vanish3()", m_n);

    m_p.reset(new Cat(33), deleter, allocator<int>());
}

Cat::~Cat() {
    results.emplace_back("Cat::~Cat()", m_n);

    assert(m_p.get() != this);
}

class Kitten {
public:
    explicit Kitten(int n);
    void meow();
    void vanish0();
    void vanish1();
    ~Kitten();

private:
    Kitten(const Kitten&);
    Kitten& operator=(const Kitten&);

    int m_n;
    unique_ptr<Kitten> m_p;
};

Kitten::Kitten(const int n) : m_n(n), m_p() {
    results.emplace_back("Kitten::Kitten()", m_n);
}

void Kitten::meow() {
    results.emplace_back("Kitten::meow()", m_n);

    m_p.reset(this);
}

void Kitten::vanish0() {
    results.emplace_back("Kitten::vanish0()", m_n);

    m_p.reset();
}

void Kitten::vanish1() {
    results.emplace_back("Kitten::vanish1()", m_n);

    m_p.reset(new Kitten(1234));
}

Kitten::~Kitten() {
    results.emplace_back("Kitten::~Kitten()", m_n);

    assert(m_p.get() != this);
}

int main() {
    // Dev10-635436 "shared_ptr: reset() must behave as if it is implemented with swap()"
    results.emplace_back("BEGIN", 0);

    Cat* p0 = new Cat(1729);
    p0->meow();
    p0->vanish0();

    results.emplace_back("END", 0);

    results.emplace_back("BEGIN", 1);

    Cat* p1 = new Cat(1729);
    p1->meow();
    p1->vanish1();

    results.emplace_back("END", 1);

    results.emplace_back("BEGIN", 2);

    Cat* p2 = new Cat(1729);
    p2->meow();
    p2->vanish2();

    results.emplace_back("END", 2);

    results.emplace_back("BEGIN", 3);

    Cat* p3 = new Cat(1729);
    p3->meow();
    p3->vanish3();

    results.emplace_back("END", 3);


    // DevDiv-523246 "std::unique_ptr deletes owned object before resetting pointer rather than after."
    results.emplace_back("BEGIN", 4);

    Kitten* p4 = new Kitten(257);
    p4->meow();
    p4->vanish0();

    results.emplace_back("END", 4);

    results.emplace_back("BEGIN", 5);

    Kitten* p5 = new Kitten(65537);
    p5->meow();
    p5->vanish1();

    results.emplace_back("END", 5);


    vector<pair<string, int>> correct;

    correct.emplace_back("BEGIN", 0);
    correct.emplace_back("Cat::Cat()", 1729);
    correct.emplace_back("Cat::meow()", 1729);
    correct.emplace_back("Cat::vanish0()", 1729);
    correct.emplace_back("Cat::~Cat()", 1729);
    correct.emplace_back("END", 0);
    correct.emplace_back("BEGIN", 1);
    correct.emplace_back("Cat::Cat()", 1729);
    correct.emplace_back("Cat::meow()", 1729);
    correct.emplace_back("Cat::vanish1()", 1729);
    correct.emplace_back("Cat::Cat()", 11);
    correct.emplace_back("Cat::~Cat()", 1729);
    correct.emplace_back("Cat::~Cat()", 11);
    correct.emplace_back("END", 1);
    correct.emplace_back("BEGIN", 2);
    correct.emplace_back("Cat::Cat()", 1729);
    correct.emplace_back("Cat::meow()", 1729);
    correct.emplace_back("Cat::vanish2()", 1729);
    correct.emplace_back("Cat::Cat()", 22);
    correct.emplace_back("Cat::~Cat()", 1729);
    correct.emplace_back("Cat::~Cat()", 22);
    correct.emplace_back("END", 2);
    correct.emplace_back("BEGIN", 3);
    correct.emplace_back("Cat::Cat()", 1729);
    correct.emplace_back("Cat::meow()", 1729);
    correct.emplace_back("Cat::vanish3()", 1729);
    correct.emplace_back("Cat::Cat()", 33);
    correct.emplace_back("Cat::~Cat()", 1729);
    correct.emplace_back("Cat::~Cat()", 33);
    correct.emplace_back("END", 3);
    correct.emplace_back("BEGIN", 4);
    correct.emplace_back("Kitten::Kitten()", 257);
    correct.emplace_back("Kitten::meow()", 257);
    correct.emplace_back("Kitten::vanish0()", 257);
    correct.emplace_back("Kitten::~Kitten()", 257);
    correct.emplace_back("END", 4);
    correct.emplace_back("BEGIN", 5);
    correct.emplace_back("Kitten::Kitten()", 65537);
    correct.emplace_back("Kitten::meow()", 65537);
    correct.emplace_back("Kitten::vanish1()", 65537);
    correct.emplace_back("Kitten::Kitten()", 1234);
    correct.emplace_back("Kitten::~Kitten()", 65537);
    correct.emplace_back("Kitten::~Kitten()", 1234);
    correct.emplace_back("END", 5);

    assert(results == correct);
}
