# 
# (c) Copyrights 2008 HITLabNZ, Hartmut Seichter
# 
 
find_package(ARToolKit2)
if(ARTOOLKIT2_FOUND)
	add_subdirectory(ARToolKit)
endif()

find_package(SSTT)
if(SSTT_FOUND)
	add_subdirectory(SSTT)
endif() 

find_package(StbNX)
message(STATUS "${StbNX_FOUND}")
if (StbNX_FOUND)
	add_subdirectory(StbNX)
endif()

find_package(OPIRA)
if (OPIRA_FOUND)
	add_subdirectory(OPIRA)
endif()