/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

// std include
#include <iostream>
#include <sstream>

// OpenThreads include

// OSG include
#include <osg/PositionAttitudeTransform>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

// osgART include
#include <osgART/Foundation>
#include <osgART/VideoLayer>
#include <osgART/PluginManager>
#include <osgART/VideoGeode>

#include <osgART/Utils>
#include <osgART/GeometryUtils>
#include <osgART/TrackerUtils>
#include <osgART/VideoUtils>

#include <osgART/TrackerCallback>
#include <osgART/TargetCallback>
#include <osgART/TransformFilterCallback>
#include <osgART/VideoCallback>
#include <osgART/VisualTracker>

// local include



int main(int argc, char* argv[])  {

	//ARGUMENTS INIT

	//VIEWER INIT

	//create a default viewer
	osgViewer::Viewer viewer;

	//setup default threading mode
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);//stats, press 's'
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);//resize, fullscreen 'f'
	viewer.addEventHandler(new osgViewer::ThreadingHandler);//threading mode, press 't'
	viewer.addEventHandler(new osgViewer::HelpHandler);//help menu, press 'h'


	//AR INIT

	//preload plugins
	//video plugin
	osgART::PluginManager::instance()->load("osgart_video_dummyvideo");
	//tracker plugin
	osgART::PluginManager::instance()->load("osgart_tracker_dummytracker");

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_dummyvideo"));

	// check if an instance of the video stream could be started
	if (!video.valid())
	{
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plug-in!" << std::endl;
	}


	// found video - configure now
	osgART::VideoConfiguration* _configvideo = video->getOrCreateConfiguration();

	// if the configuration is existing
	if (_configvideo)
	{
		// it is possible to configure the plugin before opening it
		_configvideo->config="Data/dummyvideo/dummyvideo.png";
	}

	//you can also configure the plugin using fields
	//before/after open/start in function of the specific field variable/function
	video->getField < osgART::TypedField<bool>* >("flip_vertical")->set(true);	

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for connecting a tracker
	// Note: configuration should be defined before opening the video
	video->init();

	osg::ref_ptr<osgART::VisualTracker> tracker 
		= dynamic_cast<osgART::VisualTracker*>(osgART::PluginManager::instance()->get("osgart_tracker_dummytracker"));

	if (!tracker.valid())
	{
		// Without tracker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plug-in!" << std::endl;

		return -1;

	}

	// found tracker - configure now
	osgART::TrackerConfiguration* _configtracker = tracker->getOrCreateConfiguration();

	// if the configuration is existing
	if (_configtracker)
	{
		// it is possible to configure the plugin before opening it
		_configtracker->config="mode=0;";
	}

	// get the tracker camera configuration object
	osg::ref_ptr<osgART::CameraConfiguration> cameraconfig = tracker->getOrCreateCameraConfiguration();
	cameraconfig->load("");


	// setup one target
	osg::ref_ptr<osgART::Target> target = tracker->addTarget("test.pattern;35.2;22.0;0.3");
	
	target->setActive(true);

	tracker->setImage(video->getStream());

	tracker->init();


	//AR SCENEGRAPH INIT
	
	//create root 
	osg::ref_ptr<osg::Group> root = new osg::Group;

	//add video update callback (update video + video stream)
	osgART::VideoUpdateCallback::addOrSet(root.get(),video.get());

	//add tracker update callback (update tracker from video stream)
	osgART::TrackerUpdateCallback::addOrSet(root.get(),tracker.get());

	//add a video background
	osg::ref_ptr<osg::Group> videoBackground = osgART::createBasicVideoBackground(video->getStream());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

	root->addChild(videoBackground.get());

	//add a virtual camera
	osg::ref_ptr<osg::Camera> cam = osgART::createBasicCamera(cameraconfig);
	root->addChild(cam.get());

	//add a target transform callback (update transform from target information)
	osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();
	arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	osgART::attachDefaultTargetCallbacks(arTransform.get(), target.get());
	
	//add a target debug callback (trace target pose on the console)
	osgART::addEventCallback(arTransform.get(), new osgART::TargetDebugCallback(target.get()));

	cam->addChild(arTransform.get());

	//add a cube to the transform node
	arTransform->addChild(osgART::createTopCube(8));


	//APPLICATION INIT

	//for this example we activate notification level to debug
	//to see log of video call
	osg::setNotifyLevel(osg::DEBUG_INFO);


	//BOOTSTRAP INIT
	viewer.setSceneData(root.get());

	viewer.realize();

	//video start
	video->start();

	//tracker start
	tracker->start();


	//MAIN LOOP
	while (!viewer.done()) {
		viewer.frame();
	}

	//EXIT CLEANUP

	//tracker stop
	tracker->stop();

	//video stop
	video->stop();

	//tracker open
	tracker->close();

	//video open
	video->close();

	return 0;
}
