// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <queue>
#define TEST_NAME "<queue>"

#include "tdefs.h"
#include <deque>
#include <functional>
#include <list>
#include <queue>
#include <stddef.h>
#include <vector>

void test_queue() { // test queue
    typedef STD allocator<char> Myal;
    typedef STD deque<char, Myal> Myimpl;
    typedef STD queue<char, Myimpl> Mycont;
    typedef STD list<char, Myal> Myimpl2;
    typedef STD queue<char, Myimpl2> Mycont2;
    struct Myqueue : public Mycont { // expose protected stuff
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
    CHECK_INT(v0.size(), 3);
    CHECK_INT(v0.front(), 'x');
    CHECK(v0a.empty());
    v0 = v0a;
    v0.push('a');
    CHECK_INT(v0.size(), 1);
    CHECK_INT(v0.front(), 'a');
    CHECK_INT(v0.back(), 'a');
    v0.push('c');
    CHECK_INT(v0.size(), 2);
    CHECK_INT(v0.front(), 'a');
    CHECK_INT(v0.back(), 'c');
    v0.push('b');
    CHECK_INT(v0.size(), 3);
    CHECK_INT(v0.front(), 'a');
    CHECK_INT(v0.back(), 'b');
    CHECK(v0 == v0);
    CHECK(v0a < v0);
    CHECK(v0 != v0a);
    CHECK(v0 > v0a);
    CHECK(v0a <= v0);
    CHECK(v0 >= v0a);
    v0.pop();
    CHECK_INT(v0.front(), 'c');
    v0.pop();
    CHECK_INT(v0.front(), 'b');
    v0.pop();
    CHECK(v0.empty());

    {
        Myal al;
        Myimpl c20(3, 'x');
        Mycont v20(c20, al), v20a(al);
        CHECK_INT(v20.size(), 3);
        CHECK_INT(v20.front(), 'x');
        CHECK_INT(v20a.size(), 0);
        Mycont v21(v20, al);
        CHECK_INT(v21.size(), 3);
        CHECK_INT(v21.front(), 'x');

        typedef STD uses_allocator<Mycont, STD allocator<int>> queue_uses_allocator;
        CHECK_INT(queue_uses_allocator::value, true);
    }

    {
        Myal al;
        Myimpl c10(3, 'x');
        Mycont v10(STD move(c10));
        CHECK_INT(v10.size(), 3);
        CHECK_INT(v10.front(), 'x');
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

        Myimpl c10a(3, 'x');
        Mycont v10a(STD move(c10a), al);
        CHECK_INT(v10a.size(), 3);
        CHECK_INT(v10a.front(), 'x');
        CHECK(c10a.empty());

        Mycont v11a(STD move(v10a), al);
        CHECK_INT(v10a.size(), 0);
        CHECK_INT(v11a.size(), 3);
    }

    { // test emplace
        typedef STD deque<Movable_int> Myimpl0;
        typedef STD queue<Movable_int, Myimpl0> Mycont0;
        Mycont0 v13;

        v13.emplace();
        CHECK_INT(v13.back().val, 0);
        v13.emplace(2);
        CHECK_INT(v13.back().val, 2);
        v13.emplace(3, 2);
        CHECK_INT(v13.back().val, 0x32);
        v13.emplace(4, 3, 2);
        CHECK_INT(v13.back().val, 0x432);
        v13.emplace(5, 4, 3, 2);
        CHECK_INT(v13.back().val, 0x5432);
        v13.emplace(6, 5, 4, 3, 2);
        CHECK_INT(v13.back().val, 0x65432);
    }
}

void test_priority_queue() { // test priority_queue
    typedef STD allocator<char> Myal;
    typedef STD less<char> Mypred;
    typedef STD vector<char, Myal> Myimpl;
    typedef STD priority_queue<char, Myimpl, Mypred> Mycont;
    typedef STD deque<char, Myal> Myimpl2;
    typedef STD priority_queue<char, Myimpl2, Mypred> Mycont2;
    struct Myqueue : public Mycont { // expose protected stuff
        const Myimpl* get_cont() const { // return pointer to container
            return &c;
        }

        const Mypred* get_pred() const { // return pointer to predicate
            return &comp;
        }
    };

    Mycont::container_type* p_cont = (Myimpl*) nullptr;
    Mycont::value_type* p_val      = (char*) nullptr;
    Mycont::size_type* p_size      = (CSTD size_t*) nullptr;

    p_cont = p_cont; // to quiet diagnostics
    p_val  = p_val;
    p_size = p_size;

    Mypred pr;
    char carr[] = "acb";
    Myimpl c0(3, 'x');
    Mycont v0(pr, c0), v0a(pr), v0b;
    Mycont2 v1;
    const Mycont v2(carr, carr + 3), v2a(carr, carr + 3, pr), v2b(carr, carr + 3, pr, c0);
    CHECK_INT(v0.size(), 3);
    CHECK_INT(v0.top(), 'x');
    CHECK(v0a.empty());
    v0 = v0a;
    CHECK_INT(v2.size(), 3);
    CHECK_INT(v2.top(), 'c');
    CHECK_INT(v2a.size(), 3);
    CHECK_INT(v2a.top(), 'c');
    CHECK_INT(v2b.size(), 6);
    CHECK_INT(v2b.top(), 'x');
    v0.push('a');
    CHECK_INT(v0.size(), 1);
    CHECK_INT(v0.top(), 'a');
    v0.push('c');
    CHECK_INT(v0.size(), 2);
    CHECK_INT(v0.top(), 'c');
    v0.push('b');
    CHECK_INT(v0.size(), 3);
    CHECK_INT(v0.top(), 'c');
    v0.pop();
    CHECK_INT(v0.top(), 'b');
    v0.pop();
    CHECK_INT(v0.top(), 'a');
    v0.pop();
    CHECK(v0.empty());

    {
        Myal al;
        Myimpl2 c20(3, 'x');
        Mycont2 v20(pr, STD move(c20), al), v20a(al), v20b(pr, al);
        CHECK_INT(v20.size(), 3);
        CHECK_INT(v20.top(), 'x');
        CHECK_INT(v20a.size(), 0);
        CHECK_INT(v20b.size(), 0);
        Mycont2 v21(v20, al);
        CHECK_INT(v21.size(), 3);
        CHECK_INT(v21.top(), 'x');

        typedef STD uses_allocator<Mycont, STD allocator<int>> priority_queue_uses_allocator;
        CHECK_INT(priority_queue_uses_allocator::value, true);
    }

    {
        Myal al;
        Myimpl2 c10(3, 'x');
        Mycont2 v10(pr, STD move(c10));
        CHECK_INT(v10.size(), 3);
        CHECK_INT(v10.top(), 'x');
        CHECK(c10.empty());

        Myimpl2 c10b(3, 'x');
        Mycont2 v12b(carr, carr + 3, pr, STD move(c10b));
        CHECK_INT(v12b.size(), 6);
        CHECK_INT(v12b.top(), 'x');

        Mycont2 v11;
        v11 = STD move(v10);
        CHECK_INT(v10.size(), 0);
        CHECK_INT(v11.size(), 3);

        Mycont2 v12(STD move(v11));
        CHECK_INT(v11.size(), 0);
        CHECK_INT(v12.size(), 3);

        v11.swap(v12);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v12.size(), 0);

        Myimpl2 c10a(3, 'x');
        Mycont2 v10a(pr, STD move(c10a), al);
        CHECK_INT(v10a.size(), 3);
        CHECK_INT(v10a.top(), 'x');
        CHECK(c10a.empty());

        Mycont2 v11a(STD move(v10a), al);
        CHECK_INT(v10a.size(), 0);
        CHECK_INT(v11a.size(), 3);
    }

    { // test emplace
        typedef STD less<int> Mypred2;
        typedef STD deque<Movable_int> Myimpl0;
        typedef STD priority_queue<Movable_int, Myimpl0, Mypred2> Mycont0;
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
}

void test_main() { // test basic workings of queue definitions
    test_queue();
    test_priority_queue();
}
