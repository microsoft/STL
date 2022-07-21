// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

using namespace std;

#define TRYING try {

#define SHOULD_THROW_OUT_OF_RANGE \
    abort();                      \
    }                             \
    catch (const out_of_range&) { \
    }

int main() {
    {
        string s("meow");
        assert(s == "meow");
        s.resize(7, 'A');
        assert(s == "meowAAA");
        s.resize(7, 'B');
        assert(s == "meowAAA");
        s.resize(3, 'C');
        assert(s == "meo");
        s.resize(0, 'D');
        assert(s == "");
        s.resize(0, 'E');
        assert(s == "");
        s.resize(5, 'F');
        assert(s == "FFFFF");
        s.resize(25, 'G');
        assert(s == "FFFFFGGGGGGGGGGGGGGGGGGGG");
    }

    assert(string("").erase() == "");
    assert(string("").erase(0) == "");
    assert(string("").erase(0, 0) == "");
    assert(string("").erase(0, 5) == "");
    assert(string("").erase(0, string::npos) == "");
    TRYING string("").erase(1);
    SHOULD_THROW_OUT_OF_RANGE
    TRYING string("").erase(1, 0);
    SHOULD_THROW_OUT_OF_RANGE
    TRYING string("").erase(1, 5);
    SHOULD_THROW_OUT_OF_RANGE
    TRYING string("").erase(1, string::npos);
    SHOULD_THROW_OUT_OF_RANGE

    assert(string("peppermint").erase() == "");
    assert(string("peppermint").erase(0) == "");
    assert(string("peppermint").erase(1) == "p");
    assert(string("peppermint").erase(2) == "pe");
    assert(string("peppermint").erase(8) == "peppermi");
    assert(string("peppermint").erase(9) == "peppermin");
    assert(string("peppermint").erase(10) == "peppermint");
    TRYING string("peppermint").erase(11);
    SHOULD_THROW_OUT_OF_RANGE

    assert(string("abcdefghij").erase(0, 0) == "abcdefghij");
    assert(string("abcdefghij").erase(0, 1) == "bcdefghij");
    assert(string("abcdefghij").erase(0, 2) == "cdefghij");
    assert(string("abcdefghij").erase(0, 8) == "ij");
    assert(string("abcdefghij").erase(0, 9) == "j");
    assert(string("abcdefghij").erase(0, 10) == "");
    assert(string("abcdefghij").erase(0, 11) == "");
    assert(string("abcdefghij").erase(0, string::npos) == "");

    assert(string("abcdefghij").erase(1, 0) == "abcdefghij");
    assert(string("abcdefghij").erase(1, 1) == "acdefghij");
    assert(string("abcdefghij").erase(1, 2) == "adefghij");
    assert(string("abcdefghij").erase(1, 7) == "aij");
    assert(string("abcdefghij").erase(1, 8) == "aj");
    assert(string("abcdefghij").erase(1, 9) == "a");
    assert(string("abcdefghij").erase(1, 10) == "a");
    assert(string("abcdefghij").erase(1, string::npos) == "a");

    assert(string("abcdefghij").erase(5, 0) == "abcdefghij");
    assert(string("abcdefghij").erase(5, 1) == "abcdeghij");
    assert(string("abcdefghij").erase(5, 2) == "abcdehij");
    assert(string("abcdefghij").erase(5, 3) == "abcdeij");
    assert(string("abcdefghij").erase(5, 4) == "abcdej");
    assert(string("abcdefghij").erase(5, 5) == "abcde");
    assert(string("abcdefghij").erase(5, 6) == "abcde");
    assert(string("abcdefghij").erase(5, string::npos) == "abcde");

    assert(string("abcdefghij").erase(8, 0) == "abcdefghij");
    assert(string("abcdefghij").erase(8, 1) == "abcdefghj");
    assert(string("abcdefghij").erase(8, 2) == "abcdefgh");
    assert(string("abcdefghij").erase(8, 3) == "abcdefgh");
    assert(string("abcdefghij").erase(8, string::npos) == "abcdefgh");

    assert(string("abcdefghij").erase(9, 0) == "abcdefghij");
    assert(string("abcdefghij").erase(9, 1) == "abcdefghi");
    assert(string("abcdefghij").erase(9, 2) == "abcdefghi");
    assert(string("abcdefghij").erase(9, string::npos) == "abcdefghi");

    assert(string("abcdefghij").erase(10, 0) == "abcdefghij");
    assert(string("abcdefghij").erase(10, 1) == "abcdefghij");
    assert(string("abcdefghij").erase(10, 2) == "abcdefghij");
    assert(string("abcdefghij").erase(10, string::npos) == "abcdefghij");

    TRYING string("abcdefghij").erase(11, 0);
    SHOULD_THROW_OUT_OF_RANGE
    TRYING string("abcdefghij").erase(11, 1);
    SHOULD_THROW_OUT_OF_RANGE
    TRYING string("abcdefghij").erase(11, 2);
    SHOULD_THROW_OUT_OF_RANGE
    TRYING string("abcdefghij").erase(11, string::npos);
    SHOULD_THROW_OUT_OF_RANGE

    { // also test insert / replace aliasing cases

        // cases in the new (as of 2017-01-11) insert algorithm
        string s;
        s.reserve(100);
        s.assign("abcdefghij");
        s.insert(1, "inserted"); // inserted content does not alias
        assert(s == "ainsertedbcdefghij");
        s.assign("abcdefghij");
        s.insert(2, s.c_str(), 1); // inserted content does not shift
        assert(s == "abacdefghij");
        s.assign("abcdefghij");
        s.insert(2, s.c_str() + 3, 2); // inserted content completely shifts
        assert(s == "abdecdefghij");
        s.assign("abcdefghij");
        s.insert(2, s.c_str(), 5); // inserted content partially shifts
        assert(s == "ababcdecdefghij");

        // cases in the new (as of 2017-01-11) replace algorithm
        s.assign("abcdefghij");
        s.replace(1, 1, s.c_str() + 4, 1); // replaced content is the same size
        assert(s == "aecdefghij");
        s.assign("abcdefghij");
        s.replace(1, 5, s.c_str() + 7, 1); // replaced content gets smaller
        assert(s == "ahghij");
        s.assign("abcdefghij");
        s.replace(1, 1, "example"); // growth, does not alias
        assert(s == "aexamplecdefghij");
        s.assign("abcdefghij");
        s.replace(6, 1, s.c_str(), 1); // growth, inserted content does not shift
        assert(s == "abcdefahij");
        s.assign("abcdefghij");
        s.replace(1, 1, s.c_str() + 2, 2); // growth, inserted content completely shifts
        assert(s == "acdcdefghij");
        s.assign("abcdefghij");
        s.replace(2, 4, s.c_str(), 10); // growth, inserted content partially shifts
        assert(s == "ababcdefghijghij");

        // Extra case from the original replace algorithm
        s.assign("abcdefghij");
        s.replace(1, 5, s.c_str() + 3, 6); // hole gets larger, substring begins in hole
        assert(s == "adefghighij");
    }

    { // Also test VSO-437242 std::string::pop_back doesn't write null terminator
        string s("abc");
        s.pop_back();
        assert(strcmp(s.c_str(), "ab") == 0);
        assert(s.size() == 2);
    }
}
