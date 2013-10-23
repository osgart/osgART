# Locating PCL
# 
# (c) 2013 Raphael Grasset
#
# Locate PCL
# This module defines
# PCL_LIBRARIES
# PCL_FOUND, if false, do not try to link to OPENNI
# PCL_INCLUDE_DIR, where to find the headers


find_path(PCL_INCLUDE_DIR .h
    PATHS
    NO_DEFAULT_PATH
)

find_library(OPENNI_LIBRARY OpenNI
    NAMES OpenNI
    PATHS
    NO_DEFAULT_PATH
)

set(OPENNI_FOUND "NO")
if(OPENNI_INCLUDE_DIR AND OPENNI_LIBRARY)
	set(OPENNI_FOUND "YES")
endif(OPENNI_INCLUDE_DIR AND OPENNI_LIBRARY)