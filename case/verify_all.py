#!/usr/bin/env python3
"""Export all parts, run structural QA, then validate all meshes."""
from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys


def run(cmd: list[str]) -> int:
    print("\n> " + " ".join(f'"{x}"' if " " in x else x for x in cmd))
    return subprocess.run(cmd).returncode


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--scad", type=Path, default=Path("alarm_case.scad"))
    p.add_argument("--openscad", default=None)
    p.add_argument("--reference", type=Path, default=None)
    p.add_argument("--out", type=Path, default=Path("verification_output"))
    args = p.parse_args()

    here = Path(__file__).resolve().parent
    out = args.out.resolve()
    stl = out / "stl"
    out.mkdir(parents=True, exist_ok=True)

    common_open = ["--openscad", args.openscad] if args.openscad else []
    rc1 = run([sys.executable, str(here / "export_parts.py"), "--scad", str(args.scad), "--out", str(stl), *common_open])
    if rc1 != 0:
        return rc1

    rc2 = run([
        sys.executable, str(here / "verify_structure.py"),
        "--scad", str(args.scad),
        "--report-out", str(out / "structure_report_local.txt"),
        "--json-out", str(out / "qa_structure_local.json"),
        *common_open,
    ])

    mesh_cmd = [
        sys.executable, str(here / "verify_meshes.py"),
        "--stl-dir", str(stl),
        "--report-out", str(out / "mesh_report_local.txt"),
        "--json-out", str(out / "qa_mesh_local.json"),
    ]
    if args.reference and args.reference.is_file():
        mesh_cmd += ["--reference", str(args.reference)]
    rc3 = run(mesh_cmd)

    print("\n==========================")
    print("FINAL:", "PASS" if rc2 == 0 and rc3 == 0 else "FAIL")
    print("Output:", out)
    return 0 if rc2 == 0 and rc3 == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
