// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstring>
#include <fstream>
#include <ios>

int main() {
    using namespace std;

    fstream f("test.txt", ios::in | ios::out | ios::trunc);

    f << "123";

    auto check = [&f](const char(&content)[4]) {
        char buffer[4]{};
        f.seekg(0);
        f.read(buffer, 3);
        assert(f);
        assert(memcmp(buffer, content, 4) == 0);
    };

    f.seekg(0);
    (void) f.get();
    f.seekp(f.tellp());
    f << "*";
    check("1*3");

    f.seekg(0);
    (void) f.get();
    f.seekp(0, ios::cur);
    f << "!";
    check("1!3");

    f.seekg(0);
    (void) f.get();
    f.seekp(1, ios::cur);
    f << "!";
    check("1!!");

    f.seekg(0);
    (void) f.get();
    f.seekp(-1, ios::cur);
    f << "!";
    check("!!!");
}
