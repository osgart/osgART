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

#include <osg/Matrixf>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgUtil/SmoothingVisitor>

#include "ImageCubeMap"

// shader stuff
#include "ShaderFactory.h"
#include "ARScene.h"
#include "ARNode.h"
#include "MotionBlurredModel.h"
#include "MotionBlurredARScene.h"
#include "Exposure.h"
#include "ExposureKeyboardEventHandler.h"


#define AR_VIDEO_WIN32_DIRECTSHOW_2_71

#ifdef _WIN32
#  ifdef AR_VIDEO_WIN32_DIRECTSHOW_2_71
#    define MY_VCONF "data/WDM_camera_flipV.xml"
#  else
#    define MY_VCONF "showDlg,flipV"
#  endif
#elif defined(__linux)
// # define MY_VCONF "videotestsrc ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
# define MY_VCONF "v4l2src use-fixed-fps=false ! decodebin ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
#else
#  define MY_VCONF ""
#endif

ShaderFactory sf;

osg::Node* addARTModel(char* name,float sizeModel,float shiftX, float shiftY, float heightMarker)
{

	// create a new node by reading in model from file
	osg::Node*	modelNode = osgDB::readNodeFile(name);
	
	//put model in origin
	osg::Matrix modelScale;
	osg::Matrix modelTranslate;
	osg::Matrix modelRot;
	
	osg::MatrixTransform*	unitTransform = new osg::MatrixTransform;
	osg::BoundingSphere bound=modelNode->getBound();

	modelScale.makeScale(sizeModel/bound.radius(),sizeModel/bound.radius(),sizeModel/bound.radius());
	modelRot.makeRotate(0.0,osg::Vec3f(1.,0.,0.));
	//modelRot.makeRotate(osg::DegreesToRadians(-90.),osg::Vec3f(1.,0.,0.));
	//modelTranslate.makeTranslate(-bound.center());

	unitTransform->postMult(modelTranslate);
	unitTransform->postMult(modelRot);
	unitTransform->postMult(modelScale);

	unitTransform->addChild(modelNode);

	//put model on the marker (not on the middle)
	osg::MatrixTransform*	trackerCenterTransform = new osg::MatrixTransform;

	osg::Matrix artCenter;
	artCenter.makeTranslate(shiftX,shiftY,heightMarker);
	trackerCenterTransform->postMult(artCenter);

	trackerCenterTransform->addChild(unitTransform);

	osg::MatrixTransform* myObject=new osg::MatrixTransform;
	
	myObject->addChild(trackerCenterTransform);


	osg::MatrixList mList = modelNode->getWorldMatrices();
	std::cout << mList.size() << std::endl;

	return myObject;
}

osg::Node* addLightAt(osg::StateSet* rootStateSet, osg::Vec3 pos)
{
    osg::Group* lightGroup = new osg::Group;
   
    // create a directional light (infinite distance place at 45 degrees)
    osg::Light* myLight = new osg::Light;
    myLight->setLightNum(1);
	myLight->setPosition(osg::Vec4(pos,1));
    myLight->setAmbient(osg::Vec4(0.5f,0.5f,0.5f,1.0f));
    myLight->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
    myLight->setConstantAttenuation(1.0f);

    osg::LightSource* lightS = new osg::LightSource;	
    lightS->setLight(myLight);
    lightS->setLocalStateSetModes(osg::StateAttribute::ON); 

    lightS->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
 
    lightGroup->addChild(lightS);

	rootStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    return lightGroup;
}


osg::TextureCubeMap* readCubeMap( const std::string filename )
{
    osg::ref_ptr<osgDB::ReaderWriter::Options> options;

	ImageCubeMap image;
    if ( !image.readImageFile( filename, options.get() ) ) return NULL;
    osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;
    image.setIntoTexture( cubemap );

    cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

	// why this doesn't work???
	//cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    //cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

	// LDR 
    cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
    return cubemap;
}

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
	osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit");

	/* check if loading the plugin was successful */
	if (!video.valid()) {
		std::cerr << "Could not initialize video!" << std::endl;
		exit(1);
	}
	
	/* load a tracker plugin */
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit_tracker");


	if (tracker.valid()) {

		/* RFC: this how you would get any type in and out through the plugin system */
		osg::ref_ptr< osgART::TypedField<int> > _threshold = 
			dynamic_cast< osgART::TypedField<int>* >(tracker->get("threshold"));

		/* values can only be accessed through a get()/set() mechanism */
		if (_threshold.valid()) 
		{
			
			// set the threshold
			_threshold->set(100);

			// check what we actually get
			std::cout << "Threshold: " << _threshold->get() << std::endl;
		}

		

	} else {

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
	tracker->init(video->getWidth(), video->getHeight());

	//
	osg::ref_ptr<ARScene> arScene = new ARScene;
	arScene->init(tracker);

	
	int videoBGWidth = video->getWidth();
	int videoBGHeight = video->getHeight();

	osg::ref_ptr<osg::Texture> backgroundTexture = arScene->initTextureVideoBackground( video.get(), videoBGWidth/10, videoBGHeight/10, true, true, GL_RGBA16F_ARB);
	osg::ref_ptr<osg::Texture> foregroundTexture = arScene->initTextureForeground(videoBGWidth/10, videoBGHeight/10, true);

	
    const float radius = 100.0f;
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),radius)));
    osg::ref_ptr<osg::Node> model = geode;

	// load truck model
	//osg::ref_ptr<osg::Node> model = addARTModel("david.ive", 150, 0,0,120);
	//osg::ref_ptr<osg::Node> model = addARTModel("cow.osg", 200, 0,0,0);
	//osg::ref_ptr<osg::Node> model = addARTModel("dumptruck.osg", 100, 0,0,0);
	osg::Vec3f lightPos = osg::Vec3f(100.0,100.0,100.0);
	addLightAt( model->getOrCreateStateSet(), lightPos);

    // run optimization over the scene graph
    osgUtil::Optimizer optimzer;
    optimzer.optimize(model.get());

    // create normals.    
    osgUtil::SmoothingVisitor smoother;
    model->accept(smoother);


	// put the model to marker
	osg::ref_ptr<ARNode> hiroMarkerWithTruck =		
		arScene->addNewARNodeWith( model.get() );

	// Enviromapping with "uffizi_cross.hdr" which is HDR image :)
	std::string cubemapFileName = "uffizi_cross.hdr";
	osg::Texture* cubemap = readCubeMap( cubemapFileName.c_str() );
	
	sf.addVertexAndFragmentShaderFromFile("./data/shader/Enviromapping.vert",
    									  "./data/shader/Enviromapping.frag", 
										  model.get());

	
	model->getOrCreateStateSet()->setTextureAttributeAndModes(0, cubemap ,osg::StateAttribute::ON);	

	//osg::ref_ptr<DummyImageLayer> finalImageLayer = new DummyImageLayer;
	//finalImageLayer->init(videoBGWidth,videoBGHeight);
	//finalImageLayer->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

	// add HDR into SceneGroup
	osg::ref_ptr<osg::Group> sceneGroup = arScene->getSceneGroup();
	sf.addVertexAndFragmentShaderFromFile("./data/shader/ImageFilter.vert",
										  "./data/shader/readtexture.frag",
										  sceneGroup.get());
	
	//sceneGroup
	Exposure *exposure = new Exposure();

	sceneGroup->getOrCreateStateSet()->addUniform(exposure->getUniformMax()); 
	sceneGroup->getOrCreateStateSet()->addUniform(exposure->getUniformMin()); 
	
	
	
	
	//finalImageLayer->setTexture( backgroundTexture.get(), 0);
	//finalImageLayer->setTexture( foregroundTexture.get(), 1);
	//finalImageLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex0", 0)); 
	//finalImageLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex1", 1)); 
    //arScene->getSceneGroup()->addChild(finalImageLayer.get());


	viewer.setSceneData(arScene.get());


	//
	ExposureKeyboardEventHandler *ekbh = new ExposureKeyboardEventHandler();
	ekbh->setExposure(exposure);

	// set keyboard call back
	//viewer.getEventHandlerList().push_front( ekbh );
	viewer.getEventHandlerList().push_back( ekbh );
	
	//
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
