// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stljobs.h"
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include <Windows.h>

// Gets the command line with the path to jobify.exe removed from the beginning.
[[nodiscard]] wchar_t* get_subcommand() {
    auto first      = GetCommandLineW();
    const auto last = first + wcslen(first);
    first           = std::find_if_not(first, last, is_exactly_space); // skip leading whitespace
    if (first != last) {
        if (*first == '"') {
            // assumes no escaped quotes in path
            ++first;
            first = std::find(first, last, L'"');
            if (first != last) {
                ++first;
            }
        } else {
            first = std::find_if(first, last, is_exactly_space);
        }

        first = std::find_if_not(first, last, is_exactly_space);
    }

    return first;
}

int main() {
    try {
        const auto subcommand = get_subcommand();
        if (*subcommand) {
            put_self_in_job();
            printf("[jobify] Executing: %ls\n", subcommand);
            fflush(stdout);

            STARTUPINFOW si{};
            si.cb            = sizeof(si);
            auto processInfo = create_process(
                nullptr, subcommand, nullptr, nullptr, TRUE, INHERIT_PARENT_AFFINITY, nullptr, nullptr, &si);

            processInfo.hThread.close();
            if (WaitForSingleObject(processInfo.hProcess.get(), INFINITE) != WAIT_OBJECT_0) {
                api_failure("WaitForSingleObject");
            }

            unsigned long exitCode;
            if (!GetExitCodeProcess(processInfo.hProcess.get(), &exitCode)) {
                api_failure("GetExitCodeProcess");
            }

            printf("[jobify] Command exited with 0x%lX\n", exitCode);

            return static_cast<int>(exitCode);
        }

        puts("[jobify] Usage: jobify.exe subcommand");
        puts("[jobify] No command supplied, terminating.");
        return 1;
    } catch (api_exception& api) {
        api.give_up();
    }
}
