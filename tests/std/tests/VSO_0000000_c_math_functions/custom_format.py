# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        exeSource = test.getSourcePath()
        cmathSource = os.path.join(os.path.dirname(exeSource), 'cmath.cpp')
        cstdlibSource = os.path.join(os.path.dirname(exeSource), 'cstdlib.cpp')

        outputDir, outputBase = test.getTempPaths()

        if TestType.COMPILE in test.testType:
            cmd = [test.cxx, '/c', exeSource, cmathSource, cstdlibSource, *test.flags, *test.compileFlags]
        elif TestType.RUN in test.testType:
            shared.execFile = outputBase + '.exe'
            cmd = [test.cxx, exeSource, cmathSource, cstdlibSource, *test.flags, *test.compileFlags, '/Fe' + shared.execFile,
                   '/link', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)
