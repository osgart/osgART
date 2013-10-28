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

// A simple example to demonstrate the SSTT plugin with osgART

#include <osgART/Foundation>
#include <osgART/VideoLayer>
#include <osgART/PluginManager>
#include <osgART/VideoGeode>
#include <osgART/Utils>
#include <osgART/GeometryUtils>

#include <osgART/TargetCallback>
#include <osgART/TransformFilterCallback>
#include <osgART/VideoCallback>

#include <osgART/VideoUtils>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <iostream>
#include <sstream>

int main(int argc, char* argv[])  {

	//VIEWER INIT
	//create a default viewer
	//
	//osgART::SimpleViewer viewer;
	//viewer.setOptions(osgART::Viewer::WS_HANDLER|osgART::Viewer::STATS_HANDLER);
	//viewer.setThreading(osgViewer::Viewer::SingleThreaded);

	osgViewer::Viewer viewer;

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::ThreadingHandler);
	viewer.addEventHandler(new osgViewer::HelpHandler);

	//AR INIT

	// preload the video plugin
	osgART::PluginManager::instance()->load("osgart_video_dummyvideo");

	// Load the video plugin
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
		_configvideo->config="data/dummyvideo/dummyvideo.png";
	}

	//you can also configure the plugin using fields
	//before/after open/start in function of the specific field variable/function
	
	video->getField < osgART::TypedField<bool>* >("flip_vertical")->set(true);
	 
	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for connecting a tracker
	// Note: configuration should be defined before opening the video
	if (!video->init()) {
		// If the video doesn't start,  video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not start the video !" << std::endl;
	}


	// AR SCENE GRAPH INIT
	osg::ref_ptr<osg::Group> root = new osg::Group;
	
	//add video update callback (update video + video stream)
	osgART::VideoUpdateCallback::addOrSet(root.get(),video.get());

	//APPLICATION INIT

	//for the demo we activate notification level to debug
	//to see log of video call
	osg::setNotifyLevel(osg::DEBUG_INFO);

	//BOOTSTRAP INIT

	viewer.setSceneData(root.get());

	viewer.realize();

	//start the video capture.
	video->start();


	//MAIN LOOP
	while (!viewer.done()) {
		viewer.frame();
	}


	//EXIT CLEANUP

	//video stop
	video->stop();

	//video open
	video->close();

	return 0;

}
