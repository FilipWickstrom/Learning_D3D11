-- Solution configuration file.
workspace "Learning_D3D11"
    system "Windows"
    architecture "x86_64"
    flags { 
        "MultiProcessorCompile" 
    }
    
    configurations {
        "Debug",
        "Release"
    }
	-- Set the startup project.
    startproject "Engine"
		targetname "Learning_D3D11"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Subprojects.
include "Engine"


-- Clean.
newaction {
    trigger = "clean",
    description = "Remove all binaries and intermediate binaries, and vs files.",
    execute = function()
        print("Removing binaries")
        os.rmdir("Engine/build/bin")

        print("Removing intermediate binaries")
        os.rmdir("Engine/build/bin-int")

        print("Removing project files")
        os.rmdir("./.vs")
        os.remove("**.sln")
        os.remove("**.vcxproj")
        os.remove("**.vcxproj.filters")
        os.remove("**.vcxproj.user")
        print("Done")
    end
}