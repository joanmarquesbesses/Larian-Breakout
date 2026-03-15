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
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Engine/**.h",
        "Engine/**.cpp",
        "Game/**.h",
        "Game/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp",
        "vendor/stb_truetype/**.h",
        "vendor/stb_truetype/**.cpp"
    }

    includedirs {
        "Engine",
        "Game",
        "vendor/spdlog/include",
        "vendor/GLFW/include",
        "vendor/Glad/include",
        "vendor/glm",
        "vendor/stb_image", 
        "vendor/stb_truetype",
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