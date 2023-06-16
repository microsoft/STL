// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <fstream>
#include <cassert>

int main() {
    using namespace std;

    fstream f("test.txt", ios::in | ios::out | ios::trunc);

    f << "123";

    auto check = [&](string_view content) {
        char buffer[4]{};
        f.seekg(0);
        f.read(buffer, 3);
        assert(f && buffer == content);
    };

    f.seekg(0);
    (void) f.get();
    f.seekp(f.tellp());
    f << "*";
    check("1*3");

    f.seekg(0);
    (void) f.get();
    f.seekp(0, std::ios::cur);
    f << "!";
    check("1!3");

    f.seekg(0);
    (void) f.get();
    f.seekp(1, std::ios::cur);
    f << "!";
    check("1!!");

    f.seekg(0);
    (void) f.get();
    f.seekp(-1, std::ios::cur);
    f << "!";
    check("!!!");
}
