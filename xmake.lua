-- ─────────────────────────────────────────────────────────────────────────────
-- Reference config commands (run once per session; -y is mandatory)
--
-- Windows:
--   xmake f --qt=C:/qt/6.10.2/msvc2022_64 -m release -p windows -a x64 -c -y
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

add_repositories("BuildWithCollab https://github.com/BuildWithCollab/Packages")

add_rules("mode.release")
set_defaultmode("release")

set_languages("c++23")
set_policy("build.c++.modules", true)
set_policy("build.c++.modules.gcc.cxx11abi", true)

add_requires("fmt")
add_requires("spdlog")
add_requires("rang")
add_requires("catch2")

if is_plat("windows") then
    add_cxxflags("/utf-8", { public = true })
end

-- ─── Static library ─────────────────────────────────────────────────────────
-- Aggregates the canonical inline headers, the generated module partitions
-- (src/<area>.cppm), the generated force-emission impl units
-- (src/<area>_impl.cpp), the umbrella src/collab.cppm, and the hand-written
-- native impl files (src/<area>_native.cpp).
target("collab")
    set_kind("static")
    add_files("src/**.cpp")
    add_files("src/**.cppm", { public = true })
    add_includedirs("include", { public = true })
    add_packages("fmt", { public = true })
    add_packages("spdlog")
    add_packages("rang")
target_end()

-- ─── Tests ──────────────────────────────────────────────────────────────────
-- tests-include: pure header-only — does NOT link `collab`. Exercises only
-- the surface available through <collab.hpp> with no external impl. fmt is
-- still linked (it's a transitive header dep, and we don't want FMT_HEADER_ONLY
-- in tests).
target("tests-include")
    set_kind("binary")
    add_files("tests/test_include_only.cpp")
    add_includedirs("include")
    add_packages("fmt")
    add_packages("catch2")
    set_rundir("$(projectdir)")
    add_tests("default", {runargs = {"--durations", "yes"}})
target_end()

-- tests-import: `import collab;` only, links the static library.
target("tests-import")
    set_kind("binary")
    add_files("tests/test_import_only.cpp")
    add_deps("collab")
    add_packages("catch2")
    set_rundir("$(projectdir)")
    add_tests("default", {runargs = {"--durations", "yes"}})
target_end()

-- tests-dual: both `#include <collab.hpp>` AND `import collab;` in the same TU.
-- The architecture's load-bearing test.
target("tests-dual")
    set_kind("binary")
    add_files("tests/test_dual.cpp")
    add_deps("collab")
    add_packages("catch2")
    set_rundir("$(projectdir)")
    add_tests("default", {runargs = {"--durations", "yes"}})
target_end()
