workspace "SoftwareRenderer"
toolset "clang"
configurations { "Debug", "Release" }

project "run_main"
kind "Makefile"
buildcommands {
    "make SoftwareRenderer",
    "bin/%{cfg.buildcfg}/SoftwareRenderer"
    }

    rebuildcommands {
    "%{cfg.buildcfg}/SoftwareRenderer"
    }

    cleancommands {
    ""
    }

project "SoftwareRenderer"
    includedirs { "libs/include", "libs/GLAD/include", "libs" }
    kind "ConsoleApp"
    links { "imgui", "Fonts", "GLAD", "glfw", "GL", "dl" }
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/**.cpp", "src/**.hpp"}
    
    postbuildcommands {
        "cp -r assets bin/%{cfg.buildcfg}"
    }
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"

project "imgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    files { "libs/imgui/**.cpp", "libs/imgui/**.h"}
    defines { "NDEBUG" }
    optimize "Speed"

project "GLAD"
    kind "StaticLib"
    includedirs { "libs/GLAD/include" }
    files { "libs/GLAD/**.c", "libs/GLAD/**.h" }
    language "C"
    cdialect "C99"

project "Fonts"
    kind "StaticLib"
    language "C"
    cdialect "C99"
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/**.c", "src/**.h"}
