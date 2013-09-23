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

#include <osg/Geometry>
#include <osg/ShapeDrawable>

#include <osgART/Foundation>
#include <osgART/VideoLayer>
#include <osgART/PluginManager>
#include <osgART/VideoGeode>
#include <osgART/Utils>
#include <osgART/GeometryUtils>
#include <osgART/ImageStreamCallback>
#include <osgART/TargetCallback>
#include <osgART/VideoUtils>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>


int main(int argc, char* argv[]) 
{
	// parse arguments
	osg::ArgumentParser args(&argc,argv);
	
	std::string videoName;
	std::string videoConfig;
		
	while (args.read("--video",videoName)) {};
	while (args.read("--config",videoConfig)) {};

    osg::setNotifyLevel(osg::DEBUG_INFO);

	// preload the video
	if (!osgART::PluginManager::instance()->load("osgart_video_" + videoName)) {

		OSG_FATAL << "Can't find video plugin" << std::endl;
		return -1;
	}
	
	// Set up the osgART viewer (a wrapper around osgProducer or osgViewer).
	osgViewer::Viewer viewer;

	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// create a root node
	osg::ref_ptr<osg::Group> root = new osg::Group;

	// connect the scenegraph with the viewer
	viewer.setSceneData(root.get());

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = 
		dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_" + videoName));

	// check if an instance of the video stream could be started
	if (!video.valid()) 
	{   
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
		return -1;
	}

	// found video - configure now
	osgART::VideoConfiguration* config = video->getVideoConfiguration();

	// if the configuration is existing
	if (config) 
	{
		config->deviceconfig = videoConfig;
		// it is possible to configure the plugin before opening it

		//_config->deviceconfig = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		//	"<dsvl_input><avi_file use_reference_clock=\"true\" file_name=\"Data\\MyVideo.avi\" loop_avi=\"true\" render_secondary=\"true\">"
		//	"<pixel_format><RGB32/></pixel_format></avi_file></dsvl_input>";
	}
	

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();

	if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(video.get())) {
		osgART::addEventCallback(root.get(), new osgART::ImageStreamCallback(imagestream));
	}


	// attach a stats handler
	video->setStats(new osg::Stats("osgART video " + videoName));

	
	// Creating a video background
	osg::ref_ptr<osg::Group> videoBackground = osgART::createImageBackground(video.get());

	root->addChild(videoBackground.get());
	
	video->start();

	return viewer.run();

	
}
