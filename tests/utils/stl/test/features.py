#===----------------------------------------------------------------------===//
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===//

import locale
import os
from libcxx.test.dsl import Feature

def hasLocale(loc):
    assert loc is not None
    default_locale = locale.setlocale(locale.LC_ALL)
    try:
        locale.setlocale(locale.LC_ALL, loc)
        return True
    except locale.Error:
        return False
    finally:
        locale.setlocale(locale.LC_ALL, default_locale)

def getDefaultFeatures(config, litConfig):
    DEFAULT_FEATURES = [Feature(name='msvc'), Feature(name='windows')]
    locales = {
      'en_US.UTF-8':     ['en_US.UTF-8', 'en_US.utf8', 'English_United States.1252'],
      'fr_FR.UTF-8':     ['fr_FR.UTF-8', 'fr_FR.utf8', 'French_France.1252'],
      'ja_JP.UTF-8':     ['ja_JP.UTF-8', 'ja_JP.utf8', 'Japanese_Japan.932'],
      'ru_RU.UTF-8':     ['ru_RU.UTF-8', 'ru_RU.utf8', 'Russian_Russia.1251'],
      'zh_CN.UTF-8':     ['zh_CN.UTF-8', 'zh_CN.utf8', 'Chinese_China.936'],
      'fr_CA.ISO8859-1': ['fr_CA.ISO8859-1', 'French_Canada.1252'],
      'cs_CZ.ISO8859-2': ['cs_CZ.ISO8859-2', 'Czech_Czech Republic.1250']
    }
    for loc, alts in locales.items():
      # Note: Using alts directly in the lambda body here will bind it to the value at the
      # end of the loop. Assigning it to a default argument works around this issue.
      DEFAULT_FEATURES.append(Feature(name='locale.{}'.format(loc),
                                      when=lambda cfg, alts=alts: any(hasLocale(alt) for alt in alts)))
    env_var = 'STL_EDG_DROP'
    litConfig.edg_drop = None
    if env_var in os.environ and os.environ[env_var] is not None:
        litConfig.edg_drop = os.environ[env_var]
        DEFAULT_FEATURES.append(Feature(name='edg_drop'))

    if litConfig.target_arch.casefold() == 'x86'.casefold():
        DEFAULT_FEATURES.append(Feature(name='arch_ia32'))
        DEFAULT_FEATURES.append(Feature(name='arch_avx2'))
        DEFAULT_FEATURES.append(Feature(name='x86'))

    elif litConfig.target_arch.casefold() == 'x64'.casefold():
        # TRANSITION, GH-3568
        # DEFAULT_FEATURES.append(Feature(name='ubsan'))
        DEFAULT_FEATURES.append(Feature(name='edg'))
        DEFAULT_FEATURES.append(Feature(name='arch_avx2'))
        DEFAULT_FEATURES.append(Feature(name='x64'))

    elif litConfig.target_arch.casefold() == 'arm'.casefold():
        DEFAULT_FEATURES.append(Feature(name='arch_vfpv4'))
        DEFAULT_FEATURES.append(Feature(name='arm'))

    elif litConfig.target_arch.casefold() == 'arm64'.casefold():
        DEFAULT_FEATURES.append(Feature(name='arm64'))

    return DEFAULT_FEATURES
