# Locating StbTracker
# 
# (c) 2008 HITLabNZ, Hartmut Seichter, Julian Looser
#
# Locating StbTracker headers and libraries
# This module defines
# STBTRACKER_LIBRARIES
# STBTRACKER_FOUND, if false, do not try to link to StbTracker
# STBTRACKER_INCLUDE_DIR, where to find the headers


find_path(STBTRACKER_INCLUDE_DIR StbTracker.h
    PATHS
    NO_DEFAULT_PATH
)


find_library(STBTRACKER_StbTracker_LIBRARY
    NAMES StbTracker.lib
    PATHS
	${STBTRACKER_INCLUDE_DIR}/../Link
    NO_DEFAULT_PATH
)

find_library(STBTRACKER_StbCore_LIBRARY
    NAMES StbCore.lib
    PATHS
	${STBTRACKER_INCLUDE_DIR}/../Link
    NO_DEFAULT_PATH
)

set(STBTRACKER_FOUND "NO")
if(STBTRACKER_INCLUDE_DIR AND STBTRACKER_StbTracker_LIBRARY AND STBTRACKER_StbCore_LIBRARY)
	set(STBTRACKER_FOUND "YES")
	set(STBTRACKER_LIBRARIES
		${STBTRACKER_StbTracker_LIBRARY}
		${STBTRACKER_StbCore_LIBRARY}
	)
endif(STBTRACKER_INCLUDE_DIR AND STBTRACKER_StbTracker_LIBRARY AND STBTRACKER_StbCore_LIBRARY)

