workspace "TerrariaClone"
    architecture "x86_64"
    configurations { "Debug", "Release" }
    -- Put TerrariaClone.sln in the repo root; project files stay in build/.
    location "."
    startproject "TerrariaClone"
    -- Exported engine classes have private STL members. This is safe because
    -- the engine and game are always built together with the same compiler/CRT.
    disablewarnings { "4251" }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter {}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
project "GLFW"
    location "build/GLFW"
    kind "StaticLib"
    language "C"
    staticruntime "Off"
    targetdir ("build/bin/" .. outputdir)
    objdir ("build/obj/" .. outputdir .. "/%{prj.name}")

    defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }
    includedirs { "vendor/glfw/include", "vendor/glfw/src" }
    files {
        "vendor/glfw/include/GLFW/*.h",
        "vendor/glfw/src/*.h",
        "vendor/glfw/src/context.c",
        "vendor/glfw/src/init.c",
        "vendor/glfw/src/input.c",
        "vendor/glfw/src/monitor.c",
        "vendor/glfw/src/platform.c",
        "vendor/glfw/src/vulkan.c",
        "vendor/glfw/src/window.c",
        "vendor/glfw/src/egl_context.c",
        "vendor/glfw/src/osmesa_context.c",
        "vendor/glfw/src/null_init.c",
        "vendor/glfw/src/null_monitor.c",
        "vendor/glfw/src/null_window.c",
        "vendor/glfw/src/null_joystick.c",
        "vendor/glfw/src/win32_init.c",
        "vendor/glfw/src/win32_joystick.c",
        "vendor/glfw/src/win32_module.c",
        "vendor/glfw/src/win32_monitor.c",
        "vendor/glfw/src/win32_thread.c",
        "vendor/glfw/src/win32_time.c",
        "vendor/glfw/src/win32_window.c",
        "vendor/glfw/src/wgl_context.c"
    }

group ""

project "Engine"
    location "build/Engine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    targetdir ("build/bin/" .. outputdir)
    objdir ("build/obj/" .. outputdir .. "/%{prj.name}")

    files { "Engine/src/**.h", "Engine/src/**.cpp" }
    defines { "ENGINE_BUILD_DLL" }
    includedirs {
        "Engine/src",
        "vendor/glfw/include",
        "vendor/glfw/deps",
        "vendor/glm",
        "vendor/stb",
        "vendor/OpenGL-Registry/api"
    }
    links { "GLFW", "opengl32" }

project "TerrariaClone"
    location "build/TerrariaClone"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    -- Same output directory as Engine so TerrariaClone.exe finds Engine.dll.
    targetdir ("build/bin/" .. outputdir)
    objdir ("build/obj/" .. outputdir .. "/%{prj.name}")

    files { "TerrariaClone/src/**.h", "TerrariaClone/src/**.cpp" }
    -- Run from the repo root in Visual Studio so "assets/..." paths resolve.
    debugdir "."
    includedirs { "TerrariaClone/src", "Engine/src", "vendor/glm" }
    links { "Engine" }

-- Compiles the engine sources directly (ENGINE_STATIC) so the test can also
-- call OpenGL itself to read back pixels.
project "RendererTests"
    location "build/RendererTests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    targetdir ("build/bin/" .. outputdir)
    objdir ("build/obj/" .. outputdir .. "/%{prj.name}")

    files { "tests/RendererTests.cpp", "Engine/src/**.h", "Engine/src/**.cpp" }
    defines { "ENGINE_STATIC" }
    includedirs { "Engine/src", "vendor/glfw/include", "vendor/glfw/deps", "vendor/glm", "vendor/stb" }
    links { "GLFW", "opengl32" }
