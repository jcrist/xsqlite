from __future__ import annotations

import importlib.resources
import os
from sqlite3 import Connection, OperationalError


def extensions() -> dict[str, str]:
    out = {}
    for path in importlib.resources.files("xsqlite.exts").iterdir():
        path = path.resolve()
        name = path.stem.removeprefix("lib").removeprefix("xsqlite")
        out[name] = str(path)
    return out


def install(con: Connection, exts: str | list[str] | None = None) -> None:
    ext_mapping = extensions()
    if exts is None:
        paths = set(ext_mapping.values())
    else:
        if isinstance(exts, str):
            exts = [exts]
        paths = set()
        for ext in exts:
            try:
                paths.add(ext_mapping[ext])
            except KeyError:
                if os.path.exists(ext):
                    paths.add(ext)
                else:
                    raise ValueError(f"Unknown extension {ext!r}")

    if not paths:
        return

    first, *rest = paths
    disable = False

    try:
        try:
            con.load_extension(first)
        except OperationalError:
            con.enable_load_extension(True)
            disable = True
            con.load_extension(first)
        for path in rest:
            con.load_extension(path)
    finally:
        if disable:
            con.enable_load_extension(False)
