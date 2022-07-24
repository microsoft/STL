// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <cassert>
#include <experimental/filesystem>
#include <fstream>
#include <ios>
#include <system_error>

#include <test_filesystem_support.hpp>

using namespace std;
namespace fs = experimental::filesystem;

int main() {
    error_code ec;

    {
        const auto testDir = get_test_directory("path_stream_parameter");
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
    }

#if _HAS_CXX17
    {
        const auto testDir = get_new_test_directory("path_stream_parameter");
        fs::create_directories(testDir.native(), ec);
        assert(!ec);

        {
            const auto filebufPath = testDir / L"filebuf_file";
            filebuf buf;
            buf.open(filebufPath, ios::out | ios::trunc);
            buf.close();
            assert(fs::exists(filebufPath.native(), ec));
            assert(!ec);
        }

        const auto ofstreamFile     = testDir / L"ofstream_file";
        const auto ofstreamOpenFile = testDir / L"ofstream_open_file";
        {
            ofstream ostr(ofstreamFile);
            assert(ostr);
            ostr << 1729;
            assert(fs::exists(ofstreamFile.native(), ec));
            assert(!ec);
            ostr.close();
            ostr.open(ofstreamOpenFile);
            ostr << 42;
            assert(ostr);
        }

        assert(fs::exists(ofstreamOpenFile.native(), ec));
        assert(!ec);

        const auto fstreamFile     = testDir / L"fstream_file";
        const auto fstreamOpenFile = testDir / L"fstream_open_file";
        {
            fstream fstr(fstreamFile, ios::out | ios::trunc);
            assert(fstr);
            assert(fs::exists(fstreamFile.native(), ec));
            assert(!ec);
            fstr.close();
            fstr.open(fstreamOpenFile, ios::out | ios::trunc);
            assert(fstr);
        }

        assert(fs::exists(fstreamOpenFile.native(), ec));
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

        fs::remove_all(testDir.native(), ec);
        assert(!ec);
    }
#endif // _HAS_CXX17
}
