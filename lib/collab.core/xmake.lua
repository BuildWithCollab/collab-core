target("collab.core")
    set_kind("static")
    add_files("src/**.cpp")
    add_files("src/**.cppm", { public = true })
    add_packages("fmt", { public = true })
    add_packages("spdlog")
    add_packages("rang")

if get_config("build_tests") then
    target("tests-collab.core")
        set_kind("binary")
        add_files("tests/**.cpp")
        add_deps("collab.core")
        add_packages("catch2")
end
