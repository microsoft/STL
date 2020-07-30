# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, lit_config, shared):
        shared.exec_dir = test.getExecDir()
        exe_source = Path(test.getSourcePath())
        test2_source = exe_source.parent / 'test2.cpp'
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()

        cmd, out_files, shared.exec_file = \
            test.cxx.executeBasedOnFlagsCmd([exe_source, test2_source],
                                            output_dir, shared.exec_dir,
                                            output_base, [], [], [])

        yield TestStep(cmd, shared.exec_dir, [exe_source, test2_source],
                       test.cxx.compile_env)
