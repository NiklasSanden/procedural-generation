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
        -- source
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp", 
        "%{prj.name}/src/**.c",

        -- shaders
        "%{prj.name}/src/shaders/**.vert",
        "%{prj.name}/src/shaders/**.frag", 
        "%{prj.name}/src/shaders/**.geom",
        "%{prj.name}/src/shaders/**.comp",
        "%{prj.name}/src/shaders/**.tese", 
        "%{prj.name}/src/shaders/**.tesc",
        "%{prj.name}/src/shaders/**.glsl",

        -- includes
        "%{prj.name}/include/GLAD/glad.c",
        "%{prj.name}/include/stb/stb_image.cpp"

        -- not necessary
        --"%{prj.name}/include/**.h",
        --"%{prj.name}/include/**.cpp", 
        --"%{prj.name}/include/**.c"
        --"%{prj.name}/include/**.hpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/include",
        "%{prj.name}/include/GLAD",
        "%{prj.name}/include/glm",
        "%{prj.name}/include/stb"
    }

    libdirs
    {
        "%{prj.name}/lib"
    }

    links
    {
        "glfw3.lib"
        --"opengl32.lib"
    }

    postbuildcommands
    {
        -- resources
        "{RMDIR} ../bin/" .. outputdir .. "/%{prj.name}/res",
        "{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/res",
        "{COPY} res ../bin/" .. outputdir .. "/%{prj.name}/res"
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
            -- shaders for debug
            "{RMDIR} ../bin/" .. outputdir .. "/%{prj.name}/src/shaders",
            "{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/src/shaders",
            "{COPY} src/shaders ../bin/" .. outputdir .. "/%{prj.name}/src/shaders"
        }
    
    filter "configurations:Release"
        defines "CFG_RELEASE"
        runtime "Release"
        optimize "On"
