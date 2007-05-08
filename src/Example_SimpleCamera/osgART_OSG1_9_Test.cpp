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

//#include <Producer/RenderSurface>
//#include <osgProducer/Viewer>

#include <osgViewer/Viewer>
#include <osgViewer/StatsHandler>

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
#include <osgART/ARTTransform>
#include <osgART/VideoLayer>
#include <osgART/ARSceneNode>
#include <osgART/PluginManager>

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

int main(int argc, char* argv[]) 
{

	// preload the tracker
	osgART::PluginManager::getInstance()->load("osgart_tracker_artoolkit");

	// preload the video
	osgART::PluginManager::getInstance()->load("osgart_video_artoolkit");
	
	// Set up the osg viewer.
	//osgProducer::Viewer viewer;
	//viewer.setUpViewer(osgProducer::Viewer::ESCAPE_SETS_DONE);
	//viewer.getCullSettings().setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	osgViewer::Viewer viewer;
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	//SingleThreaded ( only Main screen when DUAL MODE)
	//CullDrawThreadPerContext, ( both screen like clone screen when DUAL MODE)
	//DrawThreadPerContext (both screen like CLONE MODE when DUAL MODE) and (NOT SAFE IF YOU USE FBO AND DUAL MODE)
	//CullThreadPerCameraDrawThreadPerContext (both screen like CLONE MODE when DUAL MODE) and (NOT SAFE IF YOU USE FBO AND DUAL MODE)
	//AutomaticSelection ( only Main screen when DUAL MODE)

	viewer.addEventHandler(new FullScreenToggleHandler);

	viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	osg::ref_ptr<osgART::ARSceneNode> root = new osgART::ARSceneNode;

	viewer.setSceneData(root.get());

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
	
	// Load a tracker plugin.
	osg::ref_ptr<osgART::GenericTracker> tracker = 
		dynamic_cast<osgART::GenericTracker*>(osgART::PluginManager::getInstance()->get("tracker_artoolkit"));

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

	// Connect the video to a tracker. This will also init the tracker.
	if (!root->connect(tracker.get(),video.get())) {
		osg::notify(osg::FATAL) << "Error connecting video with tracker!" << std::endl;
		exit(-1);
	}
	// Tracker parameters are read and written via a field mechanism.
	// Init access to a field within the tracker, in this case, the binarization threshhold.
	osg::ref_ptr< osgART::TypedField<int> > _threshold = 
		reinterpret_cast< osgART::TypedField<int>* >(tracker->get("threshold"));
	
	// Values are be accessed through a get()/set() mechanism on the field pointer.
	if (_threshold.valid())  {			
		// Set the threshold, and read back.
		_threshold->set(100);
		osg::notify(osg::WARN) << "Field 'threshold' = " << _threshold->get() << std::endl;
	} else {
		osg::notify(osg::WARN) << "Field 'threshold' not supported for this tracker" << std::endl;
	}
	
	
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

	// create a matrix transform related to the marker
	osg::ref_ptr<osg::MatrixTransform> markerTrans = 
		new osgART::ARTTransform(marker.get());

	//and simply create a blue cube object
	float boxSize = 40.0f;
	osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0, 0, boxSize / 2.0f), boxSize));
	sd->setColor(osg::Vec4(0, 0, 1, 1));
	
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(sd);
	markerTrans->addChild(geode);

	//assemble all things together
	osg::Group* sceneGroup = new osg::Group();
	sceneGroup->getOrCreateStateSet()->setRenderBinDetails(5, "RenderBin");
	sceneGroup->addChild(markerTrans.get());
	foregroundGroup->addChild(sceneGroup);
	
	osg::MatrixTransform* modelViewMatrix = new osg::MatrixTransform();
	modelViewMatrix->addChild(foregroundGroup);
	projectionMatrix->addChild(modelViewMatrix);
	
	root->addChild(projectionMatrix);
	

	video->start();
	viewer.realize();

	
    while (!viewer.done()) 
	{
        viewer.frame();	
    }
    
	//stop the video
	video->stop();
	//close the video
	video->close();
	
}
