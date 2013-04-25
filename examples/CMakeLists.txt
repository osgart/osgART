
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)


# -- default test --
add_subdirectory(osgartblank)

# -- high level --

## hello world
add_subdirectory(osgartsimple) #default
add_subdirectory(osgartsimpleartoolkit)

## framework
add_subdirectory(osgartsimpletargetmultipleartoolkit)

## media
add_subdirectory(osgartsimplemodel)
add_subdirectory(osgartsimplerbanim)
add_subdirectory(osgartsimpleparticle)
add_subdirectory(osgartsimplerbanimartoolkit)
add_subdirectory(osgartsimpleparticleartoolkit)
### media integration
#add_subdirectory(osgartsimplesoundosgAL) 
#add_subdirectory(osgartsimplemovie)

## interaction
#add_subdirectory(osgartsimplemouse)
#add_subdirectory(osgartsimplekeyboard)
#add_subdirectory(osgartsimplemousepicking)

add_subdirectory(osgartsimplekeyboardinput)
add_subdirectory(osgartsimplemousepicking)
add_subdirectory(osgartsimplemanipulator)

add_subdirectory(osgartsimplekeyboardinputartoolkit)
add_subdirectory(osgartsimplemousepickingartoolkit)
add_subdirectory(osgartsimplemanipulatorartoolkit)
add_subdirectory(osgartsimpletarget2cproximityartoolkit)
add_subdirectory(osgartsimpletarget2tproximityartoolkit)
#add_subdirectory(osgartsimpletargetmotionartoolkit)
#add_subdirectory(osgartsimpletargetpaddleartoolkit)
#add_subdirectory(osgartsimplemagiclensartoolkit)
#add_subdirectory(osgartsimpletransitionaluserinterface)

## rendering
#add_subdirectory(osgartsimpleshadow) 
#add_subdirectory(osgartsimpleocclusion)
#add_subdirectory(osgartsimpleclippingplanes) 

##development
#add_subdirectory(osgartsimplestats)

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

