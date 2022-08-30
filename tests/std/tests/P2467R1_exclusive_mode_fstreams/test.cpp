// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

#define FILENAME L"test.dat"

struct CreateTempFile {
    fs::path m_path;
    CreateTempFile(const fs::path& path) : m_path(path) {
        std::ofstream c{path};
    }

    ~CreateTempFile() {
        fs::remove(m_path);
    }
};

void test_file_create(std::ios_base::openmode mode) {
    fs::path p{FILENAME};
    {
        std::fstream file{p, mode};
        assert(!file.fail());
    }
    assert(fs::exists(p));
    fs::remove(p);
}

void test_file_already_exist(std::ios_base::openmode mode) {
    fs::path p{FILENAME};
    CreateTempFile tmp{p};
    std::fstream file{p, mode};
    assert(file.fail());
}

void test_file_create_fail(std::ios_base::openmode bad_mode) {
    fs::path p{FILENAME};
    {
        std::fstream file{p, bad_mode};
        assert(file.fail());
    }
    assert(!fs::exists(p));
}

int main() {
    test_file_create(std::ios_base::out | std::ios_base::noreplace);
    test_file_create(std::ios_base::out | std::ios_base::trunc | std::ios_base::noreplace);
    test_file_create(std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::noreplace);
    test_file_create(std::ios_base::out | std::ios_base::binary | std::ios_base::noreplace);
    test_file_create(std::ios_base::out | std::ios_base::binary | std::ios_base::trunc | std::ios_base::noreplace);
    test_file_create(std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::binary
                     | std::ios_base::noreplace);

    test_file_already_exist(std::ios_base::out | std::ios_base::noreplace);
    test_file_already_exist(std::ios_base::out | std::ios_base::trunc | std::ios_base::noreplace);
    test_file_already_exist(std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::noreplace);
    test_file_already_exist(std::ios_base::out | std::ios_base::binary | std::ios_base::noreplace);
    test_file_already_exist(
        std::ios_base::out | std::ios_base::binary | std::ios_base::trunc | std::ios_base::noreplace);
    test_file_already_exist(std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::binary
                            | std::ios_base::noreplace);

    test_file_create_fail(std::ios_base::in | std::ios_base::noreplace);
    test_file_create_fail(std::ios_base::in | std::ios_base::trunc | std::ios_base::noreplace);
    test_file_create_fail(std::ios_base::binary | std::ios_base::in | std::ios_base::trunc | std::ios_base::noreplace);
}
