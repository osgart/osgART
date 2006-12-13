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
#include "FBOManager.h"
#include "DummyImageLayer.h"
#include "ARScene.h"
#include "ARNode.h"
#include "DummyBackgroundPlane.h"

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

// variables
osg::Vec3 lightPos;
ShaderFactory sf;
osg::ref_ptr<FBOManager> fboManager;
osg::ref_ptr<DummyImageLayer> backgroundFiltered1;


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
    modelRot.makeRotate(0.,osg::Vec3f(1.,0.,0.));
	modelTranslate.makeTranslate(-bound.center());

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

	return myObject;
}

osg::Node* build_quad(osg::Texture *tex)
{
	float quadSize = 50;
    osg::Geometry *geo = new osg::Geometry;
    osg::Vec3Array *vx = new osg::Vec3Array;
    vx->push_back(osg::Vec3(-quadSize, 0, -quadSize));
    vx->push_back(osg::Vec3(quadSize, 0, -quadSize));
    vx->push_back(osg::Vec3(quadSize, 0, quadSize));
    vx->push_back(osg::Vec3(-quadSize, 0, quadSize));
    geo->setVertexArray(vx);
    osg::Vec3Array *nx = new osg::Vec3Array;
    nx->push_back(osg::Vec3(0, -1, 0));
    geo->setNormalArray(nx);
    geo->setNormalBinding(osg::Geometry::BIND_OVERALL);
    osg::Vec2Array *tx = new osg::Vec2Array;
    tx->push_back(osg::Vec2(0, 0));
    tx->push_back(osg::Vec2(1, 0));
    tx->push_back(osg::Vec2(1, 1));
    tx->push_back(osg::Vec2(0, 1));
    geo->setTexCoordArray(0, tx);
    geo->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
    geo->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);

    osg::Geode *geode = new osg::Geode;
    geode->addDrawable(geo);
    return geode;
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

    return lightGroup;
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
		osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit", cfg);
	
	/* load a tracker plugin */
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		osgART::TrackerManager::createTrackerFromPlugin("osgart_artoolkit_tracker");

	/* RFC: this how you would get any type in and out through the plugin system */
	osg::ref_ptr< osgART::TypedField<int> > _field = 
		dynamic_cast< osgART::TypedField<int>* >(tracker->get("threshold"));

	/* values can only be accessed through a get()/set() mechanism */
	if (_field.valid()) _field->set(50);


	/* open the video */
	video->open();

	//creating an instance of a marker-based tracking
	// osg::ref_ptr<osgART::GenericTracker> tracker = new osgART::ARToolKitTracker;
	
	// add the tracker to the tracker manager
	// osgART::TrackerManager::getInstance()->addTracker(tracker.get());

	tracker->init(video->getWidth(), video->getHeight());


	////////////////////////////////////////////////////////////////////////////////
	osg::ref_ptr<ARScene> arScene = new ARScene;
	
	arScene->init(tracker);
	
	//// example 01: Simple shader
	//// 1. Add sobel filter to the video background
	//// 2. Add toon shader to the truck

	////load default video <osgART::VideoBackground> 
	//osg::ref_ptr<osgART::VideoBackground> videoBackground =
	//	arScene->initDefaultVideoBackground( video->getId() );
	//arScene->initDefaultForeground();

	//int videoBGWidth = video->getWidth();
	//int videoBGHeight = video->getHeight();

	//// notice <osgART::VideoBackground> use textureRectangle so use sampler2DRect in the shader !!!
	//sf.addFragmentShaderFromFile("./data/shader/SimpleSobelFilterRect.frag", videoBackground.get());
	//videoBackground->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0));
	//videoBackground->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight))); 
	//videoBackground->getOrCreateStateSet()->addUniform(new osg::Uniform("threshold", 0.0001f)); 

	//// load truck model
	//osg::ref_ptr<osg::Node> truckModel = addARTModel("dumptruck.osg", 100, 0,0,0);

	//// and add light to it
	//lightPos = osg::Vec3f(1.0,-1.0,-1.0);
	//addLightAt( truckModel->getOrCreateStateSet(), lightPos);

	//// and add cartoon shader
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/CartoonShader.vert",
	//                                      "./data/shader/CartoonShader.frag", 
	//								      truckModel.get());
	//lightPos.normalize();
	//truckModel.get()->getOrCreateStateSet()->addUniform(new osg::Uniform("lightDir", lightPos)); 
	//

	//// put the model to marker
	//osg::ref_ptr<ARNode> hiroMarkerWithTruck =		
	//	arScene->addNewARNodeWith( truckModel );
	////////////////////////////////////////////////////////////////// example 01 end here 
	

		
	//// example 02: Simple shader with texture background version
	//// 1. Add sobel filter to the video background
	//// 2. Add toon shader to the truck

	//// init texture video. The background is rendered to a texture
	//// if you set addDummy as false, it does not add dummyLayer.
	//// By default it is true.

	//arScene->initTextureVideoBackground( video->getId());
	//arScene->initDefaultForeground();
	//int videoBGWidth = video->getWidth();
	//int videoBGHeight = video->getHeight();

	//// we inited with dummyLayer so the 1st child is the dummyLayer
	//osg::ref_ptr<osg::Node> dummyBackground = arScene->getSceneGroup()->getChild(0);

	//sf.addFragmentShaderFromFile("./data/shader/SimpleSobelFilter.frag", dummyBackground.get());
	//dummyBackground->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0));
	//dummyBackground->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight))); 
	//dummyBackground->getOrCreateStateSet()->addUniform(new osg::Uniform("threshold", 0.25f)); 

	//// load truck model
	//osg::ref_ptr<osg::Node> truckModel = addARTModel("dumptruck.osg", 100, 0,0,0);

	//// and add light to it
	//lightPos = osg::Vec3f(1.0,-1.0,-1.0);
	//addLightAt( truckModel->getOrCreateStateSet(), lightPos);

	//// and add cartoon shader
	//sf.addVertexAndFragmentShaderFromFile("./data/shader/CartoonShader.vert",
	//                                      "./data/shader/CartoonShader.frag", 
	//								      truckModel.get());
	//lightPos.normalize();
	//truckModel.get()->getOrCreateStateSet()->addUniform(new osg::Uniform("lightDir", lightPos)); 
	//

	//// put the model to marker
	//osg::ref_ptr<ARNode> hiroMarkerWithTruck =		
	//	arScene->addNewARNodeWith( truckModel );
	//////////////////////////////////////////////////////////////// example 02 end here 

	// example 03: Advanced shader  
	// 1. Initialise background as a texture. The background is rendered to texture
	//    by default.
	// 2. Add toon shader to the truck
	// 3. Add truck to background texture group ( not the video background )
	// 4. Blur the background texture group ( 1st pass )
	// 5. Apply Sobel filter on the blurred image from 1st pass ( 2nd pass )
	// 6. Add the final image to the background group

	// 1. Init background as a texture and no dummy geometry this time!
	osg::ref_ptr<osg::Texture> bgTexture = arScene->initTextureVideoBackground( video->getId(), false);
	int videoBGWidth = video->getWidth();
	int videoBGHeight = video->getHeight();

	
	osg::ref_ptr<osg::Node> truckModel = addARTModel("dumptruck.osg", 100, 0,0,0);
	lightPos = osg::Vec3f(1.0,-1.0,-1.0);
	addLightAt( truckModel->getOrCreateStateSet(), lightPos);
	// 2. Add cartoon shader
/*	sf.addVertexAndFragmentShaderFromFile("./data/shader/CartoonShader.vert",
	                                      "./data/shader/CartoonShader.frag", 
									      truckModel.get());
	lightPos.normalize();
	truckModel.get()->getOrCreateStateSet()->addUniform(new osg::Uniform("lightDir", lightPos));*/ 
	
	// 3. Add truck to background texture group ( not the video background )
	// note. we have to make a ARNode because of markerTrans
	osg::ref_ptr<ARNode> arNode = new ARNode();
	arNode->init(0); // 0 marker!
	arNode->addModel(truckModel.get());

	arScene->addToBackgroundTextureGroup( arNode.get(), true );	
	osg::ref_ptr<osg::Group> backgroundTextureGroup = arScene->getBackgroundTextureGroup();

	// 4. Blur the background texture group ( 1st pass )
	// To do it, we have to render them to a texture
	osg::ref_ptr<DummyImageLayer> dummyLayer01 = new DummyImageLayer;
	dummyLayer01->init(videoBGWidth, videoBGHeight);
	dummyLayer01->setTexture( arScene->getBackgroundTexture() );

	sf.addFragmentShaderFromFile("./data/shader/GaussianBlur.frag", dummyLayer01.get());
	dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	dummyLayer01->getOrCreateStateSet()->addUniform(
		new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight))); 

	fboManager = new FBOManager();
	fboManager->init(videoBGWidth, videoBGHeight, arScene.get());
	fboManager->attachTarget( dummyLayer01.get(), 900);
	osg::ref_ptr<osg::Texture> blurredTexture = fboManager->getTexture(0);

	// 5. Apply Sobel filter on the blurred image from 1st pass ( 2nd pass )
	osg::ref_ptr<DummyImageLayer> dummyLayer02 = new DummyImageLayer;
	dummyLayer02->init(videoBGWidth, videoBGHeight);
	dummyLayer02->setTexture( blurredTexture.get() );

	sf.addFragmentShaderFromFile("./data/shader/SimpleSobelFilter.frag", dummyLayer02.get());
	dummyLayer02->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0));
	dummyLayer02->getOrCreateStateSet()->addUniform(
		new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight))); 
	dummyLayer02->getOrCreateStateSet()->addUniform(new osg::Uniform("threshold", 0.25f)); 
	dummyLayer02->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");

	// 6. Add the final image to the background group
	arScene->addToBackgroundGroup(dummyLayer02.get());

	// put the model to marker
	arScene->addARNode( arNode, 1100, false ); // just add to ARScene but not to scene graph

	viewer.setSceneData(arScene.get());
	
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
