-- ─────────────────────────────────────────────────────────────────────────────
-- Reference config commands (run once per session; -y is mandatory)
--
-- Linux / GCC 15 (release):
--   xmake f -c -y --cc=gcc-15 --cxx=g++-15 --ld=g++-15 --sh=g++-15 -m release
--
-- Linux / Clang + libc++ (release):
--   xmake f -c -y --cc=clang --cxx=clang++ --ld=clang++ --sh=clang++ \
--     --cxxflags="-stdlib=libc++" --ldflags="-stdlib=libc++"
--
-- macOS / brew LLVM (release):
--   xmake f -c -y --toolchain=llvm --sdk="$(brew --prefix llvm)"
-- ─────────────────────────────────────────────────────────────────────────────

add_rules("mode.release")
set_defaultmode("release")

set_languages("c++23")
set_policy("build.c++.modules", true)

if is_plat("windows") then
    add_cxxflags("/utf-8", { public = true })
end

set_policy("build.c++.modules.gcc.cxx11abi", true)

option("build_tests")
    set_default(true)
    set_showmenu(true)
    set_description("Build test targets")
option_end()

add_repositories("BuildWithCollab https://github.com/BuildWithCollab/Packages")

add_requires("collab-hpp")
add_requires("spdlog")
add_requires("rang")

if get_config("build_tests") then
    add_requires("catch2")
end

includes("lib/collab/xmake.lua")
