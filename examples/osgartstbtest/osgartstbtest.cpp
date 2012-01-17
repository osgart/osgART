/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2008 Human Interface Technology Laboratory New Zealand
 * 
 * This file is part of osgART 2.0
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

// A simple example to demonstrate the StbTracker plugin with osgART

#include <osgART/Foundation>
#include <osgART/VideoLayer>
#include <osgART/PluginManager>
#include <osgART/VideoGeode>
#include <osgART/Utils>
#include <osgART/GeometryUtils>
#include <osgART/MarkerCallback>
#include <osgART/TransformFilterCallback>
#include <osgART/ImageStreamCallback>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <iostream>
#include <sstream>

osg::Group* createImageBackground(osg::Image* video, bool useTextureRectangle = false) {
	osgART::VideoLayer* _layer = new osgART::VideoLayer();
	osgART::VideoGeode* _geode = new osgART::VideoGeode(video, NULL, 1, 1, 20, 20, 
		useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D);
	_layer->addChild(_geode);
	return _layer;
}

int main(int argc, char* argv[])  
{
	
	osg::ArgumentParser args(&argc,argv);

	std::string videoName = "sstt";
	std::string trackerName = "stbnx";
	
	//std::string trackerConfig = "Frame,0,80"; // can use ID or Frame
	//std::string trackerConfig = "single,soccerSet,soccer,hyper_FCBarcelona";
	std::string trackerConfig = "single,multiset,multiset,target_vienna3";

	//std::string trackerNameFeature = "stbnx";
	std::string trackerNameFeature = "stbnx_nft2";
	
	
	while(args.read("--video",videoName)) {}
	while(args.read("--tracker",trackerName,trackerConfig)) {}
	while(args.read("--tracker_config",trackerConfig)) {}

	osg::ref_ptr<osg::Group> root = new osg::Group;
	osgViewer::Viewer viewer;
	
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.setSceneData(root.get());

	viewer.realize();
	
	osgViewer::Viewer::Windows windows;
	viewer.getWindows(windows);
	
	for (osgViewer::Viewer::Windows::iterator wi = windows.begin(); wi != windows.end(); ++wi)
	{
		(*wi)->setWindowName("osgART - StbNX demo");
	}


	// preload the video and tracker
	osgART::PluginManager::instance()->load("osgart_video_" + videoName);
	osgART::PluginManager::instance()->load("osgart_tracker_stbnx");
	
	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = 
		dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_" + videoName));

	
	// check if an instance of the video stream could be started
	if (!video.valid()) 
	{   
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
		exit(-1);
	}
	
	
	OSG_NOTICE << "Now opening the video" << std::endl;

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();


	osg::ref_ptr<osgART::Tracker> tracker = 
		dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get("osgart_tracker_" + trackerNameFeature));


	if (!tracker.valid()) 
	{
		// Without tracker an AR application can not work. Quit if none found.
		OSG_FATAL << "Could not initialize tracker plugin!" << std::endl;
		exit(-1);
	}

	// get the tracker calibration object
	osg::ref_ptr<osgART::Calibration> calibration = tracker->getOrCreateCalibration();
	calibration->load("data/DefaultCalibration.cal");

	tracker->setImage(video.get());

	osgART::TrackerCallback::addOrSet(root.get(),tracker.get());
	
	if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(video.get())) 
	{
		osgART::addEventCallback(root.get(), new osgART::ImageStreamCallback(imagestream));
	}

	osg::ref_ptr<osg::Camera> cam = calibration->createCamera();
	root->addChild(cam.get());


	osg::ref_ptr<osgART::Marker> marker = tracker->addMarker(trackerConfig);
	marker->setActive(true);
	
	osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();
	osgART::attachDefaultEventCallbacks(arTransform.get(), marker.get());
	
	// need a higher renderbin to be on top of the video background
	arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");
	
	
	cam->addChild(arTransform.get());
	

	osg::ref_ptr<osg::Group> videoBackground = createImageBackground(video.get());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
	cam->addChild(videoBackground.get());
	
	video->start();
	return viewer.run();
	
}
