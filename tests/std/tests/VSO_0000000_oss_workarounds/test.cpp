// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING
#include <cassert>
#include <iosfwd>

int main() {
    std::streampos pos(1234);

    // The following workarounds must be present as long as Boost 1.67 is supported:
    // use of "seekpos", submitted upstream at https://github.com/boostorg/iostreams/pull/57
    assert(0 == pos.seekpos());

    // _FPOSOFF macro, left in place due to use in Boost (somewhere around VS2015 Update 3 timeframe)
    // Boost has since fixed this here; _FPOSOFF should be removed at the same time as seekpos in a future release.
    // https://github.com/boostorg/iostreams/blob/develop/include/boost/iostreams/detail/config/fpos.hpp#L36
    assert(0 == _FPOSOFF(pos.seekpos()));
}
