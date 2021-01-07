// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING
#include <algorithm>
#include <array>
#include <assert.h>
#include <chrono>
#include <cvt/cp1251>
#include <cvt/sjis>
#include <cvt/utf8_utf16>
#include <filesystem>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <optional>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

#include <test_filesystem_support.hpp>

using namespace std;
using namespace std::chrono;
using namespace std::filesystem;

constexpr wstring_view badPath = L"// ?? ?? ///// ?? ?? ? ////"sv;
const path nonexistentPaths[]  = {
    L"C:/This/Path/Should/Not/Exist"sv,
    L"//this_path_does_not_exist_on_the_network_e9da301701f70ead24c65bd30f600d15/docs"sv,
};
constexpr wstring_view longSuffix =
    LR"(really\long\path\longer\than\max_path\goes\here\and it just goes)"
    LR"( on and on\and it just goes on and on\and it just goes on and on\and it just goes on and on)"
    LR"(\and it just goes on and on\and it just goes on and on\and it just goes on and on\and it)"
    LR"( just goes on and on\and it just goes on and on)"sv;
constexpr uintmax_t bad_file_size      = static_cast<uintmax_t>(-1);
constexpr file_time_type bad_file_time = file_time_type::min();

template <typename Elem, typename Traits>
[[nodiscard]] constexpr bool starts_with(
    const basic_string_view<Elem, Traits> str, const basic_string_view<Elem, Traits> prefix) noexcept {
    // test if str begins with prefix
    return str.size() >= prefix.size() && Traits::compare(str.data(), prefix.data(), prefix.size()) == 0;
}

struct test_temp_directory {
    error_code ec;
    path directoryPath;
    explicit test_temp_directory(const string_view testName) : directoryPath(get_new_test_directory(testName)) {
        remove_all(directoryPath, ec);
        if (ec) {
            wcerr << L"Warning, couldn't clean up " << directoryPath << L" before test.\n";
        } else {
            create_directories(directoryPath, ec);
            if (ec) {
                wcerr << L"Warning, couldn't create test directory " << directoryPath << L" before test.\n";
            }
        }
    }

    ~test_temp_directory() noexcept {
        remove_all(directoryPath, ec);
        if (ec) {
            wcerr << L"Warning, couldn't clean up " << directoryPath << L" after test.\n";
        }
    }
};

bool pass = true;

bool expect(const bool b, const char* const func, const int line, const char* const message) {
    if (!b) {
        wcerr << func << L" @ " << line << L": check failed: " << message << L'\n';
        pass = false;
    }
    return b;
}

#define EXPECT(Expr) expect(Expr, __func__, __LINE__, #Expr)

bool good(const error_code& ec) {
    bool overall = true;
    if (ec.value() != 0) {
        wcerr << L"Unexpected error " << ec.value() << L" " << ec.message().c_str() << L"\n";
        overall = false;
    }

    if (ec.category() != system_category()) {
        wcerr << L"Unexpected category " << ec.category().name() << L"\n";
        overall = false;
    }

    return overall;
}

bool bad(const error_code& ec) {
    return ec.value() != 0 && ec.category() == system_category();
}

template <class R1, class R2>
bool narrow_equal(R1&& r1, R2&& r2) {
    if (r1.size() != r2.size()) {
        return false;
    }

    auto first1 = begin(r1);
    static_assert(sizeof(*first1) == 1);
    const auto last1 = end(r1);
    auto first2      = begin(r2);
    static_assert(sizeof(*first2) == 1);

    for (; first1 != last1; ++first1, (void) ++first2) {
        const auto x = static_cast<unsigned char>(*first1);
        const auto y = static_cast<unsigned char>(*first2);
        if (x != y) {
            return false;
        }
    }

    return true;
}

template <typename Lambda>
bool throws_system_error(Lambda lambda) {
    try {
        lambda();
        return false;
    } catch (const system_error&) {
        // Good!
        return true;
    }
}

template <typename Lambda>
bool throws_filesystem_error(Lambda lambda, string_view functionName, const path& p1 = {}, const path& p2 = {}) {
    try {
        lambda();
        return false;
    } catch (const filesystem_error& err) {
        // Good!
        return string_view(err.what()).find(functionName) != string_view::npos && bad(err.code())
            && err.path1().native() == p1.native() && err.path2().native() == p2.native();
    }
}

void create_file_containing(const path& filename, const wchar_t* const contents) {
    wofstream f(filename, wofstream::trunc);
    f << contents;
}

[[nodiscard]] wstring read_file_contents(const path& filename) {
    wifstream f(filename);
    if (!f) {
        return L"OPEN FAILURE";
    }

    wostringstream str;
    str << f.rdbuf();
    return str.str();
}

void write_file_data(const path& fileName, const vector<char>& data) {
    ofstream result(fileName, ofstream::trunc | ofstream::binary | ofstream::out);
    EXPECT(static_cast<bool>(result.write(data.data(), static_cast<streamsize>(data.size()))));
}

vector<char> read_file_data(const path& fileName) {
    constexpr size_t chunkSize = 4096;
    vector<char> ret;

    for (ifstream f(fileName, ios_base::binary); f;) {
        const size_t oldSize = ret.size();
        ret.resize(oldSize + chunkSize);
        f.read(ret.data() + oldSize, chunkSize);
        ret.resize(oldSize + static_cast<size_t>(f.gcount()));
    }

    return ret;
}

struct decomposition_test_case {
    wstring_view inputPath;
    wstring_view rootName;
    wstring_view rootDirectory;
    wstring_view relativePath;
    wstring_view parentPath;
    wstring_view filename;
    bool absolute;

    friend wostream& operator<<(wostream& wos, const decomposition_test_case& testCase) {
        wos << boolalpha << L"Input path:              " << testCase.inputPath << L"\n"
            << L"Expected root_name:      " << testCase.rootName << L"\n"
            << L"Expected root_directory: " << testCase.rootDirectory << L"\n"
            << L"Expected relative_path:  " << testCase.relativePath << L"\n"
            << L"Expected parent_path:    " << testCase.parentPath << L"\n"
            << L"Expected filename:       " << testCase.filename << L"\n"
            << L"Expected absolute:       " << testCase.absolute << L"\n";

        return wos;
    }
};

constexpr decomposition_test_case decompTestCases[] = {
    // These cases aim to provide decision coverage for all the inter-element decomposition functions:
    {L""sv, L""sv, L""sv, L""sv, L""sv, L""sv, false}, // basic empty path
    {L"h"sv, L""sv, L""sv, L"h"sv, L""sv, L"h"sv, false}, // basic short relative path
    {L"hi"sv, L""sv, L""sv, L"hi"sv, L""sv, L"hi"sv, false}, // basic relative path
    {L"c:"sv, L"c:"sv, L""sv, L""sv, L"c:"sv, L""sv, false}, // empty drive-relative path
    {L"c:dog"sv, L"c:"sv, L""sv, L"dog"sv, L"c:"sv, L"dog"sv, false}, // non-empty drive-relative path
    {L"C:"sv, L"C:"sv, L""sv, L""sv, L"C:"sv, L""sv, false}, // also test uppercase version
    {L"cc:dog"sv, L""sv, L""sv, L"cc:dog"sv, L""sv, L"cc:dog"sv, false}, // malformed drive (more follow)
    {L"::dog"sv, L""sv, L""sv, L"::dog"sv, L""sv, L"::dog"sv, false},
    {L" :dog"sv, L""sv, L""sv, L" :dog"sv, L""sv, L" :dog"sv, false},
    {L":dog"sv, L""sv, L""sv, L":dog"sv, L""sv, L":dog"sv, false},
    {LR"(\:dog)"sv, L""sv, LR"(\)"sv, L":dog"sv, LR"(\)"sv, L":dog"sv, false},
    {LR"(\:\dog)"sv, L""sv, LR"(\)"sv, LR"(:\dog)"sv, LR"(\:)"sv, L"dog"sv, false},
    {LR"(1:\dog)"sv, L""sv, L""sv, LR"(1:\dog)"sv, LR"(1:)"sv, L"dog"sv, false},
    {LR"(\Dev\msvc)"sv, L""sv, LR"(\)"sv, LR"(Dev\msvc)"sv, LR"(\Dev)"sv, L"msvc"sv,
        false}, // current drive root relative path
    {LR"(c:\)"sv, L"c:"sv, LR"(\)"sv, L""sv, LR"(c:\)"sv, L""sv, true}, // absolute Win32 path
    {L"c:/"sv, L"c:"sv, L"/"sv, L""sv, L"c:/"sv, L""sv, true},
    {LR"(c:\cat\dog\elk)"sv, L"c:"sv, LR"(\)"sv, LR"(cat\dog\elk)"sv, LR"(c:\cat\dog)"sv, L"elk"sv,
        true}, // usual path form
    {LR"(c:\cat\dog\elk\)"sv, L"c:"sv, LR"(\)"sv, LR"(cat\dog\elk\)"sv, LR"(c:\cat\dog\elk)"sv, L""sv,
        true}, // with trailing backslash
    {LR"(c:/cat\dog\elk)"sv, L"c:"sv, LR"(/)"sv, LR"(cat\dog\elk)"sv, LR"(c:/cat\dog)"sv, L"elk"sv,
        true}, // also with some /s
    {LR"(c:\cat\dog/elk/)"sv, L"c:"sv, LR"(\)"sv, LR"(cat\dog/elk/)"sv, LR"(c:\cat\dog/elk)"sv, L""sv, true},
    // with extra long root-directory:
    {LR"(c:\\\\\cat\dog\elk)"sv, L"c:"sv, LR"(\\\\\)"sv, LR"(cat\dog\elk)"sv, LR"(c:\\\\\cat\dog)"sv, L"elk"sv, true},
    {LR"(\\?\x)"sv, LR"(\\?)"sv, LR"(\)"sv, L"x"sv, LR"(\\?\)"sv, L"x"sv, true}, // special case \\?\ prefix
    {LR"(\\?\)"sv, LR"(\\?)"sv, LR"(\)"sv, L""sv, LR"(\\?\)"sv, L""sv, true}, // special case \\?\ prefix with no suffix
    // with extra trailing \, not the special prefix any longer, but becomes the \\server form:
    {LR"(\\?\\)"sv, LR"(\\?)"sv, LR"(\\)"sv, L""sv, LR"(\\?\\)"sv, L""sv, true},
    {LR"(\\.\x)"sv, LR"(\\.)"sv, LR"(\)"sv, L"x"sv, LR"(\\.\)"sv, L"x"sv, true}, // also \\.\ special prefix
    {LR"(\??\x)"sv, LR"(\??)"sv, LR"(\)"sv, L"x"sv, LR"(\??\)"sv, L"x"sv, true}, // also \??\ special prefix
    // adding an extra trailing \ to this one makes it become a root relative path:
    {LR"(\??\\)"sv, L""sv, LR"(\)"sv, LR"(??\\)"sv, LR"(\??)"sv, L""sv, false},
    {LR"(\x?\x)"sv, L""sv, LR"(\)"sv, LR"(x?\x)"sv, LR"(\x?)"sv, L"x"sv, false}, // not special (more follow)
    {LR"(\?x\x)"sv, L""sv, LR"(\)"sv, LR"(?x\x)"sv, LR"(\?x)"sv, L"x"sv, false},
    {LR"(\\x\x)"sv, LR"(\\x)"sv, LR"(\)"sv, LR"(x)"sv, LR"(\\x\)"sv, L"x"sv, true},
    {LR"(\??)"sv, LR"()"sv, LR"(\)"sv, LR"(??)"sv, LR"(\)"sv, L"??"sv, false},
    {LR"(\???)"sv, LR"()"sv, LR"(\)"sv, LR"(???)"sv, LR"(\)"sv, L"???"sv, false},
    {LR"(\????)"sv, LR"()"sv, LR"(\)"sv, LR"(????)"sv, LR"(\)"sv, L"????"sv, false},
    {LR"(\\.xx)"sv, LR"(\\.xx)"sv, L""sv, L""sv, LR"(\\.xx)"sv, L""sv, true}, // became \\server form
    {LR"(\\server\share)"sv, LR"(\\server)"sv, LR"(\)"sv, L"share"sv, LR"(\\server\)"sv, L"share"sv,
        true}, // network path
    {LR"(/\server\share)"sv, LR"(/\server)"sv, LR"(\)"sv, L"share"sv, LR"(/\server\)"sv, L"share"sv, true},
    {LR"(/\server/share)"sv, LR"(/\server)"sv, LR"(/)"sv, L"share"sv, LR"(/\server/)"sv, L"share"sv, true},
    {LR"(//server\share)"sv, LR"(//server)"sv, LR"(\)"sv, L"share"sv, LR"(//server\)"sv, L"share"sv, true},
    {LR"(//server/share)"sv, LR"(//server)"sv, LR"(/)"sv, L"share"sv, LR"(//server/)"sv, L"share"sv, true},
    {LR"(\/server\share)"sv, LR"(\/server)"sv, LR"(\)"sv, L"share"sv, LR"(\/server\)"sv, L"share"sv, true},
    {LR"(\/server/share)"sv, LR"(\/server)"sv, LR"(/)"sv, L"share"sv, LR"(\/server/)"sv, L"share"sv, true},
    // \\\ doesn't fall into \\server, becomes a current drive root relative path:
    {LR"(\\\\)"sv, L""sv, LR"(\\\\)"sv, L""sv, LR"(\\\\)"sv, L""sv, false},
    {LR"(\\\dog)"sv, L""sv, LR"(\\\)"sv, L"dog"sv, LR"(\\\)"sv, L"dog"sv, false},
    // document that we don't treat \\?\UNC special even if MSDN does:
    {LR"(\\?\UNC\server\share)"sv, LR"(\\?)"sv, LR"(\)"sv, LR"(UNC\server\share)"sv, LR"(\\?\UNC\server)"sv, L"share"sv,
        true},
    {LR"(\\?\UNC)"sv, LR"(\\?)"sv, LR"(\)"sv, LR"(UNC)"sv, LR"(\\?\)"sv, L"UNC"sv, true}, // other similar cases
    {LR"(\\?\UNC\server)"sv, LR"(\\?)"sv, LR"(\)"sv, LR"(UNC\server)"sv, LR"(\\?\UNC)"sv, L"server"sv, true},
    {LR"(\\?\UNC\server\)"sv, LR"(\\?)"sv, LR"(\)"sv, LR"(UNC\server\)"sv, LR"(\\?\UNC\server)"sv, L""sv, true},
    {LR"(\\?\UNC\\)"sv, LR"(\\?)"sv, LR"(\)"sv, LR"(UNC\\)"sv, LR"(\\?\UNC)"sv, L""sv, true},
    // document that drive letters aren't special with special prefixes:
    {LR"(\\.\C:attempt_at_relative)"sv, LR"(\\.)"sv, LR"(\)"sv, LR"(C:attempt_at_relative)"sv, LR"(\\.\)"sv,
        L"C:attempt_at_relative"sv, true},
    // other interesting user-submitted test cases:
    {LR"(\)"sv, L""sv, LR"(\)"sv, L""sv, LR"(\)"sv, L""sv, false},
    {LR"(\\)"sv, L""sv, LR"(\\)"sv, L""sv, LR"(\\)"sv, L""sv, false},
    {LR"(\\\)"sv, L""sv, LR"(\\\)"sv, L""sv, LR"(\\\)"sv, L""sv, false},
    {LR"(\\\.)"sv, L""sv, LR"(\\\)"sv, L"."sv, LR"(\\\)"sv, L"."sv, false},
    {LR"(\\\..)"sv, L""sv, LR"(\\\)"sv, L".."sv, LR"(\\\)"sv, L".."sv, false},
    {LR"(\\\.\)"sv, L""sv, LR"(\\\)"sv, LR"(.\)"sv, LR"(\\\.)"sv, L""sv, false},
    {LR"(\\\..\)"sv, L""sv, LR"(\\\)"sv, LR"(..\)"sv, LR"(\\\..)"sv, L""sv, false},
    {L"/"sv, L""sv, L"/"sv, L""sv, L"/"sv, L""sv, false}, {L"//"sv, L""sv, L"//"sv, L""sv, L"//"sv, L""sv, false},
    {L"///"sv, L""sv, L"///"sv, L""sv, L"///"sv, L""sv, false},
    {LR"(\/)"sv, L""sv, LR"(\/)"sv, L""sv, LR"(\/)"sv, L""sv, false},
    {L"/c"sv, L""sv, L"/"sv, L"c"sv, L"/"sv, L"c"sv, false}, {L"/c:"sv, L""sv, L"/"sv, L"c:"sv, L"/"sv, L"c:"sv, false},
    {L".."sv, L""sv, L""sv, L".."sv, L""sv, L".."sv, false},
    {LR"(\.)"sv, L""sv, LR"(\)"sv, L"."sv, LR"(\)"sv, L"."sv, false},
    {L"/."sv, L""sv, L"/"sv, L"."sv, L"/"sv, L"."sv, false},
    {LR"(\..)"sv, L""sv, LR"(\)"sv, L".."sv, LR"(\)"sv, L".."sv, false},
    {LR"(\..\..)"sv, L""sv, LR"(\)"sv, LR"(..\..)"sv, LR"(\..)"sv, L".."sv, false},
    {LR"(c:\..)"sv, L"c:"sv, LR"(\)"sv, L".."sv, LR"(c:\)"sv, L".."sv, true},
    {L"c:.."sv, L"c:"sv, L""sv, L".."sv, L"c:"sv, L".."sv, false},
    {LR"(c:..\..)"sv, L"c:"sv, L""sv, LR"(..\..)"sv, LR"(c:..)"sv, L".."sv, false},
    {LR"(c:\..\..)"sv, L"c:"sv, LR"(\)"sv, LR"(..\..)"sv, LR"(c:\..)"sv, L".."sv, true},
    {LR"(\\server\..)"sv, LR"(\\server)"sv, LR"(\)"sv, LR"(..)"sv, LR"(\\server\)"sv, L".."sv, true},
    {LR"(\\server\share\..\..)"sv, LR"(\\server)"sv, LR"(\)"sv, LR"(share\..\..)"sv, LR"(\\server\share\..)"sv, L".."sv,
        true},
    {LR"(\\server\.\share)"sv, LR"(\\server)"sv, LR"(\)"sv, LR"(.\share)"sv, LR"(\\server\.)"sv, L"share"sv, true},
    {LR"(\\server\.\..\share)"sv, LR"(\\server)"sv, LR"(\)"sv, LR"(.\..\share)"sv, LR"(\\server\.\..)"sv, L"share"sv,
        true},
    {LR"(\..\../)"sv, L""sv, LR"(\)"sv, LR"(..\../)"sv, LR"(\..\..)"sv, L""sv, false},
    {LR"(\\..\../)"sv, LR"(\\..)"sv, LR"(\)"sv, LR"(../)"sv, LR"(\\..\..)"sv, L""sv, true},
    {LR"(\\\..\../)"sv, L""sv, LR"(\\\)"sv, LR"(..\../)"sv, LR"(\\\..\..)"sv, L""sv, false},
    {LR"(\\?/)"sv, LR"(\\?)"sv, LR"(/)"sv, L""sv, LR"(\\?/)"sv, L""sv, true},
    {LR"(\/?/)"sv, LR"(\/?)"sv, LR"(/)"sv, L""sv, LR"(\/?/)"sv, L""sv, true},
    {L"//?/"sv, L"//?"sv, L"/"sv, L""sv, L"//?/"sv, L""sv, true},
    {L"//server"sv, L"//server"sv, L""sv, L""sv, L"//server"sv, L""sv, true},
    {L"[:/abc"sv, L""sv, L""sv, L"[:/abc"sv, L"[:"sv, L"abc"sv, false}, // bug where [ was considered a drive letter
};

struct decomposition_test_result {
    path rootName;
    bool hasRootName;
    path rootDirectory;
    bool hasRootDirectory;
    path rootPath;
    bool hasRootPath;
    path relativePath;
    bool hasRelativePath;
    path parentPath;
    bool hasParentPath;
    path filename;
    bool hasFilename;
    bool absolute;

    explicit decomposition_test_result(const path& p)
        : rootName(p.root_name()), hasRootName(p.has_root_name()), rootDirectory(p.root_directory()),
          hasRootDirectory(p.has_root_directory()), rootPath(p.root_path()), hasRootPath(p.has_root_path()),
          relativePath(p.relative_path()), hasRelativePath(p.has_relative_path()), parentPath(p.parent_path()),
          hasParentPath(p.has_parent_path()), filename(p.filename()), hasFilename(p.has_filename()),
          absolute(p.is_absolute()) {
        EXPECT(absolute == !p.is_relative());
    }

    friend wostream& operator<<(wostream& wos, const decomposition_test_result& testCase) {
        wos << boolalpha << L"Actual root_name:      " << testCase.rootName.native() << L"\n"
            << L"Actual root_directory: " << testCase.rootDirectory.native() << L"\n"
            << L"Actual root_path:      " << testCase.rootPath.native() << L"\n"
            << L"Actual relative_path:  " << testCase.relativePath.native() << L"\n"
            << L"Actual parent_path:    " << testCase.parentPath.native() << L"\n"
            << L"Actual filename:       " << testCase.filename.native() << L"\n"
            << L"Actual absolute:       " << testCase.absolute << L"\n";

        return wos;
    }
};

bool run_decomp_test_case(const decomposition_test_case& testCase) {
    const decomposition_test_result actual(testCase.inputPath);
    if (actual.rootName.native() == testCase.rootName && actual.hasRootName != testCase.rootName.empty()
        && actual.rootDirectory.native() == testCase.rootDirectory
        && actual.hasRootDirectory != testCase.rootDirectory.empty()
        && actual.rootPath.native()
               == static_cast<wstring>(testCase.rootName) + static_cast<wstring>(testCase.rootDirectory)
        && actual.hasRootPath != (testCase.rootName.empty() && testCase.rootDirectory.empty())
        && actual.relativePath.native() == testCase.relativePath
        && actual.hasRelativePath != testCase.relativePath.empty() && actual.parentPath.native() == testCase.parentPath
        && actual.hasParentPath != testCase.parentPath.empty() && actual.filename.native() == testCase.filename
        && actual.hasFilename != testCase.filename.empty() && actual.absolute == testCase.absolute) {
        return true;
    }

    wcerr << L"Test failure:\n" << testCase << actual;

    return false;
}

struct stem_test_case {
    wstring_view input;
    wstring_view stem;
    wstring_view extension;
};

constexpr stem_test_case stemTestCases[] = {
    {L"."sv, L"."sv, L""sv}, // . and .. special cases
    {L".."sv, L".."sv, L""sv},
    {L".config"sv, L".config"sv, L""sv}, // filenames with 1 leading period are special
    {L"..config"sv, L"."sv, L".config"sv}, // 2 periods aren't special anymore
    {L"...config"sv, L".."sv, L".config"sv},
    {L"classic_textfile.txt"sv, L"classic_textfile"sv, L".txt"sv},
    {L"cat.dog.elk.tar"sv, L"cat.dog.elk"sv, L".tar"sv},
    {L"cat.dog.elk"sv, L"cat.dog"sv, L".elk"sv},
    {L"cat.dog"sv, L"cat"sv, L".dog"sv},
    {L"cat"sv, L"cat"sv, L""sv},
    {L"cat."sv, L"cat"sv, L"."sv},

    // all of the above with alternate data streams
    {L".:alternate_meow"sv, L"."sv, L""sv},
    {L"..:alternate_dog"sv, L".."sv, L""sv},
    {L".config:alternate_elk"sv, L".config"sv, L""sv},
    {L"..config:alternate_other_things"sv, L"."sv, L".config"sv},
    {L"...config:alternate_other_things"sv, L".."sv, L".config"sv},
    {L"classic_textfile.txt:$DATA"sv, L"classic_textfile"sv, L".txt"sv},
    {L"cat.dog.elk.tar:pay_no_attention"sv, L"cat.dog.elk"sv, L".tar"sv},
    {L"cat.dog.elk:to behind this curtain"sv, L"cat.dog"sv, L".elk"sv},
    {L"cat.dog:even:if:this:curtain:is:malformed"sv, L"cat"sv, L".dog"sv},
    {L"cat:what?"sv, L"cat"sv, L""sv},
    {L"cat.:alternate_fun"sv, L"cat"sv, L"."sv},
};

bool run_stem_test_case(const stem_test_case& testCase) {
    const path p(testCase.input);
    if (p.stem() == testCase.stem && p.extension() == testCase.extension) {
        return true;
    }

    wcerr << L"Expected " << p.native() << L" to have stem() " << testCase.stem << L" and extension() "
          << testCase.extension << L", but it actually has stem() " << p.stem().native() << L" and extension() "
          << p.extension().native() << L"\n";

    return false;
}

enum class compare_result { less, equal, greater };

wostream& operator<<(wostream& wos, const compare_result c) {
    switch (c) {
    case compare_result::less:
        return wos << L"less";
    case compare_result::equal:
        return wos << L"equal";
    case compare_result::greater:
        return wos << L"greater";
    default:
        abort();
    }
}

struct compare_test_case {
    wstring_view left;
    wstring_view right;
    compare_result expected;
};

constexpr compare_test_case compareTestCases[] = {
    // basic tests:
    {L""sv, L""sv, compare_result::equal},
    {L""sv, L"anything"sv, compare_result::less},
    {L"anything"sv, L""sv, compare_result::greater},
    {L"c:"sv, L"c:"sv, compare_result::equal},
    // different root_directory values compare equal:
    {L"/"sv, L"//"sv, compare_result::equal},
    {L"//////"sv, L"//"sv, compare_result::equal},
    // different counts of /s and different kinds compare equal:
    {L"c://a/b"sv, L"c:/a//b"sv, compare_result::equal},
    {L"c://a/b"sv, LR"(c:/a/\b)"sv, compare_result::equal},
    {L"c:/a/b///c"sv, L"c:/a///b/c"sv, compare_result::equal},
    {LR"(c:/a/b\//c)"sv, LR"(c:/a//\b/c)"sv, compare_result::equal},
    // comparing root_name:
    {LR"(\\server\share)"sv, L"C:/a"sv, compare_result::greater},
    {LR"(//server\share)"sv, L"C:/a"sv, compare_result::less},
    // doesn't actually get to has_root_name test, since root_name comparison differs:
    {LR"(c:a)"sv, LR"(C:\a)"sv, compare_result::greater},
    // different values of has_root_directory():
    {L"c:/"sv, L"c:"sv, compare_result::greater},
    {L"c:"sv, L"c:/"sv, compare_result::less},
    {L"c:////"sv, L"c://"sv, compare_result::equal},
    {LR"(c:\a)"sv, LR"(c:a)"sv, compare_result::greater},
    {LR"(c:a)"sv, LR"(c:\a)"sv, compare_result::less},
    // fewer path elements compare less:
    {L"c:/a"sv, L"c:/a/b"sv, compare_result::less},
    {L"c:/a/b"sv, L"c:/a"sv, compare_result::greater},
    {L"c:/a/"sv, L"c:/a/b"sv, compare_result::less},
    {L"c:/a/b"sv, L"c:/a/"sv, compare_result::greater},
    {L"c:/a/b"sv, L"c:/a/b/"sv, compare_result::less}, // trailing empty element
    {L"c:/a/b/"sv, L"c:/a/b"sv, compare_result::greater},
    // comparisons inside path elements, note assumption L'A' < L'a':
    {L"c:/CASEMATTERS"sv, L"c:/casematters"sv, compare_result::less},
    {L"c:/abcdefg"sv, L"c:/hijklmn"sv, compare_result::less},
    {L"c:/casematters"sv, L"c:/CASEMATTERS"sv, compare_result::greater},
    {L"c:/hijklmn"sv, L"c:/abcdefg"sv, compare_result::greater},
    // differ only in where the / goes:
    {L"c:/ap/ple"sv, L"c:/app/le"sv, compare_result::less},
    {LR"(c:/ap\ple)"sv, LR"(c:/app\le)"sv, compare_result::less},
    {L"c:/ap/ple"sv, LR"(c:/app\le)"sv, compare_result::less},
    {LR"(c:/ap\ple)"sv, L"c:/app/le"sv, compare_result::less},
};

bool run_compare_test_case(const compare_test_case& testCase) {
    const path leftPath(testCase.left);
    const path rightPath(testCase.right);
    const int actualCmp = leftPath.compare(testCase.right); // string_view
    // technically the standard only requires that these agree in sign, but our implementation
    // wants to always return the same value
    EXPECT(leftPath.compare(rightPath) == actualCmp); // const path&
    EXPECT(leftPath.compare(rightPath.native()) == actualCmp); // const string_type&
    EXPECT(leftPath.compare(rightPath.c_str()) == actualCmp); // const value_type *
    compare_result actual;
    if (actualCmp < 0) {
        actual = compare_result::less;
        EXPECT(rightPath.compare(testCase.left) > 0);
        EXPECT(!(leftPath == rightPath));
        EXPECT(leftPath != rightPath);
        EXPECT(leftPath < rightPath);
        EXPECT(!(leftPath > rightPath));
        EXPECT(leftPath <= rightPath);
        EXPECT(!(leftPath >= rightPath));
        EXPECT(hash_value(leftPath) != hash_value(rightPath)); // not required, but desirable
    } else if (actualCmp > 0) {
        actual = compare_result::greater;
        EXPECT(rightPath.compare(testCase.left) < 0);
        EXPECT(!(leftPath == rightPath));
        EXPECT(leftPath != rightPath);
        EXPECT(!(leftPath < rightPath));
        EXPECT(leftPath > rightPath);
        EXPECT(!(leftPath <= rightPath));
        EXPECT(leftPath >= rightPath);
        EXPECT(hash_value(leftPath) != hash_value(rightPath)); // not required, but desirable
    } else {
        actual = compare_result::equal;
        EXPECT(leftPath == rightPath);
        EXPECT(!(leftPath != rightPath));
        EXPECT(!(leftPath < rightPath));
        EXPECT(!(leftPath > rightPath));
        EXPECT(leftPath <= rightPath);
        EXPECT(leftPath >= rightPath);
        EXPECT(hash_value(leftPath) == hash_value(rightPath));
    }

    if (testCase.expected == actual) {
        return true;
    }

    wcerr << L"Unexpected comparison result:\nLeft:  " << testCase.left << L"\nRight: " << testCase.right
          << L"\nExpected: " << testCase.expected << L"\n Actual: " << actual << L"\n";

    return false;
}

struct slash_test_case {
    wstring_view a;
    wstring_view b;
    wstring_view expected;
};

constexpr slash_test_case slashTestCases[] = {
    {L"relative"sv, L"other"sv, LR"(relative\other)"sv},
    {L"//server"sv, L"share"sv, LR"(//server\share)"sv},
    {L"//server/"sv, L"share"sv, LR"(//server/share)"sv},
    {L"cat"sv, L"c:/dog"sv, L"c:/dog"sv},
    {L"cat"sv, L"c:"sv, L"c:"sv},
    {L"cat"sv, L""sv, LR"(cat\)"sv}, // note addition of "magic empty path" in decomposition
    // for the empty path on the left, when testing (!is_root_directory && is_absolute) || has_filename
    // is_absolute is false, and has_filename is false, so no / should be added
    {L""sv, L"cat"sv, L"cat"sv},
    {L"./"sv, L"cat"sv, L"./cat"sv}, // original test case catching a bug in the above
    {L"c:"sv, L""sv, L"c:"sv},
    {L"c:cat"sv, L"/dog"sv, L"c:/dog"sv},
    {L"c:/cat"sv, L"/dog"sv, L"c:/dog"sv},
    {L"c:cat"sv, L"c:dog"sv, LR"(c:cat\dog)"sv},
    {L"c:cat"sv, L"d:dog"sv, L"d:dog"sv},
    {L"c:abc"sv, L"def"sv, LR"(c:abc\def)"sv},
    {L"c:abc"sv, L"dog/def"sv, LR"(c:abc\dog/def)"sv},
    {L"//?/frobinate"sv, LR"(\that's strange)"sv, LR"(//?\that's strange)"sv},
    {L"//?/"sv, LR"(\that's strange)"sv, LR"(//?\that's strange)"sv},
};

bool run_slash_test_case(const slash_test_case& testCase) {
    path p(testCase.a);
    p /= testCase.b;
    if (p.native() == testCase.expected) {
        return true;
    }

    wcerr << L"Expected " << testCase.a << L" / " << testCase.b << L" to be " << testCase.expected << L" but it was "
          << p.native() << L"\n";
    return false;
}

void test_iterators() {
    static_assert(is_same_v<path::iterator::iterator_category, input_iterator_tag>);
    static_assert(is_same_v<path::iterator::value_type, path>);
    static_assert(is_same_v<path::iterator::difference_type, ptrdiff_t>);
    static_assert(is_same_v<path::iterator::pointer, const path*>);
    static_assert(is_same_v<path::iterator::reference, const path&>);

    const path::iterator a;
    const path::iterator b;
    EXPECT(a == b);
    EXPECT(!(a != b));

    const path emptyPath{};
    EXPECT(emptyPath.begin() == emptyPath.end());

    const path firstRootName(L"c:/cat"sv);
    auto firstRootNameIter = firstRootName.begin();
    EXPECT(firstRootName.begin()->native() == L"c:");
    EXPECT((*firstRootName.begin()).native() == L"c:"); // also test op* once
    ++firstRootNameIter;
    EXPECT(firstRootNameIter->native() == L"/");
    ++firstRootNameIter;
    EXPECT(firstRootNameIter->native() == L"cat");
    ++firstRootNameIter;
    EXPECT(firstRootNameIter == firstRootName.end());
    --firstRootNameIter;
    EXPECT(firstRootNameIter->native() == L"cat");
    --firstRootNameIter;
    EXPECT(firstRootNameIter->native() == L"/");
    --firstRootNameIter;
    EXPECT(firstRootNameIter->native() == L"c:");
    EXPECT(firstRootNameIter == firstRootName.begin());

    const path noRootDirectory(L"c:cat/dog"sv);
    auto noRootDirectoryiter = noRootDirectory.begin();
    EXPECT(noRootDirectoryiter->native() == L"c:");
    ++noRootDirectoryiter;
    EXPECT(noRootDirectoryiter->native() == L"cat");
    ++noRootDirectoryiter;
    EXPECT(noRootDirectoryiter->native() == L"dog");
    ++noRootDirectoryiter;
    EXPECT(noRootDirectoryiter == noRootDirectory.end());
    --noRootDirectoryiter;
    EXPECT(noRootDirectoryiter->native() == L"dog");
    --noRootDirectoryiter;
    EXPECT(noRootDirectoryiter->native() == L"cat");
    --noRootDirectoryiter;
    EXPECT(noRootDirectoryiter->native() == L"c:");
    EXPECT(noRootDirectoryiter == noRootDirectory.begin());

    const path firstRootDirectory(L"/cat"sv);
    auto firstRootDirectoryIter = firstRootDirectory.begin();
    EXPECT(firstRootDirectoryIter->native() == L"/");
    ++firstRootDirectoryIter;
    EXPECT(firstRootDirectoryIter->native() == L"cat");
    ++firstRootDirectoryIter;
    EXPECT(firstRootDirectoryIter == firstRootDirectory.end());
    --firstRootDirectoryIter;
    EXPECT(firstRootDirectoryIter->native() == L"cat");
    --firstRootDirectoryIter;
    EXPECT(firstRootDirectoryIter->native() == L"/");
    EXPECT(firstRootDirectoryIter == firstRootDirectory.begin());

    const path completelyRelative(L"hello world"sv);
    auto completelyRelativeIter = completelyRelative.begin();
    EXPECT(completelyRelative.begin()->native() == L"hello world");
    ++completelyRelativeIter;
    EXPECT(completelyRelativeIter == completelyRelative.end());
    --completelyRelativeIter;
    EXPECT(completelyRelative.begin()->native() == L"hello world");
    EXPECT(completelyRelativeIter == completelyRelative.begin());

    const path slashesTest(L"a/b//c////d////e///"sv);
    auto slashesIter = slashesTest.begin();
    EXPECT(slashesIter->native() == L"a");
    ++slashesIter;
    EXPECT(slashesIter->native() == L"b");
    ++slashesIter;
    EXPECT(slashesIter->native() == L"c");
    ++slashesIter;
    EXPECT(slashesIter->native() == L"d");
    ++slashesIter;
    EXPECT(slashesIter->native() == L"e");
    ++slashesIter;
    EXPECT(slashesIter->native() == L"");
    ++slashesIter;
    EXPECT(slashesIter == slashesTest.end());
    --slashesIter;
    EXPECT(slashesIter->native() == L"");
    --slashesIter;
    EXPECT(slashesIter->native() == L"e");
    --slashesIter;
    EXPECT(slashesIter->native() == L"d");
    --slashesIter;
    EXPECT(slashesIter->native() == L"c");
    --slashesIter;
    EXPECT(slashesIter->native() == L"b");
    --slashesIter;
    EXPECT(slashesIter->native() == L"a");
    EXPECT(slashesIter == slashesTest.begin());

    // Also test postincrement and postdecrement:
    EXPECT(slashesIter++ == slashesTest.begin());
    const auto oldSlashesIter = slashesIter;
    EXPECT(slashesIter-- == oldSlashesIter);
    EXPECT(slashesIter == slashesTest.begin());

    // Also test algorithms:
    vector<path> vec;
    copy(slashesTest.begin(), slashesTest.end(), back_inserter(vec)); // algorithm uses op++
    EXPECT(vec == vector<path>({L"a", L"b", L"c", L"d", L"e", L""}));
    EXPECT(equal(vec.begin(), vec.end(), slashesTest.begin())); // algorithm warns about unchecked iterators
    vec.clear();
    reverse_copy(slashesTest.begin(), slashesTest.end(), back_inserter(vec)); // algorithm uses op--
    EXPECT(vec == vector<path>({L"", L"e", L"d", L"c", L"b", L"a"}));

    const path justSlash(L"/"sv);
    auto justSlashIter = justSlash.begin();
    EXPECT(justSlashIter->native() == L"/");
    ++justSlashIter;
    EXPECT(justSlashIter == justSlash.end());
    --justSlashIter;
    EXPECT(justSlashIter->native() == L"/");
    EXPECT(justSlashIter == justSlash.begin());

    const path slashRootTest(L"//?/x:/a"sv);
    auto slashRootIter = slashRootTest.begin();
    EXPECT(slashRootIter->native() == L"//?");
    ++slashRootIter;
    EXPECT(slashRootIter->native() == L"/");
    ++slashRootIter;
    EXPECT(slashRootIter->native() == L"x:");
    ++slashRootIter;
    EXPECT(slashRootIter->native() == L"a");
    ++slashRootIter;
    EXPECT(slashRootIter == slashRootTest.end());
    --slashRootIter;
    EXPECT(slashRootIter->native() == L"a");
    --slashRootIter;
    EXPECT(slashRootIter->native() == L"x:");
    --slashRootIter;
    EXPECT(slashRootIter->native() == L"/");
    --slashRootIter;
    EXPECT(slashRootIter->native() == L"//?");
    EXPECT(slashRootIter == slashRootTest.begin());
}

void test_other_path_interface() {
    // exhaustive parsing tested elsewhere
    static_assert(is_same_v<wchar_t, path::value_type>);
    static_assert(is_same_v<wstring, path::string_type>);
    path constructed_from_temporary_wstring(L"abc"s);
    EXPECT(constructed_from_temporary_wstring.native() == L"abc");
    const path constructed_from_temporary_wstring_and_auto_format(L"abc"s, path::auto_format);
    EXPECT(constructed_from_temporary_wstring_and_auto_format.native() == L"abc");
    const path constructed_from_temporary_wstring_and_native_format(L"abc"s, path::native_format);
    EXPECT(constructed_from_temporary_wstring_and_native_format.native() == L"abc");
    const path constructed_from_temporary_wstring_and_generic_format(L"abc"s, path::generic_format);
    EXPECT(constructed_from_temporary_wstring_and_generic_format.native() == L"abc");
    const path constructed_from_wstring_view(L"abc"sv);
    EXPECT(constructed_from_wstring_view.native() == L"abc");

    constructed_from_temporary_wstring = L"also assignable"s;
    EXPECT(constructed_from_temporary_wstring.native() == L"also assignable");
    constructed_from_temporary_wstring.assign(L"also assign()able"s);
    EXPECT(constructed_from_temporary_wstring.native() == L"also assign()able");

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif // __clang__

    const auto shortName = L"a"sv;
    const auto longName  = L"some extremely long path that certainly won't trigger the small string optimization"sv;
    path selfAssignPath(shortName);
    selfAssignPath = selfAssignPath;
    EXPECT(selfAssignPath.native() == shortName);
    selfAssignPath = longName;
    selfAssignPath = selfAssignPath;
    EXPECT(selfAssignPath.native() == longName);

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

    path p;
    p += path(L"abc"sv);
    EXPECT(p.native() == L"abc");
    p += L"def"s;
    EXPECT(p.native() == L"abcdef");
    p += L"hij"sv;
    EXPECT(p.native() == L"abcdefhij");
    p += L"klm";
    EXPECT(p.native() == L"abcdefhijklm");
    p += L'n';
    EXPECT(p.native() == L"abcdefhijklmn");

    EXPECT(!p.empty());
    p.clear();
    EXPECT(p.empty());
    EXPECT(p.native().empty());

    p = L"//?/a/b/c/"sv;
    EXPECT(p.make_preferred().native() == LR"(\\?\a\b\c\)");
    EXPECT(p.remove_filename().native() == LR"(\\?\a\b\c\)");
    p = L"//?/a/b/c"sv;
    EXPECT(p.remove_filename().native() == L"//?/a/b/");
    EXPECT(p.replace_filename(L"c:a/b/c/d"sv).native() == L"c:a/b/c/d");

    p = L"cat.txt"sv;
    EXPECT(p.replace_extension().native() == L"cat");
    p = L"cat.txt"sv;
    EXPECT(p.replace_extension(path{}).native() == L"cat");

    p = L"cat.txt"sv;
    EXPECT(p.replace_extension(L"doc"sv).native() == L"cat.doc");

    p = L"cat.txt"sv;
    EXPECT(p.replace_extension(L".doc"sv).native() == L"cat.doc");

    path other(L"swap me!"sv);
    p.swap(other);
    EXPECT(p.native() == L"swap me!" && other.native() == L"cat.doc");
    swap(p, other);
    EXPECT(p.native() == L"cat.doc" && other.native() == L"swap me!");
    static_assert(noexcept(p.swap(other)));
    static_assert(noexcept(swap(p, other)));

    EXPECT(p.c_str() == p.native());
    EXPECT(p.c_str() == static_cast<wstring>(p));
    EXPECT(p.c_str() == p.wstring());

    // document that we don't remove alternate data streams here
    p = LR"(c:\a\b\c:ads\d\e\f:ads)"sv;
    EXPECT(p.generic_wstring() == LR"(c:/a/b/c:ads/d/e/f:ads)");
}

void test_remove_filename_and_sep() {
    auto f = [](wstring_view before, wstring_view after) {
        path p{before};
        p._Remove_filename_and_separator();
        if (p.native() != after) { // look for exact match
            wcerr << L"_Remove_filename_and_separator('" << before << L"') => '" << p.native() << L"' expected '"
                  << after << L"'\n";
            pass = false;
        }
    };
    f(LR"(c:/cat)"sv, LR"(c:/)"sv);
    f(LR"(\\share\cat)"sv, LR"(\\share\)"sv);
    f(LR"(c:/cat/dog)"sv, LR"(c:/cat)"sv);
    f(LR"(c:/cat//////dog)"sv, LR"(c:/cat)"sv);
    f(LR"(c:///////dog)"sv, LR"(c:/)"sv);
    f(LR"(\\share\cat\dog)"sv, LR"(\\share\cat)"sv);
    f(LR"(\\share\)"sv, LR"(\\share\)"sv);
    f(LR"(\\)"sv, LR"(\)"sv);
    f(LR"(\\share\\\\\\)"sv, LR"(\\share\)"sv);
    f(LR"(c:/fname/)"sv, LR"(c:/fname)"sv);
    f(LR"()"sv, LR"()"sv);
    f(LR"(cat)"sv, LR"()"sv);
    f(LR"(cat/dog)"sv, LR"(cat)"sv);
}

void check_fs_error(const filesystem_error& e, const char* const msg, const string& p1, const string& p2,
    optional<error_code> ec = nullopt) {
    EXPECT(strstr(e.what(), msg) != nullptr);
    EXPECT(strstr(e.what(), p1.c_str()) != nullptr);
    EXPECT(strstr(e.what(), p2.c_str()) != nullptr);
    if (p1.empty()) {
        EXPECT(e.path1().empty());
    } else {
        EXPECT(e.path1() == p1);
    }

    if (p2.empty()) {
        EXPECT(e.path2().empty());
    } else {
        EXPECT(e.path2() == p2);
    }

    if (ec.has_value()) {
        EXPECT(e.code() == ec.value());
    }
};

void test_filesystem_error() {
    error_code ec1(22, system_category());
    filesystem_error fsWhatEc("cute fluffy kittens", ec1);
    check_fs_error(fsWhatEc, "cute fluffy kittens", "", "", ec1);

    error_code ec2 = make_error_code(errc::address_family_not_supported);
    filesystem_error fsWhatPathEc("hungry EVIL zombies", path(L"abc"sv), ec2);
    check_fs_error(fsWhatPathEc, "hungry EVIL zombies", "abc"s, "", ec2);
    EXPECT(fsWhatPathEc.path1().native() == L"abc"sv);

    error_code ec3(7, system_category());
    filesystem_error fsWhatPathPathEc("BRAINS!", path(L"1729"sv), path(L"42"sv), ec3);
    check_fs_error(fsWhatPathPathEc, "BRAINS!", "1729"s, "42"s, ec3);
    EXPECT(fsWhatPathPathEc.path1().native() == L"1729"sv);
    EXPECT(fsWhatPathPathEc.path2().native() == L"42"sv);

    error_code ec4(19, system_category());
    path p1(L"\u00E9"sv);
    string p1Str = p1.string();
    filesystem_error fs("hello", p1, ec4);
    EXPECT(string_view(fs.what()).find(p1Str) != string_view::npos);
}

void test_file_status() {
    auto check = [](const file_status& x, file_type ft, perms p, const char* name) {
        if (x.type() != ft || x.permissions() != p) {
            wcerr << L"test_file_status failed: " << name << L'\n';
            pass = false;
        }
    };
#define CHECK(x, y, z) check(x, y, z, #x " != {" #y ", " #z "}")

    // file_status() noexcept : file_status(file_type::none) {}
    {
        file_status defaulted;
        CHECK(defaulted, file_type::none, perms::unknown);
    }

    // explicit file_status(file_type ft, perms prms = perms::unknown) noexcept;
    {
        file_status one_arg_ctor(file_type::block);
        CHECK(one_arg_ctor, file_type::block, perms::unknown);

        file_status two_arg_ctor(file_type::socket, perms::group_write);
        CHECK(two_arg_ctor, file_type::socket, perms::group_write);
    }

    // file_status(const file_status&) noexcept = default;
    {
        const file_status copy_source(file_type::character, perms::group_all);
        file_status copied(copy_source);
        CHECK(copied, file_type::character, perms::group_all);
    }

    // file_status(file_status&&) noexcept = default;
    {
        file_status move_source(file_type::fifo, perms::group_exec);
        file_status moved(move(move_source));
        CHECK(moved, file_type::fifo, perms::group_exec);
    }

    // ~file_status(); -- tested throughout this function

    // file_status& operator=(const file_status&) noexcept = default;
    {
        const file_status copy_source(file_type::junction, perms::owner_read);
        file_status copy_assign;
        copy_assign = copy_source;
        CHECK(copy_assign, file_type::junction, perms::owner_read);
    }
    // file_status& operator=(file_status&&) noexcept = default;
    {
        file_status move_source(file_type::symlink, perms::owner_write);
        file_status move_assign;
        move_assign = move(move_source);
        CHECK(move_assign, file_type::symlink, perms::owner_write);
    }

    file_status x;
    x.type(file_type::directory);
    CHECK(x, file_type::directory, perms::unknown);
    x.type(file_type::regular);
    CHECK(x, file_type::regular, perms::unknown);

    x.permissions(perms::none);
    CHECK(x, file_type::regular, perms::none);
    x.permissions(perms::others_exec);
    CHECK(x, file_type::regular, perms::others_exec);

#undef CHECK
}

void check_symlink_permissions(const error_code& ec, const wchar_t* const function_id) {
    if (ec.category() != system_category()) {
        wcerr << L"Incorrect error category from " << function_id << L"\n";
        pass = false;
    }

    if (ec.value() != 1314) {
        wcerr << L"Expected ERROR_PRIVILEGE_NOT_HELD from " << function_id << L" but it returned "
              << ec.message().c_str() << L"\n";
        pass = false;
    }

    wcerr << L"Warning: could not test " << function_id
          << L" due to symlink creation failure, do you have admin rights?\n";
}

void test_directory_entry() {
    error_code ec;
    const path filePath(L"test_directory_entry.txt"sv);
    const path filePath2(L"test_directory_entry2.txt"sv);
    const path dirPath(L"test_directory_entry.dir"sv);
    const path linkPath(L"test_directory_entry.link"sv);
    const path cachePath(L"test_directory_entry_cache.txt"sv);
    create_file_containing(filePath, L"hello world");
    create_file_containing(filePath2, L"goodbye world");
    create_directory(dirPath, ec);
    EXPECT(good(ec));

    const directory_entry default_entry;
    EXPECT(default_entry.path().empty());
    // Test VSO-892890 "std::filesystem::directory_entry constructor initializes wrong state"
    EXPECT(!default_entry.exists());

    for (auto&& nonexistent : nonexistentPaths) {
        directory_entry nonexistentEntry(nonexistent);
        EXPECT(nonexistentEntry.path() == nonexistent);
        // Test VSO-892890 "std::filesystem::directory_entry constructor initializes wrong state"
        EXPECT(!nonexistentEntry.exists());

        directory_entry nonexistentEntryEc(nonexistent, ec);
        EXPECT(nonexistentEntryEc.path() == nonexistent);
        // Test VSO-892890 "std::filesystem::directory_entry constructor initializes wrong state"
        EXPECT(!nonexistentEntryEc.exists());
        EXPECT(good(ec));

        EXPECT(throws_filesystem_error([&] { nonexistentEntryEc.refresh(); }, "directory_entry::refresh", nonexistent));
    }

    directory_entry goodEntry(filePath, ec);
    EXPECT(good(ec));
    EXPECT(filePath == goodEntry); // also tests conversion to path
    EXPECT(filePath == goodEntry.path());

    const directory_entry goodEntry2(filePath);
    EXPECT(filePath == goodEntry2); // also tests conversion to path
    EXPECT(goodEntry == goodEntry2);

    const auto ftime = goodEntry.last_write_time(ec);
    EXPECT(good(ec));
    EXPECT(ftime != bad_file_time);
    EXPECT(goodEntry.last_write_time() == ftime);

    const directory_entry copiedEntry(goodEntry);
    EXPECT(copiedEntry == goodEntry);
    EXPECT(copiedEntry.file_size() == goodEntry.file_size());

    directory_entry assignedEntry;
    assignedEntry = goodEntry;
    EXPECT(assignedEntry == goodEntry);
    EXPECT(assignedEntry.file_size() == goodEntry.file_size());

    directory_entry dirEntry(dirPath, ec);
    EXPECT(good(ec));

    EXPECT(goodEntry.exists());
    EXPECT(!goodEntry.is_block_file());
    EXPECT(!goodEntry.is_character_file());
    EXPECT(!goodEntry.is_directory());
    EXPECT(!goodEntry.is_fifo());
    EXPECT(!goodEntry.is_other());
    EXPECT(goodEntry.is_regular_file());
    EXPECT(!goodEntry.is_socket());
    EXPECT(!goodEntry.is_symlink());
    const auto goodStatus = goodEntry.status();
    EXPECT(goodStatus.type() == file_type::regular);
    EXPECT(goodStatus.permissions() == perms::all);
    const auto goodSymlinkStatus = goodEntry.symlink_status();
    EXPECT(goodSymlinkStatus.type() == file_type::regular);
    EXPECT(goodSymlinkStatus.permissions() == perms::all);

    ec = make_error_code(errc::executable_format_error); // bad chosen at random, totally random
    EXPECT(dirEntry.exists(ec));
    EXPECT(good(ec));
    ec = make_error_code(errc::executable_format_error);
    EXPECT(!dirEntry.is_block_file(ec));
    EXPECT(good(ec));
    ec = make_error_code(errc::executable_format_error);
    EXPECT(!dirEntry.is_character_file(ec));
    EXPECT(good(ec));
    ec = make_error_code(errc::executable_format_error);
    EXPECT(dirEntry.is_directory(ec));
    EXPECT(good(ec));
    ec = make_error_code(errc::executable_format_error);
    EXPECT(!dirEntry.is_fifo(ec));
    EXPECT(good(ec));
    ec = make_error_code(errc::executable_format_error);
    EXPECT(!dirEntry.is_other(ec));
    EXPECT(good(ec));
    ec = make_error_code(errc::executable_format_error);
    EXPECT(!dirEntry.is_regular_file(ec));
    EXPECT(good(ec));
    ec = make_error_code(errc::executable_format_error);
    EXPECT(!dirEntry.is_socket(ec));
    EXPECT(good(ec));
    ec = make_error_code(errc::executable_format_error);
    EXPECT(!dirEntry.is_symlink(ec));
    EXPECT(good(ec));
    const auto dirStatus = dirEntry.status();
    EXPECT(dirStatus.type() == file_type::directory);
    EXPECT(dirStatus.permissions() == perms::all);
    const auto dirSymlinkStatus = dirEntry.symlink_status();
    EXPECT(dirSymlinkStatus.type() == file_type::directory);
    EXPECT(dirSymlinkStatus.permissions() == perms::all);

    create_directory_symlink(dirPath, linkPath, ec);
    if (ec) {
        check_symlink_permissions(ec, L"directory_entry");
    } else {
        const directory_entry linkEntry(linkPath);
        EXPECT(linkEntry.exists());
        EXPECT(!linkEntry.is_block_file());
        EXPECT(!linkEntry.is_character_file());
        EXPECT(linkEntry.is_directory());
        EXPECT(!linkEntry.is_fifo());
        EXPECT(!linkEntry.is_other());
        EXPECT(!linkEntry.is_regular_file());
        EXPECT(!linkEntry.is_socket());
        EXPECT(linkEntry.is_symlink());
        const auto linkStatus = linkEntry.status();
        EXPECT(linkStatus.type() == file_type::directory);
        EXPECT(linkStatus.permissions() == perms::all);
        const auto linkSymlinkStatus = linkEntry.symlink_status();
        EXPECT(linkSymlinkStatus.type() == file_type::symlink);
        EXPECT(linkSymlinkStatus.permissions() == perms::all);
        remove(linkPath, ec);
        EXPECT(good(ec));
    }

    goodEntry.assign(dirPath);
    EXPECT(goodEntry.is_directory());
    goodEntry.assign(filePath, ec);
    EXPECT(good(ec));
    EXPECT(goodEntry.is_regular_file());

    goodEntry.replace_filename(dirPath);
    EXPECT(goodEntry.is_directory());
    goodEntry.replace_filename(filePath, ec);
    EXPECT(good(ec));
    EXPECT(goodEntry.is_regular_file());

    const path changingPath(L"test_directory_entry.both"sv);
    create_directory(changingPath, ec);
    EXPECT(good(ec));
    directory_entry changingEntry(changingPath, ec);
    EXPECT(good(ec));
    EXPECT(changingEntry.is_directory());
    remove(changingPath, ec);
    EXPECT(good(ec));
    create_file_containing(changingPath, L"Hello");
    EXPECT(changingEntry.is_directory()); // cached
    changingEntry.refresh();
    EXPECT(!changingEntry.is_directory()); // updated
    EXPECT(changingEntry.is_regular_file());

    create_hard_link(changingPath, linkPath, ec);
    EXPECT(good(ec));
    EXPECT(changingEntry.hard_link_count() == 1); // cached
    changingEntry.refresh(ec);
    EXPECT(good(ec));
    EXPECT(changingEntry.hard_link_count(ec) == 2); // updated
    EXPECT(good(ec));

    const directory_entry aEntry(filePath, ec);
    const directory_entry bEntry(filePath2, ec);
    EXPECT(aEntry < bEntry);
    EXPECT(!(bEntry < aEntry));
    EXPECT(!(aEntry < aEntry));

    EXPECT(aEntry == aEntry);
    EXPECT(!(aEntry == bEntry));

    EXPECT(!(aEntry != aEntry));
    EXPECT(aEntry != bEntry);

    EXPECT(aEntry <= bEntry);
    EXPECT(!(bEntry <= aEntry));
    EXPECT(aEntry <= aEntry);

    EXPECT(!(aEntry > bEntry));
    EXPECT(bEntry > aEntry);
    EXPECT(!(aEntry > aEntry));

    EXPECT(!(aEntry >= bEntry));
    EXPECT(bEntry >= aEntry);
    EXPECT(aEntry >= aEntry);

    directory_entry cachingEntry(cachePath);
    // here, we are not cached:
    EXPECT(cachingEntry.file_size(ec) == static_cast<uintmax_t>(-1));
    EXPECT(bad(ec));
    // assert that the above error was not cached:
    create_file_containing(cachePath, L"abcdef");
    EXPECT(cachingEntry.file_size(ec) == 6);
    EXPECT(good(ec));
    EXPECT(cachingEntry.last_write_time(ec) != file_time_type::min());
    EXPECT(good(ec));
    EXPECT(cachingEntry.hard_link_count(ec) != static_cast<uintmax_t>(-1));
    EXPECT(good(ec));
    EXPECT(cachingEntry.is_regular_file(ec));
    EXPECT(good(ec));
    // assert that none of the above accesses restored caching:
    remove(cachePath, ec);
    EXPECT(good(ec));
    EXPECT(cachingEntry.file_size(ec) == static_cast<uintmax_t>(-1));
    EXPECT(bad(ec));
    EXPECT(cachingEntry.last_write_time(ec) == file_time_type::min());
    EXPECT(bad(ec));
    EXPECT(cachingEntry.hard_link_count(ec) == static_cast<uintmax_t>(-1));
    EXPECT(bad(ec));
    EXPECT(!cachingEntry.is_regular_file(ec));
    EXPECT(bad(ec));
    // restore caching:
    create_file_containing(cachePath, L"abcdef");
    cachingEntry.refresh();
    remove(cachePath, ec);
    EXPECT(good(ec));
    // assert that the following succeed because the values are cached, even though the file is gone
    EXPECT(cachingEntry.file_size(ec) == 6);
    EXPECT(good(ec));
    EXPECT(cachingEntry.last_write_time(ec) != file_time_type::min());
    EXPECT(good(ec));
    EXPECT(cachingEntry.hard_link_count(ec) != static_cast<uintmax_t>(-1));
    EXPECT(good(ec));
    EXPECT(cachingEntry.is_regular_file(ec));
    EXPECT(good(ec));
#if _HAS_CXX20
    // break caching again, and assert that things aren't cached
    cachingEntry.clear_cache();
    EXPECT(cachingEntry.file_size(ec) == static_cast<uintmax_t>(-1));
    EXPECT(bad(ec));
    EXPECT(cachingEntry.last_write_time(ec) == file_time_type::min());
    EXPECT(bad(ec));
    EXPECT(cachingEntry.hard_link_count(ec) == static_cast<uintmax_t>(-1));
    EXPECT(bad(ec));
    EXPECT(!cachingEntry.is_regular_file(ec));
    EXPECT(bad(ec));
#endif // _HAS_CXX20

    // assert that mutating the path doesn't fail even though the target doesn't exist
    for (auto&& nonexistent : nonexistentPaths) {
        cachingEntry.assign(nonexistent); // no fail
        cachingEntry.assign(nonexistent, ec);
        EXPECT(good(ec));
        cachingEntry.replace_filename(L"Exist2"sv); // no fail
        cachingEntry.replace_filename(L"Exist2"sv, ec);
        EXPECT(good(ec));
    }

    remove(changingPath, ec);
    EXPECT(good(ec));
    remove(linkPath, ec);
    EXPECT(good(ec));
    remove(filePath2, ec);
    EXPECT(good(ec));
    remove(filePath, ec);
    EXPECT(good(ec));
    remove(dirPath, ec);
    EXPECT(good(ec));
}

template <typename DirectoryIterator>
void test_directory_iterator_common_parts(const string_view typeName) {
    const test_temp_directory tempDir(typeName);
    const path tempFile = tempDir.directoryPath / L"example.txt"sv;
    create_file_containing(tempFile, L"Hi");

    // using iterator_category = input_iterator_tag;
    // using value_type        = directory_entry;
    // using difference_type   = ptrdiff_t;
    // using pointer           = const directory_entry*;
    // using reference         = const directory_entry&;

    static_assert(is_same_v<typename DirectoryIterator::iterator_category, input_iterator_tag>);
    static_assert(is_same_v<typename DirectoryIterator::value_type, directory_entry>);
    static_assert(is_same_v<typename DirectoryIterator::difference_type, ptrdiff_t>);
    static_assert(is_same_v<typename DirectoryIterator::pointer, const directory_entry*>);
    static_assert(is_same_v<typename DirectoryIterator::reference, const directory_entry&>);

    // [fs.dir.itr.members], member functions

    // DirectoryIterator() noexcept;
    // ~DirectoryIterator();
    { DirectoryIterator default_ctor; }

    // explicit DirectoryIterator(const path& _Path_arg);
    // DirectoryIterator(const path& _Path_arg, directory_options _Options_arg);
    // DirectoryIterator(const path& _Path_arg, error_code& _Ec) noexcept;
    // bool operator==(const DirectoryIterator& _Rhs) const;
    {
        error_code ec;
        for (auto&& nonexistent : nonexistentPaths) {
            DirectoryIterator bad_dir(nonexistent, ec);
            EXPECT(bad(ec));
            EXPECT(bad_dir == DirectoryIterator{});
            EXPECT(bad_dir == bad_dir);
            EXPECT(!(bad_dir != bad_dir));

            EXPECT(throws_filesystem_error([&] { DirectoryIterator bad_dir{nonexistent}; }, typeName, nonexistent));
            EXPECT(throws_filesystem_error(
                [&] {
                    DirectoryIterator bad_dir{nonexistent, directory_options::none};
                },
                typeName, nonexistent));
        }

        // Test VSO-844835 "directory_iterator constructed with empty path iterates over the current directory"
        DirectoryIterator empty_dir(path{}, ec);
        EXPECT(ec == error_condition(2 /* ERROR_FILE_NOT_FOUND */, system_category()));
        EXPECT(empty_dir == DirectoryIterator{});

        // Test VSO-583725 "recursive_directory_iterator blows up (memory leak + infinite loop) with embedded nulls"
        DirectoryIterator embedded_null_dir(L".\0"sv, ec);
        EXPECT(ec == error_condition(2 /* ERROR_FILE_NOT_FOUND */, system_category()));
        EXPECT(embedded_null_dir == DirectoryIterator{});
    }

    // DirectoryIterator(const path& _Path_arg, directory_options _Options_arg, error_code& _Ec) noexcept;
    // const directory_entry& operator*() const;
    // const directory_entry* operator->() const;
    // DirectoryIterator& increment(error_code& _Ec) noexcept;
    // bool operator!=(const DirectoryIterator& _Rhs) const;
    {
        error_code ec;
        DirectoryIterator good_dir(tempDir.directoryPath, directory_options::none, ec);

        if (!EXPECT(good(ec))) {
            return;
        }
        EXPECT(good_dir != DirectoryIterator{}); // should have at least one file
        EXPECT(good_dir == good_dir);
        EXPECT(!(good_dir != good_dir));

        auto& first_entry = *good_dir;
        EXPECT(first_entry.path() == good_dir->path());

        bool found_ourselves = false;

        do {
            found_ourselves = found_ourselves || good_dir->path() == tempFile;
            good_dir.increment(ec);
            EXPECT(good(ec));
        } while (good_dir != DirectoryIterator{});

        EXPECT(found_ourselves);
    }

    // DirectoryIterator(const DirectoryIterator& _Rhs);
    // DirectoryIterator& operator=(const DirectoryIterator& _Rhs);
    {
        error_code ec;
        const DirectoryIterator copy_source_dir_iter(tempDir.directoryPath, directory_options::none, ec);
        EXPECT(good(ec));
        EXPECT(copy_source_dir_iter != DirectoryIterator{});
        const DirectoryIterator copy_constructed(copy_source_dir_iter);
        EXPECT(copy_source_dir_iter == copy_constructed);
        EXPECT(!(copy_source_dir_iter != copy_constructed));
        DirectoryIterator assignment_dest;
        assignment_dest = copy_source_dir_iter;
        EXPECT(copy_source_dir_iter == assignment_dest);
        EXPECT(!(copy_source_dir_iter != assignment_dest));

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif // __clang__

        // test self-assignment
        assignment_dest = assignment_dest;
        EXPECT(copy_source_dir_iter == assignment_dest);

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
    }

    // DirectoryIterator(DirectoryIterator&& _Rhs) noexcept;
    // DirectoryIterator& operator=(DirectoryIterator&& _Rhs) noexcept;
    {
        error_code ec;
        DirectoryIterator move_source_dir_iter(tempDir.directoryPath, directory_options::none, ec);
        EXPECT(good(ec));
        EXPECT(move_source_dir_iter != DirectoryIterator{});
        const path first_entry = move_source_dir_iter->path();

        DirectoryIterator move_constructed(move(move_source_dir_iter));
        EXPECT(move_constructed != DirectoryIterator{});
        EXPECT(move_source_dir_iter == DirectoryIterator{});
        EXPECT(first_entry == move_constructed->path());

        DirectoryIterator move_dest;
        move_dest = move(move_constructed);
        EXPECT(move_dest != DirectoryIterator{});
        EXPECT(move_constructed == DirectoryIterator{});
        EXPECT(first_entry == move_dest->path());

        // test self-move-assignment
        move_dest = move(move_dest);
        EXPECT(move_dest != DirectoryIterator{});
        EXPECT(first_entry == move_dest->path());
    }

    // NB: we cannot test that ++ throws without introducing failure injection code into DirectoryIterator
    // DirectoryIterator& operator++();
    // _Directory_entry_proxy operator++(int);
    {
        error_code ec;
        create_file_containing(tempDir.directoryPath / L"a.txt"sv, L"a");
        create_file_containing(tempDir.directoryPath / L"b.txt"sv, L"b");
        DirectoryIterator it(tempDir.directoryPath, directory_options::none, ec);
        EXPECT(good(ec));

        const path first_path = it->path();
        directory_entry entry = *it++;

        EXPECT(first_path == entry.path());

        while (++it != DirectoryIterator{}) {
            EXPECT(first_path != it->path());
        }
    }

    // [fs.dir.itr.members]/Effects
    // if (options & directory_options::skip_permission_denied) != directory_options::none
    // and construction encounters an error indicating that permission to access p is denied, constructs the
    // end iterator and does not report an error.

    // Tested by hand with interactive tests
    //   -(rec)dir:AccessDeniedDir => failure
    //   -(rec)dir:!AccessDeniedDir => empty dir
}

void test_directory_iterator() {
    test_directory_iterator_common_parts<directory_iterator>("directory_iterator"sv);
}

void test_recursive_directory_iterator() {
    test_directory_iterator_common_parts<recursive_directory_iterator>("recursive_directory_iterator"sv);

    {
        const test_temp_directory recursiveTests("recursive_directory_iterator specific"sv);
        create_file_containing(recursiveTests.directoryPath / L"a.txt"sv, L"hello");

        // _NODISCARD directory_options  options() const;
        // _NODISCARD int                depth() const;
        // _NODISCARD bool               recursion_pending() const;
        // void pop(error_code& _Ec);
        // void disable_recursion_pending();
        {
            error_code ec;
            recursive_directory_iterator good_dir(recursiveTests.directoryPath, directory_options::none, ec);
            if (!EXPECT(good(ec))) {
                return;
            }

            EXPECT(good_dir.options() == directory_options::none);

            recursive_directory_iterator good_dir2(
                recursiveTests.directoryPath, directory_options::skip_permission_denied, ec);
            EXPECT(good_dir2.options() == directory_options::skip_permission_denied);

            recursive_directory_iterator good_dir3(
                recursiveTests.directoryPath, directory_options::follow_directory_symlink, ec);
            EXPECT(good_dir3.options() == directory_options::follow_directory_symlink);

            EXPECT(good_dir.depth() == 0);
            EXPECT(good_dir.recursion_pending());

            good_dir.disable_recursion_pending();
            EXPECT(!good_dir.recursion_pending());

            EXPECT(good_dir != recursive_directory_iterator{});

            good_dir.pop(ec);
            EXPECT(good(ec));

            EXPECT(good_dir == recursive_directory_iterator{});
        }

        // void pop();
        {
            recursive_directory_iterator good_dir(recursiveTests.directoryPath, directory_options::none);
            good_dir.pop();
            EXPECT(good_dir == recursive_directory_iterator{});
        }

        // Tests for following symlinks and junctions

        // Tested by hand with interactive tests
        //   -recdir:DirWithSymLinks => skipping directory symlinks
        //   -recdir:~DirWithSymLinks => going into directory symlinks

        // Tests for access denied during recursive enumeration

        // Tested by hand with interactive tests
        //   -recdir:DirWithAccessDeniedEntries => Access Denied in operator++
        //   -recdir:!DirWithAccessDeniedEntries => Skip subdirs that return Access Denied
    }

    // Also test VSO-649431 <filesystem> follow_directory_symlinks with a broken symlink causes iteration to break
    {
        const test_temp_directory followSymlinkTests("recursive_directory_iterator_VSO-649431"sv);
        const path aaa = followSymlinkTests.directoryPath / L"aaa"sv;
        const path bbb = followSymlinkTests.directoryPath / L"bbb"sv;
        const path ccc = followSymlinkTests.directoryPath / L"ccc"sv;
        error_code ec;
        create_directory_symlink(nonexistentPaths[0], bbb, ec);
        if (ec) {
            check_symlink_permissions(ec, L"recursive_directory_iterator");
        } else {
            create_directory(aaa);
            create_directory(ccc);
            static constexpr directory_options options[] = {directory_options::none,
                directory_options::follow_directory_symlink, directory_options::skip_permission_denied,
                directory_options::follow_directory_symlink | directory_options::skip_permission_denied};
            for (const auto& option : options) {
                recursive_directory_iterator first(followSymlinkTests.directoryPath, option);
                assert(first != recursive_directory_iterator{});
                EXPECT(first->is_directory());
                EXPECT(!first->is_symlink());
                EXPECT(first->exists());
                EXPECT(first->path().native() == aaa.native());
                ++first;
                assert(first != recursive_directory_iterator{});
                EXPECT(!first->is_directory());
                EXPECT(first->is_symlink());
                EXPECT(!first->exists());
                EXPECT(first->path().native() == bbb.native());
                ++first;
                assert(first != recursive_directory_iterator{});
                EXPECT(first->is_directory());
                EXPECT(!first->is_symlink());
                EXPECT(first->exists());
                EXPECT(first->path().native() == ccc.native());
                ++first;
                EXPECT(first == recursive_directory_iterator{});
            }
        }
    }
}

void expect_absolute(const path& input, const wstring_view expected) {
    error_code ec(-1, generic_category());
    const path actual = absolute(input, ec);
    if (actual.native() != expected || ec || ec.category() != system_category()) {
        wcerr << L"Expected absolute(" << input.native() << L") to be " << expected << L"\n";
        if (actual.native() != expected) {
            wcerr << L"Actual result: " << actual.native() << L"\n";
        }
        if (ec) {
            wcerr << L"The call failed.\n";
        }

        pass = false;
    }
}

void test_absolute() {
    expect_absolute(L"x:/cat/dog/../elk"sv, LR"(x:\cat\elk)"sv);

    wstring longPath(LR"(\\?\x:\some\)");
    longPath += longSuffix;
    assert(longPath.size() > 260);
    expect_absolute(longPath, longPath);

    error_code ec(-1, generic_category());
    longPath.push_back(L'\\');
    longPath.resize(40'000, L'a');
    EXPECT(throws_filesystem_error([&] { return absolute(longPath); }, "absolute"sv, longPath));
    EXPECT(absolute(longPath, ec).empty());
    if (ec.value() != 206) {
        wcerr << L"Warning: Expected absolute on a >32k long path to report ERROR_FILENAME_EXCED_RANGE, "
                 L"but it reported "
              << ec.value() << L"\n";
    }
    EXPECT(ec.category() == system_category());

    longPath.resize(260);
    expect_absolute(longPath, longPath);

    expect_absolute({}, {});
}

void test_canonical() {
    // test that !exists(p) is an error
    EXPECT(throws_filesystem_error([] { return canonical(L"nonexistent"sv); }, "canonical", L"nonexistent"sv));
    error_code ec(-1, generic_category());
    EXPECT(canonical(L"nonexistent.txt"sv, ec).empty());
    EXPECT(bad(ec));

    // test that canonical on a directory is not an error
    (void) canonical(L"."sv, ec); // == canonical(current_path())?
    EXPECT(good(ec));

    // test that canonical on an ordinary file returns that file's DOS path
    const auto filename = L"test_canonical.txt"sv;
    remove(L"test_canonical.txt", ec);
    EXPECT(good(ec));
    create_file_containing(L"test_canonical.txt", L"Hello world\n");
    const path p(canonical(filename, ec));
    EXPECT(good(ec));
    const auto& text = p.native();
    assert(text.size() > filename.size() + 1);
    EXPECT(text[1] == L':'); // this test assumes that the test directory is mapped to a drive letter
    const auto diffSize = static_cast<ptrdiff_t>(filename.size());
    EXPECT(*(text.end() - diffSize - 1) == L'\\');
    EXPECT(equal(text.end() - diffSize, text.end(), filename.begin(), filename.end()));
    EXPECT(remove(L"test_canonical.txt", ec));
    EXPECT(good(ec));
}

void test_copy() {
    error_code ec;
    const recursive_directory_iterator endIter;
    const test_temp_directory tempDir("copy"sv);
    const auto& basePath = tempDir.directoryPath;

    // examples from the standard, N4727 [fs.op.copy]/7:
    const auto createStandardBaseline = [=] {
        remove_all(basePath);
        create_directories(basePath / L"dir1/dir2"sv);
        create_file_containing(basePath / L"dir1/file1"sv, L"file1");
        create_file_containing(basePath / L"dir1/file2"sv, L"file2");
        create_file_containing(basePath / L"dir1/dir2/file3"sv, L"file3");
    };

    createStandardBaseline();
    copy(basePath / L"dir1"sv, basePath / L"dir3"sv);
    {
        recursive_directory_iterator verifier(basePath);
        EXPECT(verifier != endIter);

        EXPECT(verifier->path() == basePath / L"dir1"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/dir2"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/dir2/file3"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/file1"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/file2"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3/file1"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3/file2"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier == endIter);
    }

    copy(basePath / L"dir1"sv, basePath / L"dir3"sv, ec);
    EXPECT(bad(ec)); // targets already exist
    copy(basePath / L"dir1"sv, basePath / L"dir3"sv, copy_options::overwrite_existing, ec);
    EXPECT(good(ec)); // targets already exist

    createStandardBaseline();
    copy(basePath / L"dir1"sv, basePath / L"dir3"sv, copy_options::recursive);
    {
        recursive_directory_iterator verifier(basePath);
        EXPECT(verifier != endIter);

        EXPECT(verifier->path() == basePath / L"dir1"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/dir2"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/dir2/file3"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/file1"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/file2"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3/dir2"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3/dir2/file3"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3/file1"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3/file2"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier == endIter);
    }

    createStandardBaseline();
    copy(basePath / L"dir1"sv, basePath / L"dir3"sv, copy_options::directories_only);
    {
        recursive_directory_iterator verifier(basePath);
        EXPECT(verifier != endIter);

        EXPECT(verifier->path() == basePath / L"dir1"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/dir2"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/dir2/file3"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/file1"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/file2"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier == endIter);
    }

    createStandardBaseline();
    copy(basePath / L"dir1"sv, basePath / L"dir3"sv, copy_options::create_hard_links);
    {
        recursive_directory_iterator verifier(basePath);
        EXPECT(verifier != endIter);

        EXPECT(verifier->path() == basePath / L"dir1"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/dir2"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/dir2/file3"sv);
        EXPECT(verifier->is_regular_file());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/file1"sv);
        EXPECT(verifier->is_regular_file());
        EXPECT(verifier->hard_link_count() == 2);

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir1/file2"sv);
        EXPECT(verifier->is_regular_file());
        EXPECT(verifier->hard_link_count() == 2);

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3"sv);
        EXPECT(verifier->is_directory());

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3/file1"sv);
        EXPECT(verifier->is_regular_file());
        EXPECT(verifier->hard_link_count() == 2);

        ++verifier;
        EXPECT(verifier != endIter);
        EXPECT(verifier->path() == basePath / L"dir3/file2"sv);
        EXPECT(verifier->is_regular_file());
        EXPECT(verifier->hard_link_count() == 2);

        EXPECT(equivalent(basePath / L"dir1/file1"sv, basePath / L"dir3/file1"sv));
        EXPECT(equivalent(basePath / L"dir1/file2"sv, basePath / L"dir3/file2"sv));

        ++verifier;
        EXPECT(verifier == endIter);
    }

    remove_all(basePath);

    for (auto&& nonexistent : nonexistentPaths) {
        EXPECT(throws_filesystem_error([&] { copy(nonexistent, basePath); }, "copy"sv, nonexistent, basePath));
    }

    EXPECT(throws_filesystem_error([&] { copy(basePath, basePath); }, "copy"sv, basePath, basePath));
}

void setup_copy_file_tests(const path& directoryPath) {
    error_code ec;
    create_file_containing(directoryPath / L"older.txt"sv, L"older file");
    last_write_time(directoryPath / L"older.txt"sv, file_time_type::clock::now() - 1h, ec);
    EXPECT(good(ec));
    create_file_containing(directoryPath / L"newer.txt"sv, L"newer file");
}

void expect_copy_file_bad_all_options(const path& from, const path& to) {
    error_code ec;
    EXPECT(!copy_file(from, to, ec));
    EXPECT(bad(ec));
    EXPECT(throws_filesystem_error([&] { return copy_file(from, to); }, "copy_file"sv, from, to));
    EXPECT(!copy_file(from, to, copy_options::none, ec));
    EXPECT(bad(ec));
    EXPECT(throws_filesystem_error([&] { return copy_file(from, to, copy_options::none); }, "copy_file"sv, from, to));
    EXPECT(!copy_file(from, to, copy_options::skip_existing, ec));
    EXPECT(bad(ec));
    EXPECT(!copy_file(from, to, copy_options::overwrite_existing, ec));
    EXPECT(bad(ec));
    EXPECT(!copy_file(from, to, copy_options::update_existing, ec));
    EXPECT(bad(ec));
}

void test_copy_file() {
    const test_temp_directory tempDir("copy_file"sv);
    const auto& directoryPath = tempDir.directoryPath;

    error_code ec;
    // test error cases
    // !is_regular_file(from)
    create_directory(directoryPath / L"dir"sv, ec);
    EXPECT(good(ec));
    expect_copy_file_bad_all_options(directoryPath / L"dir"sv, directoryPath / L"dir2"sv);

    setup_copy_file_tests(directoryPath);

    // exists(to) is true and is_regular_file(to) is false
    expect_copy_file_bad_all_options(directoryPath / L"older.txt"sv, directoryPath / L"dir"sv);

    // exists(to) is true and equivalent(from, to) is true
    expect_copy_file_bad_all_options(directoryPath / L"older.txt"sv, directoryPath / L"older.txt"sv);

    // exists(to) is true and no update option tested below

    // basic tests for success
    EXPECT(copy_file(directoryPath / L"older.txt"sv, directoryPath / L"success.txt"sv, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"success.txt"sv) == L"older file");

    EXPECT(copy_file(directoryPath / L"older.txt"sv, directoryPath / L"success_none.txt"sv, copy_options::none, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"success_none.txt"sv) == L"older file");

    EXPECT(copy_file(
        directoryPath / L"older.txt"sv, directoryPath / L"success_skip.txt"sv, copy_options::skip_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"success_skip.txt"sv) == L"older file");

    EXPECT(copy_file(directoryPath / L"older.txt"sv, directoryPath / L"success_overwrite.txt"sv,
        copy_options::overwrite_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"success_overwrite.txt"sv) == L"older file");

    EXPECT(copy_file(
        directoryPath / L"older.txt"sv, directoryPath / L"success_update.txt"sv, copy_options::update_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"success_update.txt"sv) == L"older file");

    // test that none fails for any exists(to)
    EXPECT(!copy_file(directoryPath / L"older.txt"sv, directoryPath / L"newer.txt"sv, ec));
    EXPECT(bad(ec));
    EXPECT(!copy_file(directoryPath / L"older.txt"sv, directoryPath / L"newer.txt"sv, copy_options::none, ec));
    EXPECT(bad(ec));
    EXPECT(!copy_file(directoryPath / L"newer.txt"sv, directoryPath / L"older.txt"sv, ec));
    EXPECT(bad(ec));
    EXPECT(!copy_file(directoryPath / L"newer.txt"sv, directoryPath / L"older.txt"sv, copy_options::none, ec));
    EXPECT(bad(ec));

    // test that skip_existing succeeds for any exists(to) and does nothing
    EXPECT(!copy_file(directoryPath / L"older.txt"sv, directoryPath / L"newer.txt"sv, copy_options::skip_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"newer.txt"sv) == L"newer file");
    EXPECT(!copy_file(directoryPath / L"newer.txt"sv, directoryPath / L"older.txt"sv, copy_options::skip_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"older.txt"sv) == L"older file");

    // test that overwrite_existing succeeds for any exists(to) and overwrites
    EXPECT(copy_file(
        directoryPath / L"older.txt"sv, directoryPath / L"newer.txt"sv, copy_options::overwrite_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"newer.txt"sv) == L"older file");
    setup_copy_file_tests(directoryPath);
    EXPECT(copy_file(
        directoryPath / L"newer.txt"sv, directoryPath / L"older.txt"sv, copy_options::overwrite_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"older.txt"sv) == L"newer file");

    // test that update_existing succeeds for any exists(to) and overwrites if and only if the source is newer
    setup_copy_file_tests(directoryPath);
    EXPECT(
        !copy_file(directoryPath / L"older.txt"sv, directoryPath / L"newer.txt"sv, copy_options::update_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"newer.txt"sv) == L"newer file");
    EXPECT(
        copy_file(directoryPath / L"newer.txt"sv, directoryPath / L"older.txt"sv, copy_options::update_existing, ec));
    EXPECT(good(ec));
    EXPECT(read_file_contents(directoryPath / L"older.txt"sv) == L"newer file");
}

void test_create_symlink_cleanup() {
    // internal unit-level test of CreateSymbolicLinkW fixup
    const pair<wstring_view, wstring_view> testCases[] = {
        {LR"(test/\/dir)"sv, LR"(test\dir)"sv},
        {LR"(test/dir)"sv, LR"(test\dir)"sv},
        {LR"(C:////////a)"sv, LR"(C:\a)"sv},
        {LR"(//server/a////////b////////c////////d)"sv, LR"(\\server\a\b\c\d)"sv},
        {LR"(\\.\\\\\\\device)"sv, LR"(\\.\device)"sv},
        // note /// gets interpreted as root-directory:
        {LR"(///server/a////////b////////c////////d)"sv, LR"(\server\a\b\c\d)"sv},
    };

    for (const auto& [input, expected] : testCases) {
        const auto actual = _Get_cleaned_symlink_target(input);
        assert(expected == actual.get());
    }
}

void test_create_directory_symlink() {
    const auto filename = L"test_create_directory_symlink"sv;
    const auto linkname = L"test_create_directory_symlink_link"sv;

    error_code ec;
    create_directory_symlink(filename, linkname, ec); // can create symlink to nonexistent thing
    create_directory(filename); // make the thing exist later
    if (ec) {
        check_symlink_permissions(ec, L"create_directory_symlink");
        EXPECT(remove(filename, ec));
        EXPECT(good(ec));
        EXPECT(!remove(linkname, ec));
        EXPECT(good(ec));
    } else {
        EXPECT(equivalent(filename, linkname, ec));
        EXPECT(good(ec));

        EXPECT(remove(filename, ec));
        EXPECT(good(ec));
        EXPECT(remove(linkname, ec));
        EXPECT(good(ec));

        // Also test VSO-649249 create_symlink and create_directory_symlink create malformed symlinks
        // when the target contains /s (rather than \s) and the target does not exist
        create_directory_symlink(L"test_create_directory_symlink_dir/cat.dir"sv, linkname);
        EXPECT(status(linkname, ec).type() == file_type::not_found);
        EXPECT(ec == error_condition(3, system_category())); // ERROR_PATH_NOT_FOUND
        EXPECT(remove(linkname, ec));
        EXPECT(good(ec));

        create_directory_symlink(L"test_create_directory_symlink_dir/cat.dir"sv, linkname, ec);
        EXPECT(good(ec));
        EXPECT(status(linkname, ec).type() == file_type::not_found);
        EXPECT(ec == error_condition(3, system_category())); // ERROR_PATH_NOT_FOUND
        EXPECT(remove(linkname, ec));
        EXPECT(good(ec));
    }

    // example error: malformed source
    EXPECT(throws_filesystem_error(
        [&] { create_directory_symlink(filename, badPath); }, "create_directory_symlink", filename, badPath));
    create_directory_symlink(filename, badPath, ec);
    EXPECT(bad(ec));
}

void test_create_hard_link() {
    error_code ec;
    const auto filename = L"test_create_hard_link.txt";
    remove(filename, ec);
    EXPECT(good(ec));
    const auto linkname = L"test_create_hard_link_link.txt";
    remove(linkname, ec);
    EXPECT(good(ec));

    create_file_containing(filename, L"hello\n");
    create_hard_link(filename, linkname, ec);
    EXPECT(good(ec));
    EXPECT(equivalent(filename, linkname, ec));
    EXPECT(good(ec));

    // example error: self link
    EXPECT(
        throws_filesystem_error([&] { create_hard_link(filename, filename); }, "create_hard_link", filename, filename));
    create_hard_link(filename, filename, ec);
    EXPECT(bad(ec));

    remove(filename, ec);
    EXPECT(good(ec));
    remove(linkname, ec);
    EXPECT(good(ec));
}

void test_create_symlink() {
    const auto filename = L"test_create_symlink.txt"sv;
    const auto linkname = L"test_create_symlink_link.txt"sv;

    error_code ec;
    create_symlink(filename, linkname, ec); // can create symlink to nonexistent thing
    create_file_containing(filename, L"hello\n"); // make the thing exist later
    if (ec) {
        check_symlink_permissions(ec, L"create_symlink");
        EXPECT(remove(filename, ec));
        EXPECT(good(ec));
        EXPECT(!remove(linkname, ec));
        EXPECT(good(ec));
    } else {
        EXPECT(equivalent(filename, linkname, ec));
        EXPECT(good(ec));

        EXPECT(remove(filename, ec));
        EXPECT(good(ec));
        EXPECT(remove(linkname, ec));
        EXPECT(good(ec));

        // Also test VSO-649249 create_symlink and create_directory_symlink create malformed
        // symlinks when the target contains /s (rather than \s) and the target does not exist
        create_symlink(L"test_create_symlink_dir/cat.txt"sv, linkname);
        EXPECT(status(linkname, ec).type() == file_type::not_found);
        EXPECT(ec == error_condition(3, system_category())); // ERROR_PATH_NOT_FOUND
        EXPECT(remove(linkname, ec));
        EXPECT(good(ec));

        create_symlink(L"test_create_symlink_dir/cat.txt"sv, linkname, ec);
        EXPECT(good(ec));
        EXPECT(status(linkname, ec).type() == file_type::not_found);
        EXPECT(ec == error_condition(3, system_category())); // ERROR_PATH_NOT_FOUND
        EXPECT(remove(linkname, ec));
        EXPECT(good(ec));
    }

    // example error: malformed source
    EXPECT(throws_filesystem_error([&] { create_symlink(filename, badPath); }, "create_symlink", filename, badPath));
    create_symlink(filename, badPath, ec);
    EXPECT(bad(ec));
}

void test_read_symlink() {

    { // Success case
        error_code ec;
        const path filepath = L"test_read_symlink.txt";
        remove(filepath, ec);
        EXPECT(good(ec));
        const path linkpath = L"test_read_symlink_link.txt";
        remove(linkpath, ec);
        EXPECT(good(ec));

        create_file_containing(filepath, L"helloworld\n");
        create_symlink(filepath, linkpath, ec);
        if (ec) {
            check_symlink_permissions(ec, L"read_symlink");
        } else {
            EXPECT(equivalent(filepath, linkpath, ec));
            EXPECT(good(ec));

            const auto actual_ec = read_symlink(linkpath, ec);
            EXPECT(good(ec));
            EXPECT(filepath == actual_ec);

            const auto actual = read_symlink(linkpath);
            EXPECT(filepath == actual);

            remove(filepath, ec);
            EXPECT(good(ec));
            remove(linkpath, ec);
            EXPECT(good(ec));
        }
    }

    {
        // TODO: Also test junctions
    }

    { // File is not a symlink
        error_code ec;
        const path not_a_reparse_point = L"test_read_symlink_not_a_reparse_point.txt";
        remove(not_a_reparse_point, ec);
        EXPECT(good(ec));

        create_file_containing(not_a_reparse_point, L"helloworld\n");
        const auto actual = read_symlink(not_a_reparse_point, ec);
        EXPECT(ec.value() == 4390); // ERROR_NOT_A_REPARSE_POINT

        EXPECT(throws_filesystem_error(
            [&]() { return read_symlink(not_a_reparse_point); }, "read_symlink", not_a_reparse_point.native()));

        remove(not_a_reparse_point, ec);
        EXPECT(good(ec));
    }

    { // File does not exist
        error_code ec;
        const path non_existent = L"test_read_symlink_non_existent.txt";
        remove(non_existent, ec);
        EXPECT(good(ec));

        const auto actual = read_symlink(non_existent, ec);
        EXPECT(ec.value() == 2); // ERROR_FILE_NOT_FOUND

        EXPECT(throws_filesystem_error(
            [&]() { return read_symlink(non_existent); }, "read_symlink", non_existent.native()));
    }
}

void test_copy_symlink() {
    { // Success case for file
        error_code ec;
        const path filepath = L"test_copy_symlink.txt";
        remove(filepath, ec);
        EXPECT(good(ec));
        const path linkpath_original = L"test_copy_symlink_link_theoriginal.txt";
        remove(linkpath_original, ec);
        EXPECT(good(ec));
        const path linkpath_copy = L"test_copy_symlink_link_thecopy.txt";
        remove(linkpath_copy, ec);
        EXPECT(good(ec));

        create_file_containing(filepath, L"helloworld\n");
        EXPECT(!is_directory(filepath, ec));
        EXPECT(good(ec));
        create_symlink(filepath, linkpath_original, ec);
        if (ec) {
            check_symlink_permissions(ec, L"copy_symlink 1");
        } else {
            EXPECT(equivalent(filepath, linkpath_original, ec));
            EXPECT(good(ec));
            EXPECT(is_regular_file(linkpath_original, ec));
            EXPECT(good(ec));

            copy_symlink(linkpath_original, linkpath_copy, ec);
            EXPECT(good(ec));
            EXPECT(equivalent(linkpath_original, linkpath_copy, ec));
            EXPECT(good(ec));
            EXPECT(is_regular_file(linkpath_copy, ec));
            EXPECT(good(ec));
            remove(linkpath_copy, ec);
            EXPECT(good(ec));

            copy_symlink(linkpath_original, linkpath_copy);
            EXPECT(equivalent(linkpath_original, linkpath_copy));
            EXPECT(!is_directory(linkpath_copy));
            remove(linkpath_copy, ec);
            EXPECT(good(ec));

            remove(linkpath_original, ec);
            EXPECT(good(ec));
            remove(filepath, ec);
            EXPECT(good(ec));
        }
    }

    { // Success case for directory
        error_code ec;
        const path filepath = L"test_copy_symlink.dir";
        remove(filepath, ec);
        EXPECT(good(ec));
        const path linkpath_original = L"test_copy_symlink_link_theoriginal.dir";
        remove(linkpath_original, ec);
        EXPECT(good(ec));
        const path linkpath_copy = L"test_copy_symlink_link_thecopy.dir";
        remove(linkpath_copy, ec);
        EXPECT(good(ec));

        create_directory(filepath);
        EXPECT(is_directory(filepath, ec));
        EXPECT(good(ec));
        create_directory_symlink(filepath, linkpath_original, ec);
        if (ec) {
            check_symlink_permissions(ec, L"copy_symlink 2");
        } else {
            EXPECT(equivalent(filepath, linkpath_original, ec));
            EXPECT(good(ec));
            EXPECT(is_directory(linkpath_original, ec));
            EXPECT(good(ec));

            copy_symlink(linkpath_original, linkpath_copy, ec);
            EXPECT(good(ec));
            EXPECT(equivalent(linkpath_original, linkpath_copy, ec));
            EXPECT(good(ec));
            EXPECT(is_directory(linkpath_copy, ec));
            EXPECT(good(ec));
            remove(linkpath_copy, ec);
            EXPECT(good(ec));

            copy_symlink(linkpath_original, linkpath_copy);
            EXPECT(equivalent(linkpath_original, linkpath_copy));
            EXPECT(is_directory(linkpath_copy, ec));
            EXPECT(good(ec));
            remove(linkpath_copy, ec);
            EXPECT(good(ec));

            remove(linkpath_original, ec);
            EXPECT(good(ec));
            remove(filepath, ec);
            EXPECT(good(ec));
        }
    }

    { // Symlink target does not exist
        error_code ec;
        const path filepath = L"test_copy_symlink_target_not_present.txt";
        remove(filepath, ec);
        EXPECT(good(ec));
        const path linkpath_original = L"test_copy_symlink_target_not_present_theoriginal.txt";
        remove(linkpath_original, ec);
        EXPECT(good(ec));
        const path linkpath_copy = L"test_copy_symlink_target_not_present_thecopy.txt";
        remove(linkpath_copy, ec);
        EXPECT(good(ec));

        create_symlink(filepath, linkpath_original, ec);
        if (ec) {
            check_symlink_permissions(ec, L"copy_symlink 3");
        } else {
            copy_symlink(linkpath_original, linkpath_copy, ec);
            EXPECT(good(ec));
            EXPECT(read_symlink(linkpath_original) == read_symlink(linkpath_copy));
            EXPECT(good(ec));
            remove(linkpath_copy, ec);
            EXPECT(good(ec));

            copy_symlink(linkpath_original, linkpath_copy);
            EXPECT(read_symlink(linkpath_original) == read_symlink(linkpath_copy));
            remove(linkpath_copy, ec);
            EXPECT(good(ec));

            remove(linkpath_original, ec);
            EXPECT(good(ec));
        }
    }
}

void equivalent_failure_test_case(const path& left, const path& right) {
    EXPECT(throws_filesystem_error([&] { EXPECT(!equivalent(left, right)); }, "equivalent", left, right));

    error_code ec;
    if (equivalent(left, right, ec)) {
        wcerr << L"Expected equivalent(" << left << L", " << right << L") to fail but it returned true\n";
        pass = false;
    } else if (!ec) {
        EXPECT(ec.category() == system_category());
        wcerr << L"Expected equivalent(" << left << L", " << right
              << L") to fail but it "
                 L"returned no failure code\n";
        pass = false;
    }
}

void equivalent_test_case(const path& left, const path& right, const bool expected) {
    error_code ec;
    const bool actual = equivalent(left, right, ec);
    EXPECT(good(ec));
    if (expected != actual) {
        wcerr << boolalpha << L"Expected equivalent(" << left << L", " << right << L") to be " << expected
              << L" but it was " << actual << L"\n";
        pass = false;
    }
}

void test_equivalent() {
    const test_temp_directory tempDir("equivalent"sv);
    const path& directoryPath = tempDir.directoryPath;
    error_code ec(-1, generic_category());

    create_directory(directoryPath / L"dir"sv, ec);
    EXPECT(good(ec));
    create_file_containing(directoryPath / L"a.txt"sv, L"a\n");
    create_file_containing(directoryPath / L"b.txt"sv, L"b\n");

    equivalent_failure_test_case(directoryPath / L"nonexistent.txt"sv, directoryPath / L"nonexistent.txt"sv);
    equivalent_failure_test_case(directoryPath / L"a.txt"sv, directoryPath / L"nonexistent.txt");
    equivalent_failure_test_case(directoryPath / L"nonexistent.txt"sv, directoryPath / L"b.txt"sv);

    equivalent_test_case(directoryPath / L"a.txt"sv, directoryPath / L"a.txt"sv, true);
    equivalent_test_case(
        directoryPath / L"a.txt"sv, directoryPath / L".."sv / directoryPath.filename() / L"a.txt"sv, true);
    equivalent_test_case(directoryPath / L"a.txt"sv, directoryPath / L"b.txt"sv, false);
    equivalent_test_case(directoryPath, directoryPath, true);
    equivalent_test_case(directoryPath / L"dir"sv, directoryPath / L"dir"sv, true);
    equivalent_test_case(directoryPath, directoryPath / L"dir"sv, false);
}

void test_file_size_and_resize() {
    const path filename(L"file_size_test.txt"sv);
    const path dirname(L"file_size_test.dir"sv);
    vector<char> tmp{'0', '1', '2', '3', '4'};
    write_file_data(filename, tmp);

    EXPECT(file_size(filename) == tmp.size());
    error_code ec;
    EXPECT(file_size(filename, ec) == tmp.size());
    EXPECT(good(ec));

    EXPECT(throws_filesystem_error([] { return file_size(badPath); }, "file_size"sv, badPath));
    EXPECT(file_size(badPath, ec) == static_cast<uintmax_t>(-1));
    EXPECT(bad(ec));

    EXPECT(
        throws_filesystem_error([&] { resize_file(filename, static_cast<uintmax_t>(-1)); }, "resize_file", filename));
    ec.clear();
    resize_file(filename, static_cast<uintmax_t>(-1), ec);
    EXPECT(bad(ec));

    resize_file(filename, 100, ec);
    EXPECT(good(ec));
    EXPECT(file_size(filename, ec) == 100);
    EXPECT(good(ec));
    tmp.resize(100);
    EXPECT(tmp == read_file_data(filename));

    // with the throwing interfaces
    resize_file(filename, 2);
    EXPECT(file_size(filename) == 2);
    tmp.resize(2);
    EXPECT(tmp == read_file_data(filename));

    remove(filename, ec);
    EXPECT(good(ec));

    create_directory(dirname);
    resize_file(dirname, 100, ec);
    EXPECT(bad(ec));
}

void test_hard_link_count() {
    error_code ec;
    const path realFileName(L"test_hard_link_count.txt"sv);
    const path link1(L"test_hard_link_count_link_1"sv);
    const path link2(L"test_hard_link_count_link_2"sv);
    const auto cleanup = [&] {
        remove(realFileName, ec);
        EXPECT(good(ec));
        remove(link1, ec);
        EXPECT(good(ec));
        remove(link2, ec);
        EXPECT(good(ec));
    };

    cleanup();

    create_file_containing(realFileName, L"Hello");
    EXPECT(hard_link_count(realFileName, ec) == 1);
    EXPECT(good(ec));
    create_hard_link(realFileName, link1, ec);
    EXPECT(good(ec));
    EXPECT(hard_link_count(realFileName, ec) == 2);
    EXPECT(good(ec));
    EXPECT(hard_link_count(link1, ec) == 2);
    EXPECT(good(ec));
    create_hard_link(realFileName, link2, ec);
    EXPECT(good(ec));
    // also test throwing versions
    EXPECT(hard_link_count(realFileName) == 3);
    EXPECT(hard_link_count(link1) == 3);
    EXPECT(hard_link_count(link2) == 3);

    cleanup();

    EXPECT(throws_filesystem_error([] { return hard_link_count(badPath); }, "hard_link_count", badPath));
    EXPECT(hard_link_count(badPath, ec) == static_cast<uintmax_t>(-1));
    EXPECT(bad(ec));
}

void test_is_empty() {
    error_code ec;
    const test_temp_directory tempDir("is_empty"sv);
    const path testDir = tempDir.directoryPath / L"dir"sv;

    EXPECT(throws_filesystem_error([&] { return filesystem::is_empty(testDir); }, "is_empty", testDir));
    EXPECT(!filesystem::is_empty(testDir, ec));
    EXPECT(bad(ec));

    create_directory(testDir, ec);
    EXPECT(good(ec));
    EXPECT(filesystem::is_empty(testDir));
    EXPECT(filesystem::is_empty(testDir, ec));
    EXPECT(good(ec));

    const path testFile(testDir / L"test_file"sv);
    create_file_containing(testFile, L"");
    EXPECT(!filesystem::is_empty(testDir));
    EXPECT(!filesystem::is_empty(testDir, ec));
    EXPECT(good(ec));

    EXPECT(filesystem::is_empty(testFile));
    EXPECT(filesystem::is_empty(testFile, ec));
    EXPECT(good(ec));

    resize_file(testFile, 1);
    EXPECT(!filesystem::is_empty(testFile));
    EXPECT(!filesystem::is_empty(testFile, ec));
    EXPECT(good(ec));
}

template <typename T>
struct MyTraits : char_traits<T> {};

template <typename T>
struct MyAlloc {
    using value_type = T;

    MyAlloc() = default;

    template <typename U>
    MyAlloc(const MyAlloc<U>&) {}

    T* allocate(size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) {
        allocator<T>{}.deallocate(p, n);
    }

    template <typename U>
    bool operator==(const MyAlloc<U>&) const {
        return true;
    }
    template <typename U>
    bool operator!=(const MyAlloc<U>&) const {
        return false;
    }
};

void test_conversions() {
    static_assert(is_constructible_v<path, string>);
    static_assert(is_constructible_v<path, wstring>);
    static_assert(is_constructible_v<path, u16string>);
    static_assert(is_constructible_v<path, u32string>);
    static_assert(is_constructible_v<path, basic_string<char, MyTraits<char>, MyAlloc<char>>>);
    static_assert(is_constructible_v<path, basic_string<wchar_t, MyTraits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_constructible_v<path, basic_string<char16_t, MyTraits<char16_t>, MyAlloc<char16_t>>>);
    static_assert(is_constructible_v<path, basic_string<char32_t, MyTraits<char32_t>, MyAlloc<char32_t>>>);

    static_assert(is_constructible_v<path, string_view>);
    static_assert(is_constructible_v<path, wstring_view>);
    static_assert(is_constructible_v<path, u16string_view>);
    static_assert(is_constructible_v<path, u32string_view>);
    static_assert(is_constructible_v<path, basic_string_view<char, MyTraits<char>>>);
    static_assert(is_constructible_v<path, basic_string_view<wchar_t, MyTraits<wchar_t>>>);
    static_assert(is_constructible_v<path, basic_string_view<char16_t, MyTraits<char16_t>>>);
    static_assert(is_constructible_v<path, basic_string_view<char32_t, MyTraits<char32_t>>>);

    static_assert(is_constructible_v<path, char(&)[5]>);
    static_assert(is_constructible_v<path, wchar_t(&)[5]>);
    static_assert(is_constructible_v<path, char16_t(&)[5]>);
    static_assert(is_constructible_v<path, char32_t(&)[5]>);
    static_assert(is_constructible_v<path, const char(&)[5]>);
    static_assert(is_constructible_v<path, const wchar_t(&)[5]>);
    static_assert(is_constructible_v<path, const char16_t(&)[5]>);
    static_assert(is_constructible_v<path, const char32_t(&)[5]>);

    static_assert(is_constructible_v<path, char*>);
    static_assert(is_constructible_v<path, wchar_t*>);
    static_assert(is_constructible_v<path, char16_t*>);
    static_assert(is_constructible_v<path, char32_t*>);
    static_assert(is_constructible_v<path, const char*>);
    static_assert(is_constructible_v<path, const wchar_t*>);
    static_assert(is_constructible_v<path, const char16_t*>);
    static_assert(is_constructible_v<path, const char32_t*>);

    static_assert(is_constructible_v<path, string::iterator>);
    static_assert(is_constructible_v<path, wstring::iterator>);
    static_assert(is_constructible_v<path, u16string::iterator>);
    static_assert(is_constructible_v<path, u32string::iterator>);
    static_assert(is_constructible_v<path, string::const_iterator>);
    static_assert(is_constructible_v<path, wstring::const_iterator>);
    static_assert(is_constructible_v<path, u16string::const_iterator>);
    static_assert(is_constructible_v<path, u32string::const_iterator>);

    static_assert(is_constructible_v<path, forward_list<char>::iterator>);
    static_assert(is_constructible_v<path, forward_list<wchar_t>::iterator>);
    static_assert(is_constructible_v<path, forward_list<char16_t>::iterator>);
    static_assert(is_constructible_v<path, forward_list<char32_t>::iterator>);
    static_assert(is_constructible_v<path, forward_list<char>::const_iterator>);
    static_assert(is_constructible_v<path, forward_list<wchar_t>::const_iterator>);
    static_assert(is_constructible_v<path, forward_list<char16_t>::const_iterator>);
    static_assert(is_constructible_v<path, forward_list<char32_t>::const_iterator>);

    static_assert(!is_constructible_v<path, basic_string<signed char>>);
    static_assert(!is_constructible_v<path, basic_string_view<signed char>>);
    static_assert(!is_constructible_v<path, const signed char(&)[5]>);
    static_assert(!is_constructible_v<path, const signed char*>);
    static_assert(!is_constructible_v<path, basic_string<unsigned char>>);
    static_assert(!is_constructible_v<path, basic_string_view<unsigned char>>);
    static_assert(!is_constructible_v<path, const unsigned char(&)[5]>);
    static_assert(!is_constructible_v<path, const unsigned char*>);
    static_assert(!is_constructible_v<path, char>);
    static_assert(!is_constructible_v<path, double>);

    static_assert(is_constructible_v<path, const char32_t*, path::format>);

    static_assert(is_convertible_v<const char32_t*, path>);

    static_assert(is_constructible_v<path, char*, char*>);
    static_assert(is_constructible_v<path, wchar_t*, wchar_t*>);
    static_assert(is_constructible_v<path, char16_t*, char16_t*>);
    static_assert(is_constructible_v<path, char32_t*, char32_t*>);
    static_assert(is_constructible_v<path, const char*, const char*>);
    static_assert(is_constructible_v<path, const wchar_t*, const wchar_t*>);
    static_assert(is_constructible_v<path, const char16_t*, const char16_t*>);
    static_assert(is_constructible_v<path, const char32_t*, const char32_t*>);

    static_assert(is_constructible_v<path, string::iterator, string::iterator>);
    static_assert(is_constructible_v<path, wstring::iterator, wstring::iterator>);
    static_assert(is_constructible_v<path, u16string::iterator, u16string::iterator>);
    static_assert(is_constructible_v<path, u32string::iterator, u32string::iterator>);
    static_assert(is_constructible_v<path, string::const_iterator, string::const_iterator>);
    static_assert(is_constructible_v<path, wstring::const_iterator, wstring::const_iterator>);
    static_assert(is_constructible_v<path, u16string::const_iterator, u16string::const_iterator>);
    static_assert(is_constructible_v<path, u32string::const_iterator, u32string::const_iterator>);

    static_assert(is_constructible_v<path, forward_list<char>::iterator, forward_list<char>::iterator>);
    static_assert(is_constructible_v<path, forward_list<wchar_t>::iterator, forward_list<wchar_t>::iterator>);
    static_assert(is_constructible_v<path, forward_list<char16_t>::iterator, forward_list<char16_t>::iterator>);
    static_assert(is_constructible_v<path, forward_list<char32_t>::iterator, forward_list<char32_t>::iterator>);
    static_assert(is_constructible_v<path, forward_list<char>::const_iterator, forward_list<char>::const_iterator>);
    static_assert(
        is_constructible_v<path, forward_list<wchar_t>::const_iterator, forward_list<wchar_t>::const_iterator>);
    static_assert(
        is_constructible_v<path, forward_list<char16_t>::const_iterator, forward_list<char16_t>::const_iterator>);
    static_assert(
        is_constructible_v<path, forward_list<char32_t>::const_iterator, forward_list<char32_t>::const_iterator>);

    static_assert(is_constructible_v<path, const char32_t*, const char32_t*, path::format>);

    const path p1("narrow.txt"sv);
    EXPECT(p1.native() == L"narrow.txt");

    const path p2(L"wide_\xD83D\xDC08.txt"sv);
    EXPECT(p2.native() == L"wide_\xD83D\xDC08.txt");

    const path p3(u"utf16_\xD83D\xDC08.txt"sv);
    EXPECT(p3.native() == L"utf16_\xD83D\xDC08.txt");

    const path p4(U"utf32_\x1F408.txt"sv);
    EXPECT(p4.native() == L"utf32_\xD83D\xDC08.txt");

    const path p5("basic_string.txt"s);
    EXPECT(p5.native() == L"basic_string.txt");

    const path p6("array.txt");
    EXPECT(p6.native() == L"array.txt");

    const char* const ptr = "pointer.txt";
    const path p7(ptr);
    EXPECT(p7.native() == L"pointer.txt");

    const vector<char> vec = {'v', 'e', 'c', '.', 't', 'x', 't', '\0'};
    const path p8(vec.begin());
    EXPECT(p8.native() == L"vec.txt");

    const forward_list<char> fl = {'f', 'l', '.', 't', 'x', 't', '\0'};
    const path p9(fl.begin());
    EXPECT(p9.native() == L"fl.txt");

    const path p10(ptr + 1, ptr + 10);
    EXPECT(p10.native() == L"ointer.tx");

    const path p11(vec.begin() + 1, vec.begin() + 6);
    EXPECT(p11.native() == L"ec.tx");

    const path p12(next(fl.begin()), next(fl.begin(), 5));
    EXPECT(p12.native() == L"l.tx");

    path p("one");
    EXPECT(p.native() == L"one");
    p = "two";
    EXPECT(p.native() == L"two");
    p.assign("three");
    EXPECT(p.native() == L"three");
    p.assign(vec.rbegin() + 1, vec.rend());
    EXPECT(p.native() == L"txt.cev");
    p /= "earth";
    EXPECT(p.native() == LR"(txt.cev\earth)");
    p.append("mars");
    EXPECT(p.native() == LR"(txt.cev\earth\mars)");
    p.append(vec.begin(), vec.begin() + 3);
    EXPECT(p.native() == LR"(txt.cev\earth\mars\vec)");
    p += "tor";
    EXPECT(p.native() == LR"(txt.cev\earth\mars\vector)");
    p += U'\U0001F408';
    EXPECT(p.native()
           == LR"(txt.cev\earth\mars\vector)"
              L"\xD83D\xDC08");
    p.concat("meow");
    EXPECT(p.native()
           == LR"(txt.cev\earth\mars\vector)"
              L"\xD83D\xDC08"
              LR"(meow)");
    p.concat(ptr, ptr + 5);
    EXPECT(p.native()
           == LR"(txt.cev\earth\mars\vector)"
              L"\xD83D\xDC08"
              LR"(meowpoint)");

    const path p13(R"(pepper\mint)");
    EXPECT(p13.string() == R"(pepper\mint)");
    EXPECT(p13.string<char>() == R"(pepper\mint)");
    EXPECT(p13.generic_string() == R"(pepper/mint)");
    EXPECT(p13.generic_string<char>() == R"(pepper/mint)");

    EXPECT(narrow_equal(u8"\U0001F408"sv, "\xF0\x9F\x90\x88"sv)); // UTF-8 basic check
    const path p14(U"cat_\x1F408_emoji\\meow");
    EXPECT(p14.wstring() == L"cat_\xD83D\xDC08_emoji\\meow");

#ifdef __cpp_lib_char8_t
    using utf8_result_t = basic_string<char8_t>;
#else // ^^^ __cpp_lib_char8_t / !__cpp_lib_char8_t vvv
    using utf8_result_t = basic_string<char>;
#endif // __cpp_lib_char8_t

    static_assert(is_same_v<decltype(p14.u8string()), utf8_result_t>);
    EXPECT(narrow_equal(p14.u8string(), "cat_\xF0\x9F\x90\x88_emoji\\meow"sv));

    EXPECT(p14.u16string() == u"cat_\xD83D\xDC08_emoji\\meow");
    EXPECT(p14.u32string() == U"cat_\x1F408_emoji\\meow");
    EXPECT(p14.string<wchar_t>() == L"cat_\xD83D\xDC08_emoji\\meow");
    EXPECT(p14.string<char16_t>() == u"cat_\xD83D\xDC08_emoji\\meow");
    EXPECT(p14.string<char32_t>() == U"cat_\x1F408_emoji\\meow");
    const auto str1 = p14.string<char32_t, MyTraits<char32_t>, MyAlloc<char32_t>>();
    const auto str2 = p14.string<char32_t, MyTraits<char32_t>>(MyAlloc<char32_t>{});
    static_assert(is_same_v<decltype(str1), const basic_string<char32_t, MyTraits<char32_t>, MyAlloc<char32_t>>>);
    static_assert(is_same_v<decltype(str2), const basic_string<char32_t, MyTraits<char32_t>, MyAlloc<char32_t>>>);
    EXPECT(str1 == U"cat_\x1F408_emoji\\meow");
    EXPECT(str2 == U"cat_\x1F408_emoji\\meow");

    EXPECT(p14.generic_wstring() == L"cat_\xD83D\xDC08_emoji/meow");

    static_assert(is_same_v<decltype(p14.generic_u8string()), utf8_result_t>);
    EXPECT(narrow_equal(p14.generic_u8string(), "cat_\xF0\x9F\x90\x88_emoji/meow"sv));

    EXPECT(p14.generic_u16string() == u"cat_\xD83D\xDC08_emoji/meow");
    EXPECT(p14.generic_u32string() == U"cat_\x1F408_emoji/meow");
    EXPECT(p14.generic_string<wchar_t>() == L"cat_\xD83D\xDC08_emoji/meow");
    EXPECT(p14.generic_string<char16_t>() == u"cat_\xD83D\xDC08_emoji/meow");
    EXPECT(p14.generic_string<char32_t>() == U"cat_\x1F408_emoji/meow");
    const auto str3 = p14.generic_string<char32_t, MyTraits<char32_t>, MyAlloc<char32_t>>();
    const auto str4 = p14.generic_string<char32_t, MyTraits<char32_t>>(MyAlloc<char32_t>{});
    static_assert(is_same_v<decltype(str3), const basic_string<char32_t, MyTraits<char32_t>, MyAlloc<char32_t>>>);
    static_assert(is_same_v<decltype(str4), const basic_string<char32_t, MyTraits<char32_t>, MyAlloc<char32_t>>>);
    EXPECT(str3 == U"cat_\x1F408_emoji/meow");
    EXPECT(str4 == U"cat_\x1F408_emoji/meow");

    p = R"(one\two\three)";
    ostringstream oss;
    oss << p;
    EXPECT(oss.str() == R"("one\\two\\three")");
    istringstream iss(R"("four\\five\\six")");
    iss >> p;
    EXPECT(p.string() == R"(four\five\six)");

    const string utf8_str("cat_\xF0\x9F\x90\x88_emoji"s);
    const path p15 = u8path(utf8_str);
    EXPECT(p15.native() == L"cat_\xD83D\xDC08_emoji");
    const path p16 = u8path(utf8_str.begin(), utf8_str.end());
    EXPECT(p16.native() == L"cat_\xD83D\xDC08_emoji");

#ifdef __cpp_lib_char8_t
    const u8string real_utf8_str(u8"cat_\U0001F408_emoji");
    const path p17 = path(real_utf8_str);
    EXPECT(p17.native() == L"cat_\xD83D\xDC08_emoji");
    const path p18 = path(real_utf8_str.begin(), real_utf8_str.end());
    EXPECT(p18.native() == L"cat_\xD83D\xDC08_emoji");
#endif // __cpp_lib_char8_t
}

void test_file_size() {
    const test_temp_directory tempDir("file_size"sv);
    const path filePath = tempDir.directoryPath / L"example.txt"sv;
    create_file_containing(filePath, L"hello world");

    // _NODISCARD inline uintmax_t file_size(const path& _Path);
    // _NODISCARD inline uintmax_t file_size(const path& _Path, error_code& _Ec) noexcept;

    // test invalid or empty path
    {
        error_code ec;
        EXPECT(bad_file_size == file_size(L""sv, ec));
        EXPECT(ec == errc::no_such_file_or_directory);

        for (auto&& nonexistent : nonexistentPaths) {
            EXPECT(bad_file_size == file_size(nonexistent, ec));
            EXPECT(ec == errc::no_such_file_or_directory);
        }

        EXPECT(throws_filesystem_error([] { (void) file_size(L""sv); }, "file_size", L""sv));
    }

    // test happy
    {
        error_code ec;
        const auto fsize = file_size(filePath, ec);
        EXPECT(good(ec));
        EXPECT(fsize > 0);
        EXPECT(file_size(filePath) == fsize);
    }

    // _NODISCARD uintmax_t directory_entry::file_size() const;
    // _NODISCARD uintmax_t directory::file_size(error_code& _Ec) const noexcept;

    // test empty path
    {
        error_code ec;
        const directory_entry default_de;
        EXPECT(bad_file_size == default_de.file_size(ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(throws_filesystem_error([&] { (void) default_de.file_size(); }, "directory_entry::file_size"));
    }

    // test happy file
    {
        error_code ec;
        const directory_entry de(filePath);
        const auto fsize = de.file_size(ec);
        EXPECT(fsize > 0);
        EXPECT(good(ec));
        EXPECT(de.file_size() == fsize);
    }
}

void test_last_write_time() {
    // set the last write time to a very early value; this will fail implementations that attempt to adjust
    // from the system_clock epoch to the NT epoch when writing to disk
    error_code ec;
    constexpr file_time_type testTime{file_time_type::duration{1}};

    const test_temp_directory tempDir("last_write_time"sv);

    const auto filePath = tempDir.directoryPath / L"test_last_write_time.txt"sv;

    // should fail because file does not exist:
    EXPECT(throws_filesystem_error([&] { return last_write_time(filePath); }, "last_write_time", filePath));
    EXPECT(throws_filesystem_error([&] { last_write_time(filePath, testTime); }, "last_write_time", filePath));
    EXPECT(last_write_time(filePath, ec) == file_time_type::min());
    EXPECT(bad(ec));
    last_write_time(filePath, testTime, ec);
    EXPECT(bad(ec));

    create_file_containing(filePath, L"last write time testing");

    auto actual = last_write_time(filePath, ec);
    EXPECT(good(ec));
    // test that the file we just created has last write time now +/- 30 minutes
    auto minAcceptable = file_time_type::clock::now() - 30min;
    auto maxAcceptable = minAcceptable + 1h;
    EXPECT(actual >= minAcceptable && actual <= maxAcceptable);

    last_write_time(filePath, testTime, ec);
    EXPECT(good(ec));

    actual = last_write_time(filePath, ec);
    EXPECT(good(ec));
    EXPECT(actual == testTime); // assumes test is running on NTFS or ReFS

    // _NODISCARD inline file_time_type last_write_time(const path& _Path);
    // _NODISCARD inline file_time_type last_write_time(const path& _Path, error_code& _Ec) noexcept;

    // test invalid or empty path
    {
        EXPECT(bad_file_time == last_write_time(L""sv, ec));
        EXPECT(ec == errc::no_such_file_or_directory);

        for (auto&& nonexistent : nonexistentPaths) {
            EXPECT(bad_file_time == last_write_time(nonexistent, ec));
            EXPECT(ec == errc::no_such_file_or_directory);
        }

        EXPECT(throws_filesystem_error([] { (void) last_write_time(L""sv); }, "last_write_time", L""sv));
    }

    // test happy
    {
        const auto ftime = last_write_time(filePath, ec);
        EXPECT(good(ec));
        EXPECT(ftime != bad_file_time);
        EXPECT(last_write_time(filePath) == ftime);

        const auto dtime = last_write_time(tempDir.directoryPath, ec);
        EXPECT(good(ec));
        EXPECT(dtime != bad_file_time);
        EXPECT(last_write_time(tempDir.directoryPath) == dtime);
    }

    // _NODISCARD file_time_type directory_entry::last_write_time() const;
    // _NODISCARD file_time_type directory::last_write_time(error_code& _Ec) const noexcept;

    // test empty path
    {
        const directory_entry default_de;
        EXPECT(bad_file_time == default_de.last_write_time(ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(
            throws_filesystem_error([&] { (void) default_de.last_write_time(); }, "directory_entry::last_write_time"));
    }

    // test happy file
    {
        const directory_entry de(filePath);
        const auto ftime = de.last_write_time(ec);
        EXPECT(good(ec));
        EXPECT(ftime != bad_file_time);
        EXPECT(de.last_write_time() == ftime);
    }

    // test happy dir
    {
        const directory_entry de(tempDir.directoryPath);
        const auto ftime = de.last_write_time(ec);
        EXPECT(good(ec));
        EXPECT(ftime != bad_file_time);
        EXPECT(de.last_write_time() == ftime);
        const auto newTime = ftime + 1h;
        last_write_time(tempDir.directoryPath, newTime);
        EXPECT(last_write_time(tempDir.directoryPath) == newTime);
    }
}

void test_invalid_conversions() {
    // N4727 30.11.7.2.2 [fs.path.type.cvt] is vague, but we throw
    // system_error to prevent silent data loss from invalid conversions.
    const string_view invalid_utf8     = "forbidden_\xC0_byte"sv;
    const wstring_view invalid_utf16   = L"reversed_\xDC08\xD83D_surrogates"sv;
    const u32string_view invalid_utf32 = U"huge_\x110000_codepoint"sv;

    const path invalid_path(invalid_utf16);

    EXPECT(throws_system_error([&invalid_path] { (void) invalid_path.string(); }));
    EXPECT(throws_system_error([&invalid_path] { (void) invalid_path.u8string(); }));
    EXPECT(throws_system_error([&invalid_path] { (void) invalid_path.u32string(); }));
    EXPECT(throws_system_error([&invalid_utf8] { (void) u8path(invalid_utf8); }));
    EXPECT(throws_system_error([&invalid_utf32] { (void) path{invalid_utf32}; }));
}

void test_status() {
    const test_temp_directory tempDir("test_status"sv);
    const path& testDir = tempDir.directoryPath;
    const path testFile(testDir / L"test_file"sv);
    const path testLink(testDir / L"test_link"sv);
    error_code ec;
    create_file_containing(testFile, L"Hello");

    for (auto&& nonexistent : nonexistentPaths) {
        EXPECT(status(nonexistent).type() == file_type::not_found); // should not throw
        EXPECT(status(nonexistent, ec).type() == file_type::not_found);
        EXPECT(ec.category() == system_category());
        // Accept ERROR_FILE_NOT_FOUND (2), ERROR_PATH_NOT_FOUND (3), ERROR_BAD_NETPATH (53), ERROR_INVALID_NAME (123).
        // This should match __std_is_file_not_found() in <xfilesystem_abi.h>.
        EXPECT(ec.value() == 2 || ec.value() == 3 || ec.value() == 53 || ec.value() == 123);
        EXPECT(ec == errc::no_such_file_or_directory);
    }

    EXPECT(!exists(file_status{}));
    EXPECT(!exists(file_status{file_type::not_found}));
    EXPECT(exists(file_status{file_type::regular}));
    EXPECT(!is_block_file(file_status{}));
    EXPECT(is_block_file(file_status{file_type::block}));
    EXPECT(!is_character_file(file_status{}));
    EXPECT(is_character_file(file_status{file_type::character}));
    EXPECT(!is_directory(file_status{}));
    EXPECT(is_directory(file_status{file_type::directory}));
    EXPECT(!is_fifo(file_status{}));
    EXPECT(is_fifo(file_status{file_type::fifo}));
    EXPECT(!is_other(file_status{}));
    EXPECT(!is_other(file_status{file_type::not_found}));
    EXPECT(is_other(file_status{file_type::block}));
    EXPECT(is_other(file_status{file_type::character}));
    EXPECT(is_other(file_status{file_type::junction}));
    EXPECT(is_other(file_status{file_type::socket}));
    EXPECT(is_other(file_status{file_type::unknown}));
    EXPECT(!is_regular_file(file_status{}));
    EXPECT(is_regular_file(file_status{file_type::regular}));
    EXPECT(!is_socket(file_status{}));
    EXPECT(is_socket(file_status{file_type::socket}));
    EXPECT(!is_symlink(file_status{}));
    EXPECT(is_symlink(file_status{file_type::symlink}));

    for (auto&& nonexistent : nonexistentPaths) {
        EXPECT(!exists(nonexistent));
        EXPECT(!is_block_file(nonexistent));
        EXPECT(!is_character_file(nonexistent));
        EXPECT(!is_directory(nonexistent));
        EXPECT(!is_fifo(nonexistent));
        EXPECT(!is_other(nonexistent));
        EXPECT(!is_regular_file(nonexistent));
        EXPECT(!is_socket(nonexistent));
        EXPECT(!is_symlink(nonexistent));

        EXPECT(!exists(nonexistent, ec));
        EXPECT(good(ec));
        EXPECT(!is_block_file(nonexistent, ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(!is_character_file(nonexistent, ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(!is_directory(nonexistent, ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(!is_fifo(nonexistent, ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(!is_other(nonexistent, ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(!is_regular_file(nonexistent, ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(!is_socket(nonexistent, ec));
        EXPECT(ec == errc::no_such_file_or_directory);
        EXPECT(!is_symlink(nonexistent, ec));
        EXPECT(ec == errc::no_such_file_or_directory);
    }

    EXPECT(exists(testDir));
    EXPECT(exists(testDir, ec));
    EXPECT(good(ec));
    EXPECT(!is_block_file(testDir));
    EXPECT(!is_block_file(testDir, ec));
    EXPECT(good(ec));
    EXPECT(!is_character_file(testDir));
    EXPECT(is_directory(testDir));
    EXPECT(is_directory(testDir, ec));
    EXPECT(good(ec));
    EXPECT(!is_fifo(testDir));
    EXPECT(!is_other(testDir));
    EXPECT(!is_other(testDir, ec));
    EXPECT(good(ec));
    EXPECT(!is_regular_file(testDir));
    EXPECT(!is_socket(testDir));
    EXPECT(!is_symlink(testDir));

    EXPECT(exists(testFile));
    EXPECT(exists(testFile, ec));
    EXPECT(good(ec));
    EXPECT(!is_block_file(testFile));
    EXPECT(!is_block_file(testFile, ec));
    EXPECT(good(ec));
    EXPECT(!is_character_file(testFile));
    EXPECT(!is_directory(testFile));
    EXPECT(!is_fifo(testFile));
    EXPECT(!is_other(testFile));
    EXPECT(is_regular_file(testFile));
    EXPECT(is_regular_file(testFile, ec));
    EXPECT(good(ec));
    EXPECT(!is_socket(testFile));
    EXPECT(!is_symlink(testFile));

    create_symlink(testFile, testLink, ec);
    if (ec) {
        check_symlink_permissions(ec, L"status functions");
    } else {
        EXPECT(exists(testLink));
        EXPECT(exists(testLink, ec));
        EXPECT(good(ec));
        EXPECT(!is_block_file(testLink));
        EXPECT(!is_block_file(testLink, ec));
        EXPECT(good(ec));
        EXPECT(!is_character_file(testLink));
        EXPECT(!is_directory(testLink));
        EXPECT(!is_fifo(testLink));
        EXPECT(!is_other(testLink));
        EXPECT(is_regular_file(testLink)); // followed symlink
        EXPECT(is_regular_file(testLink, ec));
        EXPECT(good(ec));
        EXPECT(!is_socket(testLink));
        EXPECT(is_symlink(testLink));
        EXPECT(is_symlink(testLink, ec)); // didn't follow symlink
        EXPECT(good(ec));
    }
}

void test_locale_conversions() {
    {
        const locale cyrillic_locale(locale::classic(), new stdext::cvt::codecvt_cp1251<wchar_t>());

        const string_view cp1251_koshka = "\xEA\xEE\xF8\xEA\xE0"sv;
        const wstring_view utf16_koshka = L"\x043A\x043E\x0448\x043A\x0430"sv;

        const path p1(cp1251_koshka, cyrillic_locale);
        EXPECT(p1.native() == utf16_koshka);

        const path p2(cp1251_koshka.begin(), cp1251_koshka.end(), cyrillic_locale);
        EXPECT(p2.native() == utf16_koshka);
    }

    {
        const locale sjis_locale(locale::classic(), new stdext::cvt::codecvt_sjis<wchar_t>());
        const string_view sjis_katakana_letter_yo   = "\x83\x88"sv;
        const wstring_view utf16_katakana_letter_yo = L"\x30E8"sv;

        const path p3(sjis_katakana_letter_yo.begin(), sjis_katakana_letter_yo.end(), sjis_locale);
        EXPECT(p3.native() == utf16_katakana_letter_yo);
    }

    {
        const string_view utf8_koshka_cat = "\xD0\xBA\xD0\xBE\xD1\x88\xD0\xBA\xD0\xB0_\xF0\x9F\x90\x88"sv;
        EXPECT(narrow_equal(utf8_koshka_cat, u8"\u043A\u043E\u0448\u043A\u0430_\U0001F408"sv)); // UTF-8 basic check
        const wstring_view utf16_koshka_cat = L"\x043A\x043E\x0448\x043A\x0430_\xD83D\xDC08"sv;
        EXPECT(u8path(utf8_koshka_cat).native() == utf16_koshka_cat); // UTF-16 basic check

        const locale utf8_locale(locale::classic(), new stdext::cvt::codecvt_utf8_utf16<wchar_t>());

        const path p4(utf8_koshka_cat.begin(), utf8_koshka_cat.end(), utf8_locale);
        EXPECT(p4.native() == utf16_koshka_cat);

        EXPECT(throws_system_error([&] {
            (void) path{utf8_koshka_cat.begin() + 1, utf8_koshka_cat.end(), utf8_locale};
        }));

        // stdext::cvt::codecvt_utf8_utf16 doesn't appear to handle codecvt_base::partial correctly.
    }
}

void test_lexically_normal() {
    // N4727 30.11.7.4.11 [fs.path.gen]/2
    EXPECT(path("cat/./dog/..").lexically_normal() == "cat/");
    EXPECT(path("cat/.///dog/../").lexically_normal() == "cat/");

    EXPECT(path(LR"(cat/./dog/..)"sv).lexically_normal().native() == LR"(cat\)"sv);
    EXPECT(path(LR"(cat/.///dog/../)"sv).lexically_normal().native() == LR"(cat\)"sv);

    EXPECT(path(LR"()"sv).lexically_normal().native() == LR"()"sv);

    EXPECT(path(LR"(X:)"sv).lexically_normal().native() == LR"(X:)"sv);

    EXPECT(path(LR"(X:DriveRelative)"sv).lexically_normal().native() == LR"(X:DriveRelative)"sv);

    EXPECT(path(LR"(X:\)"sv).lexically_normal().native() == LR"(X:\)"sv);
    EXPECT(path(LR"(X:/)"sv).lexically_normal().native() == LR"(X:\)"sv);
    EXPECT(path(LR"(X:\\\)"sv).lexically_normal().native() == LR"(X:\)"sv);
    EXPECT(path(LR"(X:///)"sv).lexically_normal().native() == LR"(X:\)"sv);

    EXPECT(path(LR"(X:\DosAbsolute)"sv).lexically_normal().native() == LR"(X:\DosAbsolute)"sv);
    EXPECT(path(LR"(X:/DosAbsolute)"sv).lexically_normal().native() == LR"(X:\DosAbsolute)"sv);
    EXPECT(path(LR"(X:\\\DosAbsolute)"sv).lexically_normal().native() == LR"(X:\DosAbsolute)"sv);
    EXPECT(path(LR"(X:///DosAbsolute)"sv).lexically_normal().native() == LR"(X:\DosAbsolute)"sv);

    EXPECT(path(LR"(\RootRelative)"sv).lexically_normal().native() == LR"(\RootRelative)"sv);
    EXPECT(path(LR"(/RootRelative)"sv).lexically_normal().native() == LR"(\RootRelative)"sv);
    EXPECT(path(LR"(\\\RootRelative)"sv).lexically_normal().native() == LR"(\RootRelative)"sv);
    EXPECT(path(LR"(///RootRelative)"sv).lexically_normal().native() == LR"(\RootRelative)"sv);

    EXPECT(path(LR"(\\server\share)"sv).lexically_normal().native() == LR"(\\server\share)"sv);
    EXPECT(path(LR"(//server/share)"sv).lexically_normal().native() == LR"(\\server\share)"sv);
    EXPECT(path(LR"(\\server\\\share)"sv).lexically_normal().native() == LR"(\\server\share)"sv);
    EXPECT(path(LR"(//server///share)"sv).lexically_normal().native() == LR"(\\server\share)"sv);

    EXPECT(path(LR"(\\?\device)"sv).lexically_normal().native() == LR"(\\?\device)"sv);
    EXPECT(path(LR"(//?/device)"sv).lexically_normal().native() == LR"(\\?\device)"sv);

    EXPECT(path(LR"(\??\device)"sv).lexically_normal().native() == LR"(\??\device)"sv);
    EXPECT(path(LR"(/??/device)"sv).lexically_normal().native() == LR"(\??\device)"sv);

    EXPECT(path(LR"(\\.\device)"sv).lexically_normal().native() == LR"(\\.\device)"sv);
    EXPECT(path(LR"(//./device)"sv).lexically_normal().native() == LR"(\\.\device)"sv);

    EXPECT(path(LR"(\\?\UNC\server\share)"sv).lexically_normal().native() == LR"(\\?\UNC\server\share)"sv);
    EXPECT(path(LR"(//?/UNC/server/share)"sv).lexically_normal().native() == LR"(\\?\UNC\server\share)"sv);

    EXPECT(path(LR"(C:\a/b\\c\/d/\e//f)"sv).lexically_normal().native() == LR"(C:\a\b\c\d\e\f)"sv);

    EXPECT(path(LR"(C:\meow\)"sv).lexically_normal().native() == LR"(C:\meow\)"sv);
    EXPECT(path(LR"(C:\meow/)"sv).lexically_normal().native() == LR"(C:\meow\)"sv);
    EXPECT(path(LR"(C:\meow\\)"sv).lexically_normal().native() == LR"(C:\meow\)"sv);
    EXPECT(path(LR"(C:\meow\/)"sv).lexically_normal().native() == LR"(C:\meow\)"sv);
    EXPECT(path(LR"(C:\meow/\)"sv).lexically_normal().native() == LR"(C:\meow\)"sv);
    EXPECT(path(LR"(C:\meow//)"sv).lexically_normal().native() == LR"(C:\meow\)"sv);

    EXPECT(path(LR"(C:\a\.\b\.\.\c\.\.\.)"sv).lexically_normal().native() == LR"(C:\a\b\c\)"sv);
    EXPECT(path(LR"(C:\a\.\b\.\.\c\.\.\.\)"sv).lexically_normal().native() == LR"(C:\a\b\c\)"sv);

    EXPECT(path(LR"(.)"sv).lexically_normal().native() == LR"(.)"sv);
    EXPECT(path(LR"(.\)"sv).lexically_normal().native() == LR"(.)"sv);
    EXPECT(path(LR"(.\.)"sv).lexically_normal().native() == LR"(.)"sv);
    EXPECT(path(LR"(.\.\)"sv).lexically_normal().native() == LR"(.)"sv);

    EXPECT(path(LR"(C:\a\b\c\d\e\..\f\..\..\..\g\h)"sv).lexically_normal().native() == LR"(C:\a\b\g\h)"sv);

    EXPECT(path(LR"(C:\a\b\c\d\e\..\f\..\..\..\g\h\..)"sv).lexically_normal().native() == LR"(C:\a\b\g\)"sv);
    EXPECT(path(LR"(C:\a\b\c\d\e\..\f\..\..\..\g\h\..\)"sv).lexically_normal().native() == LR"(C:\a\b\g\)"sv);

    EXPECT(path(LR"(..\..\..)"sv).lexically_normal().native() == LR"(..\..\..)"sv);
    EXPECT(path(LR"(..\..\..\)"sv).lexically_normal().native() == LR"(..\..\..)"sv);

    EXPECT(path(LR"(..\..\..\a\b\c)"sv).lexically_normal().native() == LR"(..\..\..\a\b\c)"sv);

    EXPECT(path(LR"(\..\..\..)"sv).lexically_normal().native() == LR"(\)"sv);
    EXPECT(path(LR"(\..\..\..\)"sv).lexically_normal().native() == LR"(\)"sv);

    EXPECT(path(LR"(\..\..\..\a\b\c)"sv).lexically_normal().native() == LR"(\a\b\c)"sv);

    EXPECT(path(LR"(a\..)"sv).lexically_normal().native() == LR"(.)"sv);
    EXPECT(path(LR"(a\..\)"sv).lexically_normal().native() == LR"(.)"sv);

    EXPECT(path(LR"(/\server/\share/\a/\b/\c/\./\./\d/\../\../\../\../\../\../\../\other/x/y/z/.././..\meow.txt)"sv)
               .lexically_normal()
               .native()
           == LR"(\\server\other\x\meow.txt)"sv);
}

void test_lexically_relative() {
    // N4727 30.11.7.4.11 [fs.path.gen]/5
    EXPECT(path("/a/d").lexically_relative("/a/b/c") == "../../d");
    EXPECT(path("/a/b/c").lexically_relative("/a/d") == "../b/c");
    EXPECT(path("a/b/c").lexically_relative("a") == "b/c");
    EXPECT(path("a/b/c").lexically_relative("a/b/c/x/y") == "../..");
    EXPECT(path("a/b/c").lexically_relative("a/b/c") == ".");
    EXPECT(path("a/b").lexically_relative("c/d") == "../../a/b");

    EXPECT(path(LR"(/a/d)"sv).lexically_relative(LR"(/a/b/c)"sv).native() == LR"(..\..\d)"sv);
    EXPECT(path(LR"(/a/b/c)"sv).lexically_relative(LR"(/a/d)"sv).native() == LR"(..\b\c)"sv);
    EXPECT(path(LR"(a/b/c)"sv).lexically_relative(LR"(a)"sv).native() == LR"(b\c)"sv);
    EXPECT(path(LR"(a/b/c)"sv).lexically_relative(LR"(a/b/c/x/y)"sv).native() == LR"(..\..)"sv);
    EXPECT(path(LR"(a/b/c)"sv).lexically_relative(LR"(a/b/c)"sv).native() == LR"(.)"sv);
    EXPECT(path(LR"(a/b/c)"sv).lexically_relative(LR"(a/b/c/)"sv).native() == LR"(.)"sv);
    EXPECT(path(LR"(a/b)"sv).lexically_relative(LR"(c/d)"sv).native() == LR"(..\..\a\b)"sv);

    EXPECT(path(LR"()"sv).lexically_relative(LR"()"sv).native() == LR"(.)"sv);

    EXPECT(path(LR"(C:\Temp)"sv).lexically_relative(LR"(D:\Temp)"sv).native() == LR"()"sv);

    EXPECT(path(LR"(C:\Temp)"sv).lexically_relative(LR"(Temp)"sv).native() == LR"()"sv);
    EXPECT(path(LR"(Temp)"sv).lexically_relative(LR"(C:\Temp)"sv).native() == LR"()"sv);

    EXPECT(path(LR"(one)"sv).lexically_relative(LR"(\two)"sv).native() == LR"()"sv);

    EXPECT(path(LR"(cat)"sv).lexically_relative(LR"(..\..\..\meow)"sv).native() == LR"()"sv);
    EXPECT(path(LR"(cat)"sv).lexically_relative(LR"(..\..\..\meow\.\.\.\.\.)"sv).native() == LR"()"sv);

    EXPECT(path(LR"(a\b\c\x\y\z)"sv).lexically_relative(LR"(a\b\c\d\.\e\..\f\g)"sv).native() == LR"(..\..\..\x\y\z)"sv);

    EXPECT(path(LR"(a\b\c\x\y\z)"sv).lexically_relative(LR"(a\b\c\d\.\e\..\f\g\..\..\..)"sv).native() == LR"(x\y\z)"sv);

    // LWG-3070
    EXPECT(path(LR"(\a:\b:)"sv).lexically_relative(LR"(\a:\c:)"sv).native() == LR"()"sv);
}

void test_lexically_proximate() {
    EXPECT(path(LR"(/a/d)"sv).lexically_proximate(LR"(/a/b/c)"sv).native() == LR"(..\..\d)"sv);
    EXPECT(path(LR"(/a/b/c)"sv).lexically_proximate(LR"(/a/d)"sv).native() == LR"(..\b\c)"sv);
    EXPECT(path(LR"(a/b/c)"sv).lexically_proximate(LR"(a)"sv).native() == LR"(b\c)"sv);
    EXPECT(path(LR"(a/b/c)"sv).lexically_proximate(LR"(a/b/c/x/y)"sv).native() == LR"(..\..)"sv);
    EXPECT(path(LR"(a/b/c)"sv).lexically_proximate(LR"(a/b/c)"sv).native() == LR"(.)"sv);
    EXPECT(path(LR"(a/b)"sv).lexically_proximate(LR"(c/d)"sv).native() == LR"(..\..\a\b)"sv);

    EXPECT(path(LR"()"sv).lexically_proximate(LR"()"sv).native() == LR"(.)"sv);

    EXPECT(path(LR"(C:\Temp)"sv).lexically_proximate(LR"(D:\Temp)"sv).native() == LR"(C:\Temp)"sv);

    EXPECT(path(LR"(C:\Temp)"sv).lexically_proximate(LR"(Temp)"sv).native() == LR"(C:\Temp)"sv);
    EXPECT(path(LR"(Temp)"sv).lexically_proximate(LR"(C:\Temp)"sv).native() == LR"(Temp)"sv);

    EXPECT(path(LR"(one)"sv).lexically_proximate(LR"(\two)"sv).native() == LR"(one)"sv);

    EXPECT(path(LR"(cat)"sv).lexically_proximate(LR"(..\..\..\meow)"sv).native() == LR"(cat)"sv);
    EXPECT(path(LR"(cat)"sv).lexically_proximate(LR"(..\..\..\meow\.\.\.\.\.)"sv).native() == LR"(cat)"sv);

    EXPECT(
        path(LR"(a\b\c\x\y\z)"sv).lexically_proximate(LR"(a\b\c\d\.\e\..\f\g)"sv).native() == LR"(..\..\..\x\y\z)"sv);

    EXPECT(
        path(LR"(a\b\c\x\y\z)"sv).lexically_proximate(LR"(a\b\c\d\.\e\..\f\g\..\..\..)"sv).native() == LR"(x\y\z)"sv);

    // LWG-3070
    EXPECT(path(LR"(\a:\b:)"sv).lexically_proximate(LR"(\a:\c:)"sv).native() == LR"(\a:\b:)"sv);
}

void test_weakly_canonical() {
    error_code ec;

    create_directories(L"test_weakly_canonical/a/b/c"sv, ec);
    EXPECT(good(ec));

    const path meow_txt(L"test_weakly_canonical/a/b/c/meow.txt"sv);
    const wstring peppermint(L"peppermint"sv);
    create_file_containing(meow_txt, peppermint.c_str());
    EXPECT(read_file_contents(meow_txt) == peppermint);

    const path p1 = weakly_canonical(meow_txt, ec);
    EXPECT(good(ec));
    EXPECT(!meow_txt.is_absolute());
    EXPECT(p1.is_absolute());
    EXPECT(meow_txt.native() != meow_txt.lexically_normal().native());
    EXPECT(p1.native() == p1.lexically_normal().native());
    EXPECT(read_file_contents(p1) == peppermint);

    const path p2 = weakly_canonical(meow_txt);
    EXPECT(p1.native() == p2.native());

    const path woof_txt(L"test_weakly_canonical/a/b/x/y/woof.txt"sv);
    const path p3 = weakly_canonical(woof_txt, ec);
    EXPECT(good(ec));
    EXPECT(p3.lexically_relative(p1.parent_path()).native() == LR"(..\x\y\woof.txt)"sv);

    const path p4 = weakly_canonical(L"test_weakly_canonical/a/b/c/nonexistent.txt"sv);
    EXPECT(good(ec));
    EXPECT(p4.parent_path().native() == p1.parent_path().native());
    EXPECT(p4.filename().native() == L"nonexistent.txt"sv);

    // Also test VSO-850856
    // "std::filesystem::weakly_canonical should not throw an exception when a prefix is an existing file"
    const path fileWithSuffix                = meow_txt.native() + L"/";
    const path weaklyCanonicalFileWithSuffix = weakly_canonical(fileWithSuffix); // this should not throw
    assert(weaklyCanonicalFileWithSuffix.native().back() == L'\\');

    // Test relative()
    const path p5 = relative(woof_txt, meow_txt, ec);
    EXPECT(good(ec));
    EXPECT(p5.native() == LR"(..\..\x\y\woof.txt)"sv);

    const path p6 = relative(meow_txt, ec);
    EXPECT(good(ec));
    EXPECT(p6.native() == LR"(test_weakly_canonical\a\b\c\meow.txt)"sv);

    const path p7 = relative(woof_txt, ec);
    EXPECT(good(ec));
    EXPECT(p7.native() == LR"(test_weakly_canonical\a\b\x\y\woof.txt)"sv);

    const path p8 = relative(woof_txt, meow_txt);
    EXPECT(p8.native() == p5.native());

    const path p9 = relative(meow_txt);
    EXPECT(p9.native() == p6.native());

    // Test proximate()
    const path p10 = proximate(L"C:/Nonexistent/a/b/c/one.txt"sv, L"Y:/Impossible/two.txt"sv, ec);
    EXPECT(good(ec));
    EXPECT(p10.native() == LR"(C:\Nonexistent\a\b\c\one.txt)"sv);

    const path p11 = proximate(L"C:/Nonexistent/a/b/c/one.txt"sv, L"C:/Nonexistent/a/b/x/y"sv, ec);
    EXPECT(good(ec));
    EXPECT(p11.native() == LR"(..\..\c\one.txt)"sv);

    const path p12 = proximate(L"C:/Nonexistent/a/b/c/one.txt"sv, L"C:/Nonexistent/a/b/x/y"sv);
    EXPECT(p12.native() == p11.native());

    const path p13 = proximate(meow_txt, ec);
    EXPECT(good(ec));
    EXPECT(p13.native() == LR"(test_weakly_canonical\a\b\c\meow.txt)"sv);

    const path p14 = proximate(meow_txt);
    EXPECT(p14.native() == p13.native());

    if (p14.root_name().native() != L"M:"sv) {
        const path p15 = proximate(L"M:/Imaginary/three.txt"sv, ec);
        EXPECT(good(ec));
        EXPECT(p15.native() == LR"(M:\Imaginary\three.txt)"sv);
    }

    remove_all(L"test_weakly_canonical"sv, ec);
    EXPECT(good(ec));
}

void test_remove() {
    const test_temp_directory tempDir("remove"sv);
    const path& dirname = tempDir.directoryPath;
    const auto filename = dirname / L"remove_test.txt"sv;

    error_code ec;
    EXPECT(!remove(filename));
    EXPECT(!remove(filename, ec));
    EXPECT(good(ec));

    create_file_containing(filename, L"hello");

    // attempting to remove a non-empty directory should fail
    EXPECT(!remove(dirname, ec));
    EXPECT(bad(ec));

    EXPECT(remove(filename, ec));
    EXPECT(good(ec));
    EXPECT(!exists(filename, ec));
    EXPECT(good(ec));

    create_file_containing(filename, L"hello");
    permissions(filename, perms::owner_write | perms::group_write | perms::others_write, perm_options::remove);

    // remove a read-only file also
    EXPECT(remove(filename, ec));
    EXPECT(good(ec));
    EXPECT(!exists(filename, ec));
    EXPECT(good(ec));

    EXPECT(remove(dirname, ec));
    EXPECT(good(ec));
    EXPECT(!exists(dirname, ec));
    EXPECT(good(ec));

    remove(badPath); // we ignore invalid paths
    remove(badPath, ec);
    EXPECT(good(ec));
}

void test_rename() {
    const test_temp_directory tempDir("rename"sv);
    error_code ec;
    const path dir(tempDir.directoryPath / L"dir"sv);
    const path otherDir(tempDir.directoryPath / L"dirAfter"sv);
    const path fileA(tempDir.directoryPath / L"filea.txt"sv);
    const path fileB(tempDir.directoryPath / L"fileb.txt"sv);

    create_directories(dir.native(), ec);
    EXPECT(good(ec));
    create_directory(otherDir.native(), ec);
    EXPECT(good(ec));
    create_file_containing(fileA, L"hello");
    create_file_containing(fileB, L"world");

    // If old_p and new_p resolve to the same existing file, no action is taken
    rename(dir, dir, ec);
    EXPECT(good(ec));
    rename(fileA, fileA, ec);
    EXPECT(good(ec));
    EXPECT(read_file_contents(fileA) == L"hello");

    // If new_p resolves to an existing non-directory file, new_p is removed
    rename(fileA, fileB, ec);
    EXPECT(good(ec));
    EXPECT(!exists(fileA.native()));
    EXPECT(read_file_contents(fileB) == L"hello");

    // Standard rename where target doesn't exist
    rename(fileB, fileA);
    EXPECT(read_file_contents(fileA) == L"hello");

    // Bad cases
    EXPECT(throws_filesystem_error([&] { rename(dir, otherDir); }, "rename", dir, otherDir));
    rename(dir, otherDir, ec);
    EXPECT(bad(ec));
}

void test_space() {
    const auto maxValue = static_cast<uintmax_t>(-1);
    const path dir(L"test_space_dir"sv);
    const path file(dir / L"test_space_file.txt"sv);

    error_code ec;
    create_directory(dir.native(), ec);
    EXPECT(good(ec));
    create_file_containing(file, L"hello");

    space_info info = space(dir);
    EXPECT(info.available != maxValue);
    EXPECT(info.free != maxValue);
    EXPECT(info.capacity != maxValue);

    info = space(file, ec);
    EXPECT(good(ec));
    EXPECT(info.available != maxValue);
    EXPECT(info.free != maxValue);
    EXPECT(info.capacity != maxValue);

    EXPECT(throws_filesystem_error([] { return space(badPath); }, "space", badPath));
    info = space(badPath, ec);
    EXPECT(bad(ec));
    EXPECT(info.available == maxValue);
    EXPECT(info.free == maxValue);
    EXPECT(info.capacity == maxValue);

    info = space(path(), ec);
    EXPECT(bad(ec));
    EXPECT(info.available == maxValue);
    EXPECT(info.free == maxValue);
    EXPECT(info.capacity == maxValue);

    const path nonexistent(dir / L"nonexistent"sv);
    info = space(nonexistent, ec);
    EXPECT(bad(ec));
    EXPECT(info.available == maxValue);
    EXPECT(info.free == maxValue);
    EXPECT(info.capacity == maxValue);

    info = space(LR"(C:\Some\Path\That\Does\Not\Exist)", ec);
    EXPECT(bad(ec));
    EXPECT(info.available == maxValue);
    EXPECT(info.free == maxValue);
    EXPECT(info.capacity == maxValue);

    info = space(LR"(??malformed??)", ec);
    EXPECT(bad(ec));
    EXPECT(info.available == maxValue);
    EXPECT(info.free == maxValue);
    EXPECT(info.capacity == maxValue);

    // Also test VSO-732321 [Feedback] Filesystem::space() doesn't work if the parameter is a relative path
    info = space(L"."sv, ec);
    EXPECT(good(ec));
    EXPECT(info.available != maxValue);
    EXPECT(info.free != maxValue);
    EXPECT(info.capacity != maxValue);

    const path symDirPresent(dir / L"directory_symlink"sv);
    const path symDirAbsent(dir / L"broken_directory_symlink"sv);
    const path symFilePresent(dir / L"file_symlink"sv);
    const path symFileAbsent(dir / L"broken_file_symlink"sv);

    if ((create_directory_symlink(current_path() / dir, symDirPresent, ec), ec)
        || (create_directory_symlink(nonexistent, symDirAbsent, ec), ec)
        || (create_symlink(current_path() / file, symFilePresent, ec), ec)
        || (create_symlink(nonexistent, symFileAbsent, ec), ec)) {
        check_symlink_permissions(ec, L"space symlink edge cases");
    } else {
        info = space(symDirPresent, ec);
        EXPECT(good(ec));
        EXPECT(info.available != maxValue);
        EXPECT(info.free != maxValue);
        EXPECT(info.capacity != maxValue);

        info = space(symFilePresent, ec);
        EXPECT(good(ec));
        EXPECT(info.available != maxValue);
        EXPECT(info.free != maxValue);
        EXPECT(info.capacity != maxValue);

        info = space(symDirAbsent, ec);
        EXPECT(bad(ec));
        EXPECT(info.available == maxValue);
        EXPECT(info.free == maxValue);
        EXPECT(info.capacity == maxValue);

        info = space(symFileAbsent, ec);
        EXPECT(bad(ec));
        EXPECT(info.available == maxValue);
        EXPECT(info.free == maxValue);
        EXPECT(info.capacity == maxValue);
    }

    remove_all(dir, ec);
    EXPECT(good(ec));
}

void test_status_known() {
    const file_status noneStatus(file_type::none);
    const file_status dirStatus(file_type::directory);
    const file_status unknownStatus(file_type::unknown);
    EXPECT(!status_known(noneStatus));
    EXPECT(status_known(dirStatus));
    EXPECT(status_known(unknownStatus)); // we've gotten this wrong before
}

void test_permissions() {
    const test_temp_directory tempDir("permissions"sv);
    const path& dirname = tempDir.directoryPath;
    const path filename(dirname / L"file.txt"sv);
    const path linkname(dirname / L"symlink.txt"sv);

    create_file_containing(filename, L"hello world");

    error_code ec;
    EXPECT(status(dirname, ec).permissions() == perms::all);
    EXPECT(good(ec));
    EXPECT(status(filename, ec).permissions() == perms::all);
    EXPECT(good(ec));

    permissions(filename, perms::owner_write); // has no effect because some write bits still set
    EXPECT(status(filename, ec).permissions() == perms::all);
    EXPECT(good(ec));

    constexpr auto readonlyPerms = perms::owner_read | perms::owner_exec | perms::group_read | perms::group_exec
                                 | perms::others_read | perms::others_exec;
    permissions(filename, readonlyPerms, perm_options::replace);
    EXPECT(status(filename, ec).permissions() == readonlyPerms);
    EXPECT(good(ec));

    // adding any write bit removes FILE_ATTRIBUTE_READONLY
    permissions(filename, perms::owner_write, perm_options::add, ec);
    EXPECT(good(ec));
    EXPECT(status(filename, ec).permissions() == perms::all);
    EXPECT(good(ec));
    // removing any write bit but not all has no effect
    permissions(filename, perms::owner_write, perm_options::remove, ec);
    EXPECT(good(ec));
    EXPECT(status(filename, ec).permissions() == perms::all);
    EXPECT(good(ec));
    // removing all write bits sets FILE_ATTRIBUTE_READONLY
    permissions(filename, perms::owner_write | perms::group_write | perms::others_write, perm_options::remove, ec);
    EXPECT(good(ec));
    EXPECT(status(filename, ec).permissions() == readonlyPerms);
    EXPECT(good(ec));

    create_symlink(absolute(filename), linkname, ec);
    if (ec) {
        check_symlink_permissions(ec, L"permissions");
    } else {
        EXPECT(symlink_status(linkname).permissions() == perms::all);
        EXPECT(good(ec));
        // try to make the symlink readonly
        permissions(linkname, readonlyPerms, perm_options::replace | perm_options::nofollow, ec);
        EXPECT(good(ec));
        EXPECT(symlink_status(linkname).permissions() == readonlyPerms);
        EXPECT(good(ec));
        // try to make the symlink target not readonly
        permissions(linkname, perms::all, perm_options::replace, ec);
        EXPECT(good(ec));
        // symlink unchanged:
        EXPECT(symlink_status(linkname).permissions() == readonlyPerms);
        EXPECT(good(ec));
        // target changed:
        EXPECT(status(linkname).permissions() == perms::all);
        EXPECT(good(ec));
    }

    permissions(linkname, perms::all, perm_options::replace | perm_options::nofollow, ec);
    // ignore ec on purpose (symlink may not have been made and that's OK)
    permissions(filename, perms::all, ec);
    EXPECT(good(ec));
    permissions(dirname, perms::all, ec);
    EXPECT(good(ec));
}

void test_temp_directory_path() {
    (void) temp_directory_path(); // no meaningful assertions on result
    error_code ec;
    (void) temp_directory_path(ec);
    EXPECT(good(ec));

    // Test:
    // Effects: If exists(p) is false or is_directory(p) is false, an error is reported
    wchar_t* oldTemp;
    size_t unused;
    assert(_wdupenv_s(&oldTemp, &unused, L"TMP") == 0);
    assert(oldTemp);

    // exists(p) is false
    assert(_wputenv_s(L"TMP", L"nonexistent.dir") == 0);
    try {
        (void) temp_directory_path();
        EXPECT(false);
    } catch (const filesystem_error& err) {
        EXPECT(err.code() == make_error_code(errc::not_a_directory));
        const auto& p1Native = err.path1().native();
        EXPECT(p1Native.find(LR"(\nonexistent.dir\)") == p1Native.size() - 17);
        EXPECT(err.path2().empty());
    }

    const auto nonexistentTemp = temp_directory_path(ec).native();
    EXPECT(nonexistentTemp.find(LR"(\nonexistent.dir\)") == nonexistentTemp.size() - 17);
    EXPECT(ec == make_error_code(errc::not_a_directory));

    // TODO: automated test is_directory(p) is false, symlinks, after other filesystem components are implemented

    assert(_wputenv_s(L"TMP", oldTemp) == 0);
    free(oldTemp);
}

void test_create_directory() {
    const test_temp_directory tempDir("create_directory"sv);
    const path p = tempDir.directoryPath / L"__std.c++17.filesystem.create_directory"sv;

    // test happy
    {
        remove(p); // may or may not exist
        EXPECT(create_directory(p) == true); // created
        EXPECT(create_directory(p) == false); // already exists
        remove(p);
        EXPECT(create_directory(p, tempDir.directoryPath) == true); // created
        EXPECT(create_directory(p, tempDir.directoryPath) == false); // already exists
    }

    // test happy ec
    {
        remove(p);
        error_code ec;
        EXPECT(create_directory(p, ec) == true); // created
        EXPECT(good(ec));
        EXPECT(create_directory(p, ec) == false); // already exists
        EXPECT(good(ec));
        remove(p);
        EXPECT(create_directory(p, tempDir.directoryPath, ec) == true); // created
        EXPECT(good(ec));
        EXPECT(create_directory(p, tempDir.directoryPath, ec) == false); // already exists
        EXPECT(good(ec));
        remove(p);
    }

    // test invalid path
    {
        error_code ec;
        for (auto&& nonexistent : nonexistentPaths) {
            EXPECT(create_directory(nonexistent, ec) == false); // failed
            EXPECT(bad(ec));

            EXPECT(throws_filesystem_error([&] { create_directory(nonexistent); }, "create_directory", nonexistent));
        }
    }

    // test VSO-654638 where create_directory(p, existing_p) was doing copy_symlink behavior
    {
        error_code ec;
        create_directory_symlink(nonexistentPaths[0], p, ec);
        if (ec) {
            check_symlink_permissions(ec, L"test_create_directory/VSO-654638");
        } else {
            const path q = tempDir.directoryPath / L"__std.c++17.filesystem.test.target"sv;
            create_directory(q, p, ec);
            EXPECT(good(ec));
            EXPECT(!is_symlink(q));
            remove(q, ec);
            EXPECT(good(ec));
            remove(p, ec);
            EXPECT(good(ec));
        }
    }

    // test D1164R0 Make create_directory() Intuitive
    {
        error_code ec;
        create_file_containing(p, L"example");
        EXPECT(throws_filesystem_error([&p] { create_directory(p); }, "create_directory", p));
        EXPECT(!create_directory(p, ec));
        EXPECT(ec == make_error_condition(errc::file_exists));
        EXPECT(ec == error_code(183 /*ERROR_ALREADY_EXISTS*/, system_category()));
    }
}

void test_create_dirs_and_remove_all() {
    const test_temp_directory tempDir("create_dirs_and_remove_all"sv);
    const path& r = tempDir.directoryPath;

    // test long path support
    const path longRoot(LR"(\\?\)"s + r.native());
    const path longFull = longRoot / longSuffix;

    auto p = r;
    p /= L"a/b/c/d/e/f/g/h/i/j"sv;

    error_code ec;
    EXPECT(create_directories(p) == true); // created
    EXPECT(create_directories(p) == false); // already exists

    EXPECT(remove_all(r) > 0); // deleted
    EXPECT(remove_all(r) == 0); // should not exist

    EXPECT(create_directories(p, ec) == true); // created
    EXPECT(good(ec));
    EXPECT(create_directories(p, ec) == false); // already exists
    EXPECT(good(ec));
    remove_all(r);

    EXPECT(throws_filesystem_error([] { create_directories(badPath); }, "create_directories", badPath));
    create_directories(badPath, ec);
    EXPECT(bad(ec));

    remove_all(badPath); // we ignore invalid paths as in remove
    remove_all(badPath, ec);
    EXPECT(good(ec));

    // test GH-1283 create_directories() should throw for empty paths
    EXPECT(throws_filesystem_error([] { create_directories(path{}); }, "create_directories", path{}));
    EXPECT(create_directories(path{}, ec) == false);
    EXPECT(bad(ec));

    // test that normalization isn't done first
    auto dots = r / L"a/../b/../c"sv;
    EXPECT(create_directories(dots));
    EXPECT(is_directory(r / L"a"));
    EXPECT(is_directory(r / L"b"));
    EXPECT(is_directory(r / L"c"));

    EXPECT(create_directories(longFull, ec));
    EXPECT(good(ec));

    remove_all(longRoot, ec);
    EXPECT(good(ec));

    // test D1164R0 Make create_directory() Intuitive
    {
        auto sub = r / L"a"sv;
        // non-last directory:
        create_file_containing(r, L"example");
        EXPECT(throws_filesystem_error([&sub] { create_directories(sub); }, "create_directories", sub));
        EXPECT(!create_directories(p, ec));
        EXPECT(ec == make_error_condition(errc::file_exists));
        EXPECT(ec == error_code(183 /*ERROR_ALREADY_EXISTS*/, system_category()));
        remove(r);
        // last directory:
        create_directory(r);
        create_file_containing(sub, L"example");
        EXPECT(throws_filesystem_error([&sub] { create_directories(sub); }, "create_directories", sub));
        EXPECT(!create_directories(p, ec));
        EXPECT(ec == make_error_condition(errc::file_exists));
        EXPECT(ec == error_code(183 /*ERROR_ALREADY_EXISTS*/, system_category()));
        remove_all(r);
    }
}

void test_symlink_status() {
    const test_temp_directory tempDir("symlink_status"sv);
    const path tempFile = tempDir.directoryPath / L"example.txt";
    create_file_containing(tempFile, L"hello symlink world");

    {
        error_code ec;
        auto ft = symlink_status(tempFile, ec);
        EXPECT(good(ec));
        EXPECT(ft.type() == file_type::regular);
        EXPECT(ft.permissions() == perms{0555} || ft.permissions() == perms::all);

        auto ft2 = symlink_status(tempFile);
        EXPECT(ft.type() == ft2.type());
        EXPECT(ft.permissions() == ft2.permissions());
    }

    {
        error_code ec;
        auto ft = symlink_status(tempDir.directoryPath, ec);
        EXPECT(good(ec));
        EXPECT(ft.type() == file_type::directory);
        EXPECT(ft.permissions() == perms{0555} || ft.permissions() == perms::all);

        auto ft2 = symlink_status(tempDir.directoryPath);
        EXPECT(ft.type() == ft2.type());
        EXPECT(ft.permissions() == ft2.permissions());
    }

    for (auto&& nonexistent : nonexistentPaths) {
        error_code ec;
        auto ft = symlink_status(nonexistent, ec);
        EXPECT(bad(ec));
        EXPECT(ft.type() == file_type::not_found);
        EXPECT(ft.permissions() == perms::unknown);

        auto ft2 = symlink_status(nonexistent);
        EXPECT(ft.type() == ft2.type());
        EXPECT(ft.permissions() == ft2.permissions());
    }
}

void test_current_path(const path& expected) {

    // Getter
    {
        error_code ec;
        const auto actual = current_path(ec);
        EXPECT(good(ec));
        EXPECT(expected == actual);
    }

    {
        const auto actual = current_path();
        EXPECT(expected == actual);
    }

    // Setter
    {
        // We are changing current dir at the start of the test, so that is a success test case
        // Dealing with a non-existent dir here
        error_code ec;
        const path non_existent_path = LR"(Z:\nonexistent.dir)";
        current_path(non_existent_path, ec);
        EXPECT(bad(ec));
        // Make sure the previous call didn't change dir
        const auto actual = current_path(ec);
        EXPECT(good(ec));
        EXPECT(expected == actual);
    }

    {
        const path non_existent_path = LR"(Z:\nonexistent.dir)";
        EXPECT(throws_filesystem_error(
            [&]() { current_path(non_existent_path); }, "current_path", non_existent_path.native()));
    }
}

void test_file_time_type() {
    constexpr long long ticks_per_second = 10'000'000LL;
    constexpr long long epoch            = 0x19DB1DED53E8000LL;

    // 24h tolerance
    constexpr long long tolerance = duration_cast<seconds>(24h).count() * ticks_per_second;

    const auto file_time_now             = file_time_type::clock::now();
    const long long file_time_tick_count = file_time_now.time_since_epoch().count() - epoch;

    const auto system_clock_now             = system_clock::now();
    const long long system_clock_tick_count = system_clock_now.time_since_epoch().count();

    if (file_time_tick_count + tolerance < system_clock_tick_count
        || file_time_tick_count - tolerance > system_clock_tick_count) {
        wcerr << L"test_file_time_type failed: " << file_time_tick_count << L" ticks too different from "
              << system_clock_tick_count << L" ticks from system_clock\n";
        pass = false;
    }
}

template <typename DirIter>
void interactive_dir_iter(wstring_view p) {
    wcerr << L"iterate over: \"" << p << L"\":\n";

    directory_options opts = {};
    // ! => skip_permission_denied
    // ~ => follow_directory_symlink

    while (!p.empty()) {
        const auto ch = p.front();
        if (ch == L'!') {
            opts |= directory_options::skip_permission_denied;
        } else if (ch == L'~') {
            opts |= directory_options::follow_directory_symlink;
        } else {
            break;
        }
        p.remove_prefix(1);
    }

    for (const auto& entry : DirIter(p, opts)) {
        wcerr << entry.path().native() << L'\n';
    }
    wcerr << L"---- iteration complete -----\n";
}

template <typename Elem, typename Traits>
basic_ostream<Elem, Traits>& operator<<(basic_ostream<Elem, Traits>& ostr, const file_type ft) {
    static constexpr array<wstring_view, 11> names{{L"none"sv, L"not_found"sv, L"regular"sv, L"directory"sv,
        L"symlink"sv, L"block"sv, L"character"sv, L"fifo"sv, L"socket"sv, L"unknown"sv, L"junction"sv}};

    const size_t index = static_cast<size_t>(ft);
    if (!EXPECT(index < names.size())) {
        return ostr << L"!!! INVALID file_type(" << index << L") !!!!";
    }

    return ostr << L"file_type::" << names[index];
}

template <typename Elem, typename Traits>
basic_ostream<Elem, Traits>& operator<<(basic_ostream<Elem, Traits>& ostr, const perms p) {
    if (p == perms::owner_all) {
        return ostr << L"perms::owner_all";
    }
    if (p == perms::owner_read) {
        return ostr << L"perms::owner_read";
    }
    return ostr << L"perms{" << hex << static_cast<unsigned int>(p) << L"}";
}

template <typename Elem, typename Traits>
basic_ostream<Elem, Traits>& operator<<(basic_ostream<Elem, Traits>& str, const file_status& status) {
    return str << status.type() << L' ' << status.permissions();
}

template <typename Elem, typename Traits>
basic_ostream<Elem, Traits>& operator<<(basic_ostream<Elem, Traits>& str, const directory_entry& de) {
    return str << L"\n    symlink_status: " << de.symlink_status() << L"\n             status: " << de.status()
               << L"\n               size: " << de.file_size() << L"\n    last_write_time: "
               << de.last_write_time().time_since_epoch().count() << L"\n    hard_link_count: " << de.hard_link_count();
}

void run_interactive_tests(int argc, wchar_t* argv[]) {
    wstring_view the_rest;
    const auto starts_with = [&](wstring_view arg, wstring_view prefix) {
        if (::starts_with(arg, prefix)) {
            the_rest = arg.substr(prefix.size());
            return true;
        }
        return false;
    };

    constexpr wstring_view usage = L"Usage: test [-recdir:<path>] [-dir:<path>] [-lstat:<path>]"
                                   L" [-stat:<path>] [-de:<path>] [-mkdir:<path>] [-mkdirs:<path>]"
                                   L" [-now] [-rm:<path>] [-rmall:<path>] [-sz:<path>]"
                                   L"\n"sv;
    for (int i = 1; i < argc; ++i) {
        const wstring_view arg = argv[i];
        if (arg == L"-?"sv) {
            wcerr << usage;
        } else if (starts_with(arg, L"-recdir:"sv)) {
            interactive_dir_iter<recursive_directory_iterator>(the_rest);
        } else if (starts_with(arg, L"-dir:"sv)) {
            interactive_dir_iter<directory_iterator>(the_rest);
        } else if (starts_with(arg, L"-lstat:"sv)) {
            wcerr << quoted(arg) << L" => " << symlink_status(the_rest) << "\n";
        } else if (starts_with(arg, L"-stat:"sv)) {
            wcerr << quoted(arg) << L" => " << status(the_rest) << "\n";
        } else if (starts_with(arg, L"-de:"sv)) {
            wcerr << quoted(arg) << L" => " << directory_entry(the_rest) << "\n";
        } else if (starts_with(arg, L"-mkdir:"sv)) {
            wcerr << L"create_directory => " << create_directory(the_rest) << "\n";
        } else if (starts_with(arg, L"-mkdirs:"sv)) {
            wcerr << L"create_directory => " << create_directories(the_rest) << "\n";
        } else if (starts_with(arg, L"-now"sv)) {
            wcerr << L"         system_clock: " << system_clock::now().time_since_epoch().count() << L'\n';
            wcerr << L"file_time_type::clock: " << file_time_type::clock::now().time_since_epoch().count() << L'\n';
        } else if (starts_with(arg, L"-rm:"sv)) {
            wcerr << L"remove => " << remove(the_rest) << "\n";
        } else if (starts_with(arg, L"-rmall:"sv)) {
            wcerr << L"remove_all => " << remove_all(the_rest) << "\n";
        } else if (starts_with(arg, L"-sz:"sv)) {
            wcerr << L"file_size => " << file_size(the_rest) << "\n";
        } else {
            wcerr << usage;
        }
    }
}

// Also test DevCom-953628, which witnessed a failure of filesystem::path's "Source" constructor to SFINAE away when
// given a non-iterator argument for which iter_value_t is valid when defining _Iter_value_t to iter_value_t in concepts
// mode.
void test_devcom_953628() { // COMPILE-ONLY
    struct S : wstring {};
    path{S{}};
}

int wmain(int argc, wchar_t* argv[]) {
    error_code ec;

    // Store old path and change current path to a temporary path
    const auto old_current_path = current_path(ec);
    EXPECT(good(ec));
    const test_temp_directory tempDir("P0218R1_filesystem"sv);
    current_path(tempDir.directoryPath, ec);
    EXPECT(good(ec));


    if (argc > 1) {
        run_interactive_tests(argc, argv);
        return 0; // not a PM_ constant because the caller isn't run.pl here
    }

    for (const auto& testCase : decompTestCases) {
        pass = pass && run_decomp_test_case(testCase);
    }

    for (const auto& testCase : stemTestCases) {
        pass = pass && run_stem_test_case(testCase);
    }

    for (const auto& testCase : compareTestCases) {
        pass = pass && run_compare_test_case(testCase);
    }

    for (const auto& testCase : slashTestCases) {
        pass = pass && run_slash_test_case(testCase);
    }

    test_iterators();

    test_other_path_interface();

    test_remove_filename_and_sep();

    test_filesystem_error();

    test_file_status();

    test_temp_directory_path();

    test_directory_entry();

    test_directory_iterator();

    test_recursive_directory_iterator();

    test_absolute();

    test_canonical();

    test_create_symlink_cleanup();

    test_create_directory_symlink();

    test_create_hard_link();

    test_create_symlink();

    test_read_symlink();

    test_copy();

    test_copy_file();

    test_copy_symlink();

    test_conversions();

    test_file_size();

    test_last_write_time();

    test_invalid_conversions();

    test_status();

    test_locale_conversions();

    test_lexically_normal();

    test_lexically_relative();

    test_lexically_proximate();

    test_weakly_canonical();

    test_equivalent();

    test_file_size_and_resize();

    test_hard_link_count();

    test_is_empty();

    test_last_write_time();

    test_permissions();

    test_remove();

    test_create_directory();

    test_rename();

    test_space();

    test_status_known();

    test_create_dirs_and_remove_all();

    test_symlink_status();

    test_current_path(tempDir.directoryPath);

    test_file_time_type();

    // Restore old current path and cleanup
    current_path(old_current_path, ec);
    EXPECT(good(ec));

    assert(pass);
}
