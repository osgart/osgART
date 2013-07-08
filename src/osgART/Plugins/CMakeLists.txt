
option(OSGART_BUILD_PLUGIN_OPENCV "Build with OpenCV (OpenCV 2.3 and later)" OFF)
option(OSGART_BUILD_PLUGIN_ARTOOLKIT "ARToolKit Multiplatforms interface" OFF)

add_subdirectory(ARToolKit)
add_subdirectory(OpenCV)
