# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        shared.env["caexcludepath"] = os.environ["WindowsSdkDir"] + ";" + os.environ["VCToolsInstallDir"]
        shared.env["esp.extensions"] = "cppcorecheck.dll"
        shared.env["esp.annotationbuildlevel"] = "ignore"
        for step in super().getBuildSteps(test, litConfig, shared):
            yield step
