add_rules("mode.release")
set_defaultmode("release")

set_languages("c++23")
set_policy("build.c++.modules", true)

if is_plat("windows") then
    add_cxxflags("/utf-8")
end

-- xmake forces -D_GLIBCXX_USE_CXX11_ABI=0 for GCC < 15 when modules are on
-- (workaround for xmake#2716/#3855). That mismatches the default-new-ABI
-- Catch2 package and breaks the link. Opt back into the new ABI.
set_policy("build.c++.modules.gcc.cxx11abi", true)

add_requires("spdlog")
add_requires("fmt")
add_requires("rang")

option("build_tests")
    set_default(true)
    set_showmenu(true)
    set_description("Build test targets")
option_end()

if get_config("build_tests") then
    add_requires("catch2")
end

includes("lib/collab.core/xmake.lua")
