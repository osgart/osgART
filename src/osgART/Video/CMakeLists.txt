find_package(ARToolKit2)

if   (ARTOOLKIT2_FOUND)
	add_subdirectory(ARToolKit)
endif(ARTOOLKIT2_FOUND) 

add_subdirectory(DummyImage)
add_subdirectory(VideoInput)
add_subdirectory(SSTT)
add_subdirectory(OpenCV)

