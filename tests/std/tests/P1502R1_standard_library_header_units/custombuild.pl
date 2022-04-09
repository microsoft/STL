# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use strict;
use warnings;

use JSON::PP;
use Run;

sub readFile
{
    my $filename = $_[0];
    open(my $handle, "<", $filename) or die("Couldn't open $filename: $!");
    read($handle, my $string, -s $handle);
    return $string;
}

sub loadJson
{
    my $filename = $_[0];
    my $jsonStr = readFile($filename);
    return JSON::PP->new->utf8->decode($jsonStr);
}

sub loadJsonWithComments
{
    my $filename = $_[0];
    my $jsonStr = readFile($filename);
    return JSON::PP->new->relaxed->utf8->decode($jsonStr);
}

sub getAllHeaders
{
    my $headerUnitsJsonFilename = $_[0];
    my $jsonObject = loadJsonWithComments($headerUnitsJsonFilename);
    my @buildAsHeaderUnits = @{$jsonObject->{"BuildAsHeaderUnits"}};
    # We want to build everything that's mentioned in header-units.json, plus all of the
    # headers that were commented out for providing macros that control header inclusion.
    push(@buildAsHeaderUnits, "version", "yvals.h", "yvals_core.h");
    return @buildAsHeaderUnits;
}

sub getImportableCxxLibraryHeaders()
{
    # This JSON With Comments file is shared between Python and Perl,
    # reducing the number of things we need to update when adding new Standard headers.
    my $jsonObject = loadJsonWithComments("importable_cxx_library_headers.jsonc");
    return @{$jsonObject};
}

sub arrayDifference
{
    # (Perl) Takes two arrays by reference. Returns everything in the first array (minuend) that doesn't appear
    # in the second array (subtrahend). Doesn't require the arrays to be sorted, so the complexity is O(M * N).
    my @minuend = @{$_[0]};
    my @subtrahend = @{$_[1]};

    my @result = ();
    foreach my $elem (@minuend) {
        if (!grep($elem eq $_, @subtrahend)) {
            push(@result, $elem);
        }
    }
    return @result;
}

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();

    # This is a list of compiler options to consume the header units that we've built so far.
    my @consumeBuiltHeaderUnits = ();

    # Output files:
    my @objFilenames = ();

    if ($ENV{PM_CL} =~ m</DTEST_TOPO_SORT\b>) { # Build deduplicated header units:
        # Compiler options, common to both scanning dependencies and building header units.
        my @clOptions = ("/exportHeader", "/headerName:angle", "/translateInclude", "/Fo", "/MP");

        # Store the list of headers to build.
        my $stlIncludeDir = $ENV{STL_INCLUDE_DIR};
        my @allHeaders = getAllHeaders("$stlIncludeDir\\header-units.json");

        # Generate JSON files that record how these headers depend on one another.
        Run::ExecuteCL(join(" ", @clOptions, "/scanDependencies", ".\\", @allHeaders));

        # The JSON files also record what object files will be produced.
        # IFC filenames and OBJ filenames follow different patterns. For example:
        # <filesystem> produces filesystem.ifc and filesystem.obj
        # <xbit_ops.h> produces xbit_ops.h.ifc and xbit_ops.obj
        # We can easily synthesize IFC filenames, but it's easier to get the OBJ filenames from the JSON files.

        # This dictionary powers the topological sort.
        # Key: Header name, e.g. 'bitset'.
        # Value: List of dependencies that remain to be built, e.g. ['iosfwd', 'limits', 'xstring'].
        my %remainingDependencies;

        # Read the JSON files, storing the results in objFilenames and remainingDependencies.
        foreach my $hdr (@allHeaders) {
            my $jsonObject = loadJson("$hdr.module.json");
            push(@objFilenames, $jsonObject->{"rules"}[0]{"primary-output"});

            my @dep = ();
            foreach my $req (@{$jsonObject->{"rules"}[0]{"requires"}}) {
                push(@dep, $req->{"logical-name"});
            }
            $remainingDependencies{$hdr} = \@dep;
        }

        # Build header units in topologically sorted order.
        while (%remainingDependencies) {
            # When a header has no remaining dependencies, it is ready to be built.
            my @readyToBuild = ();
            foreach my $hdr (keys(%remainingDependencies)) {
                my @dep = @{$remainingDependencies{$hdr}};
                if (!@dep) {
                    push(@readyToBuild, $hdr);
                }
            }

            # Each layer can be built in parallel.
            Run::ExecuteCL(join(" ", @clOptions, @consumeBuiltHeaderUnits, @readyToBuild));

            # Update remainingDependencies by doing two things.

            # (Perl) First, eliminate headers that we just built.
            foreach my $hdr (@readyToBuild) {
                delete $remainingDependencies{$hdr};
            }

            # hdr, dep is the current key-value pair.
            foreach my $hdr (keys(%remainingDependencies)) {
                my @dep = @{$remainingDependencies{$hdr}};

                # Second, filter dep, eliminating anything that appears in readyToBuild. (If we're left with
                # an empty list, then hdr will be ready to build in the next iteration.)
                my @filtered = arrayDifference(\@dep, \@readyToBuild);

                $remainingDependencies{$hdr} = \@filtered;
            }

            # Add compiler options to consume the header units that we just built.
            foreach my $hdr (@readyToBuild) {
                push(@consumeBuiltHeaderUnits, "/headerUnit:angle", "$hdr=$hdr.ifc");
            }
        }
    } else { # Build independent header units:
        my @stlHeaders = getImportableCxxLibraryHeaders();
        my @exportHeaderOptions = ("/exportHeader", "/headerName:angle", "/Fo", "/MP");

        foreach my $hdr (@stlHeaders) {
            push(@consumeBuiltHeaderUnits, "/headerUnit:angle", "$hdr=$hdr.ifc");
            push(@objFilenames, "$hdr.obj");
        }

        Run::ExecuteCL(join(" ", @exportHeaderOptions, @stlHeaders));
    }

    # For convenience, create a library file containing all of the object files that were produced.
    my $libFilename = "stl_header_units.lib";
    Run::ExecuteCommand(join(" ", "lib.exe", "/nologo", "/out:$libFilename", @objFilenames));

    Run::ExecuteCL(join(" ", "test.cpp", "/Fe$cwd.exe", @consumeBuiltHeaderUnits, $libFilename));
}
1
