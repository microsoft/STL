# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path
import os

import lit.Test

from stl.test.format import STLTestFormat
import stl


class CustomTestFormat(STLTestFormat):
    def _evaluate_pass_test(self, test, lit_config):
        exec_dir = test.getExecDir()
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        pass_var, fail_var = test.getPassFailResultCodes()
        exe_source = Path(test.getSourcePath())
        exe_source_dir = exe_source.parent
        source_files = []

        for filename in os.listdir(exe_source_dir):
            if filename.endswith('.cpp'):
                source_files.append(exe_source_dir / filename)

        compile_cmd, out, err, rc, out_files, exec_file = \
            test.cxx.executeBasedOnFlags(source_files, output_dir, exec_dir,
                                         output_base, [], [], [])

        report = ""
        if rc != 0:
            report += stl.util.makeReport(compile_cmd, out, err, rc)

            if not test.isExpectedToFail():
                report += "Compilation of failed unexpectedly!"
            lit_config.note(report)
            return lit.Test.Result(fail_var, report)
        elif exec_file is None:
            report = stl.util.makeReport(compile_cmd, out, err, rc)
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
