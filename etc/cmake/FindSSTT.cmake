# Locating SSTT (Simplified Spatial Target Tracker) 
# 
# (c) 2008 HITLabNZ, Hartmut Seichter
#
# Locating SSTT headers and libraries
# This module defines
# SSTT_LIBRARIES
# SSTT_FOUND, if false, do not try to link to SSTT
# SSTT_INCLUDE_DIR, where to find the headers


find_path(SSTT_INCLUDE_DIR sstt/sstt.h
    PATHS
    $ENV{SSTT_ROOT}/include
	~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;SSTT_ROOT]/include
    NO_DEFAULT_PATH
)


find_library(SSTT_sstt_LIBRARY
    NAMES sstt
    PATHS	
	$ENV{SSTT_ROOT}/lib
	[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;SSTT_ROOT]/lib
	${SSTT_INCLUDE_DIR}/../lib
    NO_DEFAULT_PATH
)

set(SSTT_FOUND "NO")
if(SSTT_INCLUDE_DIR AND SSTT_sstt_LIBRARY)
	set(SSTT_FOUND "YES")
	set(SSTT_LIBRARIES
		${SSTT_sstt_LIBRARY}
	)
endif(SSTT_INCLUDE_DIR AND SSTT_sstt_LIBRARY)

