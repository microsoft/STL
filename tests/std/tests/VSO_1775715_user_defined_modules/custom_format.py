# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        _, outputBase = test.getTempPaths()

        testCpp = test.getSourcePath()
        sourceDir = os.path.dirname(testCpp)
        userIxx = os.path.join(sourceDir, 'user.ixx')

        # Dependency order is important here:
        inputPaths = [userIxx, testCpp]

        cmd = [test.cxx, *inputPaths, *test.flags, *test.compileFlags]

        if TestType.COMPILE in test.testType:
            cmd += ['/c']
        elif TestType.RUN in test.testType:
            shared.execFile = f'{outputBase}.exe'
            cmd += [f'/Fe{shared.execFile}', '/link', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)
