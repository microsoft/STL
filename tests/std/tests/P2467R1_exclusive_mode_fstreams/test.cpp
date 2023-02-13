// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <filesystem>
#include <fstream>
#include <ios>
#include <type_traits>

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
    using namespace std;
    using IB        = ios_base;
    auto test_flags = [](const auto first, const auto second) {
        auto flags = first;
        assert(flags & first);

        flags |= second;
        assert(flags & first);
        assert(flags & second);

        flags &= ~second;
        assert(flags & first);
        assert(!(flags & second));

        flags = first | second;
        assert(flags & first);
        assert(flags & second);

        flags = first ^ second;
        assert(flags & first);
        assert(flags & second);

        flags ^= second;
        assert(flags & first);
        assert(!(flags & second));
    };
    test_flags(IB::binary, IB::app);
    test_flags(IB::dec, IB::oct);
    test_flags(IB::badbit, IB::failbit);
    test_flags(IB::cur, IB::end);

    static_assert(is_same_v<remove_const_t<decltype(IB::beg)>, IB::seekdir>);
    static_assert(is_same_v<remove_const_t<decltype(IB::cur)>, IB::seekdir>);
    static_assert(is_same_v<remove_const_t<decltype(IB::end)>, IB::seekdir>);

    static_assert(is_same_v<remove_const_t<decltype(IB::in)>, IB::openmode>);
    static_assert(is_same_v<remove_const_t<decltype(IB::out)>, IB::openmode>);
    static_assert(is_same_v<remove_const_t<decltype(IB::ate)>, IB::openmode>);
    static_assert(is_same_v<remove_const_t<decltype(IB::app)>, IB::openmode>);
    static_assert(is_same_v<remove_const_t<decltype(IB::trunc)>, IB::openmode>);
    static_assert(is_same_v<remove_const_t<decltype(IB::binary)>, IB::openmode>);
#if _HAS_CXX23
    static_assert(is_same_v<remove_const_t<decltype(IB::noreplace)>, IB::openmode>);
#endif

    static_assert(is_same_v<remove_const_t<decltype(IB::goodbit)>, IB::iostate>);
    static_assert(is_same_v<remove_const_t<decltype(IB::eofbit)>, IB::iostate>);
    static_assert(is_same_v<remove_const_t<decltype(IB::failbit)>, IB::iostate>);
    static_assert(is_same_v<remove_const_t<decltype(IB::badbit)>, IB::iostate>);

    static_assert(is_same_v<remove_const_t<decltype(IB::skipws)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::unitbuf)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::uppercase)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::showbase)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::showpoint)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::showpos)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::left)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::right)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::internal)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::dec)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::oct)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::hex)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::scientific)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::fixed)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::boolalpha)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::adjustfield)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::basefield)>, IB::fmtflags>);
    static_assert(is_same_v<remove_const_t<decltype(IB::floatfield)>, IB::fmtflags>);

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
