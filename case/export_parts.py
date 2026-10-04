#!/usr/bin/env python3
"""Export the printable parts from alarm_case.scad using OpenSCAD CLI."""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

PARTS = {
    "front": "front_shell.stl",
    "rear": "rear_shell.stl",
    "camera_cradle": "camera_cradle.stl",
    "dupont_2": "dupont_insert_2pin.stl",
    "dupont_3": "dupont_insert_3pin.stl",
    "dupont_4": "dupont_insert_4pin.stl",
    "dupont_6": "dupont_insert_6pin.stl",
    "angle_gauge": "camera_angle_gauge.stl",
}


def find_openscad(explicit: str | None = None) -> str:
    candidates: list[str] = []
    if explicit:
        candidates.append(explicit)
    if os.environ.get("OPENSCAD_EXE"):
        candidates.append(os.environ["OPENSCAD_EXE"])
    found = shutil.which("openscad") or shutil.which("openscad.exe")
    if found:
        candidates.append(found)
    candidates += [
        r"C:\Program Files\OpenSCAD\openscad.exe",
        r"C:\Program Files (x86)\OpenSCAD\openscad.exe",
    ]
    for candidate in candidates:
        if candidate and Path(candidate).is_file():
            return str(Path(candidate))
    raise FileNotFoundError(
        "No encuentro OpenSCAD. Usa --openscad \"C:\\Program Files\\OpenSCAD\\openscad.exe\" "
        "o define OPENSCAD_EXE."
    )


def export_all(scad: Path, out_dir: Path, openscad: str, verbose: bool = True) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    for selector, filename in PARTS.items():
        target = out_dir / filename
        cmd = [openscad, "-o", str(target), "-D", f'PART="{selector}"', str(scad)]
        if verbose:
            print(f"[EXPORT] {filename}  <- PART=\"{selector}\"")
        proc = subprocess.run(cmd, text=True, capture_output=True)
        if proc.returncode != 0 or not target.exists() or target.stat().st_size == 0:
            sys.stderr.write(proc.stdout)
            sys.stderr.write(proc.stderr)
            raise RuntimeError(f"OpenSCAD no pudo generar {filename} (exit={proc.returncode})")
        if verbose and proc.stderr.strip():
            warnings = [x for x in proc.stderr.splitlines() if "WARNING" in x.upper()]
            for line in warnings:
                print(f"  {line}")


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--scad", type=Path, default=Path("alarm_case.scad"))
    p.add_argument("--out", type=Path, default=Path("stl"))
    p.add_argument("--openscad", default=None)
    args = p.parse_args()

    scad = args.scad.resolve()
    if not scad.is_file():
        p.error(f"No existe: {scad}")
    try:
        openscad = find_openscad(args.openscad)
        print(f"OpenSCAD: {openscad}")
        export_all(scad, args.out.resolve(), openscad)
    except Exception as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2
    print(f"OK: STL exportados en {args.out.resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
