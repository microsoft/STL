# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use strict;
use warnings;

use Run;

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();

    Run::ExecuteCL(join(" ", "test.cpp", "/Fe$cwd.exe", "/link", "/MANIFESTINPUT:long_path_aware.manifest"));
}
1
