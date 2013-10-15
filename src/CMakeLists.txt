
include_directories(
	${CMAKE_SOURCE_DIR}/include 
	${CMAKE_BINARY_DIR}/include
	)
	
add_subdirectory(osgART)
#add_subdirectory(osgARTPlugins)
add_subdirectory(osgPlugins)
add_subdirectory(osgWrappers)


