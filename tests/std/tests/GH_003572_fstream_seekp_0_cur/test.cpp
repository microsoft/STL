// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <fstream>
#include <ios>
#include <string_view>

int main() {
    using namespace std;

    fstream f("test.txt", ios::in | ios::out | ios::trunc);

    f << "123";

    auto check = [&](string_view content) {
        char buffer[4]{};
        f.seekg(0);
        f.read(buffer, 3);
        assert(f);
        assert(buffer == content);
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
