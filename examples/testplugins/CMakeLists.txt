
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# -- PLUGINS -- 

## -- SENSOR PLUGINS -- 
add_subdirectory(osgartpluginsdummysensortest)
add_subdirectory(osgartpluginsdummygpssensortest)
add_subdirectory(osgartpluginsdummyinertialsensortest)

## -- VIDEO PLUGINS -- 
add_subdirectory(osgartpluginvosgvideotest)
add_subdirectory(osgartpluginvdummyvideotest)
add_subdirectory(osgartpluginvdummyrgbdvideotest)

add_subdirectory(osgartpluginvopencvtest)
add_subdirectory(osgartpluginvopennitest)
add_subdirectory(osgartpluginvpcltest)
add_subdirectory(osgartpluginvartoolkittest)

IF(APPLE)
add_subdirectory(osgartpluginvqtkittest)
add_subdirectory(osgartpluginvavfoundationtest)
ENDIF()

IF(WIN32)
add_subdirectory(osgartpluginvdsvideolibtest)
add_subdirectory(osgartpluginvvideoinputtest)
ENDIF()

IF(LINUX)

ENDIF()

## -- TRACKER PLUGINS -- 
add_subdirectory(osgartplugintdummytrackertest)
add_subdirectory(osgartplugintdummyslamtrackertest)
add_subdirectory(osgartplugintdummygpsinertialtrackertest)

add_subdirectory(osgartplugintartoolkittest)
add_subdirectory(osgartplugintartoolkitplustest)

#add_subdirectory(osgartpluginticg3dtrackingtest)
#add_subdirectory(osgartplugintvuforiatest)
#add_subdirectory(osgartplugintsatest)
#add_subdirectory(osgartplugintkinfutest)
#add_subdirectory(osgartplugintkinectfusiontest)
#add_subdirectory(osgartplugintopiratest)
#add_subdirectory(osgartpluginvtsstttest)
