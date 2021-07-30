// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ios>
#include <random>
#include <sstream>

using namespace std;

void check_state(const string& state_str) {
    // Check word-by-word in case of whitespace differences.
    constexpr static const char* state_ref[] = {
        "10880375256626",
        "126660097854724",
        "33643165434010",
        "78293780235492",
        "179418984296008",
        "96783156950859",
        "238199764491708",
        "34339434557790",
        "155299155394531",
        "29014415493780",
        "209265474179052",
        "263777435457028",
        "0",
    };
    constexpr auto state_size = sizeof(state_ref) / sizeof(state_ref[0]);
    stringstream sstr(state_str);

    size_t idx = 0;
    string word;
    while (sstr && idx < state_size) {
        sstr >> word;
        assert(word == state_ref[idx]);
        ++idx;
    }

    assert(sstr.rdstate() == ios_base::eofbit && idx == state_size);
}

void check(stringstream& sstr) {
    ranlux48_base eng1;
    ranlux48_base eng2;
    sstr << eng1;
    check_state(sstr.str());
    sstr >> eng2;
    assert(eng1 == eng2);
}

int main() {
    {
        stringstream sstr;
        sstr << hex;
        check(sstr);
    }

    {
        stringstream sstr;
        sstr.unsetf(ios_base::skipws);
        check(sstr);
    }

    {
        stringstream sstr;
        sstr.fill('.');
        sstr.width(40);
        check(sstr);
    }

    return 0;
}
