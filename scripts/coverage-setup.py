# python -m pip install gcovr
Import("env")

env.Append(
    CCFLAGS=["--coverage", "-O0", "-g"],
    LINKFLAGS=["--coverage"],
)