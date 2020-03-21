# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, lit_config, shared):
        exe_source = Path(test.getSourcePath())
        dll_source = exe_source.parent / 'TestDll.cpp'
        shared.exec_dir = test.getExecDir()
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        pass_var, fail_var = test.getPassFailResultCodes()
        dll_output = output_dir / 'TestDll.DLL'

        dll_compile_cmd, out_files, exec_file = \
            test.cxx.executeBasedOnFlagsCmd([dll_source], output_dir,
                                            shared.exec_dir, 'TestDll',
                                            ['/Fe' + str(dll_output)],
                                            [], ['/DLL'])

        shared.dll_file = dll_output

        yield TestStep(dll_compile_cmd, shared.exec_dir, [dll_source],
                       test.cxx.compile_env)

        exe_compile_cmd, out_files, shared.exec_file = \
            test.cxx.executeBasedOnFlagsCmd([exe_source], output_dir,
                                            shared.exec_dir, output_base,
                                            [], [], [])

        yield TestStep(exe_compile_cmd, shared.exec_dir, [exe_source],
                       test.cxx.compile_env)

    def getTestSteps(self, test, lit_config, shared):
        if shared.exec_file is not None:
            yield TestStep([str(shared.exec_file)], shared.exec_dir,
                           [shared.exec_file, shared.dll_file],
                           test.cxx.compile_env)
