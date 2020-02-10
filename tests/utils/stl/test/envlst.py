from collections import namedtuple
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, FrozenSet, List, Optional, Set, TextIO, Tuple
import itertools
import re

_envlst_cache = dict()


@dataclass
class _TmpEnvEntry:
    tags: Set[str] = field(default_factory=set)
    env: Dict[str, str] = field(default_factory=dict)


@dataclass(frozen=True)
class EnvEntry:
    def __init__(self, tmp_env: _TmpEnvEntry):
        object.__setattr__(self, "_tags", frozenset(tmp_env.tags))
        object.__setattr__(self, "_env_keys", tuple(tmp_env.env.keys()))
        object.__setattr__(self, "_env_vals", tuple(tmp_env.env.values()))

    def getEnvVal(self, key: str, default: Optional[str] = None)\
            -> Optional[str]:
        if key not in self._env_keys:
            return default

        return self._env_vals[self._env_keys.index(key)]

    def hasTag(self, tag: str) -> bool:
        return tag in self._env_entry.tags

    _tags: FrozenSet[str]
    _env_keys: Tuple[str]
    _env_vals: Tuple[str]


@dataclass
class _ParseCtx:
    current: List[_TmpEnvEntry] = field(default_factory=list)
    result: List[List[_TmpEnvEntry]] = field(default_factory=list)


_ENV_REGEX = re.compile(r"((?P<tags>(?:\w+,?)+)\t+)?(?P<env_vars_part>.+$)")
_COMMENT_REGEX = re.compile(r"#.*$")
_INCLUDE_REGEX = re.compile(r'^RUNALL_INCLUDE (?P<filename>.+$)')
_ENV_VAR_MULTI_ITEM_REGEX = re.compile(r'(?P<name>\w+)="(?P<value>.*?)"')
_CROSSLIST_REGEX = re.compile(r"^RUNALL_CROSSLIST$")


def _parse_env_line(line: str) -> Optional[_TmpEnvEntry]:
    line = _COMMENT_REGEX.sub("", line)
    m = _ENV_REGEX.match(line)
    if m is not None:
        result = _TmpEnvEntry()
        tags = m.group("tags")
        if tags is not None:
            result.tags = set(tags.split(','))
        for env_match in _ENV_VAR_MULTI_ITEM_REGEX.finditer(
                m.group("env_vars_part")):
            name = env_match.group("name")
            value = env_match.group("value")
            result.env[env_match.group("name")] = env_match.group("value")
        return result
    return None


def _append_env_entries(*args) -> _TmpEnvEntry:
    result = _TmpEnvEntry()
    for entry in args:
        result.tags.union(entry.tags)
        for k, v in entry.env.items():
            if k not in result.env:
                result.env[k] = v
            else:
                result.env[k] = ' '.join((result.env[k], v))
    return result


def _do_crosslist(ctx: _ParseCtx):
    return itertools.starmap(_append_env_entries,
                             itertools.product(*ctx.result))


def _parse_env_lst(env_list: TextIO, base_path: Path, ctx: _ParseCtx):
    for line in env_list.readlines():
        if (m:=_INCLUDE_REGEX.match(line)) is not None:
            p = base_path / Path(m.group("filename"))
            with p.open() as f:
                _parse_env_lst(f, p.parent, ctx)
        elif _CROSSLIST_REGEX.match(line) is not None:
            ctx.result.append(ctx.current)
            ctx.current = []
        else:
            entry = _parse_env_line(line)
            if(entry is not None):
                ctx.current.append(entry)
    ctx.result.append(ctx.current)
    return _do_crosslist(ctx)


def parse_env_lst_file(env_list: str) -> Tuple[EnvEntry, ...]:
    if env_list in _envlst_cache:
        return _envlst_cache[env_list]

    env_list_path = Path(env_list)
    ctx = _ParseCtx()
    with Path(env_list).open() as f:
        res = tuple(map(EnvEntry,
                        _parse_env_lst(f, env_list_path.parent, ctx)))
        _envlst_cache[env_list] = res
        return res
