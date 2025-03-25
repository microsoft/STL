// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <future>

int main() {
#ifndef _M_CEE // TRANSITION, VSO-2359637, avoid sporadic failures under /clr
    std::async([] {}).wait();
#endif
}
