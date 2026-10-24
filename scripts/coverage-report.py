import platform
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "coverage"


def main():
    OUTPUT.mkdir(exist_ok=True)

    gcov = (
        "xcrun llvm-cov gcov"
        if platform.system() == "Darwin"
        else "gcov"
    )

    subprocess.run(
        [
            sys.executable, "-m", "gcovr",
            "-r", ".",
            ".pio/build/native",
            "--filter", "lib/AlarmCore/src/",
            "--gcov-executable", gcov,
            "--html-details", "coverage/index.html",
            "--print-summary",
            "--html-details", "coverage/index.html",
            "--json-summary", "coverage/summary.json",
            "--print-summary",
        ],
        cwd=ROOT,
        check=True,
    )

    print(f"\nInforme: {OUTPUT / 'index.html'}")


if __name__ == "__main__":
    main()
