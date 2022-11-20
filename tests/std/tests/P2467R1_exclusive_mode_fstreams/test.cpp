// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <filesystem>
#include <fstream>
#include <ios>

namespace fs = std::filesystem;

const fs::path test_file{L"test.dat"};

struct CreateTempFile {
    fs::path m_path;
    CreateTempFile(const fs::path& path) : m_path(path) {
        std::ofstream{path};
    }

    ~CreateTempFile() {
        fs::remove(m_path);
    }
};

void test_file_create(const std::ios_base::openmode mode) {
    {
        std::fstream file{test_file, mode};
        assert(!file.fail());
    }
    assert(fs::exists(test_file));
    fs::remove(test_file);
}

void test_file_already_exists(const std::ios_base::openmode mode) {
    CreateTempFile tmp{test_file};
    std::fstream file{test_file, mode};
    assert(file.fail());
}

void test_file_create_fail(const std::ios_base::openmode bad_mode) {
    {
        std::fstream file{test_file, bad_mode};
        assert(file.fail());
    }
    assert(!fs::exists(test_file));
}

int main() {
    using IB = std::ios_base;

    test_file_create(IB::out | IB::noreplace);
    test_file_create(IB::out | IB::trunc | IB::noreplace);
    test_file_create(IB::out | IB::in | IB::trunc | IB::noreplace);
    test_file_create(IB::out | IB::binary | IB::noreplace);
    test_file_create(IB::out | IB::binary | IB::trunc | IB::noreplace);
    test_file_create(IB::out | IB::in | IB::trunc | IB::binary | IB::noreplace);

    test_file_already_exists(IB::out | IB::noreplace);
    test_file_already_exists(IB::out | IB::trunc | IB::noreplace);
    test_file_already_exists(IB::out | IB::in | IB::trunc | IB::noreplace);
    test_file_already_exists(IB::out | IB::binary | IB::noreplace);
    test_file_already_exists(IB::out | IB::binary | IB::trunc | IB::noreplace);
    test_file_already_exists(IB::out | IB::in | IB::trunc | IB::binary | IB::noreplace);

    test_file_create_fail(IB::in | IB::noreplace);
    test_file_create_fail(IB::in | IB::trunc | IB::noreplace);
    test_file_create_fail(IB::in | IB::binary | IB::noreplace);
    test_file_create_fail(IB::in | IB::binary | IB::trunc | IB::noreplace);
}
