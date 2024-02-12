// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>

#include "temp_file_name.hpp"

using namespace std;

void test(const string& temp_file_name_str, const ios_base::openmode mode) {
    filesystem::remove(temp_file_name_str);

    {
        ofstream out{temp_file_name_str, mode};
        out << "Line A\n";
        out << "Line B\n";
        out << "Line C\n";
    }

    {
        ifstream in{temp_file_name_str};
        string line;

        assert(getline(in, line));
        assert(line == "Line A");
        (void) in.tellg();

        assert(getline(in, line));
        assert(line == "Line B");
        (void) in.tellg();

        assert(getline(in, line));
        assert(line == "Line C");
        (void) in.tellg();

        assert(!getline(in, line));
    }

    filesystem::remove(temp_file_name_str);
}

int main() {
    const string temp_file_name_str = temp_file_name();

    test(temp_file_name_str, ios_base::out);
    test(temp_file_name_str, ios_base::binary);
}
