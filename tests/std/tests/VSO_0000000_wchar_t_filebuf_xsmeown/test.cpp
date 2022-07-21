// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <fstream>
#include <ios>
#include <iterator>

using namespace std;

int main() {
    wchar_t wideBuf[] = L"hello world";
    char narrowBuf[]  = "hello world";
    {
        wfilebuf wf;
        wf.open("test.dat", ios::out | ios::trunc);
        wf.sputn(wideBuf, static_cast<streamsize>(size(wideBuf)));
    }

    {
        filebuf nf;
        nf.open("test.dat", ios::in | ios::binary);
        char actualNarrow[size(narrowBuf)];
        nf.sgetn(actualNarrow, static_cast<streamsize>(size(actualNarrow)));
        assert(equal(begin(narrowBuf), end(narrowBuf), begin(actualNarrow), end(actualNarrow)));
    }

    {
        filebuf wf;
        wf.open("test.dat", ios::in);
        char actualWide[size(wideBuf)];
        wf.sgetn(actualWide, static_cast<streamsize>(size(actualWide)));
        assert(equal(begin(wideBuf), end(wideBuf), begin(actualWide), end(actualWide)));
    }
}
