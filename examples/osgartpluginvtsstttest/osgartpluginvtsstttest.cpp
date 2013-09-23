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
#include <osgART/ImageStreamCallback>
#include <osgART/VideoUtils>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <iostream>
#include <sstream>

int main(int argc, char* argv[])  {


	osg::ref_ptr<osg::Group> root = new osg::Group;
	osgViewer::Viewer viewer;

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.setSceneData(root.get());


	// preload the video and tracker
	osgART::PluginManager::instance()->load("osgart_video_sstt");
	osgART::PluginManager::instance()->load("osgart_tracker_sstt");

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_sstt"));

	// check if an instance of the video stream could be started
	if (!video.valid())
	{
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plug-in!" << std::endl;
	}


	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();

	osg::ref_ptr<osgART::Tracker> tracker 
		= dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get("osgart_tracker_sstt"));

	if (!tracker.valid())
	{
		// Without tracker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plug-in!" << std::endl;

		return -1;

	}

	osg::notify() << "Tracker initialized" << std::endl;

	// get the tracker calibration object
	osg::ref_ptr<osgART::Calibration> calibration = tracker->getOrCreateCalibration();
	calibration->load("");

//#define SSTT_USE_IDTRACKER
#if defined(SSTT_USE_IDTRACKER)

	osg::ref_ptr<osgART::Target> marker = tracker->addTarget("id;14681358;30");
#else
	// this is loading file watch bmp as template 35.2x22.0 mm 
	// with a confidence threshold of 30%
	osg::ref_ptr<osgART::Target> marker = tracker->addTarget("watch.bmp;35.2;22.0;0.3");
#endif
	
	marker->setActive(true);

	tracker->setImage(video.get());

	osgART::TrackerCallback::addOrSet(root.get(),tracker.get());

	if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(video.get())) {
		osgART::addEventCallback(root.get(), new osgART::ImageStreamCallback(imagestream));
	}

	osg::ref_ptr<osg::Camera> cam = calibration->createCamera();
	root->addChild(cam.get());

	osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();
	osgART::attachDefaultEventCallbacks(arTransform.get(), marker.get());


	arTransform->addChild(osgART::testCube(80));
	arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");
	cam->addChild(arTransform.get());


	osg::ref_ptr<osg::Group> videoBackground = osgART::createImageBackground(video.get());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
	cam->addChild(videoBackground.get());

	video->start();

	while (!viewer.done()) {
		viewer.frame();
	}

	return 0;

}
