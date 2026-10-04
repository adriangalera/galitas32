# Verificación local de `alarm_case.scad`

Estos scripts reproducen de forma automatizable las comprobaciones del ZIP: exportación CGAL con OpenSCAD, QA topológica de STL y varias comprobaciones estructurales/collisions.

## Requisitos (Windows)

- OpenSCAD instalado, normalmente en `C:\Program Files\OpenSCAD\openscad.exe`.
- Python 3.10+.
- PowerShell.

Pon esta carpeta junto a `alarm_case.scad` y, opcionalmente, `qa_mesh_v4.json`.

## Ejecución rápida

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\run_verification.ps1 -Scad .\alarm_case.scad -OpenSCAD "C:\Program Files\OpenSCAD\openscad.exe"
```

Si `openscad.exe` ya está en `PATH`, basta:

```powershell
.\run_verification.ps1 -Scad .\alarm_case.scad
```

El script crea `.venv`, instala `trimesh`, exporta los STL y deja los resultados en `verification_output`.

## Qué genera

- `verification_output/stl/*.stl`: piezas exportadas desde `PART`.
- `structure_report_local.txt`: comprobaciones de USB, buses, M3, pivote, cámara 10/15/20/25/30/35°, unión de clips/strain-relief, etc.
- `mesh_report_local.txt`: watertight, manifold edges, winding, componentes conectados, caras degeneradas, dimensiones y cama de impresión.
- `qa_structure_local.json` y `qa_mesh_local.json`: resultados en formato máquina.

## Exportar solamente los STL

```powershell
python .\export_parts.py --scad .\alarm_case.scad --out .\stl --openscad "C:\Program Files\OpenSCAD\openscad.exe"
```

La llamada a OpenSCAD se hace como argumento separado:

```text
-D PART="front"
```

Esto evita los problemas de comillas típicos de PowerShell al intentar escribir manualmente `-D PART=...`.

## Verificar solamente las mallas

```powershell
python .\verify_meshes.py --stl-dir .\stl --reference .\qa_mesh_v4.json
```

La cama por defecto es 220 x 220 x 250 mm (Artillery Genius). Puedes cambiarla:

```powershell
python .\verify_meshes.py --stl-dir .\stl --bed 220 220 250
```

## Verificar solamente la estructura SCAD

```powershell
python .\verify_structure.py --scad .\alarm_case.scad --openscad "C:\Program Files\OpenSCAD\openscad.exe"
```

Las pruebas CGAL generan intersecciones temporales para comprobar, entre otras cosas:

- cradle vs frontal en 10°, 15°, 20°, 25°, 30° y 35°;
- paso coaxial M2.5 por soportes y cradle;
- paso de los dos M3 por la trasera;
- paso de la abertura USB y salida hacia abajo;
- unión física de strain-relief y clips de bus con la pared trasera;
- hooks del frontal cruzando el plano de unión.

También valida relaciones paramétricas: separación de buses, zonas Z de buses/Dupont/cámara/M3, rango angular y posición de USB.

## Límites importantes

Estas pruebas verifican geometría CAD y malla, pero no pueden certificar por sí solas:

- que tu conector USB físico real pase por el hueco;
- tolerancias reales de tu impresora/material;
- ajuste real de insertos térmicos M3/M2.5;
- flexibilidad/radio real del ribbon de cámara;
- acceso real del soldador con todos los cables montados.

Esas comprobaciones requieren medir el hardware y, para el cierre/encajes críticos, una impresión de prueba.
