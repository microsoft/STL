# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use Run;

sub CustomBuildHook()
{
    my $cwd = Run::GetCWDName();
    my $stl_include_dir = $ENV{STL_INCLUDE_DIR};

    my @stl_headers = (
        "algorithm",
        "any",
        "array",
        "atomic",
        "barrier",
        "bit",
        "bitset",
        "charconv",
        "chrono",
        "codecvt",
        "compare",
        "complex",
        "concepts",
        "condition_variable",
        "coroutine",
        "deque",
        "exception",
        "execution",
        "filesystem",
        # "format",
        "forward_list",
        "fstream",
        "functional",
        "future",
        "initializer_list",
        "iomanip",
        "ios",
        "iosfwd",
        "iostream",
        "istream",
        "iterator",
        "latch",
        "limits",
        "list",
        "locale",
        "map",
        "memory_resource",
        "memory",
        "mutex",
        "new",
        "numbers",
        "numeric",
        "optional",
        "ostream",
        "queue",
        "random",
        "ranges",
        "ratio",
        "regex",
        "scoped_allocator",
        "semaphore",
        "set",
        "shared_mutex",
        # "source_location",
        "span",
        "sstream",
        "stack",
        "stdexcept",
        "stop_token",
        "streambuf",
        "string_view",
        "string",
        "strstream",
        # "syncstream",
        "system_error",
        "thread",
        "tuple",
        "type_traits",
        "typeindex",
        "typeinfo",
        "unordered_map",
        "unordered_set",
        "utility",
        "valarray",
        "variant",
        "vector",
        "version",
    );

    my $header_unit_options = "";

    foreach (@stl_headers) {
        $header_unit_options .= " /headerUnit";
        $header_unit_options .= " $stl_include_dir/$_=$_.ifc";
        $header_unit_options .= " $_.obj";

        # TRANSITION, remove /DMSVC_INTERNAL_TESTING after all compiler bugs are fixed
        Run::ExecuteCL("/DMSVC_INTERNAL_TESTING /exportHeader \"<$_>\" /Fo$_.obj");
    }

    # TRANSITION, remove /DMSVC_INTERNAL_TESTING after all compiler bugs are fixed
    Run::ExecuteCL("/DMSVC_INTERNAL_TESTING test.cpp /Fe$cwd.exe $header_unit_options");
}
1
