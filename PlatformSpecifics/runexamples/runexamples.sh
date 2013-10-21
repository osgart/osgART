echo "run osgart examples.."

export OSG_NOTIFY_LEVEL=WARN

echo "run osgartblank.."
osgartblank                            
echo "-- SIMPLE API--"
#osgartsimple
#osgartsimplemodel
#osgartsimplekeyboardinput 
#osgartsimplemousepicking   
#osgartsimplemanipulator             
#osgartsimplerbanim
#osgartsimpleparticle
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
./osgartintermvideo
echo "run interm - videobackground.."
./osgartintermvideobackground 
echo "run interm - tracker.."
./osgartintermtracker 
echo "run interm - registration.."
./osgartintermregistration
echo "run interm - simple.."
./osgartintermsimple
echo "run interm - model.."
./osgartintermmodel
echo "run interm - keyboard input.."
./osgartintermkeyboardinput
echo "run interm - mouse picking.."
./osgartintermmousepicking 
echo "run interm - manipulator.."
./osgartintermmanipulator 
#echo "run interm - stats.."
#./osgartintermstats

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
./osgartpluginvdummyvideotest 
echo "run plugin - dummy tracker.."
./osgartplugintdummytrackertest          

echo "run plugin - avfoundation video.."
./osgartpluginvqtkittest 
echo "run plugin - avfoundation video.."
./osgartpluginvavfoundationtest 

echo "run plugin - opencv video.."
./osgartpluginvopencvtest

echo "run plugin - artoolkit video.."
#./osgartpluginvartoolkittest

echo "run plugin - artoolkit tracker.."
#./osgartplugintartoolkittest

#osgartpluginsdummysensortest 
#osgartpluginsdummygsptest 
#osgartpluginsdummyinertialtest 
#osgartplugintdummytrackergpsinertialtest          
#osgartpluginvopencvtest
#osgartpluginvartoolkittest
#osgartplugintartoolkittest

#osgartviewer
#osgartmarkertraining                   