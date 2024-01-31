# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use Run;

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();
    my $machine = $ENV{'TARGET_ARCHITECTURE'};
    Run::ExecuteCL("TestDll.cpp /c");
    Run::ExecuteLink("/machine:$machine /out:TestDll.DLL /DLL TestDll.obj");
    Run::ExecuteCL("Test.cpp /c");
    Run::ExecuteLink("/machine:$machine /out:$cwd.exe Test.obj");
}
1
