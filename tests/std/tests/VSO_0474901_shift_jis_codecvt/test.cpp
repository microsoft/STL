// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <exception>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>

int main() {
    {
        // Japan in Shift JIS/cp932
        const char* const japan_932 = "\x93\xfa\x96\x7b";
        constexpr auto mode         = std::ios_base::binary | std::ios_base::out | std::ios_base::trunc;
        std::ofstream f("japan.txt", mode);
        f << japan_932;
        f.close();
    }

    try {
        std::wifstream test;
        test.imbue(std::locale("Japanese_Japan.932"));
        test.open("japan.txt");
        // Japan in Unicode
        const std::wstring cmp = L"\u65e5\u672c";
        std::wstring ref;
        test >> ref;
        if (ref == cmp) {
            std::cout << "OK\n";
            return 0;
        }

        std::cout << "FAIL\nREF\n";
        for (const unsigned int cInt : ref) {
            std::cout << std::hex << cInt << '\n';
        }

        std::cout << "EXP\n";
        for (const unsigned int cInt : cmp) {
            std::cout << std::hex << cInt << '\n';
        }
    } catch (const std::exception& e) {
        std::cout << "No Locale: " << e.what() << '\n';
    }

    return 1;
}
