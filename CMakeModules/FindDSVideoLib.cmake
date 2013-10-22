# Locating DSVideoLib (DSVL)
# 
# (c) 2013 Raphael Grasset
#
# Locate DSVideoLib
# This module defines
# DSVL_LIBRARY
# DSVL_FOUND, if false, do not try to link to DSVL
# DSVL_INCLUDE_DIR, where to find the headers


find_path(DSVL_INCLUDE_DIR DSVL.h
    PATHS
    NO_DEFAULT_PATH
)

find_library(DSVL_LIBRARY
    NAMES DSVL
    PATHS
    NO_DEFAULT_PATH
)

set(DSVL_FOUND "NO")
if(DSVL_INCLUDE_DIR AND DSVL_LIBRARY)
	set(DVSL_FOUND "YES")
endif(DSVL_INCLUDE_DIR AND DSVL_LIBRARY)

