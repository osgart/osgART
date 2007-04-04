-- generate a plugin


function createVideoPlugin(fullname, shortname)

	package = newpackage()
	package.path = project.path
	package.libdir = "../../lib"
	package.bindir = "../../bin"
	package.name = "Plugin " .. fullname.. " Video"
	package.target = "osgart_video_" .. shortname
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

	return package

end


function createTrackerPlugin(fullname, shortname)

	package = newpackage()
	package.bindir = "../../bin"
	package.libdir = "../../lib"
	package.path = project.path
	package.name = "Plugin "..fullname .. " Tracker"
	package.target = "osgart_tracker_" .. shortname
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
	return package

end