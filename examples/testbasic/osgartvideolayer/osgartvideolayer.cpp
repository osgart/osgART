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

#include <osg/GraphicsContext>

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

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_dummyvideo"));

	// check if an instance of the video stream could be started
	if (!video.valid())
	{
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plug-in!" << std::endl;
	}


	// found video - configure now
	osgART::VideoConfiguration* _configvideo = video->getConfiguration();

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
	video->init();


	//AR SCENEGRAPH INIT
	
	//create root 
	osg::ref_ptr<osg::Group> root = new osg::Group;

	//add video update callback (update video + video stream)
	osgART::VideoUpdateCallback::addOrSet(root.get(),video.get());

	
	//setup a non full screen window (necessary for the videoFlBackground to start!)
	viewer.setUpViewInWindow( 100, 100, 800, 600 );

	//add a video background: full screen and resize with change of window size
	osg::ref_ptr<osg::Group> videoBackground = osgART::createBasicVideoBackground(video->getStream());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");//place object in layer 0

	root->addChild(videoBackground.get());

	//add a fixed video background: absolute size and not affected by resize
	osg::ref_ptr<osg::Group> videoFBackground = osgART::createBasicFixedVideoBackground(video->getStream(),osg::Vec2i(350,200),osg::Vec2i(200,200));
	videoFBackground->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");
	
	root->addChild(videoFBackground.get());

	//add a test cube to show the effect of layers
	osg::ref_ptr<osg::Geode> cubeTest=osgART::createTopCube(1.5,osg::Vec4f(1.,0.,0.,1.));
	cubeTest->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

	root->addChild(osgART::translateModel(cubeTest.get(),osg::Vec3f(0.,1.,-30.)));

	//add a floating video background: relative size  and resize with change of window size
	osg::ref_ptr<osg::Group> videoFlBackground = osgART::createBasicFloatingVideoBackground(video->getStream(),osg::Vec2f(0.5,0.),osg::Vec2f(0.5,0.5),viewer.getCamera());
	videoFlBackground->getOrCreateStateSet()->setRenderBinDetails(3, "RenderBin");

	root->addChild(videoFlBackground.get());

	//add a floating video background: relative size  and resize with change of window size
	osg::ref_ptr<osg::Group> videoFlForeground = osgART::createBasicFloatingVideoForeground(video->getStream(),osg::Vec2f(0.3,0.5),osg::Vec2f(0.6,0.05),viewer.getCamera());
	videoFlForeground->getOrCreateStateSet()->setRenderBinDetails(3, "RenderBin");

	root->addChild(videoFlForeground.get());

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

