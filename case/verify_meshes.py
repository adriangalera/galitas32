#!/usr/bin/env python3
"""Validate STL topology, dimensions, connected components and printer envelope."""
from __future__ import annotations

import argparse
import itertools
import json
from pathlib import Path
import sys

import numpy as np
import trimesh

REQUIRED = [
    "front_shell.stl",
    "rear_shell.stl",
    "camera_cradle.stl",
    "camera_angle_gauge.stl",
]


def load_mesh(path: Path) -> trimesh.Trimesh:
    obj = trimesh.load(path, force="mesh", process=True)
    if not isinstance(obj, trimesh.Trimesh):
        raise TypeError(f"{path.name}: no se pudo cargar como Trimesh")
    return obj


def manifold_edges(mesh: trimesh.Trimesh) -> bool:
    # In a closed 2-manifold every undirected edge is used by exactly two faces.
    inv = mesh.edges_unique_inverse
    counts = np.bincount(inv, minlength=len(mesh.edges_unique))
    return bool(np.all(counts == 2))


def nondegenerate(mesh: trimesh.Trimesh) -> tuple[bool, int]:
    try:
        mask = mesh.nondegenerate_faces()
        bad = int((~mask).sum())
    except Exception:
        areas = np.asarray(mesh.area_faces)
        bad = int(np.sum(~np.isfinite(areas) | (areas <= 1e-12)))
    return bad == 0, bad


def fits_envelope(extents: np.ndarray, bed: tuple[float, float, float]) -> tuple[bool, tuple[float, float, float] | None]:
    e = tuple(float(x) for x in extents)
    for perm in set(itertools.permutations(e)):
        if all(perm[i] <= bed[i] + 1e-6 for i in range(3)):
            return True, perm
    return False, None


def compare_reference(name: str, result: dict, ref: dict | None, tol_mm: float, tol_volume_pct: float) -> list[str]:
    notes: list[str] = []
    if not ref or name not in ref:
        return notes
    r = ref[name]
    if "extents_mm" in r:
        delta = np.abs(np.asarray(result["extents_mm"]) - np.asarray(r["extents_mm"]))
        if np.any(delta > tol_mm):
            notes.append(f"extents difieren de referencia (max delta={delta.max():.3f} mm)")
    if "volume_mm3" in r and r["volume_mm3"]:
        pct = abs(result["volume_mm3"] - float(r["volume_mm3"])) / abs(float(r["volume_mm3"])) * 100
        if pct > tol_volume_pct:
            notes.append(f"volumen difiere de referencia ({pct:.2f}%)")
    return notes


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--stl-dir", type=Path, default=Path("stl"))
    p.add_argument("--reference", type=Path, default=None, help="qa_mesh_v4.json opcional")
    p.add_argument("--json-out", type=Path, default=Path("qa_mesh_local.json"))
    p.add_argument("--report-out", type=Path, default=Path("mesh_report_local.txt"))
    p.add_argument("--bed", nargs=3, type=float, default=(220.0, 220.0, 250.0), metavar=("X", "Y", "Z"))
    p.add_argument("--reference-tol-mm", type=float, default=0.15)
    p.add_argument("--reference-tol-volume-pct", type=float, default=1.0)
    args = p.parse_args()

    stl_dir = args.stl_dir.resolve()
    reference = None
    if args.reference:
        reference = json.loads(args.reference.read_text(encoding="utf-8"))

    results: dict[str, dict] = {}
    report: list[str] = ["Mesh verification", "=================", f"STL dir: {stl_dir}", f"Printer envelope: {tuple(args.bed)} mm", ""]
    failures = 0

    for name in REQUIRED:
        path = stl_dir / name
        if not path.is_file():
            failures += 1
            report += [f"{name}", "  FAIL: fichero no encontrado", ""]
            continue
        try:
            mesh = load_mesh(path)
            components = list(mesh.split(only_watertight=False))
            deg_ok, deg_count = nondegenerate(mesh)
            fits, orientation = fits_envelope(mesh.extents, tuple(args.bed))
            entry = {
                "watertight": bool(mesh.is_watertight),
                "manifold_edges": manifold_edges(mesh),
                "winding_consistent": bool(mesh.is_winding_consistent),
                "connected_components": len(components),
                "degenerate_faces": deg_count,
                "bounds_mm": np.asarray(mesh.bounds).round(4).tolist(),
                "extents_mm": np.asarray(mesh.extents).round(4).tolist(),
                "volume_mm3": round(float(abs(mesh.volume)), 3),
                "fits_printer_envelope": fits,
                "fitting_orientation_mm": list(orientation) if orientation else None,
            }
            entry["reference_warnings"] = compare_reference(
                name, entry, reference, args.reference_tol_mm, args.reference_tol_volume_pct
            )
            checks = {
                "watertight": entry["watertight"],
                "manifold": entry["manifold_edges"],
                "winding": entry["winding_consistent"],
                "one_component": entry["connected_components"] == 1,
                "no_degenerate_faces": deg_ok,
                "fits_printer": fits,
                "reference_match": not entry["reference_warnings"],
            }
            entry["pass"] = all(checks.values())
            results[name] = entry
            if not entry["pass"]:
                failures += 1

            report.append(name)
            report.append(f"  Watertight: {'PASS' if checks['watertight'] else 'FAIL'}")
            report.append(f"  Manifold edges: {'PASS' if checks['manifold'] else 'FAIL'}")
            report.append(f"  Winding consistent: {'PASS' if checks['winding'] else 'FAIL'}")
            report.append(f"  Connected components: {entry['connected_components']} ({'PASS' if checks['one_component'] else 'FAIL'})")
            report.append(f"  Degenerate faces: {deg_count} ({'PASS' if deg_ok else 'FAIL'})")
            report.append(f"  Extents mm: {entry['extents_mm']}")
            report.append(f"  Volume mm3: {entry['volume_mm3']}")
            report.append(f"  Fits Artillery Genius envelope: {'PASS' if fits else 'FAIL'}")
            if entry["reference_warnings"]:
                report.append("  Reference: FAIL/WARN - " + "; ".join(entry["reference_warnings"]))
            elif reference and name in reference:
                report.append("  Reference: PASS")
            report.append("")
        except Exception as exc:
            failures += 1
            report += [name, f"  FAIL: {exc}", ""]

    args.json_out.write_text(json.dumps(results, indent=2), encoding="utf-8")
    report += [f"RESULT: {'PASS' if failures == 0 else 'FAIL'} ({failures} failing part(s))"]
    args.report_out.write_text("\n".join(report) + "\n", encoding="utf-8")
    print("\n".join(report))
    print(f"JSON: {args.json_out.resolve()}")
    print(f"Report: {args.report_out.resolve()}")
    return 0 if failures == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
