# 
# (c) Copyrights 2008 HITLabNZ, Hartmut Seichter
# 
 
find_package(ARToolKit2)
find_package(SSTT)
find_package(StbTracker)

if(ARTOOLKIT2_FOUND)
	add_subdirectory(ARToolKit)
endif(ARTOOLKIT2_FOUND)

if(SSTT_FOUND)
	add_subdirectory(SSTT)
endif(SSTT_FOUND) 

if (STBTRACKER_FOUND)
	add_subdirectory(StbTracker)
endif(STBTRACKER_FOUND)
