# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use strict;
use File::Basename;

# This file is part of a Microsoft-internal legacy test harness, which we hope to replace in the future.
# Within the Microsoft-internal repo, this uses run.pl and run.pm in src\qa\VC\shared\testenv\bin .

my $path = $ENV{PATH};
$path =~ s/\//\\/g;

# pick out just *:*\testenv\bin from the path
$path =~ m/.*?([^\;]*\\testenv\\bin)($|\\$|\\;|;).*/i;

my $testenv = $1;
my $testdir = dirname($0); # $0 contains the full path including the filename, dirname($0) extracts the directory path.

# add testenv to perllib so we don't have to search for it in other files.
if ($ENV{PERLLIB})
{
    $ENV{PERLLIB} = $ENV{PERLLIB} . ";" . $testenv;
}
else
{
    $ENV{PERLLIB} = $testenv;
}

if ($ENV{PM_COMPILER} && $ENV{PM_COMPILER} eq "clang-cl" && $ENV{CLANG_TARGET} && $ENV{CLANG_TARGET} =~ /^(-m32|-m64)$/)
{
    $ENV{PM_COMPILER} .= " " . $ENV{CLANG_TARGET};
}

# add additional compiler flags if the compiler is cl.exe.
if (not $ENV{PM_COMPILER})
{
    $ENV{CL} .= " " . $ENV{PM_CL_MSVC};
}

my $RunPL = "";
if ($ENV{PM_CL} && $ENV{PM_CL} =~ m/[\-\/]BE / )
{
    $RunPL = $testdir . "\\runbe.pl";
}
else
{
    $RunPL = $testenv . "\\run.pl";
}

print "Executing: \"$RunPL\"\n";
exit(system("perl -w $RunPL")>>8);
