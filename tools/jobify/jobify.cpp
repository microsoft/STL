// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "stljobs.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Windows.h>

wchar_t* get_subcommand() {
    auto start     = ::GetCommandLineW();
    const auto end = start + ::wcslen(start);
    start          = std::find_if_not(start, end, is_space); // skip leading whitespace
    if (start != end) {
        if (*start == '"') {
            // assumes no escaped quotes in path
            ++start;
            start = std::find(start, end, L'"');
            if (start != end) {
                ++start;
            }
        } else {
            start = std::find_if(start, end, is_space);
        }

        start = std::find_if_not(start, end, is_space);
    }

    return start;
}

int main() {
    try {
        const auto subcommand = get_subcommand();
        if (*subcommand) {
            put_self_in_job();
            ::printf("[jobify] Executing: %ls\n", subcommand);
            ::fflush(stdout);

            STARTUPINFOW si{};
            si.cb   = sizeof(si);
            auto pi = create_process(
                nullptr, subcommand, nullptr, nullptr, TRUE, INHERIT_PARENT_AFFINITY, nullptr, nullptr, &si);

            pi.hThread.close();
            if (::WaitForSingleObject(pi.hProcess.get(), INFINITE) != WAIT_OBJECT_0) {
                api_failure("WaitForSingleObject");
            }

            unsigned long exitCode;
            if (!::GetExitCodeProcess(pi.hProcess.get(), &exitCode)) {
                api_failure("GetExitCodeProcess");
            }

            pi.hProcess.close();

            ::printf("[jobify] Command exited with %lX\n", exitCode);

            return static_cast<int>(exitCode);
        }

        ::puts("[jobify] Usage: jobify.exe subcommand");
        ::puts("[jobify] No command supplied, terminating.");
        return 1;
    } catch (api_exception& api) {
        api.give_up();
    }
}
