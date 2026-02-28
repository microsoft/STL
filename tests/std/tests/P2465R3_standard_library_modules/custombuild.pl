# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use strict;
use warnings;

use Run;

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();

    my $stlModulesDir = $ENV{STL_MODULES_DIR};

    my $stdIxx = "$stlModulesDir\\std.ixx";
    my $stdCompatIxx = "$stlModulesDir\\std.compat.ixx";

    my @moduleUnits = ($stdIxx, $stdCompatIxx);
    my @traditionalUnits = ("test.cpp", "test2.cpp", "test3.cpp", "test4.cpp", "classic.cpp");

    if ($ENV{PM_COMPILER} =~ m/clang/) {
        for my $modulePath (@moduleUnits) {
            Run::ExecuteCL("-x c++-module $modulePath --precompile");
        }

        Run::ExecuteCL(join(" ", "-x", "c++-module", @moduleUnits, "-x", "none", @traditionalUnits, "/Fe$cwd.exe"));
    } else {
        # Dependency order is important here:
        my @inputPaths = ($stdIxx, $stdCompatIxx, "test.cpp", "test2.cpp", "test3.cpp", "test4.cpp", "classic.cpp");

        Run::ExecuteCL(join(" ", @inputPaths, "/Fe$cwd.exe"));
    }
}
1
