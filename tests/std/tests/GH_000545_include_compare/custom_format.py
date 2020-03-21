# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path
import os

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, lit_config, shared):
        shared.exec_dir = test.getExecDir()
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        exe_source_dir = Path(test.getSourcePath()).parent

        source_files = []
        for filename in os.listdir(exe_source_dir):
            if filename.endswith('.cpp'):
                source_files.append(exe_source_dir / filename)

        cmd, out_files, shared.exec_file = \
            test.cxx.executeBasedOnFlagsCmd(source_files, output_dir,
                                            shared.exec_dir, output_base,
                                            [], [], [])

        yield TestStep(cmd, shared.exec_dir, source_files,
                       test.cxx.compile_env)
