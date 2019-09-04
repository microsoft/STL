// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Fiopen(const char */const wchar_t *, ios_base::openmode)

#include <fstream>

#include <Windows.h>

_STD_BEGIN

FILE* _Xfsopen(_In_z_ const char* filename, _In_ int mode, _In_ int prot) {
    static const char* const mods[] = {// fopen mode strings corresponding to valid[i]
        "r", "w", "w", "a", "rb", "wb", "wb", "ab", "r+", "w+", "a+", "r+b", "w+b", "a+b", 0};

    return _fsopen(filename, mods[mode], prot);
}

FILE* _Xfsopen(_In_z_ const wchar_t* filename, _In_ int mode, _In_ int prot) {
    static const wchar_t* const mods[] = {// fopen mode strings corresponding to valid[i]
        L"r", L"w", L"w", L"a", L"rb", L"wb", L"wb", L"ab", L"r+", L"w+", L"a+", L"r+b", L"w+b", L"a+b", 0};

    return _wfsopen(filename, mods[mode], prot);
}

template <typename CharT>
FILE* _Xfiopen(const CharT* filename, ios_base::openmode mode, int prot) {
    static const int valid[] = {
        // valid combinations of open flags
        ios_base::in,
        ios_base::out,
        ios_base::out | ios_base::trunc,
        ios_base::out | ios_base::app,
        ios_base::in | ios_base::binary,
        ios_base::out | ios_base::binary,
        ios_base::out | ios_base::trunc | ios_base::binary,
        ios_base::out | ios_base::app | ios_base::binary,
        ios_base::in | ios_base::out,
        ios_base::in | ios_base::out | ios_base::trunc,
        ios_base::in | ios_base::out | ios_base::app,
        ios_base::in | ios_base::out | ios_base::binary,
        ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary,
        ios_base::in | ios_base::out | ios_base::app | ios_base::binary,
        0,
    };

    FILE* fp                     = 0;
    ios_base::openmode atendflag = mode & ios_base::ate;
    ios_base::openmode norepflag = mode & ios_base::_Noreplace;

    if (mode & ios_base::_Nocreate) {
        mode |= ios_base::in; // file must exist
    }

    if (mode & ios_base::app) {
        mode |= ios_base::out; // extension -- app implies out
    }

    mode &= ~(ios_base::ate | ios_base::_Nocreate | ios_base::_Noreplace);

    int n = 0;
    while (valid[n] != 0 && valid[n] != mode) { // look for a valid mode
        ++n;
    }

    if (valid[n] == 0) {
        return 0; // no valid mode
    }

    if (norepflag && (mode & (ios_base::out | ios_base::app))
        && (fp = _Xfsopen(filename, 0, prot)) != 0) { // file must not exist, close and fail
        fclose(fp);
        return 0;
    }

    if (fp != 0 && fclose(fp) != 0) {
        return 0; // can't close after test open
    }

    if ((fp = _Xfsopen(filename, n, prot)) == 0) {
        return 0; // open failed
    }

    if (!atendflag || fseek(fp, 0, SEEK_END) == 0) {
        return fp; // no need to seek to end, or seek succeeded
    }

    fclose(fp); // can't position at end
    return 0;
}

_CRTIMP2_PURE FILE* __CLRCALL_PURE_OR_CDECL _Fiopen(
    const char* filename, ios_base::openmode mode, int prot) { // open wide-named file with byte name
    return _Xfiopen(filename, mode, prot);
}

_CRTIMP2_PURE FILE* __CLRCALL_PURE_OR_CDECL _Fiopen(
    const wchar_t* filename, ios_base::openmode mode, int prot) { // open a file with native name
    return _Xfiopen(filename, mode, prot);
}

#ifdef _NATIVE_WCHAR_T_DEFINED
_CRTIMP2_PURE FILE* __CLRCALL_PURE_OR_CDECL _Fiopen(
    const unsigned short* _Filename, ios_base::openmode _Mode, int _Prot) { // open file with wide name
    return _Fiopen((wchar_t*) (_Filename), _Mode, _Prot);
}
#endif

_STD_END
