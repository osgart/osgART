
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# -- PLUGINS -- 

## -- SENSOR PLUGINS -- 

## -- VIDEO PLUGINS -- 
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
#add_subdirectory(osgartpluginvdsvidelibtest)
#add_subdirectory(osgartpluginvvideoinputtest)
ENDIF()

## -- TRACKER PLUGINS -- 
add_subdirectory(osgartplugintdummytrackertest)
add_subdirectory(osgartplugintdummyslamtrackertest)
#add_subdirectory(osgartplugintdummytrackergpsinertialtest)

add_subdirectory(osgartplugintartoolkittest)
#add_subdirectory(osgartplugintartoolkitplustest)

#add_subdirectory(osgartplugintopiratest)
#add_subdirectory(osgartpluginvtsstttest)

#add_subdirectory(osgartpluginticg3dtrackingtest)
#add_subdirectory(osgartplugintvuforiatest)
#add_subdirectory(osgartplugintsatest)
#add_subdirectory(osgartplugintkinfutest)