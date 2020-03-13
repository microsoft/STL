# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path

import lit.Test

from stl.test.format import STLTestFormat
from stl.test.Test import STLTest
import stl


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

    def _evaluate_pass_test(self, test, lit_config):
        exe_source = Path(test.getSourcePath())
        a_source = exe_source.parent / 'a.cpp'
        exec_dir = test.getExecDir()
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        pass_var, fail_var = test.getPassFailResultCodes()

        a_compile_cmd, out, err, rc, out_files, exec_file = \
            test.cxx.executeBasedOnFlags([a_source], output_dir, exec_dir, 'a',
                                         [test.calling_convention_a, '/c'], [],
                                         [])

        report = ""
        if rc != 0:
            report += stl.util.makeReport(a_compile_cmd, out, err, rc)

            if not test.isExpectedToFail():
                report += "Compilation of a.cpp failed unexpectedly!"
            lit_config.note(report)
            return lit.Test.Result(fail_var, report)

        a_output = output_dir / 'a.obj'

        exe_compile_cmd, out, err, rc, out_files, exec_file = \
            test.cxx.executeBasedOnFlags([exe_source], output_dir, exec_dir,
                                         output_base,
                                         [test.calling_convention_b,
                                          str(a_output)], [], [])

        if rc != 0:
            report += stl.util.makeReport(exe_compile_cmd, out, err, rc)

            if not test.isExpectedToFail():
                report += "EXE creation failed unexpectedly!"
            lit_config.note(report)
            return lit.Test.Result(fail_var, report)
        elif exec_file is None:
            report = stl.util.makeReport(
                a_compile_cmd + ['&&'] + exe_compile_cmd, out, err, rc)
            return lit.Test.Result(pass_var, report)

        cmd, out, err, rc = self.executor.run(str(exec_file), [str(exec_file)],
                                              str(exec_dir), [],
                                              test.config.environment)
        report += stl.util.makeReport(cmd, out, err, rc)

        if rc == 0:
            return lit.Test.Result(pass_var, report)
        else:
            if not test.isExpectedToFail():
                report += "Compiled test failed unexpectedly!"
            return lit.Test.Result(fail_var, report)
