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


def beNice(prio: str) -> list[ConfigAction]:
  """
  Set the process priority to run tests with.
  """
  try:
    import psutil
    priority_map = {
      'normal': psutil.NORMAL_PRIORITY_CLASS,
      'low': psutil.BELOW_NORMAL_PRIORITY_CLASS,
      'idle': psutil.IDLE_PRIORITY_CLASS,
    }
    psutil.Process().nice(priority_map[prio])
  except ImportError:
    if not hasattr(beNice, 'suppress'):
      import sys
      print(f'NOTE: Module "psutil" is not installed, so the priority setting "{prio}" has no effect.', file=sys.stderr)
      beNice.suppress = True
  return []


def getDefaultParameters(config, litConfig):
    DEFAULT_PARAMETERS = [
      Parameter(name='test-only-edg', choices=[True, False], type=bool, default=False,
                help="Whether to only run edg tests (those that use the /BE flag).",
                actions=lambda enabled: [AddFeature(name='test-only-edg')] if enabled else []),
      Parameter(name='long_tests', choices=[True, False], type=bool, default=True,
                help="Whether to run tests that take a long time. This can be useful when running on a slow device.",
                actions=lambda enabled: [AddFeature(name='long_tests')] if enabled else []),
      Parameter(name="tags", type=list, default=[],
                help="Comma-separated list of run.pl tags to select tests",
                actions=lambda tags: [AddRunPLTags(tags)]),
      Parameter(name="notags", type=list, default=[],
                help="Comma-separated list of run.pl tags to exclude tests",
                actions=lambda tags: [AddRunPLNotags(tags)]),
      Parameter(name="priority", choices=["idle", "low", "normal"], default="idle", type=str,
                help='Process priority to run tests with: "idle" (the default), "low", or "normal". ' +
                  'Module "psutil" must be installed for this to have any effect.',
                actions=lambda prio: beNice(prio)),
    ]

    return DEFAULT_PARAMETERS
