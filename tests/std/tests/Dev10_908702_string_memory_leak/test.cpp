// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: debug_CRT

#include <cassert>
#include <crtdbg.h>
#include <locale>
#include <new>
#include <string>
#include <utility>
#include <vector>

using namespace std;

int main() {
    {
        vector<string> v;
        v.reserve(3);
        v.push_back("1");

        v.insert(v.begin(), 2, "2");
    }

    assert(!_CrtDumpMemoryLeaks());

    {
        string one("111");
        string two("222");
        string three("333");
        one = move(two);
        two = three;
    }

    assert(!_CrtDumpMemoryLeaks());

    // Also test DevDiv-846054 "<locale>: Spurious memory leaks".
    locale::global(locale(""));

    assert(!_CrtDumpMemoryLeaks());
}

// Also test DevDiv-810608 "<xlocale>: [torino][boost]error C2665:
// 'std::_Crt_new_delete::operator new' : none of the 2 overloads could convert all the argument types".
void meow(void* pv) { // COMPILE-ONLY
    // Saying "new" instead of "::new" is intentional here.
    new (pv) locale();
}
