# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use strict;
use warnings;

sub PostExecuteHook()
{
    Run::Delete("P0881R7_stacktrace.exe");
    Run::Delete("P0881R7_stacktrace.exp");
    Run::Delete("P0881R7_stacktrace.lib");
    Run::Delete("P0881R7_stacktrace.pdb");
    Run::Delete("test.obj");
    Run::Delete("vc140.pdb");
}
1
