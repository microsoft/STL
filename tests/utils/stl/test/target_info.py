#===----------------------------------------------------------------------===//
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===//

import locale
import sys


class DefaultTargetInfo(object):
    def __init__(self, full_config):
        self.full_config = full_config

    def platform(self):
        return sys.platform.lower().strip()

    def is_windows(self):
        return self.platform() == 'win32'

    def is_darwin(self):
        return self.platform() == 'darwin'

    def add_locale_features(self, features):
        self.full_config.lit_config.warning(
            "No locales entry for target_system: %s" % self.platform())

    def add_cxx_compile_flags(self, flags): pass

    def add_cxx_link_flags(self, flags): pass

    def configure_env(self, env): pass

    def allow_cxxabi_link(self): return True

    def add_sanitizer_features(self, sanitizer_type, features): pass

    def use_lit_shell_default(self): return False

    def add_path(self, dest_env, new_path):
        if not new_path:
            return
        if 'PATH' not in dest_env:
            dest_env['PATH'] = new_path
        else:
            split_char = ';' if self.is_windows() else ':'
            dest_env['PATH'] = '%s%s%s' % (new_path, split_char,
                                           dest_env['PATH'])


def test_locale(loc):
    assert loc is not None
    default_locale = locale.setlocale(locale.LC_ALL)
    try:
        locale.setlocale(locale.LC_ALL, loc)
        return True
    except locale.Error:
        return False
    finally:
        locale.setlocale(locale.LC_ALL, default_locale)


def add_common_locales(features, lit_config, is_windows=False):
    # A list of locales needed by the test-suite.
    # The list uses the canonical name for the locale used in the test-suite
    locales = [
        ('en_US.UTF-8', 'English_United States.1252'),
        ('fr_FR.UTF-8', 'French_France.1252'),
        ('ru_RU.UTF-8', 'Russian_Russia.1251'),
        ('zh_CN.UTF-8', 'Chinese_China.936'),
        ('fr_CA.ISO8859-1', 'French_Canada.1252'),
        ('cs_CZ.ISO8859-2', 'Czech_Czech Republic.1250')
    ]
    for loc_id, windows_loc_name in locales:
        loc_name = windows_loc_name if is_windows else loc_id
        if test_locale(loc_name):
            features.add('locale.{0}'.format(loc_id))
        else:
            lit_config.warning('The locale {0} is not supported by '
                               'your platform. Some tests will be '
                               'unsupported.'.format(loc_name))


class WindowsLocalTI(DefaultTargetInfo):
    def __init__(self, full_config):
        super(WindowsLocalTI, self).__init__(full_config)

    def add_locale_features(self, features):
        add_common_locales(features, self.full_config.lit_config,
                           is_windows=True)

    def use_lit_shell_default(self):
        # Default to the internal shell on Windows, as bash on Windows is
        # usually very slow.
        return True


def make_target_info(full_config):
    return WindowsLocalTI(full_config)
