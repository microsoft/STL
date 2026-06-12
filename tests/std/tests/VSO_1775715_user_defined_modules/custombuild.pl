# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use strict;
use warnings;

use Run;

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();

    # Dependency order is important here:
    my @inputPaths;
    if (($ENV{PM_COMPILER} // "") eq "clang-cl") {
        @inputPaths = ("-x", "c++-module", "user.ixx", "-x", "none", "test.cpp");
    } else {
        @inputPaths = ("user.ixx", "test.cpp");
    }

    Run::ExecuteCL(join(" ", @inputPaths, "/Fe$cwd.exe"));
}
1
