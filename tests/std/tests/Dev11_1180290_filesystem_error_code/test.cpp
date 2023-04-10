// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <cassert>
#include <chrono>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

using namespace std;
using namespace std::chrono;
namespace fs = std::experimental::filesystem;

void assert_success(const error_code& ec) {
    assert(!ec);
}

void assert_failure(const error_code& ec) {
    assert(!!ec);
}

void create_file_containing(const char* const filename, const char* const contents) {
    assert(!fs::exists(filename));

    ofstream f(filename);
    f << contents << endl;
    f.close();

    assert(fs::is_regular_file(filename));
}

int main() {
    (void) fs::remove_all("DELME_dir");
    assert(!fs::exists("DELME_dir"));
    assert(fs::create_directory("DELME_dir"));
    assert(fs::is_directory("DELME_dir"));

    create_file_containing("DELME_dir/DELME_file.txt", "meow");
    create_file_containing("DELME_dir/DELME_old.txt", "disco");
    create_file_containing("DELME_dir/DELME_new.txt", "DoYouWantToBuildASnowman");

    {
        const auto now = system_clock::now();
        fs::last_write_time("DELME_dir/DELME_old.txt", now - 1h);
        fs::last_write_time("DELME_dir/DELME_new.txt", now);
        assert(fs::last_write_time("DELME_dir/DELME_old.txt") < fs::last_write_time("DELME_dir/DELME_new.txt"));
    }

    error_code ec;

    fs::copy("DELME_dir", "NONEXISTENT_dir1/NONEXISTENT_dir2", fs::copy_options::recursive, ec);
    assert_failure(ec);
    assert(!fs::exists("NONEXISTENT_dir1"));

    assert(!fs::copy_file("DELME_dir/DELME_file.txt", "DELME_dir/DELME_file.txt", fs::copy_options::skip_existing, ec));
    assert_failure(ec);
    assert(fs::is_regular_file("DELME_dir/DELME_file.txt"));

    assert(!fs::copy_file("DELME_dir/DELME_old.txt", "DELME_dir/DELME_new.txt", fs::copy_options::update_existing, ec));
    assert_success(ec);
    assert(fs::file_size("DELME_dir/DELME_old.txt") != fs::file_size("DELME_dir/DELME_new.txt"));

    assert(fs::copy_file("DELME_dir/DELME_new.txt", "DELME_dir/DELME_old.txt", fs::copy_options::update_existing, ec));
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
