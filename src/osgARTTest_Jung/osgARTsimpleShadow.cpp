/*
 *
 * Simple example demonstrating the display of a 3D model on a Marker
 * with lighting (directional light) and shadow (real plane).
 * we are using the built-in depth shadow rendering (based on shader) for
 * computing virtual lighting of the virtual scene with simulating
 * a virtual shadow on the floor. For that we are using the ARPlaneShadowRenderer
 * class.
 *
 * usage: osgARTsimpleShadow.exe model_name size_object shift_x shift_y shift_z
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

#include <osg/BoundingSphere>

#include <osg/BlendFunc>

#include <osgDB/ReadFile>

#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>
#include <osgART/VideoBackground>
//#include <osgART/PlaneARShadowRenderer>
//#include "ARShadowRendererWithPhantomObj.h"
#include "BaseShadowRenderer.h"

#include "DummyImageLayer.h"

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



osg::AnimationPath* createOrbitalPath(const osg::Vec3& center,float radius, float height, double looptime) {

    osg::AnimationPath* animationPath = new osg::AnimationPath;
    int numSamples = 100;
    float yaw = 0.0f;
	float yaw_delta = 2.0f * osg::PI / ((float)numSamples - 1.0f);
    double time = 0.0f;
    double time_delta = looptime / (double)numSamples;
    
	for(int i = 0; i < numSamples; i++) {
        osg::Vec3 position(center + osg::Vec3(sinf(yaw) * radius, cosf(yaw) * radius, height));
		osg::Matrix mat = osg::Matrix::lookAt(center, position, osg::Vec3(0, 0, 1));
        osg::Quat rotation;
		mat.get(rotation);
        animationPath->insert(time, osg::AnimationPath::ControlPoint(position, rotation));
        yaw += yaw_delta;
        time += time_delta;
    }

    return animationPath; 
}

osg::MatrixTransform* addLight(osg::StateSet* rootStateSet)
{
	osg::MatrixTransform* lightTransform = new osg::MatrixTransform;

    // create a spot light.
    osg::Light* myLight = new osg::Light;
	myLight->setLightNum(0);
	myLight->setPosition(osg::Vec4(0, 0, 0,1.0f));
	//1.0,-1.0,-1.0,0.0f
	myLight->setAmbient(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	myLight->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	osg::LightSource* lightS = new osg::LightSource;    
	lightS->setLight(myLight);
	lightS->setStateSetModes(*rootStateSet,osg::StateAttribute::ON); 
  
	lightTransform->setUpdateCallback(new osg::AnimationPathCallback(createOrbitalPath(osg::Vec3(0, 0, 0), 500, 400, 100)));
	lightTransform->addChild(lightS);

	return lightTransform;
}

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
	modelTranslate.makeTranslate(-bound.center() + osg::Vec3(0,0, bound.radius() / 2.0) );
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


int main(int argc, char* argv[]) {


	if (argc<6)
	{
		std::cout<<"osgartsimple model_name size_model shift_x shift_y shift_z"<<std::endl;
		exit(-1);
	}
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

	/* load a video plugin */
	osg::ref_ptr<osgART::GenericVideo> video = 
	osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit", cfg);

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

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();

	int videoWidth = video->getWidth();
	int videoHeight = video->getHeight();
	tracker->init(videoWidth, videoHeight);

	
	//Adding video background
	osg::Group* foregroundGroup	= new osg::Group();

	osgART::VideoBackground* videoBackground=new osgART::VideoBackground(video->getId());

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
		new osgART::ARTTransform();
		//new osgART::ARTTransform(marker.get());
	

	osg::Group* sceneGroup = new osg::Group();
	sceneGroup->getOrCreateStateSet()->setRenderBinDetails(5, "RenderBin");
	sceneGroup->addChild(markerTrans.get());
	foregroundGroup->addChild(sceneGroup);

	//creating the model
	osg::ref_ptr<osg::Group> shadowedSubGraph = new osg::Group;
	osg::Node* model=addARTModel(argv[1],atof(argv[2]),atof(argv[3]),atof(argv[4]),atof(argv[5]));
	shadowedSubGraph->addChild(model);
	

	//building the light
	osg::MatrixTransform* lightSubGraph = addLight(sceneGroup->getOrCreateStateSet());

	//computing the size of the scene for the size of the real plane
	osg::BoundingSphere bsphere=shadowedSubGraph->getBound();

	
	// Shadow Renderer !!!
	osg::ref_ptr<osgART::BaseShadowRenderer> renderer = 
		new osgART::BaseShadowRenderer(shadowedSubGraph.get(),lightSubGraph, tracker, 0, videoWidth, videoHeight);
		//new osgART::BaseShadowRenderer(shadowedSubGraph.get(),lightSubGraph, tracker, 0, videoWidth, videoHeight);
	renderer->init();
	

	// make a plane ( a phantom geometry )
	float planeLength = 700.0f;	
	osg::ref_ptr<osg::Geode> planeNode = new osg::Geode();
	float planeHalfLength = planeLength / 2.0f;	
	planeNode->addDrawable(			
			osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::Vec3(planeLength, 0, 0), osg::Vec3(0, planeLength, 0))
						  );
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	osg::Matrix mt;
	mt.makeTranslate( osg::Vec3(-planeHalfLength, -planeHalfLength, 0) );
	trans->postMult(mt);
	trans->addChild( planeNode.get() );

	// make a cylinder geometry ( a phantom geometry )
	float cylinderHeight = 140;
	float cylinderRadius = 40;
	float cylinderX = 0;
	float cylinderY = - 130;

	osg::ref_ptr<osg::Cylinder> cylindershape = new osg::Cylinder;
	cylindershape->set( osg::Vec3f( cylinderX, cylinderY, cylinderHeight/ 2.0), cylinderRadius, cylinderHeight );

	osg::ref_ptr<osg::ShapeDrawable> cylinder = new osg::ShapeDrawable;
	cylinder->setShape(cylindershape.get());
	osg::ref_ptr<osg::Geode> cylinderNode = new osg::Geode();
	cylinderNode->addDrawable(cylinder.get());

	// Add phatoms geometries to the shadow renderer
	renderer->addPhantomNode( trans.get() );
	//renderer->addPhantomNode( cylinderNode.get() );
	markerTrans->addChild(renderer.get());

	osg::MatrixTransform* modelViewMatrix = new osg::MatrixTransform();
	modelViewMatrix->addChild(foregroundGroup);
	projectionMatrix->addChild(modelViewMatrix);
	
	osg::Group* root = new osg::Group();
	root->addChild(projectionMatrix);

	viewer.setSceneData(root);

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
