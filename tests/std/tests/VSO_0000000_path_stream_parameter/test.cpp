// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <cassert>
#include <fstream>
#include <ios>
#include <system_error>

#include "experimental_filesystem.hpp"
#include <test_filesystem_support.hpp>

using namespace std;

int main() {
    error_code ec;

    {
        namespace fs = experimental::filesystem;

        const auto testDir = fs::temp_directory_path() / get_test_directory_subname("path_stream_parameter");
        fs::create_directories(testDir, ec);
        assert(!ec);

        {
            const auto filebufPath = testDir / L"filebuf_file";
            filebuf buf;
            buf.open(filebufPath, ios::out | ios::trunc);
            buf.close();
            assert(fs::exists(filebufPath, ec));
            assert(!ec);
        }

        const auto ofstreamFile     = testDir / L"ofstream_file";
        const auto ofstreamOpenFile = testDir / L"ofstream_open_file";
        {
            ofstream ostr(ofstreamFile);
            assert(ostr);
            ostr << 1729;
            assert(fs::exists(ofstreamFile, ec));
            assert(!ec);
            ostr.close();
            ostr.open(ofstreamOpenFile);
            ostr << 42;
            assert(ostr);
        }

        assert(fs::exists(ofstreamOpenFile, ec));
        assert(!ec);

        const auto fstreamFile     = testDir / L"fstream_file";
        const auto fstreamOpenFile = testDir / L"fstream_open_file";
        {
            fstream fstr(fstreamFile, ios::out | ios::trunc);
            assert(fstr);
            assert(fs::exists(fstreamFile, ec));
            assert(!ec);
            fstr.close();
            fstr.open(fstreamOpenFile, ios::out | ios::trunc);
            assert(fstr);
        }

        assert(fs::exists(fstreamOpenFile, ec));
        assert(!ec);

        {
            ifstream istr(ofstreamFile);
            assert(istr);
            int temp;
            assert(istr >> temp);
            assert(temp == 1729);
            istr.close();
            istr.open(ofstreamOpenFile);
            assert(istr);
            assert(istr >> temp);
            assert(temp == 42);
        }

        fs::remove_all(testDir, ec);
        assert(!ec);
    }

#if _HAS_CXX17
    {
        const auto testDir = get_test_directory("path_stream_parameter");
        filesystem::create_directories(testDir, ec);
        assert(!ec);

        {
            const auto filebufPath = testDir / L"filebuf_file";
            filebuf buf;
            buf.open(filebufPath, ios::out | ios::trunc);
            buf.close();
            assert(filesystem::exists(filebufPath, ec));
            assert(!ec);
        }

        const auto ofstreamFile     = testDir / L"ofstream_file";
        const auto ofstreamOpenFile = testDir / L"ofstream_open_file";
        {
            ofstream ostr(ofstreamFile);
            assert(ostr);
            ostr << 1729;
            assert(filesystem::exists(ofstreamFile, ec));
            assert(!ec);
            ostr.close();
            ostr.open(ofstreamOpenFile);
            ostr << 42;
            assert(ostr);
        }

        assert(filesystem::exists(ofstreamOpenFile, ec));
        assert(!ec);

        const auto fstreamFile     = testDir / L"fstream_file";
        const auto fstreamOpenFile = testDir / L"fstream_open_file";
        {
            fstream fstr(fstreamFile, ios::out | ios::trunc);
            assert(fstr);
            assert(filesystem::exists(fstreamFile, ec));
            assert(!ec);
            fstr.close();
            fstr.open(fstreamOpenFile, ios::out | ios::trunc);
            assert(fstr);
        }

        assert(filesystem::exists(fstreamOpenFile, ec));
        assert(!ec);

        {
            ifstream istr(ofstreamFile);
            assert(istr);
            int temp;
            assert(istr >> temp);
            assert(temp == 1729);
            istr.close();
            istr.open(ofstreamOpenFile);
            assert(istr);
            assert(istr >> temp);
            assert(temp == 42);
        }

        filesystem::remove_all(testDir, ec);
        assert(!ec);
    }
#endif // _HAS_CXX17
}
