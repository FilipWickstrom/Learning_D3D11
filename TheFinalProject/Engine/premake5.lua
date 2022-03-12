project "Engine"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    targetdir("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("build/bin-int/" .. outputdir .. "/%{prj.name}")

	-- Define the location of pre-compiled header.
    pchheader "PCH.h"
	pchsource "source/PCH.cpp"

    -- Define the location(s) of files.
    files {
        "source/**.h",
		"source/**.cpp",
		"shaders/**.hlsl"
    }
	
    -- Define any includes of folder(s).
    -- Useful to remove any "../folder/file.h" in code.
    includedirs {
        "source",
        "../Engine/source",
        "../ThirdParty/directXTK/Include/",
		"../ThirdParty/stb/Include/"
    }
	
    vpaths {
		["source/BackFaceCulling"] 		= {"**BackFaceCulling.*"},
		["source/Input"]				= {"**InputHandler.*"},
		["source/Camera"] 				= {"**Camera.*"},
		["source/Deferred Rendering"] 	= {"**GeometryPass.*", "**LightingPass.*", "**InitFunctions.*", "**ScreenQuad.*"},
		["source/Handlers"] 			= {"**ConstantBuffers.*", "**FPSCounter.*", "**WindowHandler.*"},
		["source/Renderer"] 			= {"**Main.*", "**Rasterizer.*", "**Renderer.*"},
		["source/PostProcessing"]		= {"**PostProcessing.*"},
		["source/Scene"] 				= {"**MeshObject.*", "**Scene.*"},
		["source/Shadow"] 				= {"**ShadowMap.*"},
		["source/Tessellation"] 		= {"**Tessellation.*"}
    }
	
	links { "d3d11",
			"DirectXTK.lib" 
	}

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
        libdirs{ "../ThirdParty/directXTK/Lib/Debug/" }

    filter {"configurations:Release"}
        kind "WindowedApp"
        runtime "Release"
        defines{"NDEBUG", "_UNICODE", "UNICODE"}
        optimize "on"
        libdirs{ "../ThirdParty/directXTK/Lib/Release/" }
		
	filter "*"
        local ws = "shaders/%%(Filename).cso"
        
        files("*.hlsl")
            shadermodel("5.0")
            shaderobjectfileoutput(ws)

        filter("files:**VS.hlsl")
            shadertype("Vertex")

        filter("files:**PS.hlsl")
            shadertype("Pixel")
            
        filter("files:**GS.hlsl")
            shadertype("Geometry")
           
        filter("files:**CS.hlsl")
            shadertype("Compute")
			
		filter("files:**DS.hlsl")
            shadertype("Domain")
			
		filter("files:**HS.hlsl")
            shadertype("Hull")