// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <stack>
#define TEST_NAME "<stack>"

#include "tdefs.h"
#include <deque>
#include <list>
#include <stack>
#include <stddef.h>
#include <vector>

void test_main() { // test basic workings of stack definitions
    typedef STD allocator<char> Myal;
    typedef STD deque<char, Myal> Myimpl;
    typedef STD stack<char, Myimpl> Mycont;
    typedef STD list<char, Myal> Myimpl2;
    typedef STD stack<char, Myimpl2> Mycont2;
    typedef STD vector<char, Myal> Myimpl3;
    typedef STD stack<char, Myimpl3> Mycont3;
    struct Mystack : public Mycont { // expose protected stuff
        const Myimpl* get_cont() const { // return pointer to container
            return &c;
        }
    };

    Mycont::container_type* p_cont = (Myimpl*) nullptr;
    Mycont::value_type* p_val      = (char*) nullptr;
    Mycont::size_type* p_size      = (CSTD size_t*) nullptr;

    p_cont = p_cont; // to quiet diagnostics
    p_val  = p_val;
    p_size = p_size;

    Myimpl c0(3, 'x');
    Mycont v0(c0), v0a;
    Mycont2 v1;
    Mycont3 v2;
    CHECK_INT(v0.size(), 3);
    CHECK_INT(v0.top(), 'x');
    CHECK(v0a.empty());
    v0 = v0a;
    v0.push('a');
    CHECK_INT(v0.size(), 1);
    CHECK_INT(v0.top(), 'a');
    v0.push('b');
    CHECK_INT(v0.size(), 2);
    CHECK_INT(v0.top(), 'b');
    v0.push('c');
    CHECK_INT(v0.size(), 3);
    CHECK_INT(v0.top(), 'c');
    CHECK(v0 == v0);
    CHECK(v0a < v0);
    CHECK(v0 != v0a);
    CHECK(v0 > v0a);
    CHECK(v0a <= v0);
    CHECK(v0 >= v0a);
    v0.pop();
    CHECK_INT(v0.top(), 'b');
    v0.pop();
    CHECK_INT(v0.top(), 'a');
    v0.pop();
    CHECK(v0.empty());

    {
        Myimpl c10(3, 'x');
        Mycont v10(STD move(c10));
        CHECK_INT(v10.size(), 3);
        CHECK_INT(v10.top(), 'x');
        CHECK(c10.empty());

        Mycont v11;
        v11 = STD move(v10);
        CHECK_INT(v10.size(), 0);
        CHECK_INT(v11.size(), 3);

        Mycont v12(STD move(v11));
        CHECK_INT(v11.size(), 0);
        CHECK_INT(v12.size(), 3);

        v11.swap(v12);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v12.size(), 0);
    }

    { // test emplace
        typedef STD deque<Movable_int> Myimpl0;
        typedef STD stack<Movable_int, Myimpl0> Mycont0;
        Mycont0 v13;

        v13.emplace();
        CHECK_INT(v13.top().val, 0);
        v13.emplace(2);
        CHECK_INT(v13.top().val, 2);
        v13.emplace(3, 2);
        CHECK_INT(v13.top().val, 0x32);
        v13.emplace(4, 3, 2);
        CHECK_INT(v13.top().val, 0x432);
        v13.emplace(5, 4, 3, 2);
        CHECK_INT(v13.top().val, 0x5432);
        v13.emplace(6, 5, 4, 3, 2);
        CHECK_INT(v13.top().val, 0x65432);
    }

    typedef STD uses_allocator<Mycont, STD allocator<int>> stack_uses_allocator;
    CHECK_INT(stack_uses_allocator::value, true);
}
