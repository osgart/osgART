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

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/AutoTransform>
#include <osg/ShapeDrawable>
#include <osg/Geometry>

#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>
#include <osgART/VideoBackground>
#include <osgART/VideoPlane>
#include <osg/io_utils>

#include <GazeInteraction.h>


 #define AR_VIDEO_WIN32_DIRECTSHOW_2_71

#ifdef _WIN32
#  ifdef AR_VIDEO_WIN32_DIRECTSHOW_2_71
#    define MY_VCONF "data/WDM_camera_flipV.xml"
//#    define MY_VCONF "data/video_input.xml"
#  else
#    define MY_VCONF "showDlg,flipV"
#  endif
#elif defined(__linux)
// # define MY_VCONF "videotestsrc ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
# define MY_VCONF "v4l2src use-fixed-fps=false ! decodebin ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
#else
#  define MY_VCONF ""
#endif


int main(int argc, char* argv[]) {

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

	/* load a video plugin */
	osg::ref_ptr<osgART::GenericVideo> video = 
	//osgART::VideoManager::createVideoFromPlugin("osgart_dsvl2", cfg);
	osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit", cfg);
	//osgART::VideoManager::createVideoFromPlugin("osgart_cvcam", cfg);

	

	/* check if loading the plugin was successful */
	if (!video.valid()) {
		std::cerr << "Could not initialize video!" << std::endl;
		exit(1);
	}
	
	/* load a tracker plugin */
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		//osgART::TrackerManager::createTrackerFromPlugin("osgart_bazar_tracker");
		// osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit4nft_tracker");
		 osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit_tracker");

	//osg::ref_ptr<osgART::GenericTracker> tracker = 
	//	osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit_tracker");
	
	if (tracker.valid()) {
	} 
	else {
		std::cerr << "Could not initialize tracker plugin!" << std::endl;
		exit(1);
	}	
	
	/* 
	cfg.deviceconfig = "imagewithmarker.png";
	osgART::GenericVideo* video = osgART::VideoManager::createVideoFromPlugin("osgart_dummyimage", cfg);
	*/

	//osgART::GenericVideo* video = osgART::VideoManager::createVideoFromPlugin("osgart_intranel",
	//	cfg);

	/*
	cfg.id = 0;
	cfg.type = osgART::VIDEOFORMAT_Y16;
	cfg.width = 800;
	cfg.height = 600;
	cfg.framerate = osgART::VIDEOFRAMERATE_30;
	
	osg::ref_ptr<osgART::GenericVideo> video = osgART::VideoManager::createVideoFromPlugin("osgart_ptgrey", cfg);

	if (video.valid()) {
		
		osg::ref_ptr< osgART::TypedField<bool> > _roi = 
			dynamic_cast< osgART::TypedField<bool>* >(video->get("ROI"));

		osg::ref_ptr< osgART::TypedField<osg::Vec4s> > _set_roi = 
			dynamic_cast< osgART::TypedField<osg::Vec4s>* >(video->get("setROI"));

		if (_roi.valid() && _set_roi.valid()) {

			_roi->set(false);            
			_set_roi->set(osg::Vec4s(16,16,320,240));

		}
	}
	*/

	/* open the video */
	video->open();


	/* Initialise the tracker */
	//tracker->init(video->getWidth(), video->getHeight(),"data/markers_list4.dat","data/camera_c_mlee.txt");
	tracker->init(video->getWidth(), video->getHeight(),"data/markers_list4.dat","data/camera_para4.dat");
	//tracker->init(video->getWidth(), video->getHeight(),"data/artbazar_config.txt","data/camera_para.dat");


	//Adding video background
	osg::Group* foregroundGroup	= new osg::Group();

	osgART::VideoBackground* videoBackground=new osgART::VideoBackground(video->getId());

//	videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_2D);

	videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_RECTANGLE);

	videoBackground->init();
	
	foregroundGroup->addChild(videoBackground);

	foregroundGroup->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");


	osg::Projection* projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
	
	osg::ref_ptr<osgART::ARTTransform> markerTrans = new osgART::ARTTransform(0);

    
	// never assume the Marker really exists
	osg::ref_ptr<osgART::Marker> marker = markerTrans->getMarker();

	// check before accessing the linked marker
	if (marker.valid()) marker->setActive(true);

	// this adds a vertical cylinder centred at origin
	//float cylrad = 5.0f;	
	//float cylheight = 1000.0f;
	//osg::ShapeDrawable* sd2 = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0, 0, cylheight / 2.0f), cylrad, cylheight));
	//sd2->setColor(osg::Vec4(1.0f,1.0f,0.0f,1.0f));


	//// add a couple of boxes
	//float boxSize = 150.0f;

	//osg::ShapeDrawable* sd00 = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(-boxSize - 10, boxSize + 10, 0), boxSize/2));
	//sd00->setColor(osg::Vec4(0, 0, 1, 1));
	//osg::Geode* geodeB00 = new osg::Geode();
	//geodeB00->addDrawable(sd00);
	//geodeB00->setName("Sphere00");
	//markerTrans->addChild(geodeB00);

	//osg::ShapeDrawable* sd01 = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, boxSize + 10, 0), boxSize/2));
	//sd01->setColor(osg::Vec4(0, 0, 1, 1));
	//osg::Geode* geodeB01 = new osg::Geode();
	//geodeB01->addDrawable(sd01);
	//geodeB01->setName("Sphere01");
	//markerTrans->addChild(geodeB01);
	//
	//osg::ShapeDrawable* sd02 = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(boxSize + 10, boxSize + 10, 0), boxSize/2));
	//sd02->setColor(osg::Vec4(0, 0, 1, 1));
	//osg::Geode* geodeB02 = new osg::Geode();
	//geodeB02->addDrawable(sd02);
	//geodeB02->setName("Sphere02");
	//markerTrans->addChild(geodeB02);

	//osg::ShapeDrawable* sd10 = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(-boxSize - 10, 0, 0), boxSize/2));
	//sd10->setColor(osg::Vec4(0, 0, 1, 1));
	//osg::Geode* geodeB10 = new osg::Geode();
	//geodeB10->addDrawable(sd10);
	//geodeB10->setName("Sphere10");
	//markerTrans->addChild(geodeB10);
	//	
	//osg::ShapeDrawable* sd11 = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 0, 0), boxSize/2));
	//sd11->setColor(osg::Vec4(0, 0, 1, 1));
	//osg::Geode* geodeB11 = new osg::Geode();
	//geodeB11->addDrawable(sd11);
	//geodeB11->setName("Sphere11");
	//markerTrans->addChild(geodeB11);

	//osg::ShapeDrawable* sd12 = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(boxSize + 10, 0, 0), boxSize/2));
	//sd12->setColor(osg::Vec4(0, 0, 1, 1));
	//osg::Geode* geodeB12 = new osg::Geode();
	//geodeB12->addDrawable(sd12);
	//geodeB12->setName("Sphere12");
	//markerTrans->addChild(geodeB12);


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
	
	//viewer.getSceneHandlerList().at(0)->getSceneView();
	
	// put all interaction targets in this group
	osg::Group* GItargetGroup = new osg::Group();
	markerTrans->addChild(GItargetGroup);
	// init GazeInteraction plugin
	GazeInteraction GInteraction;
	GInteraction.init();
	// set target group
	GInteraction.addTargets2Scene(GItargetGroup);
	// set group used for intersection
	GInteraction.setIntersectionScene(foregroundGroup);

	osg::Node* hitNode = NULL;


	video->start();

    while (!viewer.done()) {
		
		viewer.sync();	
		
		video->update();

		tracker->setImage(video.get());
		
		tracker->update();

		projectionMatrix->setMatrix(osg::Matrix(tracker->getProjectionMatrix()));
		
		viewer.update();
		viewer.frame();
		
		hitNode = GInteraction.update();
		/*if (hitNode != NULL){	
			if(strcmp(hitNode->getName().c_str(), "Sphere00")){
				sd00->setColor(osg::Vec4(0, 1, 0, 1));
				sd01->setColor(osg::Vec4(0, 0, 1, 1));
				sd02->setColor(osg::Vec4(0, 0, 1, 1));
				sd10->setColor(osg::Vec4(0, 0, 1, 1));
				sd11->setColor(osg::Vec4(0, 0, 1, 1));
				sd12->setColor(osg::Vec4(0, 0, 1, 1));
			}
			else if (strcmp(hitNode->getName().c_str(), "Sphere01")){
				sd00->setColor(osg::Vec4(0, 0, 1, 1));
				sd01->setColor(osg::Vec4(0, 1, 0, 1));
				sd02->setColor(osg::Vec4(0, 0, 1, 1));
				sd10->setColor(osg::Vec4(0, 0, 1, 1));
				sd11->setColor(osg::Vec4(0, 0, 1, 1));
				sd12->setColor(osg::Vec4(0, 0, 1, 1));
			}
			else if (strcmp(hitNode->getName().c_str(), "Sphere02")){
				sd00->setColor(osg::Vec4(0, 0, 1, 1));
				sd01->setColor(osg::Vec4(0, 0, 1, 1));
				sd02->setColor(osg::Vec4(0, 1, 0, 1));
				sd10->setColor(osg::Vec4(0, 0, 1, 1));
				sd11->setColor(osg::Vec4(0, 0, 1, 1));
				sd12->setColor(osg::Vec4(0, 0, 1, 1));
			}
			else if (strcmp(hitNode->getName().c_str(), "Sphere10")){
				sd00->setColor(osg::Vec4(0, 0, 1, 1));
				sd01->setColor(osg::Vec4(0, 0, 1, 1));
				sd02->setColor(osg::Vec4(0, 0, 1, 1));
				sd10->setColor(osg::Vec4(0, 1, 0, 1));
				sd11->setColor(osg::Vec4(0, 0, 1, 1));
				sd12->setColor(osg::Vec4(0, 0, 1, 1));
			}
			else if (strcmp(hitNode->getName().c_str(), "Sphere11")){
				sd00->setColor(osg::Vec4(0, 0, 1, 1));
				sd01->setColor(osg::Vec4(0, 0, 1, 1));
				sd02->setColor(osg::Vec4(0, 0, 1, 1));
				sd10->setColor(osg::Vec4(0, 0, 1, 1));
				sd11->setColor(osg::Vec4(0, 1, 0, 1));
				sd12->setColor(osg::Vec4(0, 0, 1, 1));
			}
			else if (strcmp(hitNode->getName().c_str(), "Sphere12")){
				sd00->setColor(osg::Vec4(0, 0, 1, 1));
				sd01->setColor(osg::Vec4(0, 0, 1, 1));
				sd02->setColor(osg::Vec4(0, 0, 1, 1));
				sd10->setColor(osg::Vec4(0, 0, 1, 1));
				sd11->setColor(osg::Vec4(0, 0, 1, 1));
				sd12->setColor(osg::Vec4(0, 1, 0, 1));
			}
			else{
				sd00->setColor(osg::Vec4(0, 0, 1, 1));
				sd01->setColor(osg::Vec4(0, 0, 1, 1));
				sd02->setColor(osg::Vec4(0, 0, 1, 1));
				sd10->setColor(osg::Vec4(0, 0, 1, 1));
				sd11->setColor(osg::Vec4(0, 0, 1, 1));
				sd12->setColor(osg::Vec4(0, 0, 1, 1));
			}	
		}
		else{
				sd00->setColor(osg::Vec4(0, 0, 1, 1));
				sd01->setColor(osg::Vec4(0, 0, 1, 1));
				sd02->setColor(osg::Vec4(0, 0, 1, 1));
				sd10->setColor(osg::Vec4(0, 0, 1, 1));
				sd11->setColor(osg::Vec4(0, 0, 1, 1));
				sd12->setColor(osg::Vec4(0, 0, 1, 1));
		}*/

		

    }

	viewer.sync();
    viewer.cleanup_frame();
    viewer.sync();

	video->stop();
	video->close();
	
}
