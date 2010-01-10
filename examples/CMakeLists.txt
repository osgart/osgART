
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(osgartsimple)
add_subdirectory(osgartvideobackground)
add_subdirectory(osgartpick)
add_subdirectory(osgartsimplestats)

if (STBTRACKER_FOUND)
	add_subdirectory(osgartstbtest)
endif(STBTRACKER_FOUND)

if(SSTT_FOUND)
	add_subdirectory(osgartsstttest)
endif(SSTT_FOUND) 