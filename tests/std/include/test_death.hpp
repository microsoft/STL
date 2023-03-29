// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <crtdbg.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include <Windows.h>

namespace std_testing {
    constexpr int internal_failure = 103;
    using normal_function_t        = void (*)();
    using death_function_t         = void (*)();

    [[noreturn]] inline void api_unexpected(const char* const api_name) {
        const auto last_error = static_cast<unsigned int>(::GetLastError());
        printf("%s failed; LastError: 0x%08X\n", api_name, last_error);
        abort();
    }

    class death_test_executive {
        const normal_function_t run_normal_tests;
        std::vector<death_function_t> death_tests;

        int execute_death_test(const char* const test_id) const {
            const auto testId = static_cast<size_t>(atoi(test_id));

            if (errno == 0) {
                _set_abort_behavior(0, _WRITE_ABORT_MSG);
                _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
                _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

                death_tests[testId]();
                return 0;
            } else {
                puts("failed to parse test_id");
                return internal_failure;
            }
        }

        DWORD dispatch_death_test(const size_t test_id, const wchar_t* const this_program) const {
            STARTUPINFOW si{};
            si.cb = sizeof(si);

            PROCESS_INFORMATION pi{};

            std::wstring test_id_str(1, L' ');
            test_id_str.append(std::to_wstring(test_id));
            if (::CreateProcessW(this_program, &test_id_str[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)
                == 0) {
                api_unexpected("CreateProcessW");
            }

            if (::WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0) {
                api_unexpected("WaitForSingleObject");
            }

            DWORD exit_code = 0;
            if (::GetExitCodeProcess(pi.hProcess, &exit_code) == 0) {
                api_unexpected("GetExitCodeProcess");
            }

            ::CloseHandle(pi.hThread);
            ::CloseHandle(pi.hProcess);

            return exit_code;
        }

        static std::wstring get_current_process_path() {
            std::wstring result(MAX_PATH, L'\0');
            for (;;) {
                const DWORD result_size = ::GetModuleFileNameW(nullptr, &result[0], static_cast<DWORD>(result.size()));
                const size_t str_size   = result.size();
                if (result_size == str_size) {
                    // buffer was not big enough
                    const size_t str_max_size    = result.max_size();
                    const size_t result_max_size = str_max_size - str_max_size / 2;
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#endif // __clang__
                    if (result_size >= result_max_size) {
                        api_unexpected("GetModuleFileNameW");
                    }
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

                    result.resize(result_size + result_size / 2);
                } else if (result_size == 0) {
                    api_unexpected("GetModuleFileNameW");
                } else {
                    result.resize(result_size);
                    break;
                }
            }

            return result;
        }

    public:
        death_test_executive() : run_normal_tests(nullptr) {}

        explicit death_test_executive(const normal_function_t normal_tests_function)
            : run_normal_tests(normal_tests_function) {}

        template <size_t TestsCount>
        void add_death_tests(const death_function_t (&tests)[TestsCount]) {
            death_tests.insert(death_tests.end(), tests, std::end(tests));
        }

        int run(int argc, char* argv[]) const {
            if (argc == 1) {
                // first pass, run normal tests and sub-process loop
                printf("running normal tests...");
                if (run_normal_tests != nullptr) {
                    run_normal_tests();
                }
                puts(" passed!");

                ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
                const auto this_program       = get_current_process_path();
                const size_t death_tests_size = death_tests.size();
                for (size_t idx = 0; idx < death_tests_size; ++idx) {
                    printf("running death test %zu... ", idx);
                    const DWORD death_test_result = dispatch_death_test(idx, this_program.c_str());
                    if (death_test_result <= 1000U) {
                        printf("returned %lu", death_test_result);
                    } else {
                        printf("returned 0x%lX", death_test_result);
                    }

                    if (death_test_result == 0 || death_test_result == 100) {
                        puts(", a success code (this is bad)");
                        puts("Terminate!");
                        return 1;
                    } else if (death_test_result == internal_failure) {
                        puts(", an internal test harness failure");
                        puts("Terminate!");
                        return 1;
                    } else {
                        puts(", a failure code (this is good)");
                    }
                }

                return 0;
            } else if (argc == 2) {
                return execute_death_test(argv[1]);
            } else {
                puts("incorrect command line");
                return 1;
            }
        }
    };
} // namespace std_testing
