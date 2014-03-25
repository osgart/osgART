echo "run osgart examples.."

export OSG_NOTIFY_LEVEL=WARN

echo "-- PLUGIN TEST--"
echo "run plugin - artoolkit video.."
./osgartpluginvartoolkittest

echo "run plugin - artoolkit tracker.."
./osgartplugintartoolkittest
          
echo "-- SIMPLE API--"
echo "run simple - simple.."
./osgartsimpleartoolkit
echo "run simple - keyboard input.."
./osgartsimplekeyboardinputartoolkit
echo "run simple - mouse picking.."
./osgartsimplemousepickingartoolkit
echo "run simple - manipulator.."
./osgartsimplemanipulatorartoolkit     
echo "run simple - target multiple.."
./osgartsimpletargetmultipleartoolkit
echo "run simple - target 2c proximity.."
./osgartsimpletarget2cproximityartoolkit
echo "run simple - target 2t proximity.."
./osgartsimpletarget2tproximityartoolkit
echo "run simple - rigid body anim.."    
./osgartsimplerbanimartoolkit
echo "run simple - particle.."
./osgartsimpleparticleartoolkit

echo "-- INTERMEDIATE API--"

echo "run interm - simple.."
./osgartintermsimpleartoolkit
echo "run interm - keyboard input.."
./osgartintermkeyboardinputartoolkit
echo "run interm - mouse picking.."
./osgartintermmousepickingartoolkit
echo "run interm - manipulator.."
./osgartintermmanipulatorartoolkit
echo "run interm - target motion.."
./osgartintermtargetmotionartoolkit
echo "run interm - target multiple.."
./osgartintermtargetmultipleartoolkit
echo "run interm - target 2c proximity.."
./osgartintermtarget2cproximityartoolkit
echo "run interm - target 2t proximity.."
./osgartintermtarget2tproximityartoolkit
echo "run interm - target paddle.."
./osgartintermtargetpaddleyartoolkit

echo "-- ADVANCED API--"
echo "run advanced - simple.."
./osgartadvancedsimpleartoolkit

         