// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <streambuf>
#include <string>

using namespace std;

struct State {};

struct CharTraits : char_traits<char> {
    using state_type = State;
    using pos_type   = fpos<State>;
};

struct Streambuf : basic_streambuf<char, CharTraits> {};
