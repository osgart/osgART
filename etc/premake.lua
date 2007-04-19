--
-- premake script to create various versions of project files
--

require "globals"
require "plugins"


project.name = "osgART"
project.configs = { "Debug", "Release" }

if     (target == "vs2003") then
	project.path = "../VisualStudio/VS2003"
elseif (target == "vs2005") then
	project.path = "../VisualStudio/VS2005"
end


--
-- osgART
--
package = newpackage()
package.path = project.path
package.target = "osgART"

package.name = "osgART"
package.language = "c++"
package.kind = "dll"

package.libdir = "../../lib"
package.bindir = "../../bin"
package.objdir = "obj/" .. package.target

table.insert(package.config['Release'].defines,"NDEBUG")

package.defines = {
	"OSGART_LIBRARY"
	}

if (OS == "windows") then
	table.insert(package.defines,"WIN32")
	table.insert(package.defines,"_WINDOWS")
end

package.files = {
  matchfiles("../../src/osgART/*.cpp", "../../include/osgART/*"),
}

package.includepaths = {
	"../../include",
	"$(OSG_ROOT)/include"
}

package.excludes = {
	"../../src/osgART/VideoBillboard.cpp",
	"../../src/osgART/VideoForeground.cpp",
	"../../src/osgART/VideoBackground.cpp",
	"../../src/osgART/ShadowRenderer.cpp",
	"../../src/osgART/PlaneARShadowRenderer.cpp",
	}


package.libpaths = { "$(OSG_ROOT)/lib" }
package.links = { "osg", "osgDB"}

if (OS == "windows") then
	table.insert(package.links, {"OpenGL32", "OpenThreadsWin32" })
end

package.config["Debug"].target = package.target .. globals.targetsuffix


--
-- osgART Introspection Wrapper
--
package = newpackage()
package.path = project.path
package.name = "osgWrapper osgART"
package.target = "osgwrapper_osgART"
package.language = "c++"
package.kind = "dll"

package.libdir = "../../lib"
package.bindir = "../../bin"
package.objdir = "obj/" .. package.target


table.insert(package.config['Release'].defines,"NDEBUG")

if (OS == "windows") then
	table.insert(package.defines,"WIN32")
	table.insert(package.defines,"_WINDOWS")
end

package.includepaths = {
	"../../include",
	"$(OSG_ROOT)/include"
}

package.libpaths = { "$(OSG_ROOT)/lib" }

package.links = {
	"osg",
	"osgART",
	"osgIntrospection",
	"osgDB",
}

if (OS == "windows") then
	table.insert(package.links,"OpenThreadsWin32")
	table.insert(package.links,"OpenGL32")
end


package.files = {
  matchfiles("../../src/osgWrappers/osgART/*.cpp"),
}

package.excludes = {
	"../../src/osgWrappers/osgART/Field.cpp",
	"../../src/osgWrappers/osgART/ShadowRenderer.cpp",
	"../../src/osgWrappers/osgART/PlaneARShadow.cpp",
}

package.config["Debug"].target = package.target .. globals.targetsuffix

--
-- osgART NodeKit
--
package = newpackage()
package.path = project.path
package.name = "NodeKit osgART"
package.target = "osgdb_osgART"
package.language = "c++"
package.kind = "dll"

package.libdir = "../../lib"
package.bindir = "../../bin"
package.objdir = "obj/" .. package.target

table.insert(package.config['Release'].defines,"NDEBUG")


if (OS == "windows") then
	table.insert(package.defines,"WIN32")
	table.insert(package.defines,"_WINDOWS")
end

package.includepaths = {
	"../../include",
	"$(OSG_ROOT)/include"
}

package.libpaths = { "$(OSG_ROOT)/lib" }

package.links = {
	"osg",
	"osgART",
	"osgDB",
}

if (OS == "windows") then
	table.insert(package.links,"OpenThreadsWin32")
end


package.files = {
  matchfiles("../../src/osgPlugins/osgART/IO_*.cpp"),
}

package.excludes = {
}

package.config["Debug"].target = package.target .. globals.targetsuffix

--
-- Simple Example
--
package = newpackage()
package.path = project.path
package.name = "Example Simple"
package.target = "osgart_simple"
package.language = "c++"
package.kind = "exe"
package.bindir = "../../bin"

table.insert(package.config['Release'].defines,"NDEBUG")


if (OS == "windows") then
	table.insert(package.defines,"WIN32")
	table.insert(package.defines,"_WINDOWS")
end

package.includepaths = {
	"../../include",
	"$(OSG_ROOT)/include"
}

package.libpaths = { "$(OSG_ROOT)/lib" }

package.links = {
	"osg",
	"osgART",
	"Producer",
	"osgProducer"
}



package.files = {
  matchfiles("../../src/osgARTTest/*.cpp"),
}

package.config["Debug"].target = package.target .. globals.targetsuffix

--
-- Plugin ARToolKit Tracker
--
package = createTrackerPlugin("ARToolKit","artoolkit")


table.insert(package.includepaths,"$(ARTOOLKIT_2_ROOT)/include")
table.insert(package.libpaths,"$(ARTOOLKIT_2_ROOT)/lib")

package.files = {
  matchfiles("../../src/osgART/Tracker/ARToolKit/*")
}

if (OS == "windows") then
	table.insert(package.links,"libAR")
	table.insert(package.links,"libARmulti")
	table.insert(package.links,"libARgsub_lite")
	table.insert(package.links,"OpenGL32")
	table.insert(package.links,"GLU32")
end


--
-- Plugin ARToolKit 4 Tracker
--
package = createTrackerPlugin("ARToolKit 4 Professional","artoolkit4")

table.insert(package.includepaths,"$(ARTOOLKIT_4_ROOT)/include")
table.insert(package.libpaths,"$(ARTOOLKIT_4_ROOT)/lib")

package.files = {
  matchfiles("../../src/osgART/Tracker/ARToolKit4/*")
}

if (OS == "windows") then
	table.insert(package.links,"AR")
	table.insert(package.links,"ARmulti")
	table.insert(package.links,"ARgsub_lite")
	table.insert(package.links,"OpenGL32")
	table.insert(package.links,"GLU32")
end


--
-- Plugin ARToolKit Plus
--
package = createTrackerPlugin("ARToolKit Plus","artoolkitplus")

table.insert(package.includepaths,"$(ARTOOLKIT_PLUS_ROOT)/include")

package.files = {
  matchfiles("../../src/osgART/Tracker/ARToolKitPlus/*")
}

-- windows specific
if (OS == "windows") then

	table.insert(package.libpaths,"$(ARTOOLKIT_PLUS_ROOT)/lib/win32")

	table.insert(package.links,"ARToolKitPlus")
	table.insert(package.links,"OpenGL32")
	table.insert(package.links,"GLU32")
end


--
-- Plugin ARToolKit 4 NFT Tracker
--
package = createTrackerPlugin("ARToolKit 4 NFT","artoolkit4nft")

table.insert(package.includepaths,"$(ARTOOLKIT_4_ROOT)/include")
table.insert(package.includepaths,"$(ARTOOLKIT_NFT_ROOT)/include")
table.insert(package.libpaths,"$(ARTOOLKIT_NFT_ROOT)/lib")
table.insert(package.libpaths,"$(ARTOOLKIT_4_ROOT)/lib")

package.files = {
  matchfiles("../../src/osgART/Tracker/ARToolKit4NFT/*")
}

if (OS == "windows") then
	table.insert(package.links,"AR")
	table.insert(package.links,"AR2")
	table.insert(package.links,"ARmulti")
	table.insert(package.links,"ARgsub")
	table.insert(package.links,"ARgsub_lite")
	table.insert(package.links,"OpenGL32")
	table.insert(package.links,"GLU32")
end

--
-- Video Plugin: PointGrey
--
if (OS == "windows") then
	package = createVideoPlugin("PointGrey","ptgrey")

    table.insert(package.includepaths,"$(ProgramFiles)/Point Grey Research/PGR FlyCapture/include")
	table.insert(package.libpaths,"$(ProgramFiles)/Point Grey Research/PGR FlyCapture/lib")
	table.insert(package.links,"PGRFlyCapture")
	table.insert(package.links,"OpenThreadsWin32")


	package.files = {
		matchfiles("../../src/osgART/Video/PtGrey/*"),
	}
end


--
-- Video Plugin: Intranel
--
if (OS == "windows") then
	package = createVideoPlugin("Intranel RTSP","intranel")

    table.insert(package.includepaths,"$(DXSDK_DIR)/Extras/DirectShow/Samples/C++/DirectShow/BaseClasses")
	table.insert(package.libpaths,"$(DXSDK_DIR)/Extras/DirectShow/Samples//C++/DirectShow/BaseClasses/Release")
	table.insert(package.links,"strmbase")
	table.insert(package.links,"strmiids")
	table.insert(package.links,"winmm")
	table.insert(package.links,"quartz")

	table.insert(package.links,"OpenThreadsWin32")

	package.files = {
		matchfiles("../../src/osgART/Video/Intranel/*"),
	}
end


--
-- Video Plugin: DSVL
--
if (OS == "windows") then
	package = createVideoPlugin("DSVL","dsvl")

    table.insert(package.includepaths,"$(ARTOOLKIT_2_ROOT)/DSVL/src")
    table.insert(package.includepaths,"$(DXSDK_DIR)/Extras/DirectShow/Samples/C++/DirectShow/BaseClasses")
    table.insert(package.libpaths,"$(ARTOOLKIT_2_ROOT)/DSVL/lib")
	table.insert(package.libpaths,"$(DXSDK_DIR)/Extras/DirectShow/Samples//C++/DirectShow/BaseClasses/Release")
	
	table.insert(package.links,"DSVL")

	table.insert(package.links,"OpenThreadsWin32")

	package.files = {
		matchfiles("../../src/osgART/Video/DSVL/*"),
	}
end

--
-- Video Plugin: DSVL2
--
if (OS == "windows") then
	package = createVideoPlugin("DSVL2","dsvl2")

    table.insert(package.includepaths,"$(ARTOOLKIT_2_ROOT)/DSVL2/src")
    table.insert(package.includepaths,"$(DXSDK_DIR)/Extras/DirectShow/Samples/C++/DirectShow/BaseClasses")
    table.insert(package.libpaths,"$(ARTOOLKIT_2_ROOT)/DSVL/lib")
	table.insert(package.libpaths,"$(DXSDK_DIR)/Extras/DirectShow/Samples//C++/DirectShow/BaseClasses/Release")
	
	table.insert(package.links,"DSVL2")

	table.insert(package.links,"OpenThreadsWin32")

	package.files = {
		matchfiles("../../src/osgART/Video/DSVL2/*"),
	}
end

--
-- Plugin ARToolKit Video
--
package = createVideoPlugin("ARToolKit","artoolkit")

if (OS == "windows") then
	table.insert(package.links,"libARvideo")
end

table.insert(package.includepaths,"$(ARTOOLKIT_2_ROOT)/include")
table.insert(package.libpaths,"$(ARTOOLKIT_2_ROOT)/lib")


package.files = {
  matchfiles("../../src/osgART/Video/ARToolKit/*"),
}


