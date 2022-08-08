// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void check_state(const char* const input, const ios_base::iostate expected) {
    istringstream iss(input);

    string s;

    iss >> quoted(s);

    assert(iss.rdstate() == expected);
}

int main() {
    const char* const original_recipe = R"( one "two" three\four \"five "\six )";
    const char* const extra_crispy    = R"(" one \"two\" three\\four \\\"five \"\\six ")";

    {
        ostringstream oss1;

        oss1 << quoted(original_recipe);

        const bool b = oss1.str() == extra_crispy;
        assert(b);
    }

    {
        ostringstream oss2;

        const string s2(original_recipe);

        oss2 << quoted(s2);

        const bool b = oss2.str() == extra_crispy;
        assert(b);
    }

    {
        ostringstream oss3;

        string s3(original_recipe);

        oss3 << quoted(s3);

        const bool b = oss3.str() == extra_crispy;
        assert(b);
    }

    {
        istringstream iss4(extra_crispy);

        string s4("GARBAGE");

        iss4 >> quoted(s4);

        assert(s4 == original_recipe);
    }

    {
        ostringstream oss5;

        oss5 << setfill('w') << setw(30) << R"("ABC \"DEF\" \\GHI")";

        const bool b = oss5.str() == R"(wwwwwwwwwww"ABC \"DEF\" \\GHI")";
        assert(b);
    }

    {
        ostringstream oss6;

        oss6 << setfill('x') << setw(30) << quoted(R"(ABC "DEF" \GHI)");

        const bool b = oss6.str() == R"(xxxxxxxxxxx"ABC \"DEF\" \\GHI")";
        assert(b);
    }

    {
        ostringstream oss7;

        oss7 << left << setfill('y') << setw(30) << R"("ABC \"DEF\" \\GHI")";

        const bool b = oss7.str() == R"("ABC \"DEF\" \\GHI"yyyyyyyyyyy)";
        assert(b);
    }

    {
        ostringstream oss8;

        oss8 << left << setfill('z') << setw(30) << quoted(R"(ABC "DEF" \GHI)");

        const bool b = oss8.str() == R"("ABC \"DEF\" \\GHI"zzzzzzzzzzz)";
        assert(b);
    }

    {
        istringstream iss9("meow purr");

        string s9("GARBAGE");

        iss9 >> quoted(s9);

        const bool b = s9 == "meow";
        assert(b);
    }

    {
        istringstream iss10(R"(  " ONE \"TWO\\THREE\" FOUR "  )");

        string s10("GARBAGE");

        iss10 >> quoted(s10);

        const bool b = s10 == R"( ONE "TWO\THREE" FOUR )";
        assert(b);
    }

    {
        istringstream iss11("  cat  dog  ");

        string s11("GARBAGE");

        iss11 >> quoted(s11);

        const bool b = s11 == "cat";
        assert(b);
    }

    {
        istringstream iss12(R"("something""something")");

        string s12("GARBAGE");

        iss12 >> quoted(s12, '"', '"');

        const bool b = s12 == R"(something"something)";
        assert(b);
    }

    check_state(R"()", ios_base::eofbit | ios_base::failbit);
    check_state(R"(cat)", ios_base::eofbit);
    check_state(R"(  cat)", ios_base::eofbit);
    check_state(R"(cat  )", ios_base::goodbit);
    check_state(R"(  cat  )", ios_base::goodbit);
    check_state(R"(")", ios_base::eofbit | ios_base::failbit);
    check_state(R"(  ")", ios_base::eofbit | ios_base::failbit);
    check_state(R"("  )", ios_base::eofbit | ios_base::failbit);
    check_state(R"(  "  )", ios_base::eofbit | ios_base::failbit);
    check_state(R"("stuff)", ios_base::eofbit | ios_base::failbit);
    check_state(R"(  "stuff)", ios_base::eofbit | ios_base::failbit);
    check_state(R"("stuff  )", ios_base::eofbit | ios_base::failbit);
    check_state(R"(  "stuff  )", ios_base::eofbit | ios_base::failbit);
    check_state(R"("stuff\)", ios_base::eofbit | ios_base::failbit);
    check_state(R"(  "stuff\)", ios_base::eofbit | ios_base::failbit);
    check_state(R"("stuff\  )", ios_base::eofbit | ios_base::failbit);
    check_state(R"(  "stuff\  )", ios_base::eofbit | ios_base::failbit);
    check_state(R"("dog")", ios_base::goodbit);
    check_state(R"(  "dog")", ios_base::goodbit);
    check_state(R"("dog"  )", ios_base::goodbit);
    check_state(R"(  "dog"  )", ios_base::goodbit);
    check_state(extra_crispy, ios_base::goodbit);

    // Also test VSO-666592 "std::quoted fails to properly extract delimiter"
    {
        const string expectedStrings[] = {
            "spaces"s, "with"s, "some quote"s, "and another quote"s, "then"s, "more"s, "spaces"s};
        istringstream iss13(R"(spaces with "some quote" "and another quote" then more spaces)");
        string out;
        for (const auto& expected : expectedStrings) {
            iss13 >> quoted(out);
            assert(!iss13.fail());
            assert(out == expected);
        }

        assert(iss13.eof());
    }
}
