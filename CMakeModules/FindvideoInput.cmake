# Locating videoInput
# 
# (c) 2013 Raphael Grasset
#
# Locate videoInput
# This module defines
# VINPUT_LIBRARY
# VINPUT_FOUND, if false, do not try to link to videoInput
# VINPUT_INCLUDE_DIR, where to find the headers

find_path(VINPUT_INCLUDE_DIR videoInput.h
    PATHS
    NO_DEFAULT_PATH
)

find_library(VINPUT_LIBRARY
    NAMES videoInput
    PATHS
    NO_DEFAULT_PATH
)

set(VINPUT_FOUND "NO")
if(VINPUT_INCLUDE_DIR AND VINPUT_LIBRARY)
	set(VINPUT_FOUND "YES")
endif(VINPUT_INCLUDE_DIR AND VINPUT_LIBRARY)
