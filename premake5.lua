workspace "LarianBreakout"
    architecture "x64"
    startproject "Breakout" 

    configurations { "Debug", "Release", "Dist" }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/GLFW"
    include "vendor/Glad"
group ""

project "Breakout"
    kind "ConsoleApp" 
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Engine/**.h",
        "Engine/**.cpp",
        "Game/**.h",
        "Game/**.cpp"
    }

    includedirs {
        "Engine",
        "Game",
        "vendor/spdlog/include",
        "vendor/GLFW/include",
        "vendor/Glad/include",
        "vendor/glm",
        "vendor/stb_image", 
        "vendor/miniaudio"
    }

    links {
        "GLFW",
        "Glad",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"


    filter "configurations:Debug"
        defines "ARK_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ARK_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "ARK_DIST"
        kind "WindowedApp" 
        runtime "Release"
        optimize "on"