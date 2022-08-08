// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

char first(const string& s) {
    return *s.begin();
}

int main() {
    ostringstream oss;
    oss << "hello";
    string str = oss.str();

    string::const_iterator it = str.begin();
    string str2(it, it + 2);

    assert(str == "hello" && str2 == "he");

    ostringstream oss2("kitty");
    char c = first(oss2.str());

    assert(c == 'k');

    string a = "kittens";
    string b = "cute fluffy " + a;

    transform(b.begin(), b.end(), b.begin(), [](int c) { return static_cast<char>(toupper(c)); });

    assert(b == "CUTE FLUFFY KITTENS");

    istringstream iss("the quick brown fox jumps over the lazy dog");

    vector<string> v;

    for (string s; getline(iss, s, ' ');) {
        v.push_back(s);
    }

    assert(v.size() == 9 && v[0] == "the" && v[8] == "dog");

    istringstream iss2("brown;dog;fox;jumps;lazy;over;quick;the;the");

    vector<string> v2;

    for (string s; getline(iss2, s, ';');) {
        v2.push_back(s);
    }

    assert(v2.size() == 9 && v2[0] == "brown" && v2[8] == "the");

    sort(v.begin(), v.end());

    assert(v == v2);
}
