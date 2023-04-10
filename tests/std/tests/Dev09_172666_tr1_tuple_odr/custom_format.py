# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        exeSource = test.getSourcePath()
        test2Source = os.path.join(os.path.dirname(exeSource), 'test2.cpp')

        _, outputBase = test.getTempPaths()

        if TestType.COMPILE in test.testType:
            cmd = [test.cxx, '/c', exeSource, test2Source, *test.flags, *test.compileFlags]
        elif TestType.RUN in test.testType:
            shared.execFile = outputBase + '.exe'
            cmd = [test.cxx, exeSource, test2Source, *test.flags, *test.compileFlags, '/Fe' + shared.execFile,
                   '/link', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)
