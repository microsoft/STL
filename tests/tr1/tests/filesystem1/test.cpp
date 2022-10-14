// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <experimental/filesystem>
#define TEST_NAME "<experimental/filesystem>"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "tdefs.h"
#include <experimental/filesystem>
#include <fstream>
#include <sstream>

#define TMP_NAME(suf)         "tmp_name" suf
#define CHECK_TBL(str, field) CHECK_STRING(native_to_char(str), fix_bslash(STD string(field)))

#define PFX STD experimental::filesystem::

#define GAMMA16 01623
#define GAMMA8  "\316\223"

#define ALT(x, y)           y
#define NEVER_HAS_ROOT_NAME false
#define ROOT_NAME           "c:"
#define PREFSLASH           "\\"

#define BSLASH "\\"
#define SLASH  "/"

typedef PFX path::string_type path_string_type;
typedef PFX path::value_type path_value_type;

STD string fix_bslash(const STD string& tbl_str) { // convert table entry to preferred
    STD string str;
    for (size_t idx = 0; idx < tbl_str.size(); ++idx) {
        if (tbl_str[idx] == '/' || tbl_str[idx] == '\\') {
            str.append((size_t) 1, BSLASH[0]);
        } else {
            str.append((size_t) 1, tbl_str[idx]);
        }
    }
    return str;
}

STD string native_to_char(const path_string_type& path_str) { // convert native to string
    STD string str;
    for (size_t idx = 0; idx < path_str.size(); ++idx) {
        str.append((size_t) 1, (char) path_str[idx]);
    }
    return str;
}

struct Path_data {
    const char* pd_path;
    const char* pd_fields;
    const char* pd_string;
    const char* pd_file_string;
    const char* pd_root_path_string;
    const char* pd_root_name;
    const char* pd_root_directory;
    const char* pd_relative_path_string;
    const char* pd_branch_path_string;
    const char* pd_leaf;
    const char* pd_remove_leaf_string;
};

Path_data path_data[] = { // path data examples from N1975
    {"", "", "", "", "", "", "", "", "", "", ""}, {".", ".", ".", ".", "", "", "", ".", "", ".", "."},
    {"..", "..", "..", "..", "", "", "", "..", "", "..", ".."},
    {"cat", "cat", "cat", "cat", "", "", "", "cat", "", "cat", "cat"},

    {"/", "/", "/", ALT("/", "\\"), "/", "", "/", "", "", "/", "/"},
    {"/cat", "/|cat", "/cat", ALT("/cat", "\\cat"), "/", "", "/", "cat", "/", "cat", "/"},
    {"cat/", "cat|.", "cat/", ALT("cat/", "cat\\"), "", "", "", "cat/", "cat", ".", "cat"},
    {"/cat/", "/|cat|.", "/cat/", ALT("/cat/", "\\cat\\"), "/", "", "/", "cat/", "/cat", ".", "/cat"},

    {"cat/dog", "cat|dog", "cat/dog", ALT("cat/dog", "cat\\dog"), "", "", "", "cat/dog", "cat", "dog", "cat"},
    {"/cat/dog", "/|cat|dog", "/cat/dog", ALT("/cat/dog", "\\cat\\dog"), "/", "", "/", "cat/dog", "/cat", "dog",
        "/cat"},
    {"///cat///", "/|cat|.", "///cat///", ALT("///cat///", "\\cat\\\\\\"), "/", "", "/", "cat///", "/cat", ".",
        "///cat"},
    {"///cat///dog", "/|cat|dog", "///cat///dog", ALT("///cat///dog", "\\cat\\\\\\dog"), "/", "", "/", "cat///dog",
        "/cat", "dog", "///cat"},

    {"/.", "/|.", "/.", ALT("/.", "\\."), "/", "", "/", ".", "/", ".", "/"},
    {"./", ".|.", "./", ALT("./", ".\\"), "", "", "", "./", ".", ".", "."},
    {"/..", "/|..", "/..", ALT("/..", "\\.."), "/", "", "/", "..", "/", "..", "/"},
    {"../", "..|.", "../", ALT("../", "..\\"), "", "", "", "../", "..", ".", ".."},

    {"cat/.", "cat|.", "cat/.", ALT("cat/.", "cat\\."), "", "", "", "cat/.", "cat", ".", "cat"},
    {"cat/..", "cat|..", "cat/..", ALT("cat/..", "cat\\.."), "", "", "", "cat/..", "cat", "..", "cat"},
    {"cat/./", "cat|.|.", "cat/./", ALT("cat/./", "cat\\.\\"), "", "", "", "cat/./", "cat/.", ".", "cat/."},
    {"cat/./dog", "cat|.|dog", "cat/./dog", ALT("cat/./dog", "cat\\.\\dog"), "", "", "", "cat/./dog", "cat/.", "dog",
        "cat/."},
    {"cat/..", "cat|..", "cat/..", ALT("cat/..", "cat\\.."), "", "", "", "cat/..", "cat", "..", "cat"},
    {"cat/../", "cat|..|.", "cat/../", ALT("cat/../", "cat\\..\\"), "", "", "", "cat/../", "cat/..", ".", "cat/.."},
    {"cat/../dog", "cat|..|dog", "cat/../dog", ALT("cat/../dog", "cat\\..\\dog"), "", "", "", "cat/../dog", "cat/..",
        "dog", "cat/.."},

    {"c:", "c:", "c:", "c:", ALT("", "c:"), ALT("", "c:"), "", ALT("c:", ""), "", "c:", ALT("c:", "c:")},
    {"c:/", ALT("c:|.", "c:|/"), "c:/", ALT("c:/", "c:\\"), ALT("", "c:/"), ALT("", "c:"), ALT("", "/"), ALT("c:/", ""),
        "c:", ALT(".", "/"), ALT("c:", "c:/")},
    {"c:cat", ALT("c:cat", "c:|cat"), "c:cat", "c:cat", ALT("", "c:"), ALT("", "c:"), "", ALT("c:cat", "cat"),
        ALT("", "c:"), ALT("c:cat", "cat"), ALT("c:cat", "c:")},
    {"c:/cat", ALT("c:|cat", "c:|/|cat"), "c:/cat", ALT("c:/cat", "c:\\cat"), ALT("", "c:/"), ALT("", "c:"),
        ALT("", "/"), ALT("c:/cat", "cat"), ALT("c:", "c:/"), "cat", ALT("c:", "c:/")},
    {"c:cat/", ALT("c:cat|.", "c:|cat|."), "c:cat/", ALT("c:cat/", "c:cat\\"), ALT("", "c:"), ALT("", "c:"), "",
        ALT("c:cat/", "cat/"), "c:cat", ".", "c:cat"},
    {"c:/cat/", ALT("c:|cat|.", "c:|/|cat|."), "c:/cat/", ALT("c:/cat/", "c:\\cat\\"), ALT("", "c:/"), ALT("", "c:"),
        ALT("", "/"), ALT("c:/cat/", "cat/"), "c:/cat", ".", "c:/cat"},
    {"c:/cat/dog", ALT("c:|cat|dog", "c:|/|cat|dog"), "c:/cat/dog", ALT("c:/cat/dog", "c:\\cat\\dog"), ALT("", "c:/"),
        ALT("", "c:"), ALT("", "/"), ALT("c:/cat/dog", "cat/dog"), "c:/cat", "dog", "c:/cat"},

    {"prn:", "prn:", "prn:", "prn:", ALT("", "prn:"), ALT("", "prn:"), "", ALT("prn:", ""), "", "prn:", "prn:"},

    {"c:\\", ALT("c:\\", "c:|/"), ALT("c:\\", "c:/"), "c:\\", ALT("", "c:/"), ALT("", "c:"), ALT("", "/"),
        ALT("c:\\", ""), ALT("", "c:"), ALT("c:\\", "/"), ALT("c:\\", "c:/")},
    {"c:cat", ALT("c:cat", "c:|cat"), "c:cat", "c:cat", ALT("", "c:"), ALT("", "c:"), "", ALT("c:cat", "cat"),
        ALT("", "c:"), ALT("c:cat", "cat"), ALT("c:cat", "c:")},
    {"c:\\cat", ALT("c:\\cat", "c:|/|cat"), ALT("c:\\cat", "c:/cat"), "c:\\cat", ALT("", "c:/"), ALT("", "c:"),
        ALT("", "/"), ALT("c:\\cat", "cat"), ALT("", "c:/"), ALT("c:\\cat", "cat"), ALT("c:\\cat", "c:/")},
    {"c:cat\\", ALT("c:cat\\", "c:|cat|."), ALT("c:cat\\", "c:cat/"), "c:cat\\", ALT("", "c:"), ALT("", "c:"), "",
        ALT("c:cat\\", "cat/"), ALT("", "c:cat"), ALT("c:cat\\", "."), ALT("c:cat\\", "c:cat")},
    {"c:\\cat\\", ALT("c:\\cat\\", "c:|/|cat|."), ALT("c:\\cat\\", "c:/cat/"), "c:\\cat\\", ALT("", "c:/"),
        ALT("", "c:"), ALT("", "/"), ALT("c:\\cat\\", "cat/"), ALT("", "c:/cat"), ALT("c:\\cat\\", "."),
        ALT("c:\\cat\\", "c:/cat")},
    {"c:\\cat/", ALT("c:\\cat|.", "c:|/|cat|."), ALT("c:\\cat/", "c:/cat/"), ALT("c:\\cat/", "c:\\cat\\"),
        ALT("", "c:/"), ALT("", "c:"), ALT("", "/"), ALT("c:\\cat/", "cat/"), ALT("c:\\cat", "c:/cat"), ".",
        ALT("c:\\cat", "c:/cat")},
    {"c:/cat\\dog", ALT("c:|cat\\dog", "c:|/|cat|dog"), ALT("c:/cat\\dog", "c:/cat/dog"),
        ALT("c:/cat\\dog", "c:\\cat\\dog"), ALT("", "c:/"), ALT("", "c:"), ALT("", "/"), ALT("c:/cat\\dog", "cat/dog"),
        ALT("c:", "c:/cat"), ALT("cat\\dog", "dog"), ALT("c:", "c:/cat")},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}};

void test_path_data() { // test path accesses against data tables
    const Path_data* pd              = &path_data[0];
    static PFX path::value_type psep = PFX path::preferred_separator;

    CHECK_INT(psep, PREFSLASH[0]);
    for (; pd->pd_path != nullptr; ++pd) { // test a sample path
        path_string_type str;
        PFX path p(fix_bslash(pd->pd_path));
        PFX path path2;

        str.clear();
        PFX path::iterator pi = p.begin();
        for (int i = 0; i < 10 && pi != p.end(); ++i, ++pi) {
            if (str.empty()) {
                str.append(*pi);
            } else { // append with separator
                str.append(1, '|');
                str.append(*pi);
            }
        }
        CHECK_TBL(str, pd->pd_fields);

        str.clear();
        pi = p.end();
        if (pi != p.begin()) {
            for (int i = 0; i < 10; ++i) { // prepend an element
                --pi;
                if (str.empty()) {
                    str.append(*pi);
                } else { // prepend with separator
                    str.insert((size_t) 0, (size_t) 1, '|');
                    str.insert((size_t) 0, *pi);
                }

                if (pi == p.begin()) {
                    break;
                }
            }
        }
        CHECK_TBL(str, pd->pd_fields);

        str = p.native();
        CHECK_TBL(str, pd->pd_string);

        path2 = p.root_path();
        str   = path2.native();
        CHECK_TBL(str, pd->pd_root_path_string);

        str = p.root_name();
        CHECK_TBL(str, pd->pd_root_name);

        str = p.root_directory();
        CHECK_TBL(str, pd->pd_root_directory);

        path2 = p.relative_path();
        str   = path2.native();
        CHECK_TBL(str, pd->pd_relative_path_string);

        path2 = p.parent_path();
        str   = path2.native();
        CHECK_TBL(str, pd->pd_branch_path_string);

        str = p.filename();
        CHECK_TBL(str, pd->pd_leaf);

        path2 = p;
        CHECK_PTR(&path2.remove_filename(), &path2);
        str = path2.native();
        CHECK_TBL(str, pd->pd_remove_leaf_string);
    }
}

void test_path() { // test struct path
    // test constructors, assignments, etc.
    STD string str("abc");
    PFX path p(str);
    CHECK(p == PFX path(str.c_str()));
    CHECK(p == PFX path(str.begin(), str.end()));

    p = STD string("def");
    CHECK(p != PFX path(str));
    CHECK(p == PFX path("def"));
    p = "ghi";
    CHECK(p == PFX path("ghi"));
    p.assign(str.begin(), str.end());
    CHECK(p == PFX path(str));

    p = "jkl";
    CHECK(p == PFX path("jkl"));
    p.assign("mno");
    CHECK(p == PFX path("mno"));

    { // test basic constructors
        PFX path p0;

        char arr[] = "abc";
        PFX path p1(arr);
        CHECK(p1 == PFX path("abc"));
    }

    { // test wstring constructor
        STD wstring str0(L"abc");
        PFX path p0(str0);
        CHECK(p0 == PFX path(str0.c_str()));
        CHECK(p0 == PFX path(str0.begin(), str0.end()));
    }

    { // test copies, moves
        PFX path p1(str);
        PFX path p2(p1);
        CHECK(p2 == PFX path(str));
        p1 = "def";
        p2 = p1;
        CHECK(p2 == PFX path("def"));

        PFX path::iterator pi1(p1.begin());
        PFX path p1x = *pi1;
        CHECK(p1x == PFX path("def"));
        p1  = "abc";
        pi1 = p1.begin();
        CHECK(*pi1 == PFX path("abc"));

        PFX path p3(STD move(p2));
        CHECK(p3 == PFX path("def"));
        p3 = STD move(p1);
        CHECK(p3 == PFX path(str));
    }

    p = "abc";
    p /= p;
    CHECK(p == PFX path("abc/abc"));
    p /= str;
    CHECK(p == PFX path("abc/abc/abc"));
    p /= "def";
    CHECK(p == PFX path("abc/abc/abc/def"));
    p.append(str.begin(), str.end());
    CHECK(p == PFX path("abc/abc/abc/def/abc"));
    p.append("xyz");
    CHECK(p == PFX path("abc/abc/abc/def/abc/xyz"));

    p = "abc";
    p += p;
    CHECK(p == PFX path("abcabc"));
    p += PFX path(str);
    CHECK(p == PFX path("abcabcabc"));
    p += "d";
    CHECK(p == PFX path("abcabcabcd"));
    p += 'e';
    CHECK(p == PFX path("abcabcabcde"));
    p += L'f';
    CHECK(p == PFX path("abcabcabcdef"));

    p.concat(str.begin(), str.end());
    CHECK(p == PFX path("abcabcabcdefabc"));
    p.concat("xyz");
    CHECK(p == PFX path("abcabcabcdefabcxyz"));
    p.concat(STD wstring(L"XYZ"));
    CHECK(p == PFX path("abcabcabcdefabcxyzXYZ"));

    // test path modifiers
    p.clear();
    CHECK(p == PFX path(""));

    p = PFX path("a/b");
    p.make_preferred();
    CHECK_STR(p.string().c_str(), "a" PREFSLASH "b");

    p = PFX path("a/b");
    p.remove_filename();
    CHECK_STR(p.string().c_str(), "a");

    p = PFX path("a/b");
    p.replace_filename(PFX path("c"));
    CHECK_STR(p.string().c_str(), PFX path("a/c").string().c_str());

    p = PFX path("a/b.bat");
    p.replace_extension(PFX path("exe"));
    CHECK_STR(p.string().c_str(), PFX path("a/b.exe").string().c_str());

    p.clear();
    PFX path path2(str);
    p.swap(path2);

    // test native format observers
    CHECK(p.native()[0] == 'a');
    CHECK(p.c_str()[0] == 'a');
    CHECK(((PFX path::string_type) p)[0] == 'a');
    CHECK((p.string<wchar_t, STD char_traits<wchar_t>, STD allocator<wchar_t>>()[0] == L'a'));
    CHECK(p.string()[0] == 'a');
    CHECK(p.wstring()[0] == L'a');
    CHECK(p.u8string()[0] == 'a');

    // test generic format observers
    CHECK((p.generic_string<wchar_t, STD char_traits<wchar_t>, STD allocator<wchar_t>>()[0] == L'a'));
    CHECK(p.generic_string()[0] == 'a');
    CHECK(p.generic_wstring()[0] == L'a');

    CHECK(p.generic_u8string()[0] == 'a');

    // test compare
    CHECK(PFX path("abc").compare(PFX path("abd")) < 0);
    CHECK(PFX path("abc").compare(PFX path("abc")) == 0);
    CHECK(PFX path("abc").compare(PFX path("abb")) > 0);

    // test decomposition
    PFX path tp = "/abc/def.ext";
    CHECK(PFX path(ROOT_NAME).root_name() == PFX path(ROOT_NAME));
    CHECK(tp.root_directory() == PFX path("/"));
    CHECK(tp.root_path() == PFX path("/"));
    CHECK(tp.relative_path() == PFX path("abc/def.ext"));
    CHECK(tp.parent_path() == PFX path("/abc"));
    CHECK(tp.filename() == PFX path("def.ext"));
    CHECK(tp.stem() == PFX path("def"));
    CHECK(tp.extension() == PFX path(".ext"));

    // test queries
    CHECK(PFX path().empty() && !PFX path("x").empty());
    CHECK((PFX path(ROOT_NAME).has_root_name() || NEVER_HAS_ROOT_NAME) && !PFX path("x").has_root_name());
    CHECK(PFX path("/").has_root_directory() && !PFX path("x").has_root_directory());
    CHECK(PFX path("x").has_relative_path() && !PFX path("/").has_relative_path());
    CHECK(PFX path("/x").has_parent_path() && !PFX path("x").has_parent_path());
    CHECK(PFX path("/x").has_filename() && !PFX path("").has_filename());
    CHECK(PFX path("x.abc").has_extension() && !PFX path("x").has_extension());

    CHECK(PFX path("c:/abc").is_absolute());
    CHECK(!PFX path("/abc").is_absolute());
    CHECK(PFX path("/abc").is_relative());
    CHECK(!PFX path("c:/abc").is_relative());

    { // test path iterators
        PFX path pit("abc/def");
        PFX path::iterator it = pit.begin();
        CHECK_INT(*it->c_str(), 'a');
        CHECK_INT(*(++it)->c_str(), 'd');
        CHECK(++it == pit.end());
    }

    // test path non-member functions
    CHECK(p == PFX path("abc"));
    CHECK(path2 == PFX path());
    STD swap(p, path2);
    CHECK(p == PFX path());
    CHECK(path2 == PFX path("abc"));

    CHECK_INT(PFX hash_value(path2), PFX hash_value(PFX path("abc")));

    CHECK(PFX path("abc") == PFX path("abc") && !(PFX path("abc") == PFX path("def")));
    CHECK(PFX path("abc") != PFX path("def") && !(PFX path("abc") != PFX path("abc")));
    CHECK(PFX path("abc") < PFX path("def") && !(PFX path("abc") < PFX path("abc")));
    CHECK(PFX path("abc") >= PFX path("abc") && !(PFX path("abc") >= PFX path("def")));
    CHECK(PFX path("def") > PFX path("abc") && !(PFX path("abc") > PFX path("def")));
    CHECK(PFX path("abc") <= PFX path("abc") && !(PFX path("def") <= PFX path("abc")));

    STD basic_istringstream<path_value_type> istr(PFX path("xyz").native());
    istr >> p;
    CHECK(p == PFX path("xyz"));

    STD basic_ostringstream<path_value_type> ostr;
    ostr << p;
    CHECK(p == PFX path(ostr.str()));

    { // test network prefix
        STD string netstr("\\\\server\\directory\\filename.txt");
        PFX path p0(netstr);
        if (p0.has_root_name()) { // skip for Posix
            CHECK_STRING(native_to_char(p0.native()), STD string("\\\\server\\directory\\filename.txt"));
            CHECK_STRING(native_to_char(p0.root_name()), STD string("\\\\server"));
            CHECK_STRING(native_to_char(p0.make_preferred()), netstr);
        }
    }
}

void test_errors() { // test error codes
    STD error_code ec(STD make_error_code(STD errc::operation_not_permitted));

    PFX filesystem_error be0("message0");
    CHECK(CSTD strstr(be0.what(), "message0") != nullptr);
    CHECK(be0.code() == ec);
    CHECK_STR(be0.path1().string().c_str(), "");
    CHECK_STR(be0.path2().string().c_str(), "");

    PFX filesystem_error be1("message1", STD error_code(ec));
    CHECK(CSTD strstr(be1.what(), "message1") != nullptr);
    CHECK(be1.code() == ec);
    CHECK_STR(be1.path1().string().c_str(), "");
    CHECK_STR(be1.path2().string().c_str(), "");

    PFX filesystem_error be2("message2", PFX path("abc"), STD error_code());
    CHECK(CSTD strstr(be2.what(), "message2") != nullptr);
    CHECK(!be2.code());
    CHECK_STR(be2.path1().string().c_str(), "abc");
    CHECK_STR(be2.path2().string().c_str(), "");

    PFX filesystem_error be3("message3", PFX path("abc"), PFX path("def"), STD error_code(ec));
    CHECK(CSTD strstr(be3.what(), "message3") != nullptr);
    CHECK(be3.code() == ec);
    CHECK_STR(be3.path1().string().c_str(), "abc");
    CHECK_STR(be3.path2().string().c_str(), "def");
}

void test_enums() { // test enumerations
    CHECK_INT((int) PFX file_type::not_found, -1);
    CHECK_INT((int) PFX file_type::none, 0);
    CHECK_INT((int) PFX file_type::regular, 1);
    CHECK_INT((int) PFX file_type::directory, 2);
    CHECK_INT((int) PFX file_type::symlink, 3);
    CHECK_INT((int) PFX file_type::block, 4);
    CHECK_INT((int) PFX file_type::character, 5);
    CHECK_INT((int) PFX file_type::fifo, 6);
    CHECK_INT((int) PFX file_type::socket, 7);
    CHECK_INT((int) PFX file_type::unknown, 8);

    CHECK_INT((int) PFX copy_options::none, 0);
    CHECK_INT((int) PFX copy_options::skip_existing, 1);
    CHECK_INT((int) PFX copy_options::overwrite_existing, 2);
    CHECK_INT((int) PFX copy_options::update_existing, 4);
    CHECK_INT((int) PFX copy_options::recursive, 8);
    CHECK_INT((int) PFX copy_options::copy_symlinks, 16);
    CHECK_INT((int) PFX copy_options::skip_symlinks, 32);
    CHECK_INT((int) PFX copy_options::directories_only, 64);
    CHECK_INT((int) PFX copy_options::create_symlinks, 128);
    CHECK_INT((int) PFX copy_options::create_hard_links, 256);

    CHECK_INT((int) PFX perms::none, 0);
    CHECK_INT((int) PFX perms::owner_read, 0400);
    CHECK_INT((int) PFX perms::owner_write, 0200);
    CHECK_INT((int) PFX perms::owner_exec, 0100);
    CHECK_INT((int) PFX perms::owner_all, 0700);
    CHECK_INT((int) PFX perms::group_read, 0040);
    CHECK_INT((int) PFX perms::group_write, 0020);
    CHECK_INT((int) PFX perms::group_exec, 0010);
    CHECK_INT((int) PFX perms::group_all, 0070);
    CHECK_INT((int) PFX perms::others_read, 0004);
    CHECK_INT((int) PFX perms::others_write, 0002);
    CHECK_INT((int) PFX perms::others_exec, 0001);
    CHECK_INT((int) PFX perms::others_all, 0007);
    CHECK_INT((int) PFX perms::all, 0777);
    CHECK_INT((int) PFX perms::set_uid, 04000);
    CHECK_INT((int) PFX perms::set_gid, 02000);
    CHECK_INT((int) PFX perms::sticky_bit, 01000);
    CHECK_INT((int) PFX perms::mask, 07777);
    CHECK_INT((int) PFX perms::unknown, 0xffff);
    CHECK_INT((int) PFX perms::add_perms, 0x10000);
    CHECK_INT((int) PFX perms::remove_perms, 0x20000);
    CHECK_INT((int) PFX perms::resolve_symlinks, 0x40000);

    CHECK_INT((int) PFX directory_options::none, 0);
    CHECK((int) PFX directory_options::follow_directory_symlink != 0);
}

void test_file_status() { // test file_status
    PFX file_status fs0;
    CHECK(fs0.type() == PFX file_type::none);
    CHECK(fs0.permissions() == PFX perms::unknown);

    PFX file_status fs1(PFX file_type::regular);
    CHECK(fs1.type() == PFX file_type::regular);
    CHECK(fs1.permissions() == PFX perms::unknown);

    PFX file_status fs2(PFX file_type::regular, PFX perms::all);
    CHECK(fs2.type() == PFX file_type::regular);
    CHECK(fs2.permissions() == PFX perms::all);
    CHECK((PFX perms::all | PFX perms::group_all) == PFX perms::all);
}

void test_directory_entry() { // test directory_entry
    PFX directory_entry de0;
    CHECK(de0.path() == PFX path());
    CHECK(de0.status().type() == PFX file_type::none);
    CHECK(de0.status().permissions() == PFX perms::unknown);
    CHECK(de0.symlink_status().type() == PFX file_type::none);
    CHECK(de0.symlink_status().permissions() == PFX perms::unknown);

    PFX directory_entry de1(PFX path("abc"));
    CHECK(de1.path() == PFX path("abc"));
    CHECK(de1.status().type() == PFX file_type::none);
    CHECK(de1.status().permissions() == PFX perms::unknown);
    CHECK(de1.symlink_status().type() == PFX file_type::none);
    CHECK(de1.symlink_status().permissions() == PFX perms::unknown);

    PFX directory_entry de2(PFX path("abc"), PFX file_status(PFX file_type::regular));
    CHECK(de2.path() == PFX path("abc"));
    CHECK(de2.status().type() == PFX file_type::regular);
    CHECK(de2.status().permissions() == PFX perms::unknown);
    CHECK(de2.symlink_status().type() == PFX file_type::none);
    CHECK(de2.symlink_status().permissions() == PFX perms::unknown);

    PFX directory_entry de3(
        PFX path("abc"), PFX file_status(PFX file_type::regular), PFX file_status(PFX file_type::fifo));
    CHECK(de3.path() == PFX path("abc"));
    CHECK(de3.status().type() == PFX file_type::regular);
    CHECK(de3.status().permissions() == PFX perms::unknown);
    CHECK(de3.symlink_status().type() == PFX file_type::fifo);
    CHECK(de3.symlink_status().permissions() == PFX perms::unknown);

    de0 = de1;
    CHECK(de0 == de1);
    CHECK(de0.path() == PFX path("abc"));
    de0 = de2;
    CHECK(de0.status().type() == PFX file_type::regular);
    de0 = STD move(de1);
    CHECK(de0.status().type() == PFX file_type::none);

    PFX directory_entry de4(de0);
    CHECK(de4 == de0);
    CHECK(de4.path() == PFX path("abc"));
    PFX directory_entry de5(STD move(de4));
    CHECK(de5 == de0);
    CHECK(de5.path() == PFX path("abc"));

    de1.assign(PFX path("def"));
    CHECK(de1.path() == PFX path("def"));
    CHECK(de1.status().type() == PFX file_type::none);
    CHECK(de1.status().permissions() == PFX perms::unknown);
    CHECK(de1.symlink_status().type() == PFX file_type::none);
    CHECK(de1.symlink_status().permissions() == PFX perms::unknown);

    de1.assign(PFX path("ghi"), PFX file_status(PFX file_type::regular));
    CHECK(de1.path() == PFX path("ghi"));
    CHECK(de1.status().type() == PFX file_type::regular);
    CHECK(de1.status().permissions() == PFX perms::unknown);
    CHECK(de1.symlink_status().type() == PFX file_type::none);
    CHECK(de1.symlink_status().permissions() == PFX perms::unknown);

    de1.assign(PFX path("/x/jkl"), PFX file_status(PFX file_type::regular), PFX file_status(PFX file_type::fifo));
    CHECK(de1.path() == PFX path("/x/jkl"));
    CHECK(de1.status().type() == PFX file_type::regular);
    CHECK(de1.status().permissions() == PFX perms::unknown);
    CHECK(de1.symlink_status().type() == PFX file_type::fifo);
    CHECK(de1.symlink_status().permissions() == PFX perms::unknown);

    de1.replace_filename(PFX path("mno"));
    CHECK(de1.path() == PFX path("/x/mno"));
    CHECK(de1.status().type() == PFX file_type::none);
    CHECK(de1.status().permissions() == PFX perms::unknown);
    CHECK(de1.symlink_status().type() == PFX file_type::none);
    CHECK(de1.symlink_status().permissions() == PFX perms::unknown);

    de1.replace_filename(PFX path("pqr"), PFX file_status(PFX file_type::regular));
    CHECK(de1.path() == PFX path("/x/pqr"));
    CHECK(de1.status().type() == PFX file_type::regular);
    CHECK(de1.status().permissions() == PFX perms::unknown);
    CHECK(de1.symlink_status().type() == PFX file_type::none);
    CHECK(de1.symlink_status().permissions() == PFX perms::unknown);

    STD error_code errcode;
    de1.replace_filename(
        PFX path("stu"), PFX file_status(PFX file_type::directory), PFX file_status(PFX file_type::fifo));
    CHECK(de1.path() == PFX path("/x/stu"));
    CHECK(de1.status(errcode).type() == PFX file_type::directory);
    CHECK(de1.status().permissions() == PFX perms::unknown);
    CHECK(de1.symlink_status(errcode).type() == PFX file_type::fifo);
    CHECK(de1.symlink_status().permissions() == PFX perms::unknown);

    CHECK(de1 == de1);
    CHECK(de1 <= de1);
    CHECK(de1 >= de1);
    CHECK(!(de1 != de1));
    CHECK(!(de1 < de1));
    CHECK(!(de1 > de1));
}

void test_directory_iterator() { // test directory_iterator
    STD error_code errcode;
    PFX path tp(TMP_NAME("1"));
    PFX remove_all(tp);

    CHECK(PFX create_directory(tp));
    CHECK(PFX is_directory(tp));

    { // check for no files
        PFX directory_iterator first0(tp), first(STD move(first0));
        PFX directory_iterator last0, last;
        last = STD move(last0);
        CHECK(first == last);
    }

    { // check for one file
        PFX path pathx("x");
        PFX path tpx(tp / pathx);
        STD ofstream ofs(tpx);
        ofs.close();

        PFX directory_iterator first0(tp, errcode);
        PFX directory_iterator first(first0);
        PFX directory_iterator last0, last;
        last = last0;
        CHECK(first != last);

        PFX directory_entry de1 = *first;
        PFX path p              = de1;
        CHECK(p.filename() == pathx.native());

        // test copies, moves
        PFX directory_entry de2(de1);
        p = de2;
        CHECK(p.filename() == pathx.native());

        PFX directory_entry de3;
        de3 = de1;
        p   = de3;
        CHECK(p.filename() == pathx.native());

        PFX directory_entry de4(STD move(de1));
        p = de4;
        CHECK(p.filename() == pathx.native());

        de4 = PFX directory_entry();
        de4 = STD move(de2);
        p   = de4;
        CHECK(p.filename() == pathx.native());

        PFX directory_iterator di1(tp);
        PFX directory_iterator di2(STD move(di1));
        p = *di2;
        CHECK(p.filename() == pathx.native());

        PFX directory_iterator di3;
        di3 = STD move(di2);
        p   = *di3;
        CHECK(di3->path().filename() == pathx.native());
        CHECK(p.filename() == pathx.native());
        PFX directory_iterator di3a = di3;
        CHECK(++di3 == last);
        CHECK(di3.increment(errcode) == last);
        CHECK(errcode != STD error_code());

        CHECK(PFX begin(di3) == di3);
        CHECK(PFX end(di3) == PFX directory_iterator());

        PFX remove(tpx);
    }
    PFX remove_all(tp);
}

void test_recursive_directory_iterator() { // test recursive_directory_iterator
    STD error_code errcode;
    PFX path tp(TMP_NAME("1"));
    PFX remove_all(tp);

    CHECK(PFX create_directory(tp));
    CHECK(PFX is_directory(tp));

    { // check for no files
        PFX recursive_directory_iterator first0(tp);
        PFX recursive_directory_iterator first(STD move(first0));
        PFX recursive_directory_iterator last0, last;
        last = STD move(last0);
        CHECK(first == last);
    }

    { // check for one file
        PFX path pathx("x");
        PFX path tpx(tp / pathx);
        STD ofstream ofs(tpx);
        ofs.close();

        PFX recursive_directory_iterator first0(tp, errcode);
        PFX recursive_directory_iterator first(first0);
        PFX recursive_directory_iterator last0, last;
        last = last0;
        CHECK(first != last);

        PFX directory_entry de1 = *first;
        PFX path p              = de1;
        CHECK(p.filename() == pathx.native());

        PFX recursive_directory_iterator di0(tp, PFX directory_options::follow_directory_symlink);
        CHECK(di0.options() == PFX directory_options::follow_directory_symlink);
        CHECK_INT(di0.depth(), 0);
        CHECK(di0.recursion_pending());
        di0.disable_recursion_pending();
        CHECK(!di0.recursion_pending());

        CHECK(PFX begin(di0) == di0);
        CHECK(PFX end(di0) == PFX recursive_directory_iterator());

        PFX recursive_directory_iterator di1(tp, PFX directory_options::none, errcode);
        PFX recursive_directory_iterator di2(STD move(di1));
        p = *di2;
        CHECK(p.filename() == pathx.native());

        PFX recursive_directory_iterator di3;
        di3 = STD move(di2);
        p   = *di3;
        CHECK(p.filename() == pathx.native());
        CHECK(++di3 == last);

        PFX path pathy("y");
        PFX path tpy(tp / pathy);
        CHECK(PFX create_directory(tpy));
        CHECK(PFX is_directory(tpy));
        PFX recursive_directory_iterator first1(tp);

        CHECK(first1 != last);
        p = *first1;
        CHECK(!PFX is_directory(p));
        CHECK(p.filename() == pathx.native());

        CHECK(++first1 != last);
        p = *first1;
        CHECK(PFX is_directory(p));
        CHECK(p.filename() == pathy.native());

        CHECK(++first1 == last);
        CHECK(first1.increment(errcode) == last);
        CHECK(errcode != STD error_code());

        di3.pop();
        CHECK(di3 == PFX recursive_directory_iterator());

        CHECK_INT(PFX remove_all(tp), 1);
    }

    PFX remove_all(tp);
}

void test_ops() { // test operations
    STD error_code errcode;
    PFX path tpx(TMP_NAME("1"));
    PFX path tpy(TMP_NAME("2"));
    PFX path tpz(TMP_NAME("3"));

    PFX remove(tpx);
    STD ofstream ofs(tpx);
    ofs << "hello\n";
    ofs.close();
    CHECK(6 <= PFX file_size(tpx));

    // path operator/(const path&, const path&)
    CHECK(PFX path("abc") / PFX path("def") == PFX path("abc/def"));

    // path absolute(const path&, const path& = current_path());
    CHECK((PFX absolute(PFX path("abc"))).is_absolute());
    CHECK((PFX absolute(PFX path(ROOT_NAME "abc"), PFX current_path())).is_absolute());

    // path canonical(const path&, const path& _Base = current_path());
    // path canonical(const path&, error_code&);
    // path canonical(const path&, const path&, error_code&);
    PFX path abcAbsolute = PFX absolute("abc");
    PFX remove(abcAbsolute);
    PFX create_directory(abcAbsolute);
    CHECK((PFX canonical(PFX path("abc"))).is_absolute());
    CHECK((PFX canonical(abcAbsolute, PFX current_path())).is_absolute());
    CHECK((PFX canonical(PFX path("abc"), errcode)).is_absolute());
    CHECK((PFX canonical(abcAbsolute, PFX current_path(), errcode)).is_absolute());
    PFX remove(abcAbsolute);

    { // test file/directory copy
        // void copy(const path&, const path&);
        // void copy(const path&, const path&, error_code&) noexcept;
        // void copy(const path&, const path&, copy_options);
        // void copy(const path&, const path&, copy_options, error_code&) noexcept;
        PFX path tpv(TMP_NAME("4"));
        PFX path tpw(TMP_NAME("5"));
        PFX path tpw1 = tpw / PFX path("x");
        PFX path tpw2 = tpw1 / PFX path("y");
        PFX path tpw3 = tpw1 / PFX path("data");
        PFX remove(tpw3);
        PFX remove(tpw2);
        PFX remove(tpw1);
        PFX remove(tpw);
        PFX remove(tpv);

        CHECK(PFX create_directories(tpw2));
        CHECK(PFX is_directory(tpw2));
        CHECK(PFX is_directory(tpw1));
        CHECK(PFX is_directory(tpw));

        PFX copy(tpx, tpw3);
        CHECK(PFX exists(tpw3));
        PFX remove(tpw3);
        PFX copy(tpx, tpw3, errcode);
        CHECK(PFX exists(tpw3));
        PFX remove(tpw3);
        PFX copy(tpx, tpw3, PFX copy_options::copy_symlinks);
        CHECK(PFX exists(tpw3));
        PFX remove(tpw3);
        PFX copy(tpx, tpw3,
            PFX copy_options(PFX copy_options::directories_only | PFX copy_options::create_hard_links
                             | PFX copy_options::create_symlinks));
        CHECK(!PFX exists(tpw3));

        PFX copy(tpw, tpv, PFX copy_options(PFX copy_options::recursive | PFX copy_options::skip_symlinks), errcode);
        CHECK(PFX exists(tpv));
        PFX remove(tpw3);
        PFX remove(tpw2);
        PFX remove(tpw1);
        PFX remove(tpw);
        PFX remove_all(tpv);
    }

    // bool copy_file(const path&, const path&, copy_options = copy_options::none);
    // bool copy_file(const path&, const path&, error_code&) noexcept;
    // bool copy_file(const path&, const path&, copy_options, error_code&) noexcept;
    PFX remove(tpy);
    PFX remove(tpz);
    PFX copy_file(tpx, tpy);
    PFX remove(tpy);

    PFX copy_file(tpx, tpy, PFX copy_options::none);
    PFX copy_file(tpx, tpy, PFX copy_options::overwrite_existing);
    PFX copy_file(tpx, tpy, PFX copy_options::update_existing, errcode);
    PFX copy_file(tpx, tpy, PFX copy_options::skip_existing, errcode);
    CHECK_INT(PFX file_size(tpx), PFX file_size(tpy));
    PFX remove(tpy);

    // void copy_symlink(const path&, const path&);
    // void copy_symlink(const path&, const path&, error_code&) noexcept;
    try { // link may legitimately fail
        PFX create_symlink(PFX current_path(), tpy);
        PFX copy_symlink(tpy, tpz);
        CHECK(PFX exists(tpz));
        PFX remove(tpz);
        STD error_code ec;
        PFX create_symlink(PFX current_path(), tpz);
        PFX copy_symlink(tpy, tpz, ec);
        CHECK(PFX exists(tpz));
    } catch (...) { // link failed
    }
    PFX remove(tpy);
    PFX remove(tpz);

    { // test create_directories
        // bool create_directories(const path&);
        // bool create_directories(const path&, error_code&) noexcept;
        PFX path tpw(TMP_NAME("4"));
        PFX path tpw1 = tpw / PFX path("x");
        PFX path tpw2 = tpw1 / PFX path("y");
        PFX remove(tpw2);
        PFX remove(tpw1);
        PFX remove(tpw);

        CHECK(PFX create_directories(tpw2));
        CHECK(PFX is_directory(tpw2));
        CHECK(PFX is_directory(tpw1));
        CHECK(PFX is_directory(tpw));
        PFX remove(tpw2);
        PFX remove(tpw1);
        PFX remove(tpw);
    }
    // bool create_directory(const path&);
    // bool create_directory(const path&, error_code&) noexcept;
    // void create_directory(const path&, const path&);
    // void create_directory(const path&, const path& error_code&) noexcept;
    PFX create_directory(tpy);
    CHECK(PFX is_directory(tpy));
    PFX remove(tpy);
    PFX create_directory(tpy, errcode);
    CHECK(PFX is_directory(tpy));
    PFX remove(tpy);

    PFX create_directory(tpy, PFX current_path());
    CHECK(PFX is_directory(tpy));
    PFX remove(tpy);
    PFX create_directory(tpy, PFX current_path(), errcode);
    CHECK(PFX is_directory(tpy));
    PFX remove(tpy);

    // void create_directory_symlink(const path&, const path&);
    // void create_directory_symlink(const path&, const path&, error_code&) noexcept;
    try { // link may legitimately fail
        PFX create_directory_symlink(PFX current_path(), tpy);
        CHECK(PFX exists(tpy));
        STD error_code ec;
        PFX create_directory_symlink(PFX current_path(), tpz, ec);
        CHECK(PFX exists(tpz));
    } catch (...) { // link failed
        if (!terse) {
            CSTD printf("symlink not supported\n");
        }
    }
    PFX remove(tpy);
    PFX remove(tpz);

    // void create_hard_link(const path&, const path&);
    // void create_hard_link(const path&, const path&, error_code&) noexcept;
    PFX remove(tpx);
    PFX remove(tpy);

    STD ofstream ofs2(tpx);
    ofs2 << "hello\n";
    ofs2.close();
    CHECK(6 <= PFX file_size(tpx));

    try { // link may legitimately fail
        PFX create_hard_link(tpx, tpy);
        STD error_code ec;
        PFX create_hard_link(tpx, tpz, ec);
        CHECK(PFX exists(tpz));
    } catch (...) { // link failed
        if (!terse) {
            CSTD printf("hard link not supported\n");
        }
    }
    PFX remove(tpy);
    PFX remove(tpz);

    // void create_symlink(const path&, const path&);
    // void create_symlink(const path&, const path&, error_code&) noexcept;
    try { // link may legitimately fail
        PFX create_symlink(tpx, tpy);
        CHECK(PFX exists(tpy));
        STD error_code ec;
        PFX create_symlink(tpx, tpz, ec);
        CHECK(PFX exists(tpz));
    } catch (...) { // link failed
    }
    PFX remove(tpy);
    PFX remove(tpz);

    // path current_path();
    // path current_path(error_code&) noexcept;
    // void current_path(const path&);
    // void current_path(const path&, error_code&) noexcept;
    PFX path tp = PFX current_path();
    PFX current_path(tp);
    CHECK(tp == PFX current_path());

    // bool equivalent(const path&, const path&);
    // bool equivalent(const path&, const path&, error_code&) noexcept;
    CHECK(PFX equivalent(tpx, tpx));
    CHECK(!PFX equivalent(tpx, PFX path("/")));

    // bool exists(file_status) noexcept;
    // bool exists(const path&);
    // bool exists(const path&, error_code&) noexcept;
    CHECK(PFX exists(PFX status(tpx)));
    CHECK(PFX exists(tpx));
    CHECK(PFX exists(tpx, errcode));

    // uintmax_t file_size(const path&);
    // uintmax_t file_size(const path&, error_code&) noexcept;
    CHECK(6 <= PFX file_size(tpx));
    CHECK(6 <= PFX file_size(tpx, errcode));

    // uintmax_t hard_link_count(const path&);
    // uintmax_t hard_link_count(const path&, error_code&) noexcept;
    CHECK_INT(PFX hard_link_count(tpx), 1);
    CHECK_INT(PFX hard_link_count(tpx, errcode), 1);

    // bool is_block_file(file_status) noexcept;
    // bool is_block_file(const path&);
    // bool is_block_file(const path&, error_code&) noexcept;
    CHECK(!PFX is_block_file(PFX status(tpx)));
    CHECK(!PFX is_block_file(tpx));
    CHECK(!PFX is_block_file(tpx, errcode));

    // bool is_character_file(file_status) noexcept;
    // bool is_character_file(const path&);
    // bool is_character_file(const path&, error_code&) noexcept;
    CHECK(!PFX is_character_file(PFX status(tpx)));
    CHECK(!PFX is_character_file(tpx));
    CHECK(!PFX is_character_file(tpx, errcode));

    // bool is_directory(file_status) noexcept;
    // bool is_directory(const path&);
    // bool is_directory(const path&, error_code&) noexcept;
    CHECK(!PFX is_directory(PFX status(tpx)));
    CHECK(!PFX is_directory(tpx));
    CHECK(!PFX is_directory(tpx, errcode));

    // bool is_empty(const path&);
    // bool is_empty(const path&, error_code&) noexcept;
    CHECK(!PFX is_empty(tpx));
    CHECK(!PFX is_empty(tpx, errcode));

    // bool is_fifo(file_status) noexcept;
    // bool is_fifo(const path&);
    // bool is_fifo(const path&, error_code&) noexcept;
    CHECK(!PFX is_fifo(PFX status(tpx)));
    CHECK(!PFX is_fifo(tpx));
    CHECK(!PFX is_fifo(tpx, errcode));

    // bool is_other(file_status) noexcept;
    // bool is_other(const path&);
    // bool is_other(const path&, error_code&) noexcept;
    CHECK(!PFX is_other(PFX status(tpx)));
    CHECK(!PFX is_other(tpx));
    CHECK(!PFX is_other(tpx, errcode));

    // bool is_regular_file(file_status) noexcept;
    // bool is_regular_file(const path&);
    // bool is_regular_file(const path&, error_code&) noexcept;
    CHECK(PFX is_regular_file(PFX status(tpx)));
    CHECK(PFX is_regular_file(tpx));
    CHECK(PFX is_regular_file(tpx, errcode));

    // bool is_socket(file_status) noexcept;
    // bool is_socket(const path&);
    // bool is_socket(const path&, error_code&) noexcept;
    CHECK(!PFX is_socket(PFX status(tpx)));
    CHECK(!PFX is_socket(tpx));
    CHECK(!PFX is_socket(tpx, errcode));

    // bool is_symlink(file_status) noexcept;
    // bool is_symlink(const path&);
    // bool is_symlink(const path&, error_code&) noexcept;
    CHECK(!PFX is_symlink(PFX status(tpx)));
    CHECK(!PFX is_symlink(tpx));
    CHECK(!PFX is_symlink(tpx, errcode));

    // typedef chrono::time_point<chrono::system_clock> file_time_type;
    // file_time_type last_write_time(const path&, error_code&) noexcept;
    // file_time_type last_write_time(const path&);
    // void last_write_time(const path&, file_time_type, error_code&) noexcept;
    // void last_write_time(const path&, file_time_type);
    PFX file_time_type wt = PFX last_write_time(".");
    time_t cwt            = STD chrono::system_clock::to_time_t(wt);
    CHECK(cwt != (time_t) -1);
    PFX last_write_time(tpx, wt);
    PFX file_time_type wt2 = PFX last_write_time(tpx);
    CHECK(wt == wt2);

    wt  = PFX last_write_time(".", errcode);
    cwt = STD chrono::system_clock::to_time_t(wt);
    CHECK(cwt != (time_t) -1);
    PFX last_write_time(tpx, wt, errcode);
    wt2 = PFX last_write_time(tpx);
    CHECK(wt == wt2);

    // void permissions(const path&, perms);
    // void permissions(const path&, perms, error_code&) noexcept;
    PFX perms mask = PFX perms(PFX perms::others_write | PFX perms::group_write | PFX perms::owner_write);
    PFX copy(tpx, tpy);
    PFX permissions(tpy, mask);
    CHECK(PFX perms(PFX status(tpy).permissions() & mask) != PFX perms::none);
    PFX permissions(tpy, PFX perms(mask | PFX perms::remove_perms));
    CHECK(PFX perms(PFX status(tpy).permissions() & mask) == PFX perms::none);
    PFX permissions(tpy, PFX perms(mask | PFX perms::add_perms));
    CHECK(PFX perms(PFX status(tpy).permissions() & mask) != PFX perms::none);
    PFX remove(tpy);

    // path read_symlink(const path&);
    // path read_symlink(const path&, error_code&) noexcept;
    try { // link may legitimately fail
        PFX create_symlink(tpx, tpy);
        CHECK(PFX read_symlink(tpx) == tpy);
        STD error_code ec;
        PFX create_symlink(tpx, tpz, ec);
        CHECK(PFX read_symlink(tpx) == tpz);
    } catch (...) { // link failed
    }
    PFX remove(tpy);
    PFX remove(tpz);

    // bool remove(const path&);
    // bool remove(const path&, error_code&) noexcept;
    PFX copy(tpx, tpy);
    CHECK(PFX exists(tpy));
    PFX remove(tpy);
    CHECK(!PFX exists(tpy));

    PFX copy(tpx, tpy);
    CHECK(PFX exists(tpy));
    PFX remove(tpy, errcode);
    CHECK(!PFX exists(tpy));

    // uintmax_t remove_all(const path&);
    // uintmax_t remove_all(const path&, error_code&) noexcept;
    PFX path tpw(TMP_NAME("5"));
    PFX path tpw1 = tpw / PFX path("x");
    PFX path tpw2 = tpw1 / PFX path("y");
    PFX remove(tpw2);
    PFX remove(tpw1);
    PFX remove(tpw);

    CHECK(PFX create_directories(tpw2));
    CHECK(PFX is_directory(tpw));
    PFX remove_all(tpw);
    CHECK(!PFX exists(tpw));

    CHECK(PFX create_directories(tpw2));
    CHECK(PFX is_directory(tpw));
    PFX remove_all(tpw, errcode);
    CHECK(!PFX exists(tpw));

    // void rename(const path&, const path&);
    // void rename(const path&, const path&, error_code&) noexcept;
    PFX rename(tpx, tpz);
    CHECK(!PFX exists(tpx));
    CHECK(PFX exists(tpz));
    PFX rename(tpz, tpx);
    CHECK(!PFX exists(tpz));
    CHECK(PFX exists(tpx));

    // void resize_file(const path&, uintmax_t);
    // void resize(const path&, uintmax_t, error_code&) noexcept;
    PFX resize_file(tpx, 50);
    CHECK_INT(PFX file_size(tpx), 50);
    PFX resize_file(tpx, 4, errcode);
    CHECK_INT(PFX file_size(tpx), 4);

    // space_info space(const path&);
    // space_info space(const path&, error_code&) noexcept;
    PFX space_info spinfo = PFX space(PFX path("/"));
    CHECK(0 < spinfo.capacity);
    CHECK(0 < spinfo.free);
    CHECK(0 < spinfo.available);

    // file_status status(const path&);
    // file_status status(const path&, error_code&) noexcept;
    // bool status_known(file_status) noexcept;
    CHECK(PFX status_known(PFX status(tpx)));
    CHECK(PFX status_known(PFX status(tpx, errcode)));

    // file_status symlink_status(const path&);
    // file_status symlink_status(const path&, error_code&) noexcept;
    CHECK(PFX status_known(PFX symlink_status(tpx)));
    CHECK(PFX status_known(PFX symlink_status(tpx, errcode)));

    // path system_complete(const path&);
    // path system_complete(const path&, error_code&) noexcept;
    CHECK(PFX system_complete(tpx).is_absolute());
    CHECK(PFX system_complete(tpx, errcode).is_absolute());

    // path temp_directory_path();
    // path temp_directory_path(error_code&) noexcept;
    CHECK(PFX exists(PFX temp_directory_path()));
    CHECK(PFX is_directory(PFX temp_directory_path(errcode), errcode));

    PFX remove_all(tpx);
}

void test_fstream() { // test against fstream
    PFX path tp(TMP_NAME("1"));
    PFX remove(tp);

    // test output file opening
    STD ofstream ofs(tp);
    CHECK(ofs.is_open());
    ofs.close();

    PFX remove_all(tp);
}

void test_main() { // test basic workings of filesystem definitions
    test_path_data();

    test_path();
    test_errors();
    test_enums();
    test_file_status();
    test_directory_entry();
    test_directory_iterator();
    test_recursive_directory_iterator();
    test_ops();
    test_fstream();
}
