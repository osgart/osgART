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
#include "NodeBoundingAreaBillboard.h"
#include "ARNodeBoundingAreaBillboard.h"
#include "ModelMaskRenderer.h"

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

osg::MatrixTransform* addLightAt(osg::StateSet* rootStateSet, osg::Vec3 pos)
{
	rootStateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
    //osg::Group* lightGroup = new osg::Group;
    osg::MatrixTransform* lightTransform = new osg::MatrixTransform;

    // create a directional light (infinite distance place at 45 degrees)
    osg::Light* myLight = new osg::Light;
    myLight->setLightNum(1);
	myLight->setPosition(osg::Vec4(pos,1));
	myLight->setAmbient(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	myLight->setDiffuse(osg::Vec4(0.8f, 0.7f, 0.7f, 1.0f));
	myLight->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    myLight->setConstantAttenuation(1.0f);


	osg::Vec3f lightDir = myLight->getDirection();
	//myLight->setDirection( osg::Vec3f(0,0,-1) );
	std::cout << lightDir[0] << " " << lightDir[1] << " " << lightDir[2] << std::endl;

    osg::LightSource* lightS = new osg::LightSource;	
    lightS->setLight(myLight);
    lightS->setLocalStateSetModes(osg::StateAttribute::ON); 
    lightS->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
 
	rootStateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    lightTransform->addChild(lightS);

    return lightTransform;
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
	if (_field.valid()) _field->set(100);


	/* open the video */
	video->open();

	tracker->init(video->getWidth(), video->getHeight());

	////////////////////////////////////////////////////////////////////////////////
	osg::ref_ptr<ARScene> arScene = new ARScene;
	
	arScene->init(tracker);

	osg::ref_ptr<osg::Texture> bgTexture = arScene->initTextureVideoBackground( video->getId(), true);
	int videoBGWidth = video->getWidth();
	int videoBGHeight = video->getHeight();
	
	arScene->initTextureForeground();
	
	//osg::ref_ptr<osg::Node> truckModel = addARTModel("dumptruck.osg", 100, 0,0,0);
	//osg::ref_ptr<osg::Node> truckModel = addARTModel("cow.osg", 100, 0,0,0);

    const float radius = 50.0f;
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,50.0f),radius)));


	lightPos = osg::Vec3f(0.0,0.0,100);
	osg::MatrixTransform* lightSubGraph = addLightAt( geode->getOrCreateStateSet(), lightPos);
	lightSubGraph->addChild(geode);

	// add marker no1
	arScene->addNewARNodeWith( lightSubGraph, 100);

	// setup hatching !!

	// load textures
	osg::ref_ptr<osg::Texture2D> hatchTex0 = loadTexture("./data/image/Hatch0.bmp");
	osg::ref_ptr<osg::Texture2D> hatchTex1 = loadTexture("./data/image/Hatch1.bmp");
	osg::ref_ptr<osg::Texture2D> hatchTex2 = loadTexture("./data/image/Hatch2.bmp");
	osg::ref_ptr<osg::Texture2D> hatchTex3 = loadTexture("./data/image/Hatch3.bmp");
	osg::ref_ptr<osg::Texture2D> hatchTex4 = loadTexture("./data/image/Hatch4.bmp");
	osg::ref_ptr<osg::Texture2D> hatchTex5 = loadTexture("./data/image/Hatch5.bmp");

	//osg::ref_ptr<osg::Texture2D> hatchTex0 = loadTexture("./data/image/hatching00.bmp");
	//osg::ref_ptr<osg::Texture2D> hatchTex1 = loadTexture("./data/image/hatching01.bmp");
	//osg::ref_ptr<osg::Texture2D> hatchTex2 = loadTexture("./data/image/hatching02.bmp");
	//osg::ref_ptr<osg::Texture2D> hatchTex3 = loadTexture("./data/image/hatching03.bmp");
	//osg::ref_ptr<osg::Texture2D> hatchTex4 = loadTexture("./data/image/hatching04.bmp");
	//osg::ref_ptr<osg::Texture2D> hatchTex5 = loadTexture("./data/image/hatching05.bmp");


	ARNode *arNode = arScene->at(0).get();
	osg::StateSet *arNodeStateSet = arNode->getOrCreateStateSet();
	sf.addVertexAndFragmentShaderFromFile(  "./data/shader/Hatching.vert",
											"./data/shader/Hatching.frag", arNode );

	arNodeStateSet->setTextureAttributeAndModes(0, hatchTex0.get() ,osg::StateAttribute::ON);	
	arNodeStateSet->setTextureAttributeAndModes(1, hatchTex1.get() ,osg::StateAttribute::ON);	
	arNodeStateSet->setTextureAttributeAndModes(2, hatchTex2.get() ,osg::StateAttribute::ON);	
	arNodeStateSet->setTextureAttributeAndModes(3, hatchTex3.get() ,osg::StateAttribute::ON);	
	arNodeStateSet->setTextureAttributeAndModes(4, hatchTex4.get() ,osg::StateAttribute::ON);	
	arNodeStateSet->setTextureAttributeAndModes(5, hatchTex5.get() ,osg::StateAttribute::ON);	

	arNode->getOrCreateStateSet()->addUniform(new osg::Uniform("Hatch0", 0)); 
	arNode->getOrCreateStateSet()->addUniform(new osg::Uniform("Hatch1", 1)); 
	arNode->getOrCreateStateSet()->addUniform(new osg::Uniform("Hatch2", 2)); 
	arNode->getOrCreateStateSet()->addUniform(new osg::Uniform("Hatch3", 3)); 
	arNode->getOrCreateStateSet()->addUniform(new osg::Uniform("Hatch4", 4)); 
	arNode->getOrCreateStateSet()->addUniform(new osg::Uniform("Hatch5", 5)); 

	//osg::ref_ptr<DummyImageLayer> sss = new DummyImageLayer;
	//sss->init(video->getWidth(), video->getHeight());
	//sss->setTexture( hatchTex0.get() );	
	//viewer.setSceneData(sss.get());
	
	
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
