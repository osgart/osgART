# Locating ARToolKit 2.x GPL version
# 
# (c) 2008 HITLabNZ, Hartmut Seichter
# October 20th 2013: Edited by R. Grasset
# March 24th 2014: Seichter - add new libs
# 
# Locate ARToolKit
# This module defines
# ARTOOLKIT2_LIBRARIES
# ARTOOLKIT2_FOUND, if false, do not try to link to ARToolKit
# ARTOOLKIT2_INCLUDE_DIR, where to find the headers


find_path(ARTOOLKIT2_INCLUDE_DIR AR/config.h
    PATHS
    $ENV{ARTOOLKIT_ROOT}/include
	~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/include
    ${CMAKE_SOURCE_DIR}/../ARToolKit/include
    NO_DEFAULT_PATH
)

find_library(ARTOOLKIT2_LIBAR
    NAMES AR libAR
    PATHS	
	$ENV{ARTOOLKIT_ROOT}/lib
	[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/lib
	${ARTOOLKIT2_INCLUDE_DIR}/../lib
    	NO_DEFAULT_PATH
	)

# pointless if libAR couldn't be found
if (ARTOOLKIT2_LIBAR) 
	
	find_library(ARTOOLKIT2_LIBARMULTI
		NAMES ARMulti libARMulti
		PATHS	
		$ENV{ARTOOLKIT_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/lib
		${ARTOOLKIT2_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)
	
	find_library(ARTOOLKIT2_LIBARGLUTILS
		NAMES ARGLUtils libARGLUtils
		PATHS	
		$ENV{ARTOOLKIT_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/lib
		${ARTOOLKIT2_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)
	
	find_library(ARTOOLKIT2_LIBARVIDEO
		NAMES ARvideo libARvideo
		PATHS	
		$ENV{ARTOOLKIT_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/lib
		${ARTOOLKIT2_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)

	if(UNIX)
		include(UsePkgConfig)
		pkgconfig(gstreamer-0.10 GST_INCLUDE_DIR GST_LIB_DIR GST_LINK_FLAGS GST_C_FLAGS)			
		set(ARTOOLKIT2_LIBARVIDEO ${ARTOOLKIT2_LIBARVIDEO} ${GST_LINK_FLAGS})
	
	endif(UNIX)

endif()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	ARTOOLKIT2
	DEFAULT_MSG
	ARTOOLKIT2_INCLUDE_DIR
	ARTOOLKIT2_LIBAR
	ARTOOLKIT2_LIBARMULTI
	ARTOOLKIT2_LIBARGLUTILS
	ARTOOLKIT2_LIBARVIDEO
  	)