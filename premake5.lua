workspace "ProceduralGeneration"
    architecture "x64"
    startproject "ProceduralGeneration"
    
    configurations 
    { 
        "Debug", 
        "Release" 
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "ProceduralGeneration"
    location "ProceduralGeneration"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    --staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    { 
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp", 
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/**.h",
        "%{prj.name}/include/**.cpp", 
        "%{prj.name}/include/**.c"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/include"
    }

    libdirs
    {
        "%{prj.name}/lib"
    }

    links
    {
        "glfw3.lib",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "CFG_DEBUG"
        runtime "Debug"
        symbols "On"

        postbuildcommands 
        { 
            "{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/src/shaders",
            "{COPY} src/shaders ../bin/" .. outputdir .. "/%{prj.name}/src/shaders"
        }
    
    filter "configurations:Release"
        defines "CFG_RELEASE"
        runtime "Release"
        optimize "On"
