add_rules("mode.release")

-- Default to release simply because failed ASSERT causes annoying popups in debug mode :P
set_defaultmode("release")
set_languages("c++23")

option("build_tests")
    set_default(true)
    set_showmenu(true)
    set_description("Build test targets")
option_end()

add_requires("platformfolders")

if get_config("build_tests") then
    add_requires("catch2 3.x")
end

target("collab-core")
    set_kind("static")
    add_files("src/(**.cpp)")
    add_headerfiles("include/(**.hpp)")
    add_includedirs("include", { public = true })

if get_config("build_tests") then
    includes("tests/xmake.lua")
end
