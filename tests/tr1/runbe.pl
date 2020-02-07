# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use strict;
# testenv\bin must be on the PERLLIB env var.
use Run;

if ($ENV{HAS_EDG} && $ENV{HAS_EDG} eq "yes")
{
    my $callString = Run::CreateCompilandString();

    if (Run::ExecuteCL($callString) == 0)
    {
        Run::Pass();
    }
    else
    {
        Run::Fail();
    }
}
else
{
    Run::Pass();
}
