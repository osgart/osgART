#
# parses the osg/Version file and 
# determines the version of OSG we are compiling against
# 
# (c) Copyrights 2008 Hartmut Seichter, HIT Lab NZ
# 

macro(check_osg_version input)

	file(READ ${input} text)

	string(REGEX MATCH "#define[\t ]+OPENSCENEGRAPH_MAJOR_VERSION[\t ]+([0-9]+)" version_major ${text})
	string(REGEX MATCH "#define[\t ]+OPENSCENEGRAPH_MINOR_VERSION[\t ]+([0-9]+)" version_minor ${text})
	string(REGEX MATCH "#define[\t ]+OPENSCENEGRAPH_PATCH_VERSION[\t ]+([0-9]+)" version_patch ${text})
	string(REGEX MATCH "#define[\t ]+OPENSCENEGRAPH_SOVERSION[\t ]+([0-9][0-9])" version_so ${text})
	

	string(REGEX REPLACE ".*([0-9]+).*" "\\1" version_major ${version_major})
	string(REGEX REPLACE ".*([0-9]+).*" "\\1" version_minor ${version_minor})
	string(REGEX REPLACE ".*([0-9]+).*" "\\1" version_patch ${version_patch})
	string(REGEX REPLACE ".*([0-9][0-9]).*" "\\1" version_so ${version_so})

	set(OSG_VERSION_MAJOR ${version_major})
	set(OSG_VERSION_MAJOR ${version_minor})
	set(OSG_VERSION_PATCH ${version_patch})
	
	set(OSG_VERSION_SO ${version_so})

	set(OSG_VERSION_FULL "${version_major}.${version_minor}.${version_patch}")

endmacro(check_osg_version input)
