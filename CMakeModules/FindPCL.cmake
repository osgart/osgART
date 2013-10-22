# Locating OpenNI (<2.0)
# 
# (c) 2013 Raphael Grasset
#
# Locate OpenNI
# This module defines
# OPENNI_LIBRARY
# OPENNI_FOUND, if false, do not try to link to OPENNI
# OPENNI_INCLUDE_DIR, where to find the headers


find_path(OPENNI_INCLUDE_DIR DSVL.h
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