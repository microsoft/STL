# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        _, outputBase = test.getTempPaths()

        stdIxx = os.path.join(litConfig.cxx_modules, 'std.ixx')
        stdCompatIxx = os.path.join(litConfig.cxx_modules, 'std.compat.ixx')

        testCpp = test.getSourcePath()
        sourceDir = os.path.dirname(testCpp)
        test2Cpp = os.path.join(sourceDir, 'test2.cpp')
        classicCpp = os.path.join(sourceDir, 'classic.cpp')

        # Dependency order is important here:
        inputPaths = [stdIxx, stdCompatIxx, testCpp, test2Cpp, classicCpp]

        cmd = [test.cxx, *inputPaths, *test.flags, *test.compileFlags]

        if TestType.COMPILE in test.testType:
            cmd += ['/c']
        elif TestType.RUN in test.testType:
            shared.execFile = f'{outputBase}.exe'
            cmd += [f'/Fe{shared.execFile}', '/link', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)
