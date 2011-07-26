# 
# (c) Copyrights 2008 HITLabNZ, Hartmut Seichter
# 



add_subdirectory(SSTT)
add_subdirectory(StbNX)

 
find_package(ARToolKit2)
if(ARTOOLKIT2_FOUND)
	add_subdirectory(ARToolKit)
endif()



find_package(OPIRA)
if (OPIRA_FOUND)
	add_subdirectory(OPIRA)
endif()