# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Union
import itertools
import os
import re

import lit.Test

import stl.test.tests

_envlst_cache = dict()
_preprocessed_file_cache = dict()
_expected_result_entry_cache = dict()


@dataclass
class _TmpEnvEntry:
    env: Dict[str, str] = field(default_factory=dict)


@dataclass(frozen=True)
class EnvEntry:
    def __init__(self, tmp_env: _TmpEnvEntry):
        object.__setattr__(self, "_env_keys", tuple(tmp_env.env.keys()))
        object.__setattr__(self, "_env_vals", tuple(tmp_env.env.values()))

    def getEnvVal(self, key: str, default: Optional[str] = None) \
            -> Optional[str]:
        if key not in self._env_keys:
            return default

        # TRANSITION: All of this is to avoid having to install frozendict.
        # Reconsider this at a future date.
        return self._env_vals[self._env_keys.index(key)]

    _env_keys: Tuple[str]
    _env_vals: Tuple[str]


@dataclass
class _ParseCtx:
    current: List[_TmpEnvEntry] = field(default_factory=list)
    result: List[List[_TmpEnvEntry]] = field(default_factory=list)


_COMMENT_REGEX = re.compile(r"\s*#.*", re.DOTALL)
_INCLUDE_REGEX = re.compile(r'^RUNALL_INCLUDE (?P<filename>.+$)')
_ENV_VAR_MULTI_ITEM_REGEX = re.compile(r'(?P<name>\w+)="(?P<value>.*?)"')
_CROSSLIST_REGEX = re.compile(r'^RUNALL_CROSSLIST$')
_EXPECTED_RESULT_REGEX = re.compile(r'^(?P<prefix>.*) (?P<result>.*?)$')


def _parse_env_line(line: str) -> Optional[_TmpEnvEntry]:
    result = _TmpEnvEntry()
    for env_match in _ENV_VAR_MULTI_ITEM_REGEX.finditer(line):
        name = env_match.group("name")
        value = env_match.group("value")
        result.env[name] = value
    return result


def _append_env_entries(*args) -> _TmpEnvEntry:
    result = _TmpEnvEntry()
    for entry in args:
        for k, v in entry.env.items():
            if k not in result.env:
                result.env[k] = v
            else:
                result.env[k] = ' '.join((result.env[k], v))
    return result


def _do_crosslist(ctx: _ParseCtx):
    return itertools.starmap(_append_env_entries,
                             itertools.product(*ctx.result))


def _parse_env_lst(env_lst: Path, ctx: _ParseCtx):
    for line in parse_commented_file(env_lst):
        if (m:=_INCLUDE_REGEX.match(line)) is not None:
            p = env_lst.parent / Path(m.group("filename"))
            _parse_env_lst(p, ctx)
        elif _CROSSLIST_REGEX.match(line) is not None:
            ctx.result.append(ctx.current)
            ctx.current = []
        else:
            ctx.current.append(_parse_env_line(line))


def parse_commented_file(filename: Union[str, bytes, os.PathLike]) \
        -> List[str]:
    if str(filename) in _preprocessed_file_cache:
        return _preprocessed_file_cache[str(filename)]

    filename_path = Path(filename)
    result = list()
    with filename_path.open() as f:
        for line in f.readlines():
            if (line:=_COMMENT_REGEX.sub("", line)):
                line = line.strip()
                if line:
                    result.append(line)

        _preprocessed_file_cache[str(filename)] = result
        return result


def parse_result_file(filename: Union[str, bytes, os.PathLike]) \
        -> Dict[str, lit.Test.ResultCode]:
    if str(filename) in _expected_result_entry_cache:
        return _expected_result_entry_cache[str(filename)]

    res = dict()
    for line in parse_commented_file(filename):
        m = _EXPECTED_RESULT_REGEX.match(line)
        prefix = m.group("prefix")
        result = m.group("result")
        result_code = getattr(lit.Test, result, None)
        if result_code is None:
            result_code = getattr(stl.test.tests, result)
        res[prefix] = result_code

    _expected_result_entry_cache[str(filename)] = res
    return res


def parse_env_lst_file(env_list: Union[str, bytes, os.PathLike]) \
        -> Tuple[EnvEntry, ...]:
    if str(env_list) in _envlst_cache:
        return _envlst_cache[str(env_list)]

    env_list_path = Path(env_list)
    ctx = _ParseCtx()
    _parse_env_lst(env_list_path, ctx)
    ctx.result.append(ctx.current)
    res = tuple(map(EnvEntry, _do_crosslist(ctx)))
    _envlst_cache[env_list] = res
    return res
