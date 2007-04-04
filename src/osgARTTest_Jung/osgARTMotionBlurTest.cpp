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
// shader stuff
#include "ShaderFactory.h"
#include "ARScene.h"
#include "ARNode.h"
#include "MotionBlurredModel.h"
#include "MotionBlurredARScene.h"
#include "SimpleAnimatorCallback.h"

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

class SimpleKeyboardHandler : public osgGA::GUIEventHandler 
{

public:

	SimpleKeyboardHandler() : osgGA::GUIEventHandler() 
	{
	};

	virtual ~SimpleKeyboardHandler() {};

	void init( SimpleAnimatorCallback *_anim)
	{
		anim = _anim;
	};

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us) 
	{

		//
		int key;
	
		switch (ea.getEventType()) {

			case osgGA::GUIEventAdapter::KEYUP:
				
				key = ea.getKey();
				
				if ( key == osgGA::GUIEventAdapter::KEY_Page_Up)
				{					
					anim->setAnimationOn(true);
				}

				
				return true;

			case osgGA::GUIEventAdapter::PUSH:
				return true;

			default:
				break;
		}

		return false;

	};

private:
	SimpleAnimatorCallback* anim;
};

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
	modelRot.makeRotate(osg::DegreesToRadians(-90.),osg::Vec3f(1.,0.,0.));
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

osg::ref_ptr<osg::Texture2D> loadTexture( const std::string filename )
{
    osg::ref_ptr<osgDB::ReaderWriter::Options> options;

	osg::ref_ptr<osg::Image> image = osgDB::readImageFile( filename, options.get() );
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(image.get());
	
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);

    return texture;
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
	//osg::ref_ptr<MotionBlurredARScene> arScene = new MotionBlurredARScene;
	arScene->init(tracker);

	
	int videoBGWidth = video->getWidth();
	int videoBGHeight = video->getHeight();

	//osg::ref_ptr<osgART::VideoBackground> videoBackground =
	//	arScene->initDefaultVideoBackground( video->getId() );
	osg::ref_ptr<osg::Texture> backgroundTexture = arScene->initTextureVideoBackground( video.get(), videoBGWidth/10, videoBGHeight/10);
	osg::ref_ptr<osg::Texture> foregroundTexture = arScene->initTextureForeground(videoBGWidth/10, videoBGHeight/10);
	//arScene->initTextureForeground(videoBGWidth/10, videoBGHeight/10, false);
	//arScene->initOffsetTexture(6);	

	osg::ref_ptr<DummyImageLayer> bgLayer = arScene->getBackgroundDummyLayer();

	//// shader examples for background !!!

	//// HalfTone
	//sf.addFragmentShaderFromFile("./data/shader/HalfTone.frag", 
	//								      bgLayer.get());
	//bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 

	//// Sepia
	//sf.addFragmentShaderFromFile("./data/shader/Sepia.frag", 
	//								      bgLayer.get());
	//bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0));


	//// Tile
	//sf.addFragmentShaderFromFile("./data/shader/Tile.frag", 
	//								      bgLayer.get());
	//bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//bgLayer->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight)));
	//bgLayer->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("tileSize", osg::Vec2f( (float)videoBGWidth/10.0f, (float)videoBGWidth/10.0f)) );

	//// Radialblur1	
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/Radialblur1.vert", 
	//									  "./data/shader/Radialblur.frag", 
	//								      bgLayer.get());
	//bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//bgLayer->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight)));

	//bgLayer->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("blurDirectionAndLength", osg::Vec3f(-1.0,-1.0, 5) ));
	//
	//bgLayer->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("numToRepeat", 8) );
	//
	//// Radialblur2
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/Radialblur2.vert", 
	//									  "./data/shader/Radialblur.frag", 
	//								      bgLayer.get());
	//bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//bgLayer->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight)));

	//bgLayer->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("centerPos", osg::Vec2f(videoBGWidth/2.0, videoBGHeight/2.0)) );
	//
	//bgLayer->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("radAndLength", osg::Vec2f(0.125, 0.01)) );
	//

	//bgLayer->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("numToRepeat", 8) );


	//// Radialblur3
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/Radialblur3.vert", 
	//									  "./data/shader/Radialblur3.frag", 
	//								      bgLayer.get());
	//bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//	
	////bgLayer->getOrCreateStateSet()->addUniform(	new osg::Uniform("scaler", 0.1f ));
	//
	//bgLayer->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("numToRepeat", 8) );

	////// zooming 1
	sf.addVertexAndFragmentShaderFromFile("./data/shader/Zooming.vert", 
										  "./data/shader/Zooming.frag", 
									      bgLayer.get());
	bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 

	ZoomingValueAnimationCallback *zvacb = new ZoomingValueAnimationCallback();
	zvacb->init( 5000, false );
	zvacb->setAnimationOn( false );
	zvacb->linkTo( bgLayer.get() );

	osg::ref_ptr<SimpleKeyboardHandler> keyboardHandler = new SimpleKeyboardHandler;
	keyboardHandler->init( dynamic_cast<SimpleAnimatorCallback*>(zvacb) );
	viewer.getEventHandlerList().push_back( keyboardHandler.get() );


	osg::ref_ptr<osg::Texture2D> roundEdgeTex = loadTexture("./data/image/roundEdge.jpg");
	bgLayer->setTexture( roundEdgeTex.get(), 1 );
	bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 	


	//// zooming 2
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/Zooming2.vert", 
	//									  "./data/shader/Zooming2.frag", 
	//								      bgLayer.get());
	//bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 

	//ZoomingValueAnimationCallback *zvacb = new ZoomingValueAnimationCallback();
	//zvacb->init( 0.05, true );
	//zvacb->setAnimationOn( true );
	//zvacb->linkTo( bgLayer.get() );

	//osg::ref_ptr<SimpleKeyboardHandler> keyboardHandler = new SimpleKeyboardHandler;
	//keyboardHandler->init( dynamic_cast<SimpleAnimatorCallback*>(zvacb) );
	//viewer.getEventHandlerList().push_back( keyboardHandler.get() );




	//// Swirl
	//sf.addVertexAndFragmentShaderFromFile( "./data/shader/Swirl.vert", 
	//									   "./data/shader/Swirl.frag", 
	//							           bgLayer.get());
	//bgLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//bgLayer->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight)));

	//bgLayer->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("centerPos", osg::Vec2f(videoBGWidth/2.0, videoBGHeight/2.0)) );
	//
	//bgLayer->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("radius", 0.25f) );


	// load truck model
	osg::ref_ptr<osg::Node> truckModel = addARTModel("david.ive", 150, 0,0,120);
	//osg::ref_ptr<osg::Node> truckModel = addARTModel("cow.osg", 100, 0,0,0);
	//osg::ref_ptr<osg::Node> truckModel = addARTModel("dumptruck.osg", 100, 0,0,0);
	osg::Vec3f lightPos = osg::Vec3f(0.0,500.0,500.0);
	addLightAt( truckModel->getOrCreateStateSet(), lightPos);
	
	// put the model to marker
	osg::ref_ptr<ARNode> hiroMarkerWithTruck =		
		arScene->addNewARNodeWith( truckModel.get() );


	//// toon shading
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/CartoonShader.vert",
	//                                      "./data/shader/CartoonShader.frag", 
	//								      truckModel.get());
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/Gooch.vert",
	//										"./data/shader/Gooch.frag", 
	//										truckModel.get());	
	//truckModel->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("lightPos",lightPos) );

	//// Glass
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/GlassRefraction.vert",
	//										"./data/shader/GlassRefraction.frag", 
	//										truckModel.get());

	//truckModel->getOrCreateStateSet()->setTextureAttributeAndModes(0, backgroundTexture.get() ,osg::StateAttribute::ON);	
	//truckModel->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("lightPos",lightPos) );


	//// Brick	
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/Brick.vert",
	//                                      "./data/shader/Brick.frag", 
	//								      truckModel.get());

	//truckModel->getOrCreateStateSet()->setTextureAttributeAndModes(0, backgroundTexture.get() ,osg::StateAttribute::ON);	
	//truckModel->getOrCreateStateSet()->addUniform(
	//		new osg::Uniform("lightPos",lightPos) );

	//truckModel->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("colRowNum", osg::Vec2f(200,100)) );


	//truckModel->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("brickRatio", osg::Vec2f(0.8,0.8)) );

	viewer.setSceneData(arScene.get());
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
