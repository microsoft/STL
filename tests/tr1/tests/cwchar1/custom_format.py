# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getTestSteps(self, test, lit_config, shared):
        if shared.exec_file is None:
            for step in super().getTestSteps(test, lit_config, shared):
                yield step
        else:
            exec_env = test.cxx.compile_env
            exec_env['TMP'] = str(shared.exec_dir)

            yield TestStep([str(shared.exec_file)], shared.exec_dir,
                           [shared.exec_file], exec_env)
