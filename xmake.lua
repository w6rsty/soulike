add_rules("mode.debug", "mode.release")

set_languages("c++20")

add_requires("SDL3", {system = true})
add_requires("lua 5.4.7", "glm")

target("soulike")
    set_kind("binary")
    add_includedirs("src", {public = true})
    add_files("src/*.cpp")
    add_packages("SDL3", "lua", "glm")
    set_rundir("$(projectdir)")
