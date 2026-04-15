add_rules("mode.release")
set_defaultmode("release")

set_languages("c++23")
set_policy("build.c++.modules", true)

add_repositories("BuildWithCollab https://github.com/BuildWithCollab/Packages.git")

if is_plat("windows") then
    add_cxxflags("/utf-8", { public = true })
end

-- xmake forces -D_GLIBCXX_USE_CXX11_ABI=0 for GCC < 15 when modules are on
-- (workaround for xmake#2716/#3855). That mismatches the default-new-ABI
-- Catch2 package and breaks the link. Opt back into the new ABI.
set_policy("build.c++.modules.gcc.cxx11abi", true)

add_requires("spdlog")
add_requires("fmt")
add_requires("rang")
add_requires("unordered_dense")

-- nlohmann_json PR#4952 added MSVC module support (commit 11cc676)
-- currently only available on the develop branch
add_requires("nlohmann_json develop")

option("build_tests")
    set_default(true)
    set_showmenu(true)
    set_description("Build test targets")
option_end()

option("enable_pfr")
    set_default(true)
    set_showmenu(true)
    set_description("Enable PFR backend for automatic reflection")
option_end()

if get_config("enable_pfr") then
    add_requires("pfr_non_boost")
end

if get_config("build_tests") then
    add_requires("catch2")
end

includes("lib/collab.core/xmake.lua")
