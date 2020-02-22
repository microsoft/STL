# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use Run;

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();
    Run::ExecuteCL("$ENV{CALLING_CONVENTION_A} /c a.cpp");
    Run::ExecuteCL("$ENV{CALLING_CONVENTION_B} b.cpp a.obj /Fe$cwd.exe");
}
1
