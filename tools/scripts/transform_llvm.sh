#!/bin/bash

# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# This script is used to regenerate the P0220R1_any, P0220R1_optional, and P0088R3_variant tests.

SED_SCRIPTS='s/int main\(int, char\*\*\)/int run_test()/;'
SED_SCRIPTS+='s/FI[X]ME/TODO/g;'
SED_SCRIPTS+='s@// (REQUIRES|UNSUPPORTED|XFAIL):.*$@@;'

for f in $(find $* -name '*.pass.cpp' -not -name '*nothing_to_do*');
do
    echo "// -- BEGIN: $f";
    sed -E -e "$SED_SCRIPTS" < $f;
    echo -e "// -- END: $f\n";
done
