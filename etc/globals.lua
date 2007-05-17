--
-- osgART premake script
--
-- Global settings used
-- 

-- globals
globals = {}

-- debug needs to have a suffix
globals['targetsuffix'] = "_debug"
globals['osg'] = {}
globals['openthreads'] = {}
globals.osg.two = true


globals.osg.libpaths = function()
	if (globals.osg.two) then
		return { "$(OSG_ROOT)/lib/release", "$(OSG_ROOT)/lib/debug" }
	else
		return { "$(OSG_ROOT)/lib/" }
	end
end


globals.openthreads.libs = function()
	if (globals.osg.two) then
		return { "OpenThreads" }
	else
		return { "OpenThreadsWin32" }
	end
end
