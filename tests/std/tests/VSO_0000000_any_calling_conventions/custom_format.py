# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import STLTest


class CustomTest(STLTest):
    def __init__(self, suite, path_in_suite, lit_config, test_config,
                 envlst_entry, env_num, default_cxx, file_path=None):
        STLTest.__init__(self, suite, path_in_suite, lit_config, test_config,
                         envlst_entry, env_num, default_cxx, file_path)
        self.calling_convention_a = \
            envlst_entry.getEnvVal('CALLING_CONVENTION_A')
        self.calling_convention_b = \
            envlst_entry.getEnvVal('CALLING_CONVENTION_B')


class CustomTestFormat(STLTestFormat):
    def getTestsInDirectory(self, testSuite, path_in_suite,
                            litConfig, localConfig, test_class=CustomTest):
        return super().getTestsInDirectory(testSuite, path_in_suite, litConfig,
                                           localConfig, test_class)

    def getBuildSteps(self, test, lit_config, shared):
        shared.exec_dir = test.getExecDir()
        exe_source = Path(test.getSourcePath())
        a_source = exe_source.parent / 'a.cpp'
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()

        a_compile_cmd, out_files, exec_file = \
            test.cxx.executeBasedOnFlagsCmd([a_source], output_dir,
                                            shared.exec_dir, 'a',
                                            [test.calling_convention_a, '/c'],
                                            [], [])

        yield TestStep(a_compile_cmd, shared.exec_dir, [a_source],
                       test.cxx.compile_env)

        a_output = output_dir / 'a.obj'

        exe_compile_cmd, out_files, shared.exec_file = \
            test.cxx.executeBasedOnFlagsCmd([exe_source], output_dir,
                                            shared.exec_dir, output_base,
                                            [test.calling_convention_b,
                                            str(a_output)], [], [])

        yield TestStep(exe_compile_cmd, shared.exec_dir,
                       [a_output, exe_source], test.cxx.compile_env)
