
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

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