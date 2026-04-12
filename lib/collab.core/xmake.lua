target("collab.core")
    set_kind("static")
    add_files("src/**.cpp")
    add_files("src/**.cppm", { public = true })
    add_packages("boost_pfr", { public = true })

if get_config("build_tests") then
    target("tests-collab.core")
        set_kind("binary")
        add_files("tests/**.cpp")
        add_deps("collab.core")
        add_packages("catch2")
end
