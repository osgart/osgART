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
#include <osgART/VideoLayer>
#include <osgART/ARSceneNode>

#include <osgART/TransformFilterCallback>
#include "KeyboardHandler.h"


#define MARKER_CONF		"data/markers_list4.dat"
#define CAMERA_PARA		"data/camera_paraQC5000.dat"


osg::ref_ptr<osgART::TransformFilterCallback>	_transformFilterCallback;
osg::ref_ptr< osgART::TypedField<int> >			_threshold;
osgProducer::Viewer								viewer;



void outlierRejection();
void outlierRejection_inc_buf();
void outlierRejection_dec_buf();
void outlierRejection_inc_dist();
void outlierRejection_dec_dist();

void rotationalSmoothing();
void rotationalSmoothing_inc();
void rotationalSmoothing_dec();

void translationalSmoothing();
void translationalSmoothing_inc();
void translationalSmoothing_dec();

void increaseThreshold();
void decreaseThreshold();
void toggleFullscreen();

int main(int argc, char* argv[]) 
{

	//osg::setNotifyLevel(osg::DEBUG_INFO);

	//osgARTInit(&argc, argv);

	viewer;
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
		osgART::VideoManager::createVideoFromPlugin("osgart_video_artoolkit");

	// check if loading the plugin was successful
	if (!video.valid()) 
	{        
		// without video an AR application can not work
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
		exit(-1);
	}

	/* load a tracker plugin */
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		osgART::TrackerManager::createTrackerFromPlugin("osgart_tracker_artoolkit");

	// check if the tracker plugin could be loaded
	if (!tracker.valid()) 
	{
        // this example needs a tracker. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plugin!" << std::endl;
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

	// access a field within the tracker
	_threshold = 
		reinterpret_cast< osgART::TypedField<int>* >(tracker->get("threshold"));

	// values can only be accessed through a get()/set() mechanism
	if (_threshold.valid()) 
	{			
		// Set the threshold, and read back.
		_threshold->set(100);
		osg::notify(osg::WARN) << "Field 'threshold' = " << _threshold->get() << std::endl;
	} else {
		osg::notify(osg::WARN) << "Field 'threshold' not supported for this tracker" << std::endl;
	}
	
	// pass on marker and camera configuration
	tracker->init(video->getWidth(), video->getHeight(),MARKER_CONF,CAMERA_PARA);


	// Creating a video background
	osg::Group* foregroundGroup	= new osg::Group();

	// Creating a video background
	osg::ref_ptr<osgART::VideoLayer> videoBackground = 
		new osgART::VideoLayer(video.get() , 1);

	//initialize the video background
	videoBackground->init();
	
	//adding it to the scene graph
	foregroundGroup->addChild(videoBackground.get());

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

	// create and set filter callback
	_transformFilterCallback = new osgART::TransformFilterCallback();
	marker->setFilterCallback(_transformFilterCallback.get());
	
	// create marker with id number '1'
	osg::ref_ptr<osgART::Marker> marker1 = tracker->getMarker(1);
		
	// check before accessing the linked marker
	if (!marker1.valid()) 
	{
		osg::notify(osg::FATAL) << "No Marker defined!" << std::endl;
		exit(-1);
	}

	// activate the marker
	marker1->setActive(true);
	// create and set filter callback
	osgART::TransformFilterCallback* _transformFilterCallback1 = new osgART::TransformFilterCallback();
	marker1->setFilterCallback(_transformFilterCallback1);
	_transformFilterCallback1->enableRotationalSmoothing(true);
	_transformFilterCallback1->setRotationalSmoothingFactor(0.35);
	_transformFilterCallback1->enableTranslationalSmoothing(true);
	_transformFilterCallback1->setTranslationalSmoothingFactor(.05);


	// create a matrix transform related to marker 0 
	osg::ref_ptr<osg::MatrixTransform> markerTrans = 
		new osgART::ARTTransform(marker.get());

	//and simply create a blue cube object
	float boxSize = 40.0f;
	osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0, 0, boxSize / 2.0f), boxSize));
	sd->setColor(osg::Vec4(0, 0, 1, 1));
	
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(sd);
	markerTrans->addChild(geode);


	// create a matrix transform related to the marker1
	osg::ref_ptr<osg::MatrixTransform> markerTrans1 = 
		new osgART::ARTTransform(marker1.get());

	//and simply create a blue cube object
	osg::ShapeDrawable* sd1 = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0, 0, boxSize / 2.0f), boxSize));
	sd1->setColor(osg::Vec4(1, 0, 0, 1));
	
	osg::Geode* geode1 = new osg::Geode();
	geode1->addDrawable(sd1);
	markerTrans1->addChild(geode1);

	//assemble all things together
	osg::Group* sceneGroup = new osg::Group();
	sceneGroup->getOrCreateStateSet()->setRenderBinDetails(5, "RenderBin");
	sceneGroup->addChild(markerTrans.get());
	sceneGroup->addChild(markerTrans1.get());
	foregroundGroup->addChild(sceneGroup);
	
	osg::MatrixTransform* modelViewMatrix = new osg::MatrixTransform();
	modelViewMatrix->addChild(foregroundGroup);
	projectionMatrix->addChild(modelViewMatrix);
	
	
	/*Keyboard Event Handler*/
	keyboardHandler* keyboardhandler = new keyboardHandler();

	if (_transformFilterCallback.valid()){
		// Outlier Rejection
		keyboardhandler->addFunction('q', outlierRejection);
		keyboardhandler->addFunction('w', outlierRejection_inc_buf);
		keyboardhandler->addFunction('e', outlierRejection_dec_buf);
		keyboardhandler->addFunction('r', outlierRejection_inc_dist);
		keyboardhandler->addFunction('t', outlierRejection_dec_dist);
		//Rotational Smoothing
		keyboardhandler->addFunction('a', rotationalSmoothing);
		keyboardhandler->addFunction('s', rotationalSmoothing_inc);
		keyboardhandler->addFunction('d', rotationalSmoothing_dec);
		// Translational Smoothing
		keyboardhandler->addFunction('z', translationalSmoothing);
		keyboardhandler->addFunction('x', translationalSmoothing_inc);
		keyboardhandler->addFunction('c', translationalSmoothing_dec);
	}
	// ARToolKit Threshold
	keyboardhandler->addFunction(osgGA::GUIEventAdapter::KEY_KP_Add, increaseThreshold);
	keyboardhandler->addFunction(osgGA::GUIEventAdapter::KEY_KP_Subtract, decreaseThreshold);
	// Fullscreen
	keyboardhandler->addFunction('f', toggleFullscreen);

	viewer.getEventHandlerList().push_front(keyboardhandler); 
	keyboardhandler->setEnabled(true);
	/*End Keyboard*/


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

void outlierRejection(){
	if (_transformFilterCallback.valid()){
		_transformFilterCallback->enableTransOutlierRejection(!_transformFilterCallback->TransOutlierRejection());
		std::cout << "Outlier rejection enabled: " << _transformFilterCallback->TransOutlierRejection() << std::endl;
	}
	else{
		std::cerr << "_transformFilterCallback invalid!!!" << std::endl;
	}
}
void outlierRejection_inc_buf(){
	_transformFilterCallback->setTransOutlierMaxNumber(_transformFilterCallback->getTransOutlierMaxNumber()-1);
	std::cout << "Max number of Translational Outliers: " << _transformFilterCallback->getTransOutlierMaxNumber() << std::endl;
}
void outlierRejection_dec_buf(){
	_transformFilterCallback->setTransOutlierMaxNumber(_transformFilterCallback->getTransOutlierMaxNumber()+1);
	std::cout << "Max number of Translational Outliers: " << _transformFilterCallback->getTransOutlierMaxNumber() << std::endl;
}
void outlierRejection_inc_dist(){
	_transformFilterCallback->setTransOutlierDist(_transformFilterCallback->getTransOutlierDist()-1);
	std::cout << "Translational Outliers Dist: " << _transformFilterCallback->getTransOutlierDist() << std::endl;
}
void outlierRejection_dec_dist(){
	_transformFilterCallback->setTransOutlierDist(_transformFilterCallback->getTransOutlierDist()+1);
	std::cout << "Translational Outliers Dist: " << _transformFilterCallback->getTransOutlierDist() << std::endl;
}

void rotationalSmoothing(){
	_transformFilterCallback->enableRotationalSmoothing(!_transformFilterCallback->RotationalSmoothing());
	std::cout << "Rotational Smoothing enabled: " << _transformFilterCallback->RotationalSmoothing() << std::endl;
}
void rotationalSmoothing_inc(){
	_transformFilterCallback->setRotationalSmoothingFactor(_transformFilterCallback->getRotationalSmoothingFactor()-0.05);
	std::cout << "Rotational Smoothing factor: " << _transformFilterCallback->getRotationalSmoothingFactor() << std::endl;
}
void rotationalSmoothing_dec(){
	_transformFilterCallback->setRotationalSmoothingFactor(_transformFilterCallback->getRotationalSmoothingFactor()+0.05);
	std::cout << "Rotational Smoothing factor: " << _transformFilterCallback->getRotationalSmoothingFactor() << std::endl;
}

void translationalSmoothing(){
	_transformFilterCallback->enableTranslationalSmoothing(!_transformFilterCallback->TranslationalSmoothing());
	std::cout << "Translational Smoothing enabled: " << _transformFilterCallback->TranslationalSmoothing() << std::endl;
}
void translationalSmoothing_inc(){
	_transformFilterCallback->setTranslationalSmoothingFactor(_transformFilterCallback->getTranslationalSmoothingFactor()-0.05);
	std::cout << "Translational Smoothing factor: " << _transformFilterCallback->getTranslationalSmoothingFactor() << std::endl;
}
void translationalSmoothing_dec(){
	_transformFilterCallback->setTranslationalSmoothingFactor(_transformFilterCallback->getTranslationalSmoothingFactor()+0.05);
	std::cout << "Translational Smoothing factor: " << _transformFilterCallback->getTranslationalSmoothingFactor() << std::endl;
}

void increaseThreshold()
{
	std::cout << "incresing Threshold" << std::endl;
	if (_threshold.valid()) 
		{		
			/* set the threshold */
			_threshold->set(_threshold->get()+5);
			std::cerr << "Threshold: " << _threshold->get() << std::endl;
		} 
	else 
		{
			std::cerr << "Field 'threshold' supported for this tracker" << std::endl;
		}
}

void decreaseThreshold()
{
	std::cout << "incresing Threshold" << std::endl;
	if (_threshold.valid()) 
		{		
			/* set the threshold */
			_threshold->set(_threshold->get()-5);
			std::cerr << "Threshold: " << _threshold->get() << std::endl;
		} 
	else 
		{
			std::cerr << "Field 'threshold' supported for this tracker" << std::endl;
		}
}

void toggleFullscreen()
{
	viewer.getCamera(0)->getRenderSurface()->fullScreen(!viewer.getCamera(0)->getRenderSurface()->isFullScreen());
}