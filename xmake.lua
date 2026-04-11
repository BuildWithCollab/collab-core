add_rules("mode.release")
set_defaultmode("release")

set_languages("c++23")
set_policy("build.c++.modules", true)

option("build_tests")
    set_default(true)
    set_showmenu(true)
    set_description("Build test targets")
option_end()

if get_config("build_tests") then
    add_requires("catch2")
end

includes("lib/collab.core/xmake.lua")
