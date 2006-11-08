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
		// osgART::TrackerManager::createTrackerFromPlugin("osgart_bazar_tracker");
		osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit4nft_tracker");
		// osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit_tracker");

	//osg::ref_ptr<osgART::GenericTracker> tracker = 
	//	osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit_tracker");
	
	if (tracker.valid()) {

	} else {

		std::cerr << "Could not initialize BazAR tracker plugin!" << std::endl;
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
	//tracker->init(video->getWidth(), video->getHeight(),"data/markers_list4.dat","data/camera_para4.dat");
	tracker->init(video->getWidth(), video->getHeight(),"data/markers_list.dat","data/camera_para.dat");


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

	float boxSize = 50.0f;
	osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Box(osg::Vec3(125, 125, boxSize / 2.0f), boxSize));
	sd->setColor(osg::Vec4(0, 0, 1, 1));
	
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(sd);
	markerTrans->addChild(geode);

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
	
    while (!viewer.done()) {
		
		viewer.sync();	
		
		video->update();

		tracker->setImage(video.get());
		tracker->update();
		
		projectionMatrix->setMatrix(osg::Matrix(tracker->getProjectionMatrix()));
	
		osg::Matrix projMat=osg::Matrix(tracker->getProjectionMatrix());
		
		osg::Matrix modelMat=osgART::TrackerManager::getInstance()->getTracker(0)->getMarker(0)->getTransform();

        viewer.update();
        viewer.frame();
	
    }
    
	viewer.sync();
    viewer.cleanup_frame();
    viewer.sync();

	video->stop();
	video->close();
	
}
