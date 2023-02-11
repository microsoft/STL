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

// Also test GH-3401: <ios>: std::ios_base::openmode is not a bitmask type
constexpr bool test_gh_3401() {
    using IB = std::ios_base;
    {
        auto flags = IB::binary;
        assert(flags & IB::binary);

        flags |= IB::app;
        assert(flags & IB::binary);
        assert(flags & IB::app);

        flags &= ~IB::app;
        assert(flags & IB::binary);
        assert(!(flags & IB::app));

        flags = IB::binary | IB::app;
        assert(flags & IB::binary);
        assert(flags & IB::app);

        flags = IB::binary ^ IB::app;
        assert(flags & IB::binary);
        assert(flags & IB::app);

        flags ^= IB::app;
        assert(flags & IB::binary);
        assert(!(flags & IB::app));
    }
    {
        auto flags = IB::dec;
        assert(flags & IB::dec);

        flags |= IB::oct;
        assert(flags & IB::dec);
        assert(flags & IB::oct);

        flags &= ~IB::oct;
        assert(flags & IB::dec);
        assert(!(flags & IB::oct));

        flags = IB::dec | IB::oct;
        assert(flags & IB::dec);
        assert(flags & IB::oct);

        flags = IB::dec ^ IB::oct;
        assert(flags & IB::dec);
        assert(flags & IB::oct);

        flags ^= IB::oct;
        assert(flags & IB::dec);
        assert(!(flags & IB::oct));
    }
    {
        auto flags = IB::badbit;
        assert(flags & IB::badbit);

        flags |= IB::failbit;
        assert(flags & IB::badbit);
        assert(flags & IB::failbit);

        flags &= ~IB::failbit;
        assert(flags & IB::badbit);
        assert(!(flags & IB::failbit));

        flags = IB::failbit | IB::badbit;
        assert(flags & IB::badbit);
        assert(flags & IB::failbit);

        flags = IB::badbit ^ IB::failbit;
        assert(flags & IB::badbit);
        assert(flags & IB::failbit);

        flags ^= IB::failbit;
        assert(flags & IB::badbit);
        assert(!(flags & IB::failbit));
    }
    {
        auto flags = IB::cur;
        assert(flags & IB::cur);

        flags |= IB::end;
        assert(flags & IB::cur);
        assert(flags & IB::end);

        flags &= ~IB::end;
        assert(flags & IB::cur);
        assert(!(flags & IB::end));

        flags = IB::end | IB::cur;
        assert(flags & IB::cur);
        assert(flags & IB::end);

        flags = IB::cur ^ IB::end;
        assert(flags & IB::cur);
        assert(flags & IB::end);

        flags ^= IB::end;
        assert(flags & IB::cur);
        assert(!(flags & IB::end));
    }

    return true;
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

    test_gh_3401();
    static_assert(test_gh_3401());
}
