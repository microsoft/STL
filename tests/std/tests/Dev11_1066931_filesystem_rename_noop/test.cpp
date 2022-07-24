// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <experimental/filesystem>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

#include <test_filesystem_support.hpp>

using namespace std;
namespace fs = experimental::filesystem;

void assert_success(const error_code& ec) {
    assert(!ec);
}

void assert_failure(const error_code& ec) {
    assert(!!ec);
}

string ReadFile(const char* const filename) {
    printf(R"(reading contents of "%s"... )", filename);
    ifstream f(filename);
    if (f.good()) {
        string s;
        assert(getline(f, s).good());
        puts("success");
        return s;
    } else {
        puts("nonexistent");
        return "NONEXISTENT";
    }
}

void WriteToFile(const char* const s, const char* const filename) {
    printf(R"(writing "%s" to "%s"... )", s, filename);
    ofstream f(filename);
    f << s << endl;
    assert(f.good());
    puts("success");
}

void TouchFile(const char* const filename) {
    WriteToFile("", filename);
}

void Cleanup() {
    printf("cleaning up... ");
    error_code ec;
    fs::remove("missing.txt", ec);
    assert_success(ec);
    fs::remove("missing2.txt", ec);
    assert_success(ec);
    fs::remove("innocent.txt", ec);
    assert_success(ec);
    fs::remove("meow_old.txt", ec);
    assert_success(ec);
    fs::remove("meow_new.txt", ec);
    assert_success(ec);
    fs::remove("same.txt", ec);
    assert_success(ec);
    fs::remove("cats.txt", ec);
    assert_success(ec);
    fs::remove("dogs.txt", ec);
    assert_success(ec);
    fs::remove_all("subdir", ec);
    assert_success(ec);
    fs::remove_all("subdir_copied", ec);
    assert_success(ec);
    puts("done.");
}

error_code TestRename(const char* const old_p, const char* const new_p) {
    printf("attempting to rename \"%s\" to \"%s\"...\n", old_p, new_p);
    error_code ec;
    fs::rename(old_p, new_p, ec);
    return ec;
}

void Test_Dev11_1066931_filesystem_rename_noop() {
    WriteToFile("innocent", "innocent.txt");
    WriteToFile("meow", "meow_old.txt");
    WriteToFile("same", "same.txt");
    WriteToFile("cats", "cats.txt");
    WriteToFile("dogs", "dogs.txt");


    assert(ReadFile("missing.txt") == "NONEXISTENT");
    assert_failure(TestRename("missing.txt", "missing.txt"));
    assert(ReadFile("missing.txt") == "NONEXISTENT");

    assert(ReadFile("missing.txt") == "NONEXISTENT");
    assert(ReadFile("missing2.txt") == "NONEXISTENT");
    assert_failure(TestRename("missing.txt", "missing2.txt"));
    assert(ReadFile("missing.txt") == "NONEXISTENT");
    assert(ReadFile("missing2.txt") == "NONEXISTENT");

    assert(ReadFile("missing.txt") == "NONEXISTENT");
    assert(ReadFile("innocent.txt") == "innocent");
    assert_failure(TestRename("missing.txt", "innocent.txt"));
    assert(ReadFile("missing.txt") == "NONEXISTENT");
    assert(ReadFile("innocent.txt") == "innocent");

    assert(ReadFile("meow_old.txt") == "meow");
    assert(ReadFile("meow_new.txt") == "NONEXISTENT");
    assert_success(TestRename("meow_old.txt", "meow_new.txt"));
    assert(ReadFile("meow_old.txt") == "NONEXISTENT");
    assert(ReadFile("meow_new.txt") == "meow");

    assert(ReadFile("same.txt") == "same");
    assert_success(TestRename("same.txt", "same.txt"));
    assert(ReadFile("same.txt") == "same");

    assert(ReadFile("cats.txt") == "cats");
    assert(ReadFile("dogs.txt") == "dogs");
    assert_success(TestRename("cats.txt", "dogs.txt"));
    assert(ReadFile("cats.txt") == "NONEXISTENT");
    assert(ReadFile("dogs.txt") == "cats");
}

void Test_VSO_121387_filesystem_equivalent_should_accept_directories() {
    error_code ec;
    fs::create_directory("subdir", ec);
    assert_success(ec);
    fs::create_directory("subdir/dir1", ec);
    assert_success(ec);
    fs::create_directory("subdir/dir2", ec);
    assert_success(ec);
    TouchFile("subdir/file.txt");
    fs::create_hard_link("subdir/file.txt", "subdir/file_link.txt", ec);
    assert_success(ec);

    assert(fs::equivalent("subdir/dir2", "subdir/dir2", ec));
    assert_success(ec);
    assert(fs::equivalent("subdir/file.txt", "subdir/file.txt", ec));
    assert_success(ec);

    assert(!fs::equivalent("subdir/dir1", "subdir", ec));
    assert_success(ec);
    assert(!fs::equivalent("subdir/dir1", "subdir/dir2", ec));
    assert_success(ec);
    assert(!fs::equivalent("subdir/dir1", "subdir/file.txt", ec));
    assert_success(ec);
    assert(!fs::equivalent("subdir/file.txt", "subdir/dir1", ec));
    assert_success(ec);

    assert(fs::equivalent("subdir/file.txt", "subdir/.././subdir/file.txt", ec));
    assert_success(ec);
    assert(fs::equivalent("subdir/dir1", "subdir/.././subdir/dir1", ec));
    assert_success(ec);

    assert(fs::equivalent("subdir/file.txt", "subdir/file_link.txt", ec));
    assert_success(ec);
}

void Test_VSO_121387_filesystem_hard_link_count_should_accept_directories() {
    error_code ec;
    fs::create_directory("subdir", ec);
    assert_success(ec);
    fs::create_directory("subdir/dir1", ec);
    assert_success(ec);
    TouchFile("meow_old.txt");
    fs::create_hard_link("meow_old.txt", "meow_new.txt", ec);
    assert_success(ec);

    fs::create_hard_link("subdir/dir1", "subdir/dir2", ec);
    assert_failure(ec); // hard links aren't supported for directories on Windows

    assert(fs::hard_link_count("subdir", ec) == 1u);
    assert_success(ec);
    assert(fs::hard_link_count("meow_old.txt", ec) == 2u);
    assert_success(ec);
    assert(fs::hard_link_count("meow_new.txt", ec) == 2u);
    assert_success(ec);

    assert(fs::hard_link_count("missing.txt", ec) == static_cast<uintmax_t>(-1));
    assert_failure(ec);
}

void Test_VSO_121387_filesystem_set_write_time_should_accept_directories() {
    error_code ec;
    fs::create_directory("subdir", ec);
    assert_success(ec);

    // Technically speaking the underlying file system may not have a high
    // enough resolution for the equality tests here; but in practice our
    // target is going to be NTFS or ReFS which have 100ns increment resolution,
    // and is >= that which system_clock offers.
    auto const expectedTime = chrono::system_clock::now() - 48h;
    fs::last_write_time("subdir", expectedTime, ec);
    assert_success(ec);
    assert(fs::last_write_time("subdir", ec) == expectedTime);
    assert_success(ec);

    TouchFile("meow_old.txt");
    fs::last_write_time("meow_old.txt", expectedTime, ec);
    assert_success(ec);
    assert(fs::last_write_time("meow_old.txt", ec) == expectedTime);
    assert_success(ec);

    fs::last_write_time("missing.txt", expectedTime, ec);
    assert_failure(ec);
    assert(fs::last_write_time("missing.txt", ec) == fs::file_time_type::time_point::min());
    assert_failure(ec);
}

void Create_copy_source() {
    error_code ec;
    fs::create_directory("subdir", ec);
    assert_success(ec);
    TouchFile("subdir/cats.txt");
    TouchFile("subdir/dogs.txt");
    fs::create_directory("subdir/subsubdir", ec);
    assert_success(ec);
    TouchFile("subdir/subsubdir/meow.txt");
}

void Test_VSO_153113_copy_filename() {
    Create_copy_source();
    error_code ec;
    fs::create_directory("subdir_copied", ec);
    assert_success(ec);
    fs::copy("subdir/cats.txt", "subdir_copied", ec);
    assert_success(ec);
    assert(fs::exists("subdir_copied/cats.txt", ec));
    assert_success(ec);
}

void Test_VSO_153113_copy_examples() {
    // This example is taken directly from N4100 15.3 [fs.ops.copy]/32
    // (This tests copy_options::_Unspecified_recursion_prevention_tag)
    Create_copy_source();
    error_code ec;
    fs::copy("subdir", "subdir_copied", ec);
    assert_success(ec);
    assert(fs::exists("subdir_copied/cats.txt", ec));
    assert_success(ec);
    assert(fs::exists("subdir_copied/dogs.txt", ec));
    assert_success(ec);
    assert(!fs::exists("subdir_copied/subsubdir", ec));
    assert_success(ec);

    // N4100 15.3 [fs.ops.copy]/34
    fs::remove_all("subdir_copied", ec);
    assert_success(ec);
    fs::copy("subdir", "subdir_copied", fs::copy_options::recursive, ec);
    assert_success(ec);
    assert(fs::exists("subdir_copied/cats.txt", ec));
    assert_success(ec);
    assert(fs::exists("subdir_copied/dogs.txt", ec));
    assert_success(ec);
    assert(fs::exists("subdir_copied/subsubdir/meow.txt", ec));
    assert_success(ec);
}

template <class PathIsh>
PathIsh Get_child_dir_name(const PathIsh& p) {
    auto filenameStr = p.filename().native();
    filenameStr.push_back(L'c');
    return p / filenameStr;
}

void Test_VSO_171729_disable_recursion_pending_should_not_be_permanent() {
    error_code ec;
    fs::create_directories("subdir/subdir1/subdir1c", ec);
    assert_success(ec);
    fs::create_directories("subdir/subdir2/subdir2c", ec);
    assert_success(ec);
    fs::create_directories("subdir/subdir3/subdir3c", ec);
    assert_success(ec);

    {
        fs::recursive_directory_iterator iter("subdir");
        fs::recursive_directory_iterator end;

        assert("recursive_directory_iterator had no entries" && iter != end);
        fs::path badDirName1 = Get_child_dir_name(iter->path());
        iter.disable_recursion_pending();

        ++iter;
        assert("recursive_directory_iterator had only 1 entry" && iter != end);
        assert("disable_recursion_pending didn't work" && iter->path() != badDirName1);
        fs::path goodDirName2 = Get_child_dir_name(iter->path());

        ++iter;
        assert("recursive_directory_iterator had only 2 entries" && iter != end);
        assert("disable_recursion_pending was permanent" && iter->path() == goodDirName2);

        iter.disable_recursion_pending();
        ++iter;

        assert("recursive_directory_iterator increment didn't reset recursion_pending" && iter.recursion_pending());
        assert("recursive_directory_iterator didn't have enough entries" && iter != end);

        iter.disable_recursion_pending();
        ++iter;

        assert("recursive_directory_iterator had too many entries" && iter == end);
    }

#if _HAS_CXX17
    {
        filesystem::recursive_directory_iterator iter("subdir");
        filesystem::recursive_directory_iterator end;

        assert("std recursive_directory_iterator had no entries" && iter != end);
        filesystem::path badDirName1 = Get_child_dir_name(iter->path());
        iter.disable_recursion_pending();

        ++iter;
        assert("std recursive_directory_iterator had only 1 entry" && iter != end);
        assert("disable_recursion_pending didn't work" && iter->path() != badDirName1);
        filesystem::path goodDirName2 = Get_child_dir_name(iter->path());

        ++iter;
        assert("std recursive_directory_iterator had only 2 entries" && iter != end);
        assert("disable_recursion_pending was permanent" && iter->path() == goodDirName2);

        iter.disable_recursion_pending();
        iter.pop();

        assert("std recursive_directory_iterator pop didn't reset recursion_pending" && iter.recursion_pending());
        assert("std recursive_directory_iterator at end after pop" && iter != end);
        filesystem::path goodDirName3 = Get_child_dir_name(iter->path());

        ++iter;
        assert("std recursive_directory_iterator at end in pop subdirectory" && iter != end);
        assert("disable_recursion_pending was permanent" && iter->path() == goodDirName3);

        ++iter;
        assert("std recursive_directory_iterator had too many entries" && iter == end);
    }
#endif // _HAS_CXX17
}

int main() {
    error_code ec;
    const auto previousCd = fs::current_path(ec);
    assert_success(ec);
    const auto testDir = get_test_directory("filesystem_rename_noop");
    printf("changing directory to \"%ls\"\n", testDir.native().c_str());
    fs::create_directory(testDir, ec);
    assert_success(ec);
    fs::current_path(testDir, ec);
    assert_success(ec);

    puts("running test Test_Dev11_1066931_filesystem_rename_noop");
    Test_Dev11_1066931_filesystem_rename_noop();
    Cleanup();
    puts("running test Test_VSO_121387_filesystem_equivalent_should_accept_directories");
    Test_VSO_121387_filesystem_equivalent_should_accept_directories();
    Cleanup();
    puts("running test Test_VSO_121387_filesystem_hard_link_count_should_accept_directories");
    Test_VSO_121387_filesystem_hard_link_count_should_accept_directories();
    Cleanup();
    puts("running test Test_VSO_121387_filesystem_set_write_time_should_accept_directories");
    Test_VSO_121387_filesystem_set_write_time_should_accept_directories();
    Cleanup();
    puts("running test Test_VSO_153113_copy_filename");
    Test_VSO_153113_copy_filename();
    Cleanup();
    puts("running test Test_VSO_153113_copy_examples");
    Test_VSO_153113_copy_examples();
    Cleanup();
    puts("running test Test_VSO_171729_disable_recursion_pending_should_not_be_permanent");
    Test_VSO_171729_disable_recursion_pending_should_not_be_permanent();
    Cleanup();

    fs::current_path(previousCd, ec);
    assert_success(ec);
    fs::remove_all(testDir, ec);
    assert_success(ec);
}
