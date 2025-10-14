# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType

class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        exeSource = test.getSourcePath()
        manifestFile = os.path.join(os.path.dirname(exeSource), 'long_path_aware.manifest')

        _, outputBase = test.getTempPaths()

        if TestType.COMPILE in test.testType:
            cmd = [test.cxx, '/c', exeSource, *test.flags, *test.compileFlags]
        elif TestType.RUN in test.testType:
            shared.execFile = outputBase + '.exe'
            cmd = [test.cxx, exeSource, *test.flags, *test.compileFlags, '/Fe' + shared.execFile,
                   '/link', *test.linkFlags, 'Advapi32.lib', f'/MANIFESTINPUT:{manifestFile}']

        yield TestStep(cmd, shared.execDir, shared.env, False)
