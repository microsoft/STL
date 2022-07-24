// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <random>

using namespace std;

int main() {
    // In VSO-664587, this caused divide by zero:
    linear_congruential_engine<unsigned long long, 25214903917, 11, 0> eng;
    assert(eng() == 25214903928ull);
    assert(eng() == 8602081314781131043ull);
    assert(eng() == 4749291277619109362ull);
    assert(eng() == 15888805192744905749ull);
    assert(eng() == 10851646879081865980ull);
    assert(eng() == 10901909820910194007ull);
    assert(eng() == 9139579755994322966ull);
}
