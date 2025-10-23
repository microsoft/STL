# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use strict;
use warnings;

use Run;

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();

    Run::ExecuteCL("/c test.cpp");
    Run::ExecuteLink("/MANIFESTINPUT:long_path_aware.manifest /OUT:$cwd.exe test.obj");
}
1
