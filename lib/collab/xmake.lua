target("collab")
    set_kind("static")
    add_files("src/**.cpp")
    add_files("src/**.cppm", { public = true })
    add_packages("collab-hpp", { public = true })
    add_packages("spdlog")
    add_packages("rang")
    -- Tell spdlog to use std::format instead of any fmt path. Otherwise spdlog
    -- pulls fmt into log.cpp's TU, which already sees fmt via the module's
    -- header-unit import of collab-hpp → C2752 partial-specialization conflict.
    add_defines({"SPDLOG_USE_STD_FORMAT"})

if get_config("build_tests") then
    target("tests-collab")
        set_kind("binary")
        add_files("tests/*.cpp")
        add_deps("collab")
        add_packages("catch2")
        set_rundir("$(projectdir)")
        add_tests("default", {runargs = {"--durations", "yes"}})
end
