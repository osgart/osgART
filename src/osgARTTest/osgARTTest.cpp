/*
 *	osgART/osgARTTest
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

/*
 *
 * A simple example to demonstrate the most basic functionality of osgART.
 * By Julian Looser, Philip Lamb, Raphael Grasset, Hartmut Seichter.
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
#include <osgART/VideoForeground>
#include <osgART/ARSceneNode>


int main(int argc, char* argv[]) 
{

	osg::setNotifyLevel(osg::NOTICE);

	osgARTInit(&argc, argv);

	osgProducer::Viewer viewer;
	viewer.setUpViewer(osgProducer::Viewer::ESCAPE_SETS_DONE);
	viewer.getCullSettings().setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

#ifndef __linux
	// somehow on Ubuntu Dapper this ends up in a segmentation fault
	viewer.getCamera(0)->getRenderSurface()->fullScreen(false);
#endif

	osg::ref_ptr<osgART::ARSceneNode> root = new osgART::ARSceneNode;

	viewer.setSceneData(root.get());

	// load a video plugin
	osg::ref_ptr<osgART::GenericVideo> video = 
		osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit");

	// check if loading the plugin was successful
	if (!video.valid()) 
	{   
		// without video an AR application can not work
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;

		// quit the program
		exit(-1);
	}
	/* load a tracker plugin */
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit_tracker");

	if (tracker.valid()) 
	{

		// access a field within the tracker
		osg::ref_ptr< osgART::TypedField<int> > _threshold = 
			reinterpret_cast< osgART::TypedField<int>* >(tracker->get("threshold"));

		// values can only be accessed through a get()/set() mechanism
		if (_threshold.valid()) 
		{			
			// set the threshold
			_threshold->set(150);

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
	
	// flipping the video can be done on the fly or in advance
	video->setFlip(false,true);

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();

	// Connect the video to a tracker
	if (!root->connect(tracker.get(),video.get())) 
	{
		osg::notify(osg::FATAL) << "Error connecting video with tracker!" << std::endl;
		exit(-1);
	}
	

	// Adding video background
	osg::Group* foregroundGroup	= new osg::Group();

	osgART::VideoBackground* videoBackground=new osgART::VideoBackground(video.get());

	//specify a video texture rectangle (faster)
	videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_RECTANGLE);

	//initialize the video background
	videoBackground->init();
	
	//adding it to the scene graph
	foregroundGroup->addChild(videoBackground);

	foregroundGroup->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

	//use the projection matrix from the tracker (i.e. intrinsic camera parameters)
	//for the projection matrix.
	osg::Projection* projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));

	// create marker with id number '0'
	osg::ref_ptr<osgART::Marker> marker = tracker->getMarker(0);
		
	// check before accessing the linked marker
	if (!marker.valid()) 
	{
		osg::notify(osg::FATAL) << "No Marker defined!" << std::endl;
		exit(-1);
	}

	// activate the marker
	marker->setActive(true);

	// create a matrix transform related to the marker
	osg::ref_ptr<osg::MatrixTransform> markerTrans = 
		new osgART::ARTTransform(marker.get());

	//and simply create a blue cube object
	float boxSize = 40.0f;
	osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0, 0, boxSize / 2.0f), boxSize));
	sd->setColor(osg::Vec4(0, 0, 1, 1));
	
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(sd);
	markerTrans->addChild(geode);

	//assemble all things together
	osg::Group* sceneGroup = new osg::Group();
	sceneGroup->getOrCreateStateSet()->setRenderBinDetails(5, "RenderBin");
	sceneGroup->addChild(markerTrans.get());
	foregroundGroup->addChild(sceneGroup);
	
	osg::MatrixTransform* modelViewMatrix = new osg::MatrixTransform();
	modelViewMatrix->addChild(foregroundGroup);
	projectionMatrix->addChild(modelViewMatrix);
	
	root->addChild(projectionMatrix);
	
	video->start();

	viewer.realize();

	
    while (!viewer.done()) 
	{
		viewer.sync();	
        viewer.update();
        viewer.frame();	
    }
    
	viewer.sync();
    viewer.cleanup_frame();
    viewer.sync();

	//stop the video
	video->stop();
	//close the video
	video->close();
	
}
