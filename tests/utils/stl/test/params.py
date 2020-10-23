#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from libcxx.test.dsl import *

def getDefaultParameters(config, litConfig):
    DEFAULT_PARAMETERS = [
      Parameter(name='long_tests', choices=[True, False], type=bool, default=True,
                help="Whether to tests that take longer to run. This can be useful when running on a very slow device.",
                feature=lambda enabled: Feature(name='long_tests') if enabled else None),
    ]

    return DEFAULT_PARAMETERS
