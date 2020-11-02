#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

import os
import secrets
import stl.util

def configure(parameters, features, config, lit_config):
  # Apply parameters to the configuration first, since parameters are things
  # that we request explicitly and which might influence what features are
  # implicitly made available next.
  for param in parameters:
    feature = param.getFeature(config, lit_config.params)
    if feature:
      feature.enableIn(config)
      lit_config.note(
        "Enabling Lit feature '{}' as a result of parameter '{}'".format(feature.getName(config), param.name))

  # Then, apply the automatically-detected features.
  printFeatures = []
  for feature in features:
    if feature.isSupported(config):
      feature.enableIn(config)
      printFeatures.append(feature.getName(config))
  printFeatures = ["'{}'".format(f) for f in sorted(printFeatures)]
  lit_config.note("Enabling implicitly detected Lit features {}".format(', '.join(printFeatures)))

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
    ['/I' + os.path.normpath(dir) for dir in lit_config.include_dirs[config.name]]
  lit_config.link_flags[config.name] = \
    ['/LIBPATH:' + os.path.normpath(dir) for dir in lit_config.library_dirs[config.name]]

  lit_config.test_env = {'PATH' : os.path.normpath(lit_config.cxx_runtime)}

  if lit_config.is_kernel:
    lit_config.cert_pass = secrets.token_hex(64)
    lit_config.cert_path = lit_config.cxx_runtime + '/MsvcStlTestingCert.pfx'
    cmd = ['powershell', '-ExecutionPolicy', 'Bypass',
           '-File', lit_config.utils_dir + '/kernel/generateMsvcCert.ps1',
           '-out', lit_config.cert_path,
           '-pass', lit_config.cert_pass]

    out, err, rc = stl.util.executeCommand(cmd)
    if rc != 0:
        report = stl.util.makeReport(cmd, out, err, rc)
        raise RuntimeError(report)
