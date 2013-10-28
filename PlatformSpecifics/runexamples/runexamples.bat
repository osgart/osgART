@echo off

echo "run osgart examples.."

set OSG_NOTIFY_LEVEL=WARN

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

REM osgartsimpleartoolkit   
REM osgartsimplerbanimartoolkit
REM osgartsimpletarget2cproximityartoolkit                           
REM osgartsimpletarget2tproximityartoolkit        
REM osgartsimpletargetmultipleartoolkit                  
REM osgartsimplemousepickingartoolkit
REM osgartsimplekeyboardinputartoolkit
REM osgartsimpleparticleartoolkit
REM osgartsimplemanipulatorartoolkit

echo "-- INTERMEDIATE API--"
REM osgartintermsensor
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
REM echo "run interm - stats.."
REM osgartintermstats

REM osgartintermtargetmultipleartoolkit              
REM osgartintermtargetpaddleartoolkit     
REM osgartintermkeyboardinputartoolkit     
REM osgartintermmanipulatorartoolkit       
REM osgartintermmousepickingartoolkit      
REM osgartintermsimpleartoolkit               
REM osgartintermtarget2tproximityartoolkit 
REM osgartintermtarget2cproximityartoolkit 

echo "-- ADVANCED API--"

echo "-- PLUGINS TESTS--"

echo "run plugin - dummy gps sensor.."
REM osgartpluginsdummysgsptest 
echo "run plugin - dummy inertial sensor.."
REM osgartpluginsdummysinertialtest 
echo "run plugin - dummy video.."
osgartpluginvdummyvideotest 
echo "run plugin - dummy rgbd video.."
osgartpluginvdummyrgbdvideotest 
echo "run plugin - dummy tracker.."
osgartplugintdummytrackertest          
echo "run plugin - dummy slam tracker.."
osgartplugintdummyslamtrackertest   
echo "run plugin - dummy gps inertial tracker.."
REM osgartplugintdummyttrackergpsinertialtest

REM echo "run plugin - opencv video.."
REM osgartpluginvopencvtest

REM echo "run plugin - artoolkit video.."
REM osgartpluginvartoolkittest

REM echo "run plugin - artoolkit tracker.."
REM osgartplugintartoolkittest

REM echo "run plugin - artoolkit plus tracker.."
REM osgartplugintartoolkitplustest



REM osgartviewer
REM osgartmarkertraining                   