#!/usr/bin/env python3
"""Structural QA for alarm_case.scad.

Combines:
  1) static parameter/source checks for design constraints;
  2) OpenSCAD CGAL intersection probes for clearances/collisions.

It intentionally does not claim to replace a physical fit test of the real USB plug,
heat-set inserts, PCB, ribbon cable or printed tolerances.
"""
from __future__ import annotations

import argparse
import ast
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

CAMERA_ANGLES = (10, 15, 20, 25, 30, 35)


def find_openscad(explicit: str | None = None) -> str:
    candidates: list[str] = []
    if explicit:
        candidates.append(explicit)
    if os.environ.get("OPENSCAD_EXE"):
        candidates.append(os.environ["OPENSCAD_EXE"])
    found = shutil.which("openscad") or shutil.which("openscad.exe")
    if found:
        candidates.append(found)
    candidates += [r"C:\Program Files\OpenSCAD\openscad.exe", r"C:\Program Files (x86)\OpenSCAD\openscad.exe"]
    for c in candidates:
        if c and Path(c).is_file():
            return str(Path(c))
    raise FileNotFoundError("OpenSCAD no encontrado; usa --openscad o OPENSCAD_EXE")


def strip_selector(source: str) -> str:
    marker = "// ---------------- Part selector ----------------"
    if marker not in source:
        raise ValueError("No encuentro el marcador 'Part selector' en alarm_case.scad")
    return source.split(marker, 1)[0]


def parse_simple_params(source: str) -> dict[str, object]:
    params: dict[str, object] = {}
    for raw in source.splitlines():
        line = raw.split("//", 1)[0].strip()
        m = re.match(r"^([A-Za-z_]\w*)\s*=\s*(.+?)\s*;\s*$", line)
        if not m:
            continue
        key, expr = m.groups()
        if key == "PART":
            continue
        normalized = re.sub(r"\btrue\b", "True", expr, flags=re.I)
        normalized = re.sub(r"\bfalse\b", "False", normalized, flags=re.I)
        try:
            value = ast.literal_eval(normalized)
        except Exception:
            try:
                value = float(expr)
                if value.is_integer():
                    value = int(value)
            except Exception:
                continue
        params[key] = value
    return params


def numeric(params: dict[str, object], name: str) -> float:
    value = params.get(name)
    if not isinstance(value, (int, float)):
        raise KeyError(f"Parámetro numérico no encontrado: {name}")
    return float(value)


def run_probe(openscad: str, base: str, snippet: str, work: Path, name: str) -> tuple[bool, str]:
    wrapper = work / f"{name}.scad"
    out_stl = work / f"{name}.stl"
    wrapper.write_text(base + "\n\n// QA probe\n" + snippet + "\n", encoding="utf-8")
    proc = subprocess.run([openscad, "-o", str(out_stl), str(wrapper)], text=True, capture_output=True)
    log = (proc.stdout or "") + "\n" + (proc.stderr or "")
    is_empty = "Current top level object is empty" in log
    # OpenSCAD commonly returns exit code 1 when the requested boolean result is empty.
    # For a QA intersection probe, that is a valid result rather than a process failure.
    if proc.returncode != 0 and not is_empty:
        raise RuntimeError(f"OpenSCAD exit={proc.returncode}: {log[-1500:]}")
    nonempty = out_stl.exists() and out_stl.stat().st_size > 0 and not is_empty
    return nonempty, log


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--scad", type=Path, default=Path("alarm_case.scad"))
    p.add_argument("--openscad", default=None)
    p.add_argument("--report-out", type=Path, default=Path("structure_report_local.txt"))
    p.add_argument("--json-out", type=Path, default=Path("qa_structure_local.json"))
    p.add_argument("--keep-temp", action="store_true")
    args = p.parse_args()

    scad = args.scad.resolve()
    source = scad.read_text(encoding="utf-8")
    base = strip_selector(source)
    params = parse_simple_params(base)
    openscad = find_openscad(args.openscad)

    checks: list[dict] = []

    def add(name: str, ok: bool, detail: str, kind: str = "static") -> None:
        checks.append({"name": name, "pass": bool(ok), "detail": detail, "kind": kind})

    # --- Parameter / source constraints ---
    try:
        case_h = numeric(params, "case_h")
        case_w = numeric(params, "case_w")
        case_d = numeric(params, "case_d")
        bus_x = numeric(params, "bus_x")
        bus_z_min = numeric(params, "bus_z_min")
        bus_z_max = numeric(params, "bus_z_max")
        bay_z = numeric(params, "bay_z")
        bay_h = numeric(params, "bay_h")
        camera_z = numeric(params, "camera_z")
        camera_cradle_h = numeric(params, "camera_cradle_h")
        m3_x = numeric(params, "m3_x")
        m3_z = numeric(params, "m3_z")
        usb_x = numeric(params, "usb_entry_x")
        usb_z = numeric(params, "usb_entry_z")
        usb_w = numeric(params, "usb_entry_w") + numeric(params, "usb_entry_clearance")
        usb_h = numeric(params, "usb_entry_h") + numeric(params, "usb_entry_clearance")

        add("Case target envelope", abs(case_h-190) <= 1 and abs(case_w-74) <= 1 and abs(case_d-44) <= 2,
            f"case={case_w:g} x {case_d:g} x {case_h:g} mm")
        add("USB entry centered", abs(usb_x) < 1e-6, f"usb_entry_x={usb_x:g} mm")
        usb_half = usb_w / 2
        horizontal_gap = abs(m3_x - usb_x) - usb_half
        add("USB clear of both M3 axes", horizontal_gap > 2.0,
            f"edge-to-M3-axis gap={horizontal_gap:.2f} mm; M3 x=±{m3_x:g}")
        add("USB below Dupont bays", usb_z + usb_h/2 < bay_z - bay_h/2,
            f"USB top z={usb_z+usb_h/2:.2f}; Dupont bottom z={bay_z-bay_h/2:.2f}")
        add("USB opens at lower case edge", usb_z - usb_h/2 <= -case_h/2,
            f"USB bottom z={usb_z-usb_h/2:.2f}; case bottom z={-case_h/2:.2f}")
        add("5V/GND buses on opposite sides", bus_x > 0 and 2*bus_x >= 50,
            f"bus centers x=±{bus_x:g}; separation={2*bus_x:g} mm")
        add("Bus supports above Dupont zone", bus_z_min - 3.5 > bay_z + bay_h/2,
            f"lowest clip approx z={bus_z_min-3.5:.1f}; bay top z={bay_z+bay_h/2:.1f}")
        bus_clip_inner_x = bus_x - 5.8/2
        camera_mech_outer_x = numeric(params, "camera_cradle_w")/2 + 2.2 + 2.0
        z_separated = bus_z_max + 3.5 < camera_z - camera_cradle_h/2
        x_separated = bus_clip_inner_x > camera_mech_outer_x
        add("Bus supports clear of camera mechanism", z_separated or x_separated,
            f"Z: clip top~{bus_z_max+3.5:.1f}, cradle bottom~{camera_z-camera_cradle_h/2:.1f}; "
            f"X: clip inner~{bus_clip_inner_x:.1f}, camera outer~{camera_mech_outer_x:.1f}")
        add("Bus supports clear of lower M3", bus_z_min - 3.5 > m3_z + 6,
            f"bus lower approx z={bus_z_min-3.5:.1f}; M3 z={m3_z:g}")
        min_a = numeric(params, "cam_min_angle")
        nom_a = numeric(params, "cam_nom_angle")
        max_a = numeric(params, "cam_max_angle")
        add("Camera angle range", min_a <= 10 and nom_a == 20 and max_a >= 35,
            f"min/nom/max={min_a:g}/{nom_a:g}/{max_a:g} deg")
        pivot_d = numeric(params, "pivot_clearance_d")
        lock_d = numeric(params, "lock_clearance_d")
        add("M2.5 pivot clearance", pivot_d > 2.5, f"pivot_clearance_d={pivot_d:g} mm")
        add("M2.5 lock clearance", lock_d > 2.5, f"lock_clearance_d={lock_d:g} mm")
    except Exception as exc:
        add("Parameter extraction", False, str(exc))

    compact = re.sub(r"\s+", "", source)
    add("Two bus sides coded", "for(side=[-1,1])" in compact, "vertical_bus_supports() iterates side=[-1,1]")
    add("Two USB strain-relief bridges coded", "for(sx=[-1,1])" in compact, "usb_strain_relief_bridges() iterates sx=[-1,1]")
    add("One-sided locking slot coded", source.count("if (sx < 0)") >= 1 and source.count("arc_slot_x(") >= 2,
        "locking slot is gated by sx < 0; opposite support is pivot-only")
    add("Front hooks module present", "module front_upper_hooks()" in source, "front_upper_hooks() found")
    add("Rear catches module present", "module rear_catches()" in source, "rear_catches() found")
    add("USB cut module present", "module usb_power_entry_cut()" in source, "usb_power_entry_cut() found")

    # --- CGAL geometry probes ---
    temp_ctx = None
    if args.keep_temp:
        work = Path(".qa_structure_tmp").resolve()
        work.mkdir(exist_ok=True)
    else:
        temp_ctx = tempfile.TemporaryDirectory(prefix="alarm_case_qa_")
        work = Path(temp_ctx.name)

    def expect_empty(name: str, snippet: str, detail: str) -> None:
        try:
            nonempty, _ = run_probe(openscad, base, snippet, work, name)
            add(name, not nonempty, detail + ("; intersection EMPTY" if not nonempty else "; intersection NON-EMPTY"), "CGAL")
        except Exception as exc:
            add(name, False, str(exc), "CGAL")

    def expect_nonempty(name: str, snippet: str, detail: str) -> None:
        try:
            nonempty, _ = run_probe(openscad, base, snippet, work, name)
            add(name, nonempty, detail + ("; overlap NON-EMPTY" if nonempty else "; overlap EMPTY"), "CGAL")
        except Exception as exc:
            add(name, False, str(exc), "CGAL")

    def expect_empty_group(group_name: str, tests: list[tuple[str, str, str]]) -> None:
        # Fast path: one CGAL render for the union of all forbidden intersections.
        # If the union is empty, every test passed. If not, rerun individually
        # so the report identifies the failing member(s).
        combined = "union(){\n" + "\n".join(f"  {snippet}" for _, snippet, _ in tests) + "\n}"
        try:
            nonempty, _ = run_probe(openscad, base, combined, work, group_name)
            if not nonempty:
                for name, _snippet, detail in tests:
                    add(name, True, detail + "; grouped intersection EMPTY", "CGAL")
                return
        except Exception as exc:
            # Fall back to individual probes below.
            pass
        for name, snippet, detail in tests:
            expect_empty(name, snippet, detail)

    camera_tests = [
        (
            f"Camera vs front @ {angle} deg",
            f"intersection(){{ front_shell(); camera_at({angle}); }}",
            "cradle must not collide with front shell/supports",
        )
        for angle in CAMERA_ANGLES
    ]
    expect_empty_group("camera_collision_group", camera_tests)

    expect_empty_group("clearance_group", [
        (
            "Front pivot M2.5 path",
            "intersection(){ front_shell(); translate([0,camera_pivot_y,camera_z]) hole_x(2.5,camera_cradle_w+12); }",
            "2.5 mm coaxial shaft probe through fixed supports",
        ),
        (
            "Cradle pivot M2.5 path",
            "intersection(){ camera_cradle(); hole_x(2.5,camera_cradle_w+5); }",
            "2.5 mm coaxial shaft probe through cradle",
        ),
        (
            "Rear M3 through-holes",
            "intersection(){ rear_shell(); for(x=[-m3_x,m3_x]) translate([x,10.5,m3_z]) rotate([90,0,0]) cylinder(d=3.0,h=25,center=true); }",
            "3.0 mm shaft probes through both rear M3 paths",
        ),
        (
            "USB plug passage",
            "intersection(){ rear_shell(); translate([usb_entry_x,case_d/4+1,usb_entry_z]) rounded_prism_y(usb_entry_w,case_d/2-2,usb_entry_h,2.0); }",
            "nominal USB rectangle through rear exterior region",
        ),
        (
            "USB downward exit",
            "intersection(){ rear_shell(); translate([usb_entry_x,case_d/4+1,-case_h/2+1]) cube([usb_entry_w,case_d/2-2,2],center=true); }",
            "bottom-edge path must be open downward",
        ),
    ])

    expect_nonempty(
        "Strain relief attached to rear wall",
        "intersection(){ rear_skin(); usb_strain_relief_bridges(); }",
        "bridges must overlap rear skin so they are not floating",
    )
    expect_nonempty(
        "Bus clips attached to rear wall",
        "intersection(){ rear_skin(); vertical_bus_supports(); }",
        "bus clip bridges must overlap rear skin",
    )
    expect_nonempty(
        "Front hooks cross seam",
        "intersection(){ front_upper_hooks(); translate([0,5,0]) cube([case_w+10,10,case_h+10],center=true); }",
        "front hooks must extend into y>0 rear half",
    )

    if temp_ctx:
        temp_ctx.cleanup()

    failed = [c for c in checks if not c["pass"]]
    report = [
        "Structural verification",
        "=======================",
        f"SCAD: {scad}",
        f"OpenSCAD: {openscad}",
        "",
    ]
    current_kind = None
    for c in checks:
        if c["kind"] != current_kind:
            current_kind = c["kind"]
            report += [f"[{current_kind}]", ""]
        report.append(f"{'PASS' if c['pass'] else 'FAIL'} - {c['name']}")
        report.append(f"       {c['detail']}")
    report += ["", f"RESULT: {'PASS' if not failed else 'FAIL'} ({len(failed)} failed check(s))"]

    args.report_out.write_text("\n".join(report) + "\n", encoding="utf-8")
    args.json_out.write_text(json.dumps({"checks": checks, "pass": not failed}, indent=2), encoding="utf-8")
    print("\n".join(report))
    print(f"Report: {args.report_out.resolve()}")
    print(f"JSON: {args.json_out.resolve()}")
    return 0 if not failed else 1


if __name__ == "__main__":
    raise SystemExit(main())
