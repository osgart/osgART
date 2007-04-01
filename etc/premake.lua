--
-- premake script to create various versions of project files
--


require "plugins"


project.name = "osgART"
project.configs = { "Debug", "ReleaseWithSymbols", "Release" }

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
package.name = "osgART"
package.language = "c++"
package.kind = "dll"

package.bindir = "../../bin"

package.defines = {
	"OSGART_LIBRARY"
	}

if (OS == "windows") then
	table.insert(package.defines,"WIN32")
	table.insert(package.defines,"_WINDOWS")
end

package.config["Debug"].targetprefix = "Debug_"

package.files = {
  matchfiles("../../src/osgART/*.cpp", "../../include/osgART/*"),
}

package.includepaths = {
	"../../include",
	"$(OSG_ROOT)/include"
}

package.excludes = {
	"../../src/osgART/VideoTexture.cpp",
	"../../src/osgART/VideoTextureRectangle.cpp",
	"../../src/osgART/VideoTexCallback.cpp",
	"../../src/osgART/VideoTexRectCallback.cpp",
	"../../src/osgART/VideoBillboard.cpp",
	"../../src/osgART/VideoForeground.cpp",
	"../../src/osgART/VideoBackground.cpp",
	"../../src/osgART/VideoPlane.cpp",
	"../../src/osgART/ShadowRenderer.cpp",
	"../../src/osgART/PlaneARShadowRenderer.cpp",
	}


package.libpaths = { "$(OSG_ROOT)/lib" }
package.links = { "osg", "osgDB"}

if (OS == "windows") then
	table.insert(package.links, {"OpenGL32", "OpenThreadsWin32" })
end


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

--
-- Plugin ARToolKit Tracker
--
package = createTrackerPlugin("ARToolKit","artoolkit")

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
-- Video Plugin: PointGrey
--
if (OS == "windows") then
	package = createVideoPlugin("PointGrey","ptgrey")

    table.insert(package.includepaths,"C://Program Files//Point Grey Research//PGR FlyCapture//include")
	table.insert(package.libpaths,"C://Program Files//Point Grey Research//PGR FlyCapture//lib")
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

    table.insert(package.includepaths,"$(DXSDK_DIR)//Extras//DirectShow//Samples//C++//DirectShow//BaseClasses")
	table.insert(package.libpaths,"$(DXSDK_DIR)//Extras//DirectShow//Samples//C++//DirectShow//BaseClasses//Release")
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
-- Plugin ARToolKit Video
--

package = createVideoPlugin("ARToolKit","artoolkit")

if (OS == "windows") then
	table.insert(package.links,"libARvideo")
end


package.files = {
  matchfiles("../../src/osgART/Video/ARToolKit/*"),
}


