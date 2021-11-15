// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <ios>
#include <random>
#include <sstream>
#include <string>
using namespace std;

void check_state(const string& state_str) {
    // Check word-by-word in case of whitespace differences.
    static constexpr const char* state_ref[] = {
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
    constexpr auto state_size = size(state_ref);
    stringstream sstr(state_str);

    size_t idx = 0;
    string word;
    while (sstr && idx < state_size) {
        sstr >> word;
        assert(word == state_ref[idx]);
        ++idx;
    }

    assert(sstr.rdstate() == ios_base::eofbit);
    assert(idx == state_size);
}

void check(stringstream& sstr) {
    // N4892 [tab:rand.req.eng]: "Postconditions: The os.fmtflags and fill character are unchanged."
    // and "Postconditions: The is.fmtflags are unchanged."
    const auto old_flags = sstr.flags();
    const auto old_fill  = sstr.fill();
    ranlux48_base eng1;
    ranlux48_base eng2;
    sstr << eng1;
    check_state(sstr.str());
    sstr >> eng2;
    assert(eng1 == eng2);
    assert(sstr.flags() == old_flags);
    assert(sstr.fill() == old_fill);
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
