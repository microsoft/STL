# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, lit_config, shared):
        exe_source = Path(test.getSourcePath())
        dll_source = exe_source.parent / 'TestDll.cpp'
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        dll_output = output_dir / 'TestDll.DLL'

        dll_compile_cmd, out_files, exec_file = \
            test.cxx.executeBasedOnFlagsCmd([dll_source], output_dir,
                                            shared.exec_dir, 'TestDll',
                                            ['/Fe' + str(dll_output)],
                                            [], ['/DLL'])

        shared.dll_file = dll_output

        yield TestStep(cmd=dll_compile_cmd, dependencies=[dll_source],
                       env=shared.exec_dir, out_files=out_files,
                       work_dir=shared.exec_dir)

        exe_compile_cmd, out_files, shared.exec_file = \
            test.cxx.executeBasedOnFlagsCmd([exe_source], output_dir,
                                            shared.exec_dir, output_base,
                                            [], [], [])

        yield TestStep(cmd=exe_compile_cmd, dependencies=[exe_source],
                       env=shared.exec_dir, out_files=out_files,
                       work_dir=shared.exec_dir, num=1)

    def getTestSteps(self, test, lit_config, shared):
        if shared.exec_file is not None:
            yield TestStep(cmd=[str(shared.exec_file)],
                           dependencies=[shared.exec_file, shared.dll_file],
                           env=shared.exec_env,
                           work_dir=shared.exec_dir)
