echo "run osgart examples.."

export OSG_NOTIFY_LEVEL=WARN

echo "-- BASIC TEST--"
echo "run osgartblank.."
osgartblank 

echo "-- VIDEO TEST--"
echo "run osgartvideolayer.."
osgartvideolayer
                      
echo "-- SIMPLE API--"
echo "run simple - simple.."
osgartsimple
echo "run simple - model.."
osgartsimplemodel
echo "run simple - keyboard input.."
osgartsimplekeyboardinput 
echo "run simple - mouse picking.."
osgartsimplemousepicking   
echo "run simple - manipulator.."
osgartsimplemanipulator         
echo "run simple - rigid body anim.."    
osgartsimplerbanim
echo "run simple - particle.."
osgartsimpleparticle

#osgartsimpleartoolkit   
#osgartsimplerbanimartoolkit
#osgartsimpletarget2cproximityartoolkit                           
#osgartsimpletarget2tproximityartoolkit        
#osgartsimpletargetmultipleartoolkit                  
#osgartsimplemousepickingartoolkit
#osgartsimplekeyboardinputartoolkit
#osgartsimpleparticleartoolkit
#osgartsimplemanipulatorartoolkit

echo "-- INTERMEDIATE API--"
#osgartintermsensor
echo "run interm - video.."
osgartintermvideo
echo "run interm - videobackground.."
osgartintermvideobackground 
echo "run interm - tracker.."
osgartintermtracker 
echo "run interm - registration.."
osgartintermregistration
echo "run interm - simple.."
osgartintermsimple
echo "run interm - model.."
osgartintermmodel
echo "run interm - keyboard input.."
osgartintermkeyboardinput
echo "run interm - mouse picking.."
osgartintermmousepicking 
echo "run interm - manipulator.."
osgartintermmanipulator 
#echo "run interm - stats.."
#osgartintermstats

#osgartintermtargetmultipleartoolkit              
#osgartintermtargetpaddleartoolkit     
#osgartintermkeyboardinputartoolkit     
#osgartintermmanipulatorartoolkit       
#osgartintermmousepickingartoolkit      
#osgartintermsimpleartoolkit               
#osgartintermtarget2tproximityartoolkit 
#osgartintermtarget2cproximityartoolkit 

echo "-- ADVANCED API--"

echo "-- PLUGINS TESTS--"
echo "run plugin - dummy video.."
osgartpluginvdummyvideotest 
echo "run plugin - dummy rgbdvideo.."
osgartpluginvdummyrgbdvideotest 
echo "run plugin - dummy tracker.."
osgartplugintdummytrackertest          
echo "run plugin - dummy slam tracker.."
osgartplugintdummyslamtrackertest   

echo "run plugin - opencv video.."
osgartpluginvopencvtest

echo "run plugin - artoolkit video.."
#osgartpluginvartoolkittest

echo "run plugin - artoolkit tracker.."
#osgartplugintartoolkittest

#osgartpluginsdummysensortest 
#osgartpluginsdummygsptest 
#osgartpluginsdummyinertialtest 
#osgartplugintdummytrackergpsinertialtest          
#osgartpluginvopencvtest
#osgartpluginvartoolkittest
#osgartplugintartoolkittest

#osgartviewer
#osgartmarkertraining                   