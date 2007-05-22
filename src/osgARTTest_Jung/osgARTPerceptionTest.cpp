/*
 *
 * Simple Example to demonstrate OSGART
 *
 * Copyright (c) 2005-2006
 * Julian Looser, Philip Lamb, Rapha‘l Grasset, Hartmut Seichter.
 *
 */

//#include <Producer/RenderSurface>
//#include <osgProducer/Viewer>


#define USE_ARTOOLKIT

#define WORKING_SPACE_DIST_THRESH 1200
//#define USE_ARTOOLKITPLUS

#include <osgViewer/Viewer>
#include <osgViewer/StatsHandler>

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/AutoTransform>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/ShapeDrawable>
#include <osg/Geometry>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/StateSetManipulator>

#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>
#include <osgART/ARSceneNode>
#include <osgART/VideoPlane>

#include <osg/Matrixf>
#include <osgDB/ReadFile>
#include <osgART/PluginManager>

#include <string>
using namespace std;


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
#include "RGBMeanStdUpdater.h"
#include "ShaderEffect.h"
#include "ColorTemperatureEffect.h"
#include "DarkeningEffect.h"
#include "ShaderEffectHandler.h"

#include "HistogramUpdateCallback.h"


#include "ARNodeImportanceValHandler.h"

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
//osg::ref_ptr<DummyImageLayer> backgroundFiltered1;

class SimpleKeyboardHandler : public osgGA::GUIEventHandler 
{

public:

	SimpleKeyboardHandler() : osgGA::GUIEventHandler() 
	{
	};

	virtual ~SimpleKeyboardHandler() {};

	void init( ShaderEffect *_effect)
	{
		shaderEffect = _effect;
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
					if ( shaderEffect->turnOn( true ) )
					{
						std::cout << "On On" << std::endl;
					}
				}
				else if ( key == osgGA::GUIEventAdapter::KEY_Page_Down)
				{						
					if ( shaderEffect->turnOn( false ) )
					{
						std::cout << "Down Down" << std::endl;
					}					
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
	ShaderEffect* shaderEffect;
};

class ShaderEffectKeyboardHandler : public osgGA::GUIEventHandler 
{

public:

	ShaderEffectKeyboardHandler() : osgGA::GUIEventHandler() 
	{
	};

	virtual ~ShaderEffectKeyboardHandler() {};

	void init( ShaderEffectHandler *_effect)
	{
		shaderEffect = _effect;
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
					shaderEffect->switchToNextEffect() ;	
				}
				else
				if ( key == osgGA::GUIEventAdapter::KEY_F1 )
				{
					shaderEffect->switchToEffect(0);
				}
				else
				if ( key == osgGA::GUIEventAdapter::KEY_F2 )
				{
					shaderEffect->switchToEffect(1);
				}				
				else
				if ( key == osgGA::GUIEventAdapter::KEY_F3 )
				{
					shaderEffect->switchToEffect(2);
				}
				else
				if ( key == osgGA::GUIEventAdapter::KEY_Home )
				{
					shaderEffect->turnOnOff();
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
	ShaderEffectHandler* shaderEffect;
};

class FullScreenToggleHandler : public osgGA::GUIEventHandler 
{
public: 

    FullScreenToggleHandler() {}
        
    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        if (!viewer) return false;
    
        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYUP):
            {
                if (ea.getKey()=='f')
                {
                    osgViewer::Viewer::Windows windows;
                    viewer->getWindows(windows);
                    
                    for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
                        itr != windows.end();
                        ++itr)
                    {
                        toggleFullscreen(*itr);
                    }
                }
            }
            default: break;
        }
        
        return false;
    }
    
    /** Get the keyboard and mouse usage of this manipulator.*/
    virtual void getUsage(osg::ApplicationUsage& usage) const
    {
        usage.addKeyboardMouseBinding("f","Toggle full screen.");
    }


    void toggleFullscreen(osgViewer::GraphicsWindow* window)
    {

        osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
        if (!wsi) 
        {
            osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot toggle window fullscreen."<<std::endl;
            return;
        }
        
        unsigned int screen_width, screen_height;
        wsi->getScreenResolution(*(window->getTraits()), screen_width, screen_height);
        
        int x, y, width, height;
        window->getWindowRectangle(x, y, width, height);
        
        bool isFullScreen = x==0 && y==0 && width==screen_width && height==screen_height;
        if (isFullScreen)
        {
            window->setWindowRectangle(screen_width/4, screen_height/4, screen_width/2, screen_height/2);
            window->setWindowDecoration(true);
        }
        else
        {
            window->setWindowDecoration(false);
            window->setWindowRectangle(0, 0, screen_width, screen_height);
        }
        
        window->grabFocusIfPointerInWindow();
        
        return;
        
    }
        


    bool _done;
};

class ARNodImportanceValAniKeyboardHandler : public osgGA::GUIEventHandler 
{

public:

	ARNodImportanceValAniKeyboardHandler() : osgGA::GUIEventHandler() 
	{
	};

	virtual ~ARNodImportanceValAniKeyboardHandler() {};

	void init( ARNodeImportanceValHandler *_impHandle)
	{
		impHandle = _impHandle;
	};

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us) 
	{

		//
		int key;
		static int nn=0;

		switch (ea.getEventType()) {

			case osgGA::GUIEventAdapter::KEYUP:
				
				key = ea.getKey();

				if (key == osgGA::GUIEventAdapter::KEY_Home )
				{
					impHandle->turnSingleNodeOn(nn);

					nn++;
					if ( nn > 12 )
						nn = 0;
				}
				else
				if ( key == osgGA::GUIEventAdapter::KEY_F1 )
				{
					std::cout << "F1" << std::endl;
					impHandle->turnSingleNodeOn(0);
				}
				else
				if ( key == osgGA::GUIEventAdapter::KEY_F2 )
				{
					std::cout << "F2" << std::endl;
					impHandle->turnSingleNodeOn(1);
				}				
				else
				if ( key == osgGA::GUIEventAdapter::KEY_F3 )
				{
					std::cout << "F3" << std::endl;
					impHandle->turnSingleNodeOn(2);
				}
				else
				if ( key == osgGA::GUIEventAdapter::KEY_F4 )
				{
					std::cout << "F4" << std::endl;
					impHandle->turnSingleNodeOn(3);
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
	ARNodeImportanceValHandler* impHandle;
};


class FindNearARNodeCallback : public osg::NodeCallback
{
public:
	FindNearARNodeCallback()
	{
	};

	virtual ~FindNearARNodeCallback()
	{
	};

	void linkTo( osg::Node *n)
	{
		if ( n->getUpdateCallback() != NULL)
		{
			setWrappedCallback( n->getUpdateCallback() );
			n->setUpdateCallback( this );
		}
		else
		{
			n->setUpdateCallback( this );
		}
	};

	void setWrappedCallback( osg::NodeCallback *_callback)
	{
		wrappedCallback = _callback;
	};

	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv)
	{

		//std::cout << "!!!!!!!" << std::endl;
		ARNode *arNode =  dynamic_cast<ARNode*>( nd);
		ARScene *arScene = arNode->getParentScene().get();

		for ( int k=0; k < arScene->size(); k++)
		{
			if ( arScene->at(k) != arNode)
			{
				
				if ( arScene->at(k)->getMarkerTrans()->getMarker()->isValid()
					&&
					arNode->getMarkerTrans()->getMarker()->isValid()
					&&
					arScene->at(k)->getImportanceVal() > 0.9 )
				{
					osg::Vec3f aa =getWorldCoorinate( arScene->at(k)->getChild(0) );
					osg::Vec3f bb =getWorldCoorinate( arNode->getChild(0) );

					//std::cout << aa << std::endl;
					//std::cout << bb << std::endl;
					//std::cout << getDist(aa,bb) << std::endl;
					float dist = getDist(aa,bb) ;
					if ( dist != 0.0 && dist < WORKING_SPACE_DIST_THRESH)
					{
						arNode->setImportanceVal(1.0);
						std::cout << getDist(aa,bb) << std::endl;
					}
					else
					{
						arNode->setImportanceVal(0.0);
					}

				}
			}
		}

		if ( wrappedCallback.valid() )
		{
			wrappedCallback->operator()(nd, nv);
		}

		traverse(nd,nv);
	};

private:
	osg::Vec3f getWorldCoorinate( osg::Node* n)
	{
		osg::MatrixList matrices = n->getWorldMatrices();

		osg::Vec3 trans;

		for ( osg::MatrixList::const_iterator iter = matrices.begin();iter != matrices.end();iter++)
		{
			osg::Matrix m=(*iter);
			trans += m.getTrans();
		}
		return trans;
	};

	float getDist( osg::Vec3f a, osg::Vec3f b)
	{
		return (a-b).length();
	};

	osg::ref_ptr<osg::NodeCallback> wrappedCallback;
};

osg::ref_ptr<ARNodeBoundingAreaBillboard> createARBillboard(osg::ref_ptr<osg::Texture2D> maskTexture, osg::ref_ptr<ARNode> nd , float scale)
{
	osg::ref_ptr<ARNodeBoundingAreaBillboard> bbs = new ARNodeBoundingAreaBillboard;
	bbs->setOnOffType( STATIC_IMPORTANTVAL );
	
	bbs->init( maskTexture );
	bbs->addBillboardFor( nd, scale );

	osg::Program* shaderPrograme = new osg::Program;
	bbs->getOrCreateStateSet()->setAttributeAndModes(shaderPrograme, osg::StateAttribute::OFF);

	return bbs;
}

osg::Drawable* createTransparentDrawable()
{
	osg::ShapeDrawable *drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),30));
	drawable->setColor( osg::Vec4f(0,0,0,0) );
	
	return drawable;
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

void toggleFullscreen(osgViewer::GraphicsWindow* window)
{

    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot toggle window fullscreen."<<std::endl;
        return;
    }
    
    unsigned int screen_width, screen_height;
    wsi->getScreenResolution(*(window->getTraits()), screen_width, screen_height);
    
    int x, y, width, height;
    window->getWindowRectangle(x, y, width, height);
    
    bool isFullScreen = x==0 && y==0 && width==screen_width && height==screen_height;
    if (isFullScreen)
    {
        window->setWindowRectangle(screen_width/4, screen_height/4, screen_width/2, screen_height/2);
        window->setWindowDecoration(true);
    }
    else
    {
        window->setWindowDecoration(false);
        window->setWindowRectangle(0, 0, screen_width, screen_height);
    }
    
    window->grabFocusIfPointerInWindow();
    
    return;
    
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
	

    osg::LightSource* lightS = new osg::LightSource;	
    lightS->setLight(myLight);
    lightS->setLocalStateSetModes(osg::StateAttribute::ON); 
    lightS->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
 
	rootStateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    lightTransform->addChild(lightS);

    return lightTransform;
}



int main(int argc, char* argv[]) {

#ifdef USE_ARTOOLKIT
	// preload the tracker
	osgART::PluginManager::getInstance()->load("osgart_tracker_artoolkit");
#endif

#ifdef USE_ARTOOLKITPLUS
	osgART::PluginManager::getInstance()->load("osgart_tracker_artoolkitplus");
#endif

	// preload the video
	osgART::PluginManager::getInstance()->load("osgart_video_artoolkit");

	
	//osgProducer::Viewer viewer;
	osgViewer::Viewer viewer;
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);


	
    //viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );
    // add stats
    //viewer.addEventHandler( new osgViewer::StatsHandler() );
	viewer.addEventHandler(new FullScreenToggleHandler);

	//viewer.setUpViewer(osgProducer::Viewer::ESCAPE_SETS_DONE);
	//viewer.getCullSettings().setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	
	viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		
	//viewer.getCamera()
	//c.setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);


//#ifndef __linux
//	// somehow on Ubuntu Dapper this ends up in a segmentation fault
//	viewer.getCamera(0)->getRenderSurface()->fullScreen(false);
//#endif


	// Load a video plugin.
	osg::ref_ptr<osgART::GenericVideo> video = 
		dynamic_cast<osgART::GenericVideo*>(osgART::PluginManager::getInstance()->get("video_artoolkit"));


	// check if an instance of the video stream could be started
	if (!video.valid()) 
	{   
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
		exit(-1);
	}


#ifdef USE_ARTOOLKIT
	// Load a tracker plugin.
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		dynamic_cast<osgART::GenericTracker*>(osgART::PluginManager::getInstance()->get("tracker_artoolkit"));
#endif

#ifdef USE_ARTOOLKITPLUS
	// Load a tracker plugin.
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		dynamic_cast<osgART::GenericTracker*>(osgART::PluginManager::getInstance()->get("tracker_artoolkitplus"));

	osg::ref_ptr< osgART::TypedField<int> > _markerMode = 
		dynamic_cast< osgART::TypedField<int>* >(tracker->get("marker_mode"));

	if ( _markerMode.valid() )
	{
		//_markerMode->set(ARToolKitPlus::MARKER_ID_SIMPLE);
		_markerMode->set(0);
	}
	else
	{
		std::cout << "FFFFFF" << std::endl;
	}
	osg::ref_ptr< osgART::TypedField<bool> > _debugMode = 
		dynamic_cast< osgART::TypedField<bool>* >(tracker->get("debug"));

	_debugMode->set(true);
#endif


    // check if the tracker plugin could be loaded
	if (!tracker.valid()) 
	{
        // this example needs a tracker. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plugin!" << std::endl;
		exit(-1);
	}
		
	/* RFC: this how you would get any type in and out through the plugin system */
	osg::ref_ptr< osgART::TypedField<int> > _field = 
		dynamic_cast< osgART::TypedField<int>* >(tracker->get("threshold"));

	/* values can only be accessed through a get()/set() mechanism */
	if (_field.valid()) _field->set(100);


	/* open the video */
	video->open();

	////////////////////////////////////////////////////////////////////////////////
	osg::ref_ptr<osgART::ARSceneNode> root = new osgART::ARSceneNode;
	osg::ref_ptr<ARScene> arScene = new ARScene;
		
#ifdef USE_ARTOOLKIT
	//if (!root->connect(tracker.get(),video.get())) {
	//	osg::notify(osg::FATAL) << "Error connecting video with tracker!" << std::endl;
	//	exit(-1);
	//}

	//if (!root->connect(tracker.get(),
	//				   video.get(),
	//				   "Data/Menual_PLUSmarkers_list.dat",
	//				   "Data/camera_para.dat"
	//	)) {
	//	osg::notify(osg::FATAL) << "Error connecting video with tracker!" << std::endl;
	//	exit(-1);
	//}

	if (!root->connect(tracker.get(),
					   video.get(),
					   "Data/Auto_PLUSmarkers_list.dat",
					   "Data/camera_para.dat"
		)) {
		osg::notify(osg::FATAL) << "Error connecting video with tracker!" << std::endl;
		exit(-1);
	}
#endif

#ifdef USE_ARTOOLKITPLUS
	if (!root->connect(tracker.get(),
					   video.get(),
					   "Data/ARTPLUSmarkers_list.dat",
					   "Data/camera_para.dat"
		)) {
		osg::notify(osg::FATAL) << "Error connecting video with tracker!" << std::endl;
		exit(-1);
	}
#endif

			//const std::string& markerlist = "Data/markers_list.dat",
			//const std::string& cameraparam = "Data/camera_para.dat"
	
	root->addChild( arScene.get() );
	arScene->init(tracker);
	


	osg::ref_ptr<osg::Texture> bgTexture = arScene->initTextureVideoBackground( video.get(), false);
	int videoBGWidth = video->getWidth();
	int videoBGHeight = video->getHeight();
	
	osg::ref_ptr<osg::Texture> fgTexture = arScene->initTextureForeground(false);
	

	//osg::ref_ptr<osg::Node> truckModel = addARTModel("dumptruck.osg", 100, 0,0,0);
	//lightPos = osg::Vec3f(0.0,0.0,100);
	//osg::MatrixTransform* lightSubGraph = addLightAt( truckModel->getOrCreateStateSet(), lightPos);
	//lightSubGraph->addChild(truckModel.get());

	//// add marker no1
	//arScene->addNewARNodeWith( lightSubGraph, 1500, true);
	

	//// add marker no2 begin
	//// make a plane ( a phantom geometry )
	//float planeWidth = 220.0f;	
	//float planeHeight = 550.0f;	//300
	////float planeWidth = 100;	
	////float planeHeight = 100;	
	//osg::ref_ptr<osg::Geode> planeNode = new osg::Geode();
	//float planeHalfWidth = planeWidth / 2.0f;	
	//float planeHalfHeight = planeHeight / 2.0f;	
	//planeNode->addDrawable(			
	//		osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::Vec3(planeWidth, 0, 0), osg::Vec3(0, planeHeight, 0))
	//					  );
	//osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	//osg::Matrix mt;
	//mt.makeTranslate( osg::Vec3(-planeHalfWidth, -planeHalfHeight, 0) );
	//trans->postMult(mt);
	//trans->addChild( planeNode.get() );
	// add marker no2 end
	//arScene->addNewARNodeWith( trans.get(), 100, false);


	//

	// 1
	//osg::ref_ptr<osg::Geode> abox = new osg::Geode();
	//abox->addDrawable(new osg::ShapeDrawable( new osg::Box( osg::Vec3f(0,0,0) , 30 ) ));

	//arScene->addNewARNodeWith(abox.get() , 100);


	//osg::ref_ptr<osg::Geode> sphere = new osg::Geode();
	//sphere->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),30)));

	//arScene->addNewARNodeWith(sphere.get() , 100);

	//osg::ref_ptr<osg::Geode> sphere2 = new osg::Geode();
	//sphere2->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),30)));
	//arScene->addNewARNodeWith(sphere.get() , 50);

	//2 
	//osg::ref_ptr<osg::Geode> abox = new osg::Geode();
	//abox->addDrawable( createTransparentDrawable() );

	//arScene->addNewARNodeWith(abox.get() , 100);


	//osg::ref_ptr<osg::Geode> sphere = new osg::Geode();
	//sphere->addDrawable( createTransparentDrawable());

	//arScene->addNewARNodeWith(sphere.get() , 100);

	//osg::ref_ptr<osg::Geode> sphere2 = new osg::Geode();
	//sphere2->addDrawable( createTransparentDrawable() );
	//arScene->addNewARNodeWith(sphere.get() , 50);

	//3

	for ( int k=0; k < 12; k++)
	{
		osg::ref_ptr<osg::Geode> sphere = new osg::Geode();
		sphere->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),30)));
		arScene->addNewARNodeWith( sphere.get(), 50+k);
	}


	//osg::ref_ptr<osg::Node> emptyPlane = addARTModel("dumptruck.osg", 100, 0,0,0);
	osg::ref_ptr<osg::Node> emptyPlane = addARTModel("emptyPlane.ive", 150, 0,0,0);

	// add marker no1
	ARNode *truckNode = (arScene->addNewARNodeWith( emptyPlane.get(), 1500, true)).get();

	//
	
	//osg::ref_ptr<DummyImageLayer> dummyLayer01 = new DummyImageLayer;
	//dummyLayer01->init(videoBGWidth, videoBGHeight);
	
	//osg::ref_ptr<DummyImageLayer> dummyLayer02 = new DummyImageLayer;
	//dummyLayer02->init(videoBGWidth, videoBGHeight);



	// create mask texture
	//osg::ref_ptr<osg::Texture2D> weightTexture = loadTexture("./data/image/BlurWeight.bmp");
	// Billboard for bounding area
	
	osg::ref_ptr<osg::Texture2D> maskTexture = loadTexture("./data/image/mask.tga");
	//planeNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, maskTexture.get() ,osg::StateAttribute::ON);	

	//osg::ref_ptr<osg::Texture2D> normTexture = loadTexture("./data/image/normalMap.bmp");
	//planeNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, normTexture.get() ,osg::StateAttribute::ON);	
	
	////
	//osg::ref_ptr<ARNodeBoundingAreaBillboard> bbs = new ARNodeBoundingAreaBillboard;
	//bbs->setOnOffType( STATIC_IMPORTANTVAL );
	//
	//bbs->init( maskTexture );
	////bbs->addBillboardFor( arScene->at(0), 1.5 );
	//bbs->addBillboardFor( arScene->at(1), 5.5 );
	//bbs->addBillboardFor( arScene->at(2), 5.5 );
	//bbs->addBillboardFor( arScene->at(3), 5.5 );
	//osg::Program* shaderPrograme = new osg::Program;
	//bbs->getOrCreateStateSet()->setAttributeAndModes(shaderPrograme, osg::StateAttribute::OFF);
	////

	//osg::ref_ptr<ARNodeBoundingAreaBillboard> bbs0 = createARBillboard(maskTexture, arScene->at(0), 3);
	//osg::ref_ptr<ARNodeBoundingAreaBillboard> bbs1 = createARBillboard(maskTexture, arScene->at(1), 3);
	//osg::ref_ptr<ARNodeBoundingAreaBillboard> bbs2 = createARBillboard(maskTexture, arScene->at(2), 3);
	//osg::ref_ptr<ARNodeBoundingAreaBillboard> bbs3 = createARBillboard(maskTexture, arScene->at(3), 3);
	


	fboManager = new FBOManager();
	fboManager->init(videoBGWidth, videoBGHeight, arScene.get(), true );
	
		

	osg::Projection *projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
	//osg::Group *pGroup = new osg::Group();

	// add billboards!!
	for ( int k=0; k< arScene->size(); k++)
	{
		projectionMatrix->addChild( createARBillboard(maskTexture, arScene->at(k), 3).get() );
	}

	
	//add model mask
	osg::ref_ptr<ModelMaskRenderer> modelMask = new ModelMaskRenderer;
	modelMask->init();
	modelMask->setAlpha(0.3);
	modelMask->addChild( truckNode );
	projectionMatrix->addChild(modelMask.get());
	//pGroup->addChild(modelMask.get());

	//// add hand mask!
	DummyImageLayer *handMaskLayer = new DummyImageLayer();
	handMaskLayer->init( video->getWidth(), video->getHeight() );
	handMaskLayer->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	sf.addFragmentShaderFromFile("./data/shader/SkinColorMask.frag", handMaskLayer);
	handMaskLayer->setTexture( bgTexture.get() );
	//handMaskLayer->getOrCreateStateSet()->setAttributeAndModes
	//	(new osg::BlendFunc, osg::StateAttribute::ON );
	
	//projectionMatrix->addChild(handMaskLayer);
	//pGroup->addChild(handMaskLayer);

	fboManager->attachTarget( projectionMatrix, 1000, osg::Vec4(0.0f,0.0f,0.0f,0.0f));
	//fboManager->attachTarget( pGroup, 1000, osg::Vec4(0.0f,0.0f,0.0f,0.0f));
	
	osg::ref_ptr<osg::Texture> maskTextureForScreen = fboManager->getTexture(0);



	////////// masking example ///////////////////////////////////////////

	osg::ref_ptr<osg::Texture2D> paperTexture = loadTexture("./data/image/paper texture.jpg");
	osg::ref_ptr<osg::Texture2D> screenBasedWeight = loadTexture("./data/image/BlurWeight.bmp");


	//osg::ref_ptr<DummyImageLayer> dummyLayer01 = arScene->getBackgroundDummyLayer();
	osg::ref_ptr<DummyImageLayer> dummyLayer01 = new DummyImageLayer;;
	dummyLayer01->init( video->getWidth(), video->getHeight() );
	
	arScene->addToBackgroundGroup(dummyLayer01.get());

	dummyLayer01->setTexture( bgTexture.get() );
	dummyLayer01->setTexture( maskTextureForScreen, 1 );
	//dummyLayer01->setTexture( screenBasedWeight.get(), 1 );
	dummyLayer01->setTexture( paperTexture.get(), 2 );
	dummyLayer01->setTexture( fgTexture, 3);
	
	//// Darkening mask
	sf.addFragmentShaderFromFile("./data/shader/Darkening.frag", dummyLayer01.get());
	
	dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("weightTex", 1)); 
	dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("paperTex", 2)); 
	dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("foregroundTex", 3));

	dummyLayer01->getOrCreateStateSet()->addUniform(
		new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight)));
	
	
	//// Skin color test
	//sf.addFragmentShaderFromFile("./data/shader/SkinColorMask.frag", dummyLayer01.get());
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 

	//// Histogram test
	//sf.addFragmentShaderFromFile("./data/shader/Histogram.frag", dummyLayer01.get());
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//
	//HistogramUpdateCallback *histogramUpdateCallback = new HistogramUpdateCallback();
	//histogramUpdateCallback->init( video.get() );
	//dummyLayer01->setUpdateCallback( histogramUpdateCallback );

	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("histogram", 1)); 
	//dummyLayer01->setTexture( histogramUpdateCallback->getHistogram(), 1 );

	//// load cover texture of magic book
	//osg::ref_ptr<osgDB::ReaderWriter::Options> options;
	//osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "./Data/image/cover page guide.jpg", options.get() );

	//HistogramMaker hm;
	//hm.init( image.get() );
	//hm.setInverse(true);
	//hm.update();
	//
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("histogramFromBookPage", 2)); 
	//dummyLayer01->setTexture( hm.getHistogram(), 2 );

	//// RGB color transfer
	//// load cover texture of magic book
	//osg::ref_ptr<osgDB::ReaderWriter::Options> options;
	////osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "./Data/image/cover page guide.jpg", options.get() );
	////osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "./Data/image/MagicLand_lowres.jpg", options.get() );
	////osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "./Data/image/GBishop_the_house_that_jack_built_p7_marker low res only.jpg", options.get() );
	//osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "./Data/image/water with lines.jpg", options.get() );
	//	
	//MeanStdCalculator msc;
	//msc.init( image.get() );
	//msc.update();

	//osg::Uniform *sMean = new osg::Uniform("sourceMean",  msc.getMean() );
	//osg::Uniform *tMean = new osg::Uniform("targetMean", msc.getMean() );
	//osg::Uniform *stdScaler = new osg::Uniform("stdScaler", osg::Vec3f(1,1,1) );

	//sf.addFragmentShaderFromFile("./data/shader/ColorTransfer.frag", dummyLayer01.get());
	//
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//

	//dummyLayer01->getOrCreateStateSet()->addUniform(sMean ); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(tMean); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(stdScaler); 

	//MeanStdUpdateCallback *meanStdUpdateCallback = new MeanStdUpdateCallback();
	//meanStdUpdateCallback->init( video.get(), sMean, tMean, stdScaler );
	//meanStdUpdateCallback->setMeanStd( msc.getMean() , msc.getStd() );

	//dummyLayer01->setUpdateCallback( meanStdUpdateCallback );

	//// Color Transfer 2
	//osg::ref_ptr<osg::Texture2D> bookPageTexture = loadTexture("./Data/image/cover page guide.jpg");
	//dummyLayer01->setTexture( bookPageTexture.get(), 1 );

	//sf.addFragmentShaderFromFile("./data/shader/ColorTransferLocal.frag", dummyLayer01.get());
	//
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("sourceImage", 1)); 
	//

	// HSI color shift
	//sf.addFragmentShaderFromFile("./data/shader/ColorShiftingInHSI.frag", dummyLayer01.get());
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	
	//// white balance1
	//sf.addFragmentShaderFromFile("./data/shader/WhiteBalancingWithMask.frag", dummyLayer01.get());
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0));
	//dummyLayer01->setTexture( screenBasedWeight.get(), 1 );
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("mask", 1));

	///// white balance2
	//ColorTemperatureEffect *colorTempEffect = new ColorTemperatureEffect();
	//colorTempEffect->init(10000);
	//colorTempEffect->setStEdTemp( 6500, 12000);
	//colorTempEffect->linkTo( dummyLayer01.get() );
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0));


	//osg::ref_ptr<SimpleKeyboardHandler> keyboardHandler = new SimpleKeyboardHandler;
	//keyboardHandler->init( dynamic_cast<ShaderEffect*>(colorTempEffect) );
	//viewer.getEventHandlerList().push_back( keyboardHandler.get() );

	///// Darkening mask 2
	//DarkeningEffect	*darkenEffect = new DarkeningEffect();	
	//darkenEffect->init(10000);
	//darkenEffect->linkTo( dummyLayer01.get() ); 
	//
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("weightTex", 1)); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("paperTex", 2)); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("foregroundTex", 3));
	//dummyLayer01->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight)));

	//osg::ref_ptr<SimpleKeyboardHandler> keyboardHandler = new SimpleKeyboardHandler;
	//keyboardHandler->init( dynamic_cast<ShaderEffect*>(darkenEffect) );
	//viewer.addEventHandler( keyboardHandler.get() );

	//////// changing effects
	////// color temp 1
	//
	//osg::ref_ptr<ShaderEffect> ct = new ColorTemperatureEffect;
	//ColorTemperatureEffect *colorTempEffect = dynamic_cast<ColorTemperatureEffect*>( ct.get());
	//colorTempEffect->init(1500);
	//colorTempEffect->setStEdTemp( 6500, 12000);
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0));

	///// Darkening mask 2
	//osg::ref_ptr<ShaderEffect> df = new DarkeningEffect;
	//DarkeningEffect	*darkenEffect = dynamic_cast<DarkeningEffect*>(df.get());
	//darkenEffect->init(1500);
	//	
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("weightTex", 1)); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("paperTex", 2)); 
	//dummyLayer01->getOrCreateStateSet()->addUniform(new osg::Uniform("foregroundTex", 3));
	//dummyLayer01->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("imageDimension", osg::Vec2f( videoBGWidth, videoBGHeight)));

	////// color temp 2
	//osg::ref_ptr<ShaderEffect> ct2 = new ColorTemperatureEffect;
	//ColorTemperatureEffect *colorTempEffect2 = dynamic_cast<ColorTemperatureEffect*>( ct2.get());
	//colorTempEffect2->init(1500);
	//colorTempEffect2->setStEdTemp( 6500, 4100);
	//dummyLayer02->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0));


	//ShaderEffectHandler *sef = new ShaderEffectHandler();
	//sef->init( dummyLayer01.get() );

	//sef->addEffect( ct );
	//sef->addEffect( df );
	//sef->addEffect( ct2 );

	//sef->linkTo(0);

	//osg::ref_ptr<ShaderEffectKeyboardHandler> keyboardHandler = new ShaderEffectKeyboardHandler;
	//keyboardHandler->init( sef );
	////viewer.getEventHandlerList().push_back( keyboardHandler.get() );
	//viewer.addEventHandler( keyboardHandler.get() );

	////osg::ref_ptr<SimpleKeyboardHandler> keyboardHandler = new SimpleKeyboardHandler;
	////keyboardHandler->init( ct.get() );
	////viewer.getEventHandlerList().push_back( keyboardHandler.get() );




	

	/////////////////////////
	int arSceneSize = arScene->size();
	for ( int k=0; k < arSceneSize ; k++)
	{
		arScene->at(k)->setImportanceVal( 0.0 );
	}
	
	ARNodeImportanceValHandler *valHandle = new ARNodeImportanceValHandler();
	//valHandle->init( arScene, 0 , arScene->size() - 2);
	valHandle->init( arScene);
	arScene->setUpdateCallback( valHandle ); 

	osg::ref_ptr<ARNodImportanceValAniKeyboardHandler> importValkeyboardHandler = new ARNodImportanceValAniKeyboardHandler;
	importValkeyboardHandler->init( valHandle );

	viewer.addEventHandler(importValkeyboardHandler.get());


	//linkTo
	osg::ref_ptr<FindNearARNodeCallback> findCallback = new FindNearARNodeCallback;
	findCallback->linkTo( truckNode );
	
	///////////////////////////////////////////////////////////
	viewer.setSceneData(root.get());
	

	
	viewer.realize();
	
	osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
                    
	for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
		itr != windows.end();
		++itr)
	{
		toggleFullscreen(*itr);
	}
	//
	video->start();
		
	
    while (!viewer.done()) {
		
		//viewer.sync();	
		
		//video->update();
		//tracker->setImage(video.get());
		//tracker->update();
		
		//sef->update();

        //viewer.update();
        viewer.frame();
	
    }
    
	//viewer.sync();
    //viewer.cleanup_frame();
    //viewer.sync();

	video->stop();
	video->close();
	
	//delete sef;
}
