// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <locale>
#include <new>
#include <regex>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

const int N = 1000;

int g_scalars                 = 0;
int g_arrays                  = 0;
void* g_scalar_allocations[N] = {nullptr};
void* g_array_allocations[N]  = {nullptr};

void meta_new(void* p, void** allocations, const char* type) {
    for (int i = 0; i < N; ++i) {
        if (allocations[i] == p) {
            printf("meta_new(): The %s allocation %p already exists!\n", type, p);
            abort();
        }
    }

    for (int i = 0; i < N; ++i) {
        if (allocations[i] == nullptr) {
            allocations[i] = p;
            return;
        }
    }

    printf("meta_new(): We performed over %d %s allocations!\n", N, type);
    abort();
}

void scalar_new(void* p) {
    meta_new(p, g_scalar_allocations, "scalar");
    ++g_scalars;
}

void array_new(void* p) {
    meta_new(p, g_array_allocations, "array");
    ++g_arrays;
}

void meta_delete(void* p, void** allocations, const char* type) {
    int deallocated = 0;

    for (int i = 0; i < N; ++i) {
        if (allocations[i] == p) {
            allocations[i] = nullptr;
            ++deallocated;
        }
    }

    if (deallocated == 0) {
        printf("meta_delete(): The %s allocation %p doesn't exist!\n", type, p);
        abort();
    }

    if (deallocated > 1) {
        printf("meta_delete(): The %s allocation %p existed more than once!\n", type, p);
        abort();
    }
}

void scalar_delete(void* p) {
    if (p) {
        meta_delete(p, g_scalar_allocations, "scalar");
        --g_scalars;
    }
}

void array_delete(void* p) {
    if (p) {
        meta_delete(p, g_array_allocations, "array");
        --g_arrays;
    }
}


void* operator new(size_t n) {
    void* p = malloc(n ? n : 1);

    if (p == nullptr) {
        throw bad_alloc();
    }

    scalar_new(p);

    return p;
}

void* operator new(size_t n, const nothrow_t&) noexcept {
    void* p = malloc(n ? n : 1);

    scalar_new(p);

    return p;
}

void operator delete(void* p) noexcept {
    scalar_delete(p);

    free(p);
}

void operator delete(void* p, const nothrow_t&) noexcept {
    scalar_delete(p);

    free(p);
}

void* operator new[](size_t n) {
    void* p = malloc(n ? n : 1);

    if (p == nullptr) {
        throw bad_alloc();
    }

    array_new(p);

    return p;
}

void* operator new[](size_t n, const nothrow_t&) noexcept {
    void* p = malloc(n ? n : 1);

    array_new(p);

    return p;
}

void operator delete[](void* p) noexcept {
    array_delete(p);

    free(p);
}

void operator delete[](void* p, const nothrow_t&) noexcept {
    array_delete(p);

    free(p);
}


void test(const char* s) {
    const int orig_scalars = g_scalars;
    const int orig_arrays  = g_arrays;

    { regex r(s); }

    if (g_scalars != orig_scalars) {
        printf("test(): %d scalar allocation leak(s)!\n", g_scalars - orig_scalars);
        abort();
    }

    if (g_arrays != orig_arrays) {
        printf("test(): %d array allocation leak(s)!\n", g_arrays - orig_arrays);
        abort();
    }
}

// Also test LWG-3204: sub_match::swap only swaps the base class
void test_lwg3204() {
    csub_match sm1{};
    sm1.first   = "hello";
    sm1.second  = "world";
    sm1.matched = true;

    csub_match sm2{};
    sm2.first   = "fluffy";
    sm2.second  = "cat";
    sm2.matched = false;

    sm1.swap(sm2);

    assert(strcmp(sm1.first, "fluffy") == 0);
    assert(strcmp(sm1.second, "cat") == 0);
    assert(!sm1.matched);

    assert(strcmp(sm2.first, "hello") == 0);
    assert(strcmp(sm2.second, "world") == 0);
    assert(sm2.matched);

    STATIC_ASSERT(noexcept(sm1.swap(sm2)));
}


int main() {
    // Perform any locale allocations before we begin the tests.
    locale loc;
    use_facet<ctype<char>>(loc);
    use_facet<collate<char>>(loc);

    test("([[:digit:]]{4}[- ]){3}[[:digit:]]{3,4}");
    test("(^[ "
         "]*#(?:[^\\\\\\n]|\\\\[^\\n_[:punct:][:alnum:]]*[\\n[:punct:][:w:]])*)|(//[^\\n]*|/\\*.*?\\*/"
         ")|\\b([+-]?(?:(?:0x[[:xdigit:]]+)|(?:(?:[[:digit:]]*\\.)?[[:digit:]]+(?:[eE][+-]?[[:digit:]]+)?))u?(?:(?:int("
         "?:8|16|32|64))|L)?)\\b|('(?:[^\\\\']|\\\\.)*'|\"(?:[^\\\\\"]|\\\\.)*\")|\\b(__asm|__cdecl|__declspec|__"
         "export|__far16|__fastcall|__fortran|__import|__pascal|__rtti|__stdcall|_asm|_cdecl|__except|_export|_far16|_"
         "fastcall|__finally|_fortran|_import|_pascal|_stdcall|__thread|__try|asm|auto|bool|break|case|catch|cdecl|"
         "char|class|const|const_cast|continue|default|delete|do|double|dynamic_cast|else|enum|explicit|extern|false|"
         "float|for|friend|goto|if|inline|int|long|mutable|namespace|new|operator|pascal|private|protected|public|"
         "register|reinterpret_cast|return|short|signed|sizeof|static|static_cast|struct|switch|template|this|throw|"
         "true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while)\\b");
    test("(Tom|Sawyer|Huckleberry|Finn).{0,30}river|river.{0,30}(Tom|Sawyer|Huckleberry|Finn)");
    test("[[:alpha:]]+ing");
    test("^([0-9]+)(\\-| |$)(.*)$");
    test("^([a-zA-Z0-9_\\-\\.]+)@((\\[[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.)|(([a-zA-Z0-9\\-]+\\.)+))([a-zA-Z]{2,4}|["
         "0-9]{1,3})(\\]?)$");
    test("^(template[[:space:]]*<[^;:{]+>[[:space:]]*)?(class|struct)[[:space:]]*(\\<\\w+\\>([ "
         "]*\\([^)]*\\))?[[:space:]]*)*(\\<\\w*\\>)[[:space:]]*(<[^;:{]+>[[:space:]]*)?(\\{|:[^;\\{()]*\\{)");
    test("^[-+]?[[:digit:]]*\\.?[[:digit:]]*$");
    test("^[ ]*#[ ]*include[ ]+(\"[^\"]+\"|<[^>]+>)");
    test("^[ ]*#[ ]*include[ ]+(\"boost/[^\"]+\"|<boost/[^>]+>)");
    test("^[[:digit:]]{1,2}/[[:digit:]]{1,2}/[[:digit:]]{4}$");
    test("^[^ ]*?Twain");
    test("^[a-zA-Z]{1,2}[0-9][0-9A-Za-z]{0,1} {0,1}[0-9][A-Za-z]{2}$");
    test("<a[^>]+href=(\"[^\"]*\"|[^[:space:]]+)[^>]*>");
    test("<font[^>]+face=(\"[^\"]*\"|[^[:space:]]+)[^>]*>.*?</font>");
    test("<h[12345678][^>]*>.*?</h[12345678]>");
    test("<img[^>]+src=(\"[^\"]*\"|[^[:space:]]+)[^>]*>");
    test("<p>.*?</p>");
    test("aa");
    test("abc");
    test("Huck[[:alpha:]]+");
    test("Tom|Sawyer|Huckleberry|Finn");
    test("Twain");

    test_lwg3204();
}
