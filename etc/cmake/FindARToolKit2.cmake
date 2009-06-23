# Locating ARToolKit 2.x GPL version
# 
# (c) 2008 HITLabNZ, Hartmut Seichter
#
# Locate ARToolKit
# This module defines
# ARTOOLKIT2_LIBRARIES
# ARTOOLKIT2_FOUND, if false, do not try to link to ARToolKit
# ARTOOLKIT2_INCLUDE_DIR, where to find the headers




find_path(ARTOOLKIT2_INCLUDE_DIR AR/config.h
    PATHS
    $ENV{ARTOOLKIT_2_ROOT}/include
	~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_2_ROOT]/include
    ${CMAKE_SOURCE_DIR}/../ARToolKit/include
    NO_DEFAULT_PATH
)

find_library(ARTOOLKIT2_LIBAR
    NAMES AR libAR
    PATHS	
	$ENV{ARTOOLKIT_2_ROOT}/lib
	[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_2_ROOT]/lib
	${ARTOOLKIT2_INCLUDE_DIR}/../lib
    NO_DEFAULT_PATH
)

# pointless if libAR couldn't be found
if (ARTOOLKIT2_LIBAR) 
	
	find_library(ARTOOLKIT2_LIBARMULTI
		NAMES ARMulti ARmulti libARmulti
		PATHS	
		$ENV{ARTOOLKIT_2_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_2_ROOT]/lib
		${ARTOOLKIT2_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)
	
	find_library(ARTOOLKIT2_LIBARGSUB_LITE
		NAMES ARgsub_lite libARgsub_lite
		PATHS	
		$ENV{ARTOOLKIT_2_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_2_ROOT]/lib
		${ARTOOLKIT2_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)
	
	find_library(ARTOOLKIT2_LIBARVIDEO
		NAMES ARvideo libARVideo libARvideo
		PATHS	
		$ENV{ARTOOLKIT_2_ROOT}/lib
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;ARTOOLKIT_2_ROOT]/lib
		${ARTOOLKIT2_INCLUDE_DIR}/../lib
		NO_DEFAULT_PATH
	)

	if(UNIX)
		include(UsePkgConfig)
		pkgconfig(gstreamer-0.10 GST_INCLUDE_DIR GST_LIB_DIR GST_LINK_FLAGS GST_C_FLAGS)			
		set(ARTOOLKIT2_LIBARVIDEO ${ARTOOLKIT2_LIBARVIDEO} ${GST_LINK_FLAGS})
	
	endif(UNIX)

endif(ARTOOLKIT2_LIBAR)



set(ARTOOLKIT2_FOUND "NO")
if(ARTOOLKIT2_INCLUDE_DIR AND ARTOOLKIT2_LIBAR)
	set(ARTOOLKIT2_FOUND "YES")
endif(ARTOOLKIT2_INCLUDE_DIR AND ARTOOLKIT2_LIBAR)
