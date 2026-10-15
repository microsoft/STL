# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from dataclasses import dataclass, field
from pathlib import Path
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
    env: dict[str, str] = field(default_factory=dict)
    tags: set[str] = field(default_factory=set)


@dataclass(frozen=True)
class EnvEntry:
    def __init__(self, tmp_env: _TmpEnvEntry):
        object.__setattr__(self, "_env", frozendict(tmp_env.env))
        object.__setattr__(self, "_env_tags", tmp_env.tags)

    def hasAnyTag(self, tags: set[str]) -> bool:
        return not self._env_tags.isdisjoint(tags)

    def getEnvVal(self, key: str, default: str | None = None) \
            -> str | None:
        return self._env.get(key, default)

    _env: frozendict[str, str]
    _env_tags: set[str]


@dataclass
class _ParseCtx:
    current: list[_TmpEnvEntry] = field(default_factory=list)
    result: list[list[_TmpEnvEntry]] = field(default_factory=list)


_COMMENT_REGEX = re.compile(r"\s*#.*", re.DOTALL)
_INCLUDE_REGEX = re.compile(r'^RUNALL_INCLUDE (?P<filename>.+$)')
_TAGS_REGEX = re.compile(r'^(?P<tags>(\*|\w+(,\w+)*))\t+(?P<remainder>.*)$')
_ENV_VAR_MULTI_ITEM_REGEX = re.compile(r'(?P<name>\w+)="(?P<value>.*?)"')
_CROSSLIST_REGEX = re.compile(r'^RUNALL_CROSSLIST$')
_EXPECTED_RESULT_REGEX = re.compile(r'^(?P<prefix>[^ ]+) (?P<result>\w+)$')


def _parse_env_line(line: str) -> _TmpEnvEntry | None:
    result = _TmpEnvEntry()
    if (m:=_TAGS_REGEX.prefixmatch(line)) is not None:
        tags = m.group("tags").split(',')
        result.tags = {x.strip().casefold() for x in tags}
        line = m.group("remainder")
    for env_match in _ENV_VAR_MULTI_ITEM_REGEX.finditer(line):
        name = env_match.group("name")
        value = env_match.group("value")
        result.env[name] = value
    return result


def _merge_crosslist_entries(*args) -> _TmpEnvEntry:
    result = _TmpEnvEntry()
    result.tags = {'*'}
    for entry in args:
        # tags are intersected; `*` matches everything
        if len(entry.tags) == 1 and '*' in entry.tags:
            pass
        elif len(result.tags) == 1 and '*' in result.tags:
            result.tags = entry.tags
        else:
            result.tags &= entry.tags

        # values for identical keys are concatenated
        for k, v in entry.env.items():
            if k not in result.env:
                result.env[k] = v
            else:
                result.env[k] = ' '.join((result.env[k], v))
    return result


def _do_crosslist(ctx: _ParseCtx):
    return itertools.starmap(_merge_crosslist_entries,
                             itertools.product(*ctx.result))


def _parse_env_lst(env_lst: Path, ctx: _ParseCtx):
    for _, line in _parse_commented_file(env_lst):
        if (m:=_INCLUDE_REGEX.prefixmatch(line)) is not None:
            p = env_lst.parent / Path(m.group("filename"))
            _parse_env_lst(p, ctx)
        elif _CROSSLIST_REGEX.prefixmatch(line) is not None:
            ctx.result.append(ctx.current)
            ctx.current = []
        else:
            ctx.current.append(_parse_env_line(line))


def _parse_commented_file(filename: str | bytes | os.PathLike) \
        -> list[tuple[int, str]]:
    if str(filename) in _preprocessed_file_cache:
        return _preprocessed_file_cache[str(filename)]

    filename_path = Path(filename)
    result = list()
    with filename_path.open() as f:
        for line_number, line in enumerate(f, start=1):
            if (line:=_COMMENT_REGEX.sub("", line)):
                line = line.strip()
                if line:
                    result.append((line_number, line))

        _preprocessed_file_cache[str(filename)] = result
        return result


def parse_result_file(filename: str | bytes | os.PathLike) \
        -> dict[str, lit.Test.ResultCode]:
    if str(filename) in _expected_result_entry_cache:
        return _expected_result_entry_cache[str(filename)]

    res = dict()
    for line_number, line in _parse_commented_file(filename):
        m = _EXPECTED_RESULT_REGEX.prefixmatch(line)
        if m is None:
            raise Exception(f'Incorrectly formatted line {line_number}: "{line}" in {filename}.')
        prefix = m.group("prefix")
        result = m.group("result")
        result_code = getattr(lit.Test, result, None)
        if result_code is None:
            result_code = getattr(stl.test.tests, result, None)
        if result_code is None:
            raise Exception(f'Unknown result code "{result}" in line {line_number}: "{line}" in {filename}.')
        res[prefix] = result_code

    _expected_result_entry_cache[str(filename)] = res
    return res


def parse_env_lst_file(env_list: str | bytes | os.PathLike) \
        -> tuple[EnvEntry, ...]:
    if str(env_list) in _envlst_cache:
        return _envlst_cache[str(env_list)]

    env_list_path = Path(env_list)
    ctx = _ParseCtx()
    _parse_env_lst(env_list_path, ctx)
    ctx.result.append(ctx.current)
    res = tuple(map(EnvEntry, _do_crosslist(ctx)))
    _envlst_cache[env_list] = res
    return res
