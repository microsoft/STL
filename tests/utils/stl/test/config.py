#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

import os

def configure(parameters, features, config, lit_config):
  # Apply the actions supplied by parameters to the configuration first, since
  # parameters are things that we request explicitly and which might influence
  # what features are implicitly made available next.
  for param in parameters:
    actions = param.getActions(config, lit_config.params)
    for action in actions:
      action.applyTo(config)
      #lit_config.note("Applied '{}' as a result of parameter '{}'".format(
      #  action.pretty(config, lit_config.params),
      #  param.pretty(config, lit_config.params)))

  # Then, apply the automatically-detected features.
  for feature in features:
    actions = feature.getActions(config)
    for action in actions:
      action.applyTo(config)
      #lit_config.note("Applied '{}' as a result of implicitly detected feature '{}'".format(
      #  action.pretty(config, lit_config.params),
      #  feature.pretty(config)))

  # Normalize and validate all subdirectories to be tested
  lit_config.test_subdirs[config.name] = [os.path.normpath(subdir) for subdir in lit_config.test_subdirs[config.name]]
  for subdir in lit_config.test_subdirs[config.name]:
    if not os.path.exists(subdir):
        lit_config.fatal("The directory {} does not exist and was marked as a subdirectory to test".format(subdir))

  lit_config.flags         = getattr(lit_config, 'flags', dict())
  lit_config.compile_flags = getattr(lit_config, 'compile_flags', dict())
  lit_config.link_flags    = getattr(lit_config, 'link_flags', dict())

  lit_config.flags[config.name] = []
  lit_config.compile_flags[config.name] = \
    ['-I' + os.path.normpath(dir) for dir in lit_config.include_dirs[config.name]]
  lit_config.link_flags[config.name] = \
    ['-LIBPATH:' + os.path.normpath(dir) for dir in lit_config.library_dirs[config.name]]

  lit_config.test_env = {'PATH' : os.path.normpath(lit_config.cxx_runtime)}

  # Add a parallelism group that multi-threaded tests can be marked as a part of.
  lit_config.parallelism_groups['multi-threaded'] = 1
