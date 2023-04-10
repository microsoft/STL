// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <string>

using namespace std;

// VSO-575109 "[RWC][std:c++latest][prod/fe] ClangLLVM failed with error C2593"
// was fixed by the PR for
// LWG-2946 "LWG 2758's resolution missed further corrections"

int main() {
    string s({"abc", 2});
    assert(s == "ab");
    s = {"def", 2};
    assert(s == "de");
    s += {"ghi", 2};
    assert(s == "degh");
    s.append({"jkl", 2});
    assert(s == "deghjk");
    s.assign({"mno", 2});
    assert(s == "mn");
    s.insert(1, {"pqr", 2});
    assert(s == "mpqn");
    s.replace(1, 1, {"stu", 2});
    assert(s == "mstqn");
    s.replace(s.cbegin() + 1, s.cbegin() + 4, {"vwx", 2});
    assert(s == "mvwn");

    s = "_ba_ab_xy_ab_ba_";
    assert(s.find({"abc", 2}) == 4);
    assert(s.rfind({"abc", 2}) == 10);
    assert(s.find_first_of({"abc", 2}) == 1);
    assert(s.find_last_of({"abc", 2}) == 14);
    assert(s.find_first_not_of({"ab_", 3}) == 7);
    assert(s.find_last_not_of({"ab_", 3}) == 8);
    assert(s.compare({"_az", 2}) > 0);
    assert(s.compare({"_cz", 2}) < 0);
    assert(s.compare(0, 2, {"_az", 2}) > 0);
    assert(s.compare(0, 2, {"_bz", 2}) == 0);
    assert(s.compare(0, 2, {"_cz", 2}) < 0);
}
