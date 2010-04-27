# Locating OPIRA
# 
# (c) 2008 HITLabNZ, Hartmut Seichter
#
# Locating OPIRA headers and libraries
# This module defines
# OPIRA_LIBRARIES
# OPIRA_FOUND, if false, do not try to link to OPIRA
# OPIRA_INCLUDE_DIR, where to find the headers
# OPIRA_REGISTRATION_INCLUDE_DIR, where to find the headers
# OPIRA_GESTURE_INCLUDE_DIR, where to find the headers
# OPIRA_CAPTURE_INCLUDE_DIR, where to find the headers

find_path(OPIRA_INCLUDE_DIR opiralibrary.h
    PATHS
    NO_DEFAULT_PATH
)


find_path(OPIRA_REGISTRATION_INCLUDE_DIR OCVSurf.h
    PATHS
    NO_DEFAULT_PATH
)


find_path(OPIRA_GESTURE_INCLUDE_DIR GestureLibrary.h
    PATHS
    NO_DEFAULT_PATH
)

find_path(OPIRA_CAPTURE_INCLUDE_DIR CaptureLibrary.h
    PATHS
    NO_DEFAULT_PATH
)


find_library(OPIRA_OPIRA_LIBRARY
    NAMES OpiraLibrary
    PATHS
    NO_DEFAULT_PATH
)

find_library(OPIRA_OCVSurf_LIBRARY
    NAMES OCVSurf.lib
    PATHS
    NO_DEFAULT_PATH
)


find_library(OPIRA_GESTURE_LIBRARY
    NAMES GestureLibrary.lib
    PATHS
    NO_DEFAULT_PATH
)

set(OPIRA_FOUND "NO")
if(OPIRA_INCLUDE_DIR AND OPIRA_OPIRA_LIBRARY AND OPIRA_OCVSurf_LIBRARY)
	set(OPIRA_FOUND "YES")
	set(OPIRA_LIBRARIES
		${OPIRA_OPIRA_LIBRARY}
		${OPIRA_OCVSurf_LIBRARY}
		${OPIRA_GESTURE_LIBRARY}
	)
	
endif(OPIRA_INCLUDE_DIR AND OPIRA_OPIRA_LIBRARY AND OPIRA_OCVSurf_LIBRARY)

