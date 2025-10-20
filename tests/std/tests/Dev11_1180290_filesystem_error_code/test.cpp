// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <system_error>

#if _HAS_CXX17
#include <filesystem>
#endif // _HAS_CXX17

#include "experimental_filesystem.hpp"

using namespace std;
using namespace std::chrono;

void assert_success(const error_code& ec) {
    assert(!ec);
}

void assert_failure(const error_code& ec) {
    assert(!!ec);
}

namespace test_experimental {
    namespace fs = std::experimental::filesystem;

    void create_file_containing(const char* const filename, const char* const contents) {
        assert(!fs::exists(filename));

        ofstream f(filename);
        f << contents << endl;
        f.close();

        assert(fs::is_regular_file(filename));
    }

    void run_tests() {
        (void) fs::remove_all("DELME_dir");
        assert(!fs::exists("DELME_dir"));
        assert(fs::create_directory("DELME_dir"));
        assert(fs::is_directory("DELME_dir"));

        create_file_containing("DELME_dir/DELME_file.txt", "meow");
        create_file_containing("DELME_dir/DELME_old.txt", "disco");
        create_file_containing("DELME_dir/DELME_new.txt", "DoYouWantToBuildASnowman");

        {
            const auto now = fs::file_time_type::clock::now();
            fs::last_write_time("DELME_dir/DELME_old.txt", now - 1h);
            fs::last_write_time("DELME_dir/DELME_new.txt", now);
            assert(fs::last_write_time("DELME_dir/DELME_old.txt") < fs::last_write_time("DELME_dir/DELME_new.txt"));
        }

        error_code ec;

        fs::copy("DELME_dir", "NONEXISTENT_dir1/NONEXISTENT_dir2", fs::copy_options::recursive, ec);
        assert_failure(ec);
        assert(!fs::exists("NONEXISTENT_dir1"));

        assert(!fs::copy_file(
            "DELME_dir/DELME_file.txt", "DELME_dir/DELME_file.txt", fs::copy_options::skip_existing, ec));
        assert_failure(ec);
        assert(fs::is_regular_file("DELME_dir/DELME_file.txt"));

        assert(!fs::copy_file(
            "DELME_dir/DELME_old.txt", "DELME_dir/DELME_new.txt", fs::copy_options::update_existing, ec));
        assert_success(ec);
        assert(fs::file_size("DELME_dir/DELME_old.txt") != fs::file_size("DELME_dir/DELME_new.txt"));

        assert(
            fs::copy_file("DELME_dir/DELME_new.txt", "DELME_dir/DELME_old.txt", fs::copy_options::update_existing, ec));
        assert_success(ec);
        assert(fs::file_size("DELME_dir/DELME_old.txt") == fs::file_size("DELME_dir/DELME_new.txt"));

        assert(!fs::create_directories("DELME_dir/DELME_file.txt/blah/blah/blah", ec));
        assert_failure(ec);
        assert(fs::is_regular_file("DELME_dir/DELME_file.txt"));

        assert(!fs::is_empty("NONEXISTENT_file.txt", ec));
        assert_failure(ec);
        assert(!fs::exists("NONEXISTENT_file.txt"));

        assert(fs::last_write_time("NONEXISTENT_file.txt", ec) == fs::file_time_type::min());
        assert_failure(ec);
        assert(!fs::exists("NONEXISTENT_file.txt"));

        assert(_putenv_s("TMP", "NONEXISTENT_dir1") == 0);
        assert(fs::temp_directory_path(ec) == fs::path());
        assert_failure(ec);
        assert(!fs::exists("NONEXISTENT_dir1"));

        assert(fs::remove_all("DELME_dir", ec) == 3);
        assert_success(ec);
        assert(!fs::exists("DELME_dir"));
    }
} // namespace test_experimental

#if _HAS_CXX17
namespace test_standard {
    void create_file_containing(const char* const filename, const char* const contents) {
        assert(!filesystem::exists(filename));

        ofstream f(filename);
        f << contents << endl;
        f.close();

        assert(filesystem::is_regular_file(filename));
    }

    void run_tests() {
        (void) filesystem::remove_all("DELME_dir");
        assert(!filesystem::exists("DELME_dir"));
        assert(filesystem::create_directory("DELME_dir"));
        assert(filesystem::is_directory("DELME_dir"));

        create_file_containing("DELME_dir/DELME_file.txt", "meow");
        create_file_containing("DELME_dir/DELME_old.txt", "disco");
        create_file_containing("DELME_dir/DELME_new.txt", "DoYouWantToBuildASnowman");

        {
            const auto now = filesystem::file_time_type::clock::now();
            filesystem::last_write_time("DELME_dir/DELME_old.txt", now - 1h);
            filesystem::last_write_time("DELME_dir/DELME_new.txt", now);
            assert(filesystem::last_write_time("DELME_dir/DELME_old.txt")
                   < filesystem::last_write_time("DELME_dir/DELME_new.txt"));
        }

        error_code ec;

        filesystem::copy("DELME_dir", "NONEXISTENT_dir1/NONEXISTENT_dir2", filesystem::copy_options::recursive, ec);
        assert_failure(ec);
        assert(!filesystem::exists("NONEXISTENT_dir1"));

        assert(!filesystem::copy_file(
            "DELME_dir/DELME_file.txt", "DELME_dir/DELME_file.txt", filesystem::copy_options::skip_existing, ec));
        assert_failure(ec);
        assert(filesystem::is_regular_file("DELME_dir/DELME_file.txt"));

        assert(!filesystem::copy_file(
            "DELME_dir/DELME_old.txt", "DELME_dir/DELME_new.txt", filesystem::copy_options::update_existing, ec));
        assert_success(ec);
        assert(filesystem::file_size("DELME_dir/DELME_old.txt") != filesystem::file_size("DELME_dir/DELME_new.txt"));

        assert(filesystem::copy_file(
            "DELME_dir/DELME_new.txt", "DELME_dir/DELME_old.txt", filesystem::copy_options::update_existing, ec));
        assert_success(ec);
        assert(filesystem::file_size("DELME_dir/DELME_old.txt") == filesystem::file_size("DELME_dir/DELME_new.txt"));

        assert(!filesystem::create_directories("DELME_dir/DELME_file.txt/blah/blah/blah", ec));
        assert_failure(ec);
        assert(filesystem::is_regular_file("DELME_dir/DELME_file.txt"));

        assert(!filesystem::is_empty("NONEXISTENT_file.txt", ec));
        assert_failure(ec);
        assert(!filesystem::exists("NONEXISTENT_file.txt"));

        assert(filesystem::last_write_time("NONEXISTENT_file.txt", ec) == filesystem::file_time_type::min());
        assert_failure(ec);
        assert(!filesystem::exists("NONEXISTENT_file.txt"));

        assert(_putenv_s("TMP", "NONEXISTENT_dir1") == 0);
        assert(filesystem::temp_directory_path(ec) == filesystem::path());
        assert_failure(ec);
        assert(!filesystem::exists("NONEXISTENT_dir1"));

        assert(filesystem::remove_all("DELME_dir", ec) == 4);
        assert_success(ec);
        assert(!filesystem::exists("DELME_dir"));
    }
} // namespace test_standard
#endif // _HAS_CXX17

int main() {
    test_experimental::run_tests();

#if _HAS_CXX17
    test_standard::run_tests();
#endif // _HAS_CXX17
}
