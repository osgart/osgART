--
-- premake script to create various versions of project files
--

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
-- Plugin ARToolKit Video
--
package = newpackage()
package.path = project.path
package.bindir = "../../bin"
package.name = "Plugin ARToolKit Video"
package.target = "osgart_artoolkit"
package.language = "c++"
package.kind = "dll"
package.objdir = "obj/" .. package.target

package.defines = { "OSGART_PLUGIN_EXPORT" }

if (OS == "windows") then
	table.insert(package.defines,"WIN32")
	table.insert(package.defines,"_WIN32")
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
}

if (OS == "windows") then
	table.insert(package.links,"libARvideo")
end


package.files = {
  matchfiles("../../src/osgART/Video/ARToolKit/*"),
}

--
-- Plugin ARToolKit Tracker
--
package = newpackage()
package.bindir = "../../bin"
package.path = project.path
package.name = "Plugin ARToolKit Tracker"
package.target = "osgart_artoolkit_tracker"
package.language = "c++"
package.kind = "dll"
package.objdir = "obj/" .. package.target

package.files = {
  matchfiles("../../src/osgART/Tracker/ARToolKit/*")
}


package.defines = { "OSGART_PLUGIN_EXPORT" }

if (OS == "windows") then
	table.insert(package.defines,"WIN32")
	table.insert(package.defines,"_WIN32")
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
}

if (OS == "windows") then
	table.insert(package.links,"libAR")
	table.insert(package.links,"libARmulti")
	table.insert(package.links,"libARgsub_lite")
	table.insert(package.links,"OpenGL32")
	table.insert(package.links,"GLU32")
end




