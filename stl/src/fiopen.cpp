// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Fiopen(const char */const wchar_t *, ios_base::openmode)

#include <fstream>

namespace {
    using _STD ios_base;

    FILE* _Xfsopen(_In_z_ const char* filename, _In_ int mode, _In_ int prot) {
        static const char* const mods[] = {// fopen mode strings corresponding to valid[i]
            "r", "w", "w", "a", "rb", "wb", "wb", "ab", "r+", "w+", "a+", "r+b", "w+b", "a+b", "wx", "wx", "w+x", "wbx",
            "wbx", "w+bx", nullptr};

        return _fsopen(filename, mods[mode], prot);
    }

    FILE* _Xfsopen(_In_z_ const wchar_t* filename, _In_ int mode, _In_ int prot) {
        static const wchar_t* const mods[] = {// fopen mode strings corresponding to valid[i]
            L"r", L"w", L"w", L"a", L"rb", L"wb", L"wb", L"ab", L"r+", L"w+", L"a+", L"r+b", L"w+b", L"a+b", L"wx",
            L"wx", L"w+x", L"wbx", L"wbx", L"w+bx", nullptr};

        return _wfsopen(filename, mods[mode], prot);
    }

    template <class CharT>
    FILE* _Xfiopen(const CharT* filename, ios_base::openmode mode, int prot) {
        static const ios_base::openmode valid[] = {
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
            ios_base::out | ios_base::_Noreplace,
            ios_base::out | ios_base::trunc | ios_base::_Noreplace,
            ios_base::out | ios_base::in | ios_base::trunc | ios_base::_Noreplace,
            ios_base::out | ios_base::binary | ios_base::_Noreplace,
            ios_base::out | ios_base::binary | ios_base::trunc | ios_base::_Noreplace,
            ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary | ios_base::_Noreplace,
        };

        FILE* fp          = nullptr;
        const bool at_end = (mode & ios_base::ate) != 0;

        if (mode & ios_base::_Nocreate) {
            mode |= ios_base::in; // file must exist
        }

        if (mode & ios_base::app) {
            mode |= ios_base::out; // extension -- app implies out
        }

        mode &= ~(ios_base::ate | ios_base::_Nocreate);

        // look for a valid mode
        int n = 0;
        while (valid[n] != mode) {
            if (++n == static_cast<int>(_STD size(valid))) {
                return nullptr; // no valid mode
            }
        }

        if ((fp = _Xfsopen(filename, n, prot)) == nullptr) {
            return nullptr; // open failed
        }

        if (at_end && fseek(fp, 0, SEEK_END) != 0) {
            fclose(fp); // can't position at end
            return nullptr;
        }

        return fp; // no need to seek to end, or seek succeeded
    }
} // unnamed namespace

_STD_BEGIN

_CRTIMP2_PURE FILE* __CLRCALL_PURE_OR_CDECL _Fiopen(
    const char* filename, ios_base::openmode mode, int prot) { // open wide-named file with byte name
    return _Xfiopen(filename, mode, prot);
}

_CRTIMP2_PURE FILE* __CLRCALL_PURE_OR_CDECL _Fiopen(
    const wchar_t* filename, ios_base::openmode mode, int prot) { // open a file with native name
    return _Xfiopen(filename, mode, prot);
}

_CRTIMP2_PURE FILE* __CLRCALL_PURE_OR_CDECL _Fiopen(
    const unsigned short* _Filename, ios_base::openmode _Mode, int _Prot) { // open file with wide name
    return _Fiopen(reinterpret_cast<const wchar_t*>(_Filename), _Mode, _Prot);
}

_STD_END
