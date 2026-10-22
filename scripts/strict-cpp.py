Import("env")

env.Append(CXXFLAGS=[
    "-Werror=suggest-override",
    "-Werror=reorder",
])