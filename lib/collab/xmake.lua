target("collab")
    set_kind("static")
    add_files("src/**.cpp")
    add_files("src/**.cppm", { public = true })
    add_packages("collab-hpp", { public = true })
    add_packages("spdlog")
    add_packages("rang")
    -- Use external fmt in spdlog so its bundled fmt doesn't collide with the
    -- standalone fmt reached through collab-hpp's header unit.
    add_defines("SPDLOG_FMT_EXTERNAL")

if get_config("build_tests") then
    target("tests-collab")
        set_kind("binary")
        add_files("tests/*.cpp")
        add_deps("collab")
        add_packages("catch2")
        set_rundir("$(projectdir)")
        add_tests("default", {runargs = {"--durations", "yes"}})
end
