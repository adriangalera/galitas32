from pathlib import Path

Import("env", "projenv")

if env.IsIntegrationDump():
    package_dir = env.PioPlatform().get_package_dir(
        "framework-arduinoespressif32-esp32s3-libs"
    )

    if not package_dir:
        raise RuntimeError("ESP32-S3 SDK package was not found")

    include_dir = Path(package_dir) / "include"

    if not include_dir.is_dir():
        raise RuntimeError(f"SDK include directory not found: {include_dir}")

    # Expand the recursive path into real directories.
    include_paths = [include_dir.as_posix()]
    include_paths.extend(
        path.as_posix()
        for path in sorted(include_dir.rglob("*"))
        if path.is_dir()
    )

    env.AppendUnique(CPPPATH=include_paths)
    projenv.AppendUnique(CPPPATH=include_paths)

    print(f"[IntelliSense] Added {len(include_paths)} SDK include directories")