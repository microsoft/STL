# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use Run;

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();
    Run::ExecuteCL("TestDll.cpp /FeTestDll.DLL /link /DLL");
    Run::ExecuteCL("Test.cpp /Fe$cwd.exe");
}
1
