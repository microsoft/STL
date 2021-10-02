// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CRT_DISABLE_PERFCRIT_LOCKS
#include <cstdio>

// We intentionally didn't write "using namespace std;"
void testNonLocking(std::FILE* file, char* buf, std::size_t size) {
    std::fflush(file);
    std::fgetc(file);
    std::fputc('a', file);
    std::fread(buf, 1, size, file);
    std::fseek(file, 0, SEEK_SET);
    (void) std::ftell(file);
    std::fwrite(buf, 1, size, file);
    (void) std::getc(file);
    std::putc('a', file);
    std::ungetc('b', file);
    std::fclose(file);
}

int main() {} // COMPILE-ONLY
