# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        exeSource = test.getSourcePath()
        test2Source = os.path.join(os.path.dirname(exeSource), 'test2.cpp')

        outputDir, outputBase = test.getTempPaths()

        if litConfig.edg_drop is not None:
            isenseRspPath = outputBase + '.isense.rsp'
            test.compileFlags.extend(['/dE--write-isense-rsp', '/dE' + isenseRspPath])

        shared.execFile = outputBase + '.exe'
        cmd = [test.cxx, exeSource, test2Source, *test.flags, *test.compileFlags, '/Fe' + shared.execFile,
               '/link', *test.linkFlags]
        yield TestStep(cmd, shared.execDir, shared.env, False)

        for step in self._handleIsenseRspFile(test, litConfig):
            yield step
