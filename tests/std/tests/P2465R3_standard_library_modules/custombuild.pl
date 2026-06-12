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

    # Dependency order is important here:
    my @moduleUnits = ($stdIxx, $stdCompatIxx);
    my @traditionalUnits = ("test.cpp", "test2.cpp", "test3.cpp", "test4.cpp", "classic.cpp");
    my @inputPaths;
    if (($ENV{PM_COMPILER} // "") eq "clang-cl") {
        @inputPaths = ("-x", "c++-module", @moduleUnits, "-x", "none", @traditionalUnits);
    } else {
        @inputPaths = (@moduleUnits, @traditionalUnits);
    }

    Run::ExecuteCL(join(" ", @inputPaths, "/Fe$cwd.exe"));
}
1
