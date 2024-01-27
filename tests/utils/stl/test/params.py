#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from libcxx.test.dsl import *

class AddRunPLTags(ConfigAction):
  """
  Store a set of run.pl tags in the config to be later used to select test cases.
  """
  def __init__(self, taglist):
    self._taglist = taglist

  def applyTo(self, config):
    config.runPLTags = set(map(lambda x: x.casefold(), self._taglist))

  def pretty(self, config, litParams):
    return 'select run.pl tags {}'.format(str(self._taglist))


class AddRunPLNotags(ConfigAction):
  """
  Store a set of run.pl tags in the config to be later used to exclude test cases.
  """
  def __init__(self, taglist):
    self._taglist = taglist

  def applyTo(self, config):
    config.runPLNotags = set(map(lambda x: x.casefold(), self._taglist))

  def pretty(self, config, litParams):
    return 'exclude run.pl tags {}'.format(str(self._taglist))


def getDefaultParameters(config, litConfig):
    DEFAULT_PARAMETERS = [
      Parameter(name='long_tests', choices=[True, False], type=bool, default=True,
                help="Whether to run tests that take a long time. This can be useful when running on a slow device.",
                actions=lambda enabled: [AddFeature(name='long_tests')] if enabled else []),
      Parameter(name="tags", type=list, default=[],
                help="Comma-separated list of run.pl tags to select tests",
                actions=lambda tags: [AddRunPLTags(tags)]),
      Parameter(name="notags", type=list, default=[],
                help="Comma-separated list of run.pl tags to exclude tests",
                actions=lambda tags: [AddRunPLNotags(tags)]),
    ]

    return DEFAULT_PARAMETERS
