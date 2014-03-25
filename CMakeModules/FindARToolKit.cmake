# Locating ARToolKit 2.x GPL version
# 
# (c) 2008 HITLabNZ, Hartmut Seichter
# October 20th 2013: Edited by R. Grasset
#
# Locate ARToolKit
# This module defines
# ARTOOLKIT_LIBRARIES
# ARTOOLKIT_FOUND, if false, do not try to link to ARToolKit
# ARTOOLKIT_INCLUDE_DIR, where to find the headers




find_path(ARTOOLKIT_INCLUDE_DIR AR/config.h
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

find_library(ARTOOLKIT_LIBAR
    NAMES AR libAR
    PATHS	
	$ENV{ARTOOLKIT_ROOT}/lib
	[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/lib
	${ARTOOLKIT_INCLUDE_DIR}/../lib
    NO_DEFAULT_PATH
)

# pointless if libAR couldn't be found
if (ARTOOLKIT_LIBAR) 
	
	find_library(ARTOOLKIT_LIBARMULTI
		NAMES ARMulti ARmulti libARmulti
		PATHS	
		$ENV{ARTOOLKIT_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/lib
		${ARTOOLKIT_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)
	
	find_library(ARTOOLKIT_LIBARGSUB_LITE
		NAMES ARgsub_lite libARgsub_lite
		PATHS	
		$ENV{ARTOOLKIT_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/lib
		${ARTOOLKIT_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)
	
	find_library(ARTOOLKIT_LIBARVIDEO
		NAMES ARvideo libARVideo libARvideo
		PATHS	
		$ENV{ARTOOLKIT_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_ROOT]/lib
		${ARTOOLKIT_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)

	if(UNIX)
		include(UsePkgConfig)
		pkgconfig(gstreamer-0.10 GST_INCLUDE_DIR GST_LIB_DIR GST_LINK_FLAGS GST_C_FLAGS)			
		set(ARTOOLKIT2_LIBARVIDEO ${ARTOOLKIT2_LIBARVIDEO} ${GST_LINK_FLAGS})
	
	endif(UNIX)

endif(ARTOOLKIT_LIBAR)



set(ARTOOLKIT_FOUND "NO")
if(ARTOOLKIT_INCLUDE_DIR AND ARTOOLKIT_LIBAR)
	set(ARTOOLKIT_FOUND "YES")
endif(ARTOOLKIT_INCLUDE_DIR AND ARTOOLKIT_LIBAR)
