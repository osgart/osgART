
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# -- high level --
add_subdirectory(osgartsimplescene) #default
add_subdirectory(osgartsimplesceneartoolkit)


# -- middle level --

#computer-vision based registration/tracking
add_subdirectory(osgartintermvideo) ##video
add_subdirectory(osgartintermvideobackground) ##display
add_subdirectory(osgartintermtracker) ##tracking
add_subdirectory(osgartintermregistration) ##registration
add_subdirectory(osgartintermregistrationartoolkit) ##registration

#interaction
add_subdirectory(osgartintermpick)

##development
add_subdirectory(osgartintermstats)


# -- PLUGINS -- 

#add_subdirectory(osgartplugintopiratest)
#add_subdirectory(osgartplugintstbtest)
#add_subdirectory(osgartpluginvtsstttest)

## -- TODO --
#add_subdirectory(osgartvideobackground)
#add_subdirectory(osgartvideobillboard)
#add_subdirectory(osgartvideoplane)
#add_subdirectory(osgartvideoshapes)


#sensor-based registration/tracking
#add_subdirectory(osgartintermgnss) ##gps
#add_subdirectory(osgartinterminertial) ##inertial
#add_subdirectory(osgartintermregistrationgnssinertial) ##registration
#add_subdirectory(osgartintermregistrationubloxintersense)

#more
#add_subdirectory(osgartintermkinect)

##interaction
#add_subdirectory(osgartintemkeyboard)
##add_subdirectory(osgartintermmousekeyboard)
#add_subdirectory(osgartintermmultitouch)

##photometric registration
#add_subdirectory(osgartintermocclusion)
#add_subdirectory(osgartintermshadow)

#add_subdirectory(osgartvideoprocessing)

##display
#add_subdirectory(osgartvideobundistort)
#add_subdirectory(osgartsimpleopticalbackground)

##development
#add_subdirectory(osgartsideview)
#add_subdirectory(osgartdebugvideo)

#advanced example

##display
#add_subdirectory(osgartstereobackground)

#tracker
#SLAM example

# plugins tests

#sensor



#video
#add_subdirectory(osgartartoolkitvtest)
#add_subdirectory(osgartimagelisttest)
#add_subdirectory(osgartimagesequencetest)
#add_subdirectory(osgartkinecttest)
#add_subdirectory(osgartomnikinecttest)

#tracker

#video and tracker
#add_subdirectory(osgartstbtest)
#add_subdirectory(osgartsstttest)
#add_subdirectory(osgartopiratest)
#add_subdirectory(osgartartoolkitttest)
#add_subdirectory(osgartartoolkitvtest)

#videotracker

#sensor
#add_subdirectory(intersensetest)
#add_subdirectory(ubloxtest)

#advanced examples

#integration examples

