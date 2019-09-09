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
        "%{prj.name}/include/stb_image.cpp",
        "%{prj.name}/include/imgui/imgui.cpp",
        "%{prj.name}/include/imgui/imgui_demo.cpp",
        "%{prj.name}/include/imgui/imgui_draw.cpp",
        "%{prj.name}/include/imgui/imgui_widgets.cpp",
        "%{prj.name}/include/imgui/imgui_impl_glfw.cpp",
        "%{prj.name}/include/imgui/imgui_impl_opengl3.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/include",
        "%{prj.name}/include/GLAD",
        "%{prj.name}/include/glm",
        "%{prj.name}/include/stb",
        "%{prj.name}/include/PhysX"
    }

    libdirs
    {
        "%{prj.name}/lib",
        "%{prj.name}/lib/PhysX"
    }

    links
    {
        "glfw3.lib",
        
        "PhysX_64.lib",
        "PhysXCommon_64.lib",
        "PhysXCooking_64.lib",
        "PhysXExtensions_static_64.lib",
        "PhysXFoundation_64.lib"
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
