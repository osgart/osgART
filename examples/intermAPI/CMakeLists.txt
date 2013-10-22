
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# -- middle level --

##hello world
add_subdirectory(osgartintermsimple) #default
add_subdirectory(osgartintermsimpleartoolkit)

#computer-vision based registration/tracking
add_subdirectory(osgartintermvideo) ##video
add_subdirectory(osgartintermvideobackground) ##display
add_subdirectory(osgartintermtracker) ##tracking
add_subdirectory(osgartintermregistration) ##registration

#framework
add_subdirectory(osgartintermtargetmultipleartoolkit) 
#multiple video (side by side, nested)
#multiple window
#multiple tracker (side by side, nested)

#media
add_subdirectory(osgartintermmodel)

#interaction
add_subdirectory(osgartintermmousepicking)
add_subdirectory(osgartintermkeyboardinput)
add_subdirectory(osgartintermmanipulator)

add_subdirectory(osgartintermkeyboardinputartoolkit)
add_subdirectory(osgartintermmousepickingartoolkit)
add_subdirectory(osgartintermmanipulatorartoolkit)
add_subdirectory(osgartintermtarget2cproximityartoolkit)
add_subdirectory(osgartintermtarget2tproximityartoolkit)
#add_subdirectory(osgartintermtargetmotionartoolkit)
add_subdirectory(osgartintermtargetpaddleartoolkit)

##rendering
#add_subdirectory(osgartintermmagiclensartoolkit)

##development
add_subdirectory(osgartintermstats)
