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
#include <osgText/Font>
#include <osgText/Text>
#include <osg/Matrix>

#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>
#include <osgART/VideoBackground>
#include <osgART/VideoPlane>
#include <osg/io_utils>



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


osg::Matrixd* mat_average(osg::Matrixd);


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
		 osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkitMMauto_tracker");

	if (tracker.valid()) {
	} 
	else {
		std::cerr << "Could not initialize tracker plugin!" << std::endl;
		exit(1);
	}	
	
	/* open the video */
	video->open();
	
	/* Initialise the tracker */
	//tracker->init(video->getWidth(), video->getHeight(),"data/markers_list4.dat","data/camera_para4.dat");
	tracker->init(video->getWidth(), video->getHeight(),"data/autoreg_markers.dat","data/camera_para4.dat");
	
	//Adding video background to scene graph
	osg::Group* foregroundGroup	= new osg::Group();

	osgART::VideoBackground* videoBackground=new osgART::VideoBackground(video->getId());
	//videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_2D);
	videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_RECTANGLE);
	videoBackground->init();
	
	foregroundGroup->addChild(videoBackground);
	foregroundGroup->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

	osg::Projection* projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
		
	osg::ref_ptr<osgART::ARTTransform> markerTrans0 = new osgART::ARTTransform(0);
    // never assume the Marker really exists
	osg::ref_ptr<osgART::Marker> anchor_marker = markerTrans0->getMarker();
	// check before accessing the linked marker
	if (anchor_marker.valid()) anchor_marker->setActive(true);

	// Debug this adds a vertical cylinder 
	float cylrad = 5.0f;	
	float cylheight = 1000.0f;
	osg::ShapeDrawable* sd0 = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0, 0, cylheight / 2.0f), cylrad, cylheight));
	sd0->setColor(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
	osg::Geode* geode0 = new osg::Geode();
	geode0->addDrawable(sd0);
	markerTrans0->addChild(geode0);

	osg::ref_ptr<osgART::ARTTransform> markerTrans1 = new osgART::ARTTransform(1);
    // never assume the Marker really exists
	osg::ref_ptr<osgART::Marker> marker1 = markerTrans1->getMarker();
	// check before accessing the linked marker
	if (marker1.valid()) marker1->setActive(true);
	osg::ShapeDrawable* sd1 = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(100, -50, cylheight / 2.0f), cylrad, cylheight));
	sd1->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	osg::Geode* geode1 = new osg::Geode();
	geode1->addDrawable(sd1);
	markerTrans1->addChild(geode1);

	osg::Group* sceneGroup = new osg::Group();
	sceneGroup->getOrCreateStateSet()->setRenderBinDetails(5, "RenderBin");
	sceneGroup->addChild(markerTrans0.get());
	sceneGroup->addChild(markerTrans1.get());
	
	foregroundGroup->addChild(sceneGroup);

	osg::MatrixTransform* modelViewMatrix = new osg::MatrixTransform();
	modelViewMatrix->addChild(foregroundGroup);
	projectionMatrix->addChild(modelViewMatrix);
	
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(projectionMatrix);

	viewer.setSceneData(root.get());
	viewer.realize();
	
	video->start();
	
	while (!viewer.done()) {
		
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



osg::Matrixd* mat_average(osg::Matrixd){

	osg::Matrixd* average;



	return average;

}