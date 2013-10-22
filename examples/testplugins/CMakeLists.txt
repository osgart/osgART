
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# -- PLUGINS -- 

add_subdirectory(osgartpluginvdummyvideotest)
#add_subdirectory(osgartpluginvdummyvideRGBDtest)
add_subdirectory(osgartplugintdummytrackertest)

add_subdirectory(osgartpluginvopencvtest)
add_subdirectory(osgartpluginvartoolkittest)
add_subdirectory(osgartplugintartoolkittest)


IF(APPLE)
add_subdirectory(osgartpluginvqtkittest)
add_subdirectory(osgartpluginvavfoundationtest)
ENDIF()

IF(WIN32)

ENDIF()

#add_subdirectory(osgartplugintopiratest)
#add_subdirectory(osgartplugintstbtest)
#add_subdirectory(osgartpluginvtsstttest)

