
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# -- PLUGINS -- 

## -- SENSOR PLUGINS -- 

## -- VIDEO PLUGINS -- 
add_subdirectory(osgartpluginvdummyvideotest)
#add_subdirectory(osgartpluginvdummyvideRGBDtest)

add_subdirectory(osgartpluginvopencvtest)
add_subdirectory(osgartpluginvopennitest)
add_subdirectory(osgartpluginvartoolkittest)

IF(APPLE)
add_subdirectory(osgartpluginvqtkittest)
add_subdirectory(osgartpluginvavfoundationtest)
ENDIF()

IF(WIN32)

ENDIF()

## -- TRACKER PLUGINS -- 
add_subdirectory(osgartplugintdummytrackertest)
#add_subdirectory(osgartplugintdummytrackergpsinertialtest)

add_subdirectory(osgartplugintartoolkittest)

#add_subdirectory(osgartplugintopiratest)
#add_subdirectory(osgartplugintstbtest)
#add_subdirectory(osgartpluginvtsstttest)

