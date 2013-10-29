# Locating PCL
# 
# (c) 2013 Raphael Grasset
#
# Locate PCL
# This module defines
# PCL_LIBRARIES
# PCL_FOUND, if false, do not try to link to OPENNI
# PCL_INCLUDE_DIR, where to find the headers


find_path(PCL_INCLUDE_DIR pcl_base.h
    PATHS
    /usr/local/include/pcl-1.7/pcl
    NO_DEFAULT_PATH
)

find_library(PCL_COMMON_LIBRARY
    NAMES pcl_common libpcl_common
    PATHS
    NO_DEFAULT_PATH
)

find_library(PCL_IO_LIBRARY
    NAMES pcl_io libpcl_io
    /usr/local/lib
    PATHS
    NO_DEFAULT_PATH
)

set(PCL_FOUND "NO")
if(PCL_INCLUDE_DIR AND PCL_COMMON_LIBRARY AND PCL_IO_LIBRARY)
	set(OPENNI_FOUND "YES")
	set(PCL_LIBRARIES 
		${PCL_COMMON_LIBRARY}
		${PCL_IO_LIBRARY}		
	)
endif(PCL_INCLUDE_DIR AND PCL_COMMON_LIBRARY AND PCL_IO_LIBRARY)
