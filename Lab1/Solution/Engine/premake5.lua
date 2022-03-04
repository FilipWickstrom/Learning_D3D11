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
		"Source/**.cpp"
    }
	
    -- Define any includes of folder(s).
    -- Useful to remove any "../folder/file.h" in code.
    includedirs {
        "source",
        "../Engine/source"
    }
	
    vpaths {
		
    }
	
    filter {"system:windows"}
        defines {
            "WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }
		
    filter {"configurations:Debug"}
        kind "ConsoleApp"
        runtime "Debug"
        defines{"_DEBUG", "_UNICODE", "UNICODE"}
        symbols "on"

    filter {"configurations:Release"}
        kind "ConsoleApp"
        runtime "Release"
        defines{"NDEBUG", "_UNICODE", "UNICODE"}
        optimize "on"

            