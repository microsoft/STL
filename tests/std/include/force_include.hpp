//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#pragma once

#ifndef NO_TEST_ENVIRONMENT_PREPARER

#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>

struct TestEnvironmentPreparer {
    TestEnvironmentPreparer() noexcept {
        // avoid assertion dialog boxes; see GH-781
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
        _set_abort_behavior(0, _CALL_REPORTFAULT);

        // set stdout to be unbuffered; see GH-789
        setvbuf(stdout, nullptr, _IONBF, 0);
    }

    [[nodiscard]] bool succeeded() const noexcept {
        return true;
    }
};

const TestEnvironmentPreparer test_environment_preparer{};

#endif // NO_TEST_ENVIRONMENT_PREPARER
