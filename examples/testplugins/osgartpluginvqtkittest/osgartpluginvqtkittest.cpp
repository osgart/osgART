/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <osg/PositionAttitudeTransform>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

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

#include <iostream>
#include <sstream>

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
	osgART::PluginManager::instance()->load("osgart_video_qtkit");

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_qtkit"));

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
		//_configvideo->width=1280;
		//_configvideo->height=720;
		
		_configvideo->width=640;
		_configvideo->height=480;
	}

	//you can also configure the plugin using fields
	//before/after open/start in function of the specific field variable/function
	//video->getField < osgART::TypedField<bool>* >("flip_vertical")->set(true);	

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for connecting a tracker
	// Note: configuration should be defined before opening the video
	video->init();

	//AR SCENEGRAPH INIT
	
	//create root 
	osg::ref_ptr<osg::Group> root = new osg::Group;

	//add video update callback (update video + video stream)
	osgART::VideoUpdateCallback::addOrSet(root.get(),video.get());

	//add a video background
	osg::ref_ptr<osg::Group> videoBackground = osgART::createBasicVideoBackground(video->getStream(),false);
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

	root->addChild(videoBackground.get());

	//APPLICATION INIT

	//for this example we activate notification level to debug
	//to see log of video call
	//osg::setNotifyLevel(osg::DEBUG_INFO);

	//BOOTSTRAP INIT
	viewer.setSceneData(root.get());

	viewer.realize();

	//video start
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

