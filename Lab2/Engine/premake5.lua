project "Engine"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    targetdir("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("build/bin-int/" .. outputdir .. "/%{prj.name}")

	-- Define the location of pre-compiled header.
    -- pchheader "PCH.h"
	-- pchsource "source/PCH.cpp"

    -- Define the location(s) of files.
    files {
        "Source/**.h",
		"Source/**.cpp",
		"Shaders/**.hlsl"
    }
	
    -- Define any includes of folder(s).
    -- Useful to remove any "../folder/file.h" in code.
    includedirs {
        "source",
        "../Engine/source",
        "../ThirdParty/STB/",
    }
	
    vpaths {
		
    }
	
	links { "d3d11" }

    filter {"system:windows"}
        defines {
            "WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }
		
    filter {"configurations:Debug"}
        kind "WindowedApp"
        runtime "Debug"
        defines{"_DEBUG", "_UNICODE", "UNICODE"}
        symbols "on"

    filter {"configurations:Release"}
        kind "WindowedApp"
        runtime "Release"
        defines{"NDEBUG", "_UNICODE", "UNICODE"}
        optimize "on"
		
	filter "*"
        local ws = "shaders/%%(Filename).cso"
        
        files("*.hlsl")
            shadermodel("5.0")
            shaderobjectfileoutput(ws)

        filter("files:**VertexShader.hlsl")
            shadertype("Vertex")

        filter("files:**PixelShader.hlsl")
            shadertype("Pixel")
            