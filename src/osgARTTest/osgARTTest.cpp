/*
 *
 * Simple Example to demonstrate OSGART
 *
 * Copyright (c) 2005-2006
 * Julian Looser, Philip Lamb, Rapha‘l Grasset, Hartmut Seichter.
 *
 */

#include <Producer/RenderSurface>
#include <osgProducer/Viewer>

#include <osg/Notify>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/AutoTransform>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Image>


#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>
#include <osgART/VideoBackground>
#include <osgART/VideoPlane>


#ifdef _WIN32
#    define MY_VCONF "data/WDM_camera_flipV.xml"
#else
// Please read documentation for setting video parameters
#  define MY_VCONF ""
#endif


int main(int argc, char* argv[]) 
{

	osg::setNotifyLevel(osg::ALWAYS);

	osgARTInit(&argc, argv);
	
	osgProducer::Viewer viewer;
	viewer.setUpViewer(osgProducer::Viewer::ESCAPE_SETS_DONE);
	viewer.getCullSettings().setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

#ifndef __linux
	// somehow on Ubuntu Dapper this ends up in a segmentation fault
	viewer.getCamera(0)->getRenderSurface()->fullScreen(false);
#endif

	osgART::VideoConfiguration cfg;
	cfg.deviceconfig = MY_VCONF;

	// load a video plugin
	osg::ref_ptr<osgART::GenericVideo> video = 
	osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit", cfg);

	// check if loading the plugin was successful
	if (!video.valid()) 
	{
		// without video an AR application can not work
		osg::notify(osg::FATAL) << "Could not initialize video!" << std::endl;

		// quit the program
		exit(1);
	}
	
	/* load a tracker plugin */
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit_tracker");	// for ARToolkit 2.7
		//osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkitplus_tracker"); // for ARToolkit PLUS

	if (tracker.valid()) 
	{		
		// access a field within the tracker
		osg::ref_ptr< osgART::TypedField<int> > _threshold = 
			reinterpret_cast< osgART::TypedField<int>* >(tracker->get("threshold"));

		// values can only be accessed through a get()/set() mechanism
		if (_threshold.valid()) 
		{			
			// set the threshold
			_threshold->set(100);

			/* check what we actually get */
			osg::notify() << "Field 'threshold' = " << _threshold->get() << std::endl;

		} else 
		{
			osg::notify() << "Field 'threshold' supported for this tracker" << std::endl;
		}		

	} else 
	{
        // this example needs a tracker
		std::cerr << "Could not initialize tracker plugin!" << std::endl;

		// quit the program
		exit(-1);
	}	
	
	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();

	// Initialise the tracker with the dimensions of the video image
	if(!tracker->init(video->getWidth(), video->getHeight()))
	{
		std::cerr << "Could not initialize tracker plugin!" << std::endl;
		exit(-1);		
	}

	// From here on the scene is going to be built

	// Adding video background
	osg::Group* foregroundGroup	= new osg::Group();

	//osgART::VideoBackground* videoBackground=new osgART::VideoBackground(video.get());
	osgART::VideoBackground* videoBackground=new osgART::VideoBackground(0);


	videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_RECTANGLE);

	videoBackground->init();
	
	foregroundGroup->addChild(videoBackground);

	foregroundGroup->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");


	osg::Projection* projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));

	// create marker with id number '0'
	osg::ref_ptr<osgART::Marker> marker = tracker->getMarker(0);
		
	// check before accessing the linked marker
	if (!marker.valid()) {
        
		osg::notify(osg::FATAL) << "No Marker defined!" << std::endl;

		exit(-1);
	}

	// activate the marker
	marker->setActive(true);

	// create a matrix transform related to the marker
	osg::ref_ptr<osg::MatrixTransform> markerTrans = 
		//new osgART::ARTTransform(marker.get());
		new osgART::ARTTransform(0,0);


	float boxSize = 40.0f;
	osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0, 0, boxSize / 2.0f), boxSize));
	sd->setColor(osg::Vec4(0, 0, 1, 1));
	
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(sd);
	markerTrans->addChild(geode);

	// 
	osg::Group* sceneGroup = new osg::Group();
	sceneGroup->getOrCreateStateSet()->setRenderBinDetails(5, "RenderBin");
	sceneGroup->addChild(markerTrans.get());
	foregroundGroup->addChild(sceneGroup);
	
	osg::MatrixTransform* modelViewMatrix = new osg::MatrixTransform();
	modelViewMatrix->addChild(foregroundGroup);
	projectionMatrix->addChild(modelViewMatrix);
	
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(projectionMatrix);

	viewer.setSceneData(root.get());

	viewer.realize();
	
	video->start();
	
    while (!viewer.done()) 
	{
		
		viewer.sync();	
		
		video->update();

		tracker->setImage(video.get());
		tracker->update();
		
        viewer.update();
        viewer.frame();

    }
    
	viewer.sync();
    viewer.cleanup_frame();
    viewer.sync();

	video->stop();
	video->close();
	
}
