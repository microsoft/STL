#===----------------------------------------------------------------------===//
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===//

import locale


def _test_locale(loc):
    assert loc is not None
    default_locale = locale.setlocale(locale.LC_ALL)
    try:
        locale.setlocale(locale.LC_ALL, loc)
        return True
    except locale.Error:
        return False
    finally:
        locale.setlocale(locale.LC_ALL, default_locale)


class WindowsLocalTI:
    def __init__(self, lit_config):
        self.features = set()
        self._add_common_locales(lit_config)

    def _add_common_locales(self, lit_config):
        locales = [
            ('en_US.UTF-8', 'English_United States.1252'),
            ('fr_FR.UTF-8', 'French_France.1252'),
            ('ru_RU.UTF-8', 'Russian_Russia.1251'),
            ('zh_CN.UTF-8', 'Chinese_China.936'),
            ('fr_CA.ISO8859-1', 'French_Canada.1252'),
            ('cs_CZ.ISO8859-2', 'Czech_Czech Republic.1250')
        ]

        for loc_id, loc_name in locales:
            if _test_locale(loc_name):
                self.features.add('locale.{0}'.format(loc_id))
            else:
                lit_config.note('The locale {0} is not supported by '
                                'your platform. Some tests will be '
                                'unsupported.'.format(loc_name))
