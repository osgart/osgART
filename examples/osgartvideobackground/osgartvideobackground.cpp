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

#include <osg/Geometry>
#include <osg/ShapeDrawable>

#include <osgART/Foundation>
#include <osgART/VideoLayer>
#include <osgART/PluginManager>
#include <osgART/VideoGeode>
#include <osgART/Utils>
#include <osgART/GeometryUtils>
#include <osgART/ImageStreamCallback>
#include <osgART/MarkerCallback>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

osg::Group* createImageBackground(osg::Image* video, bool useTextureRectangle = false) {
	osgART::VideoLayer* _layer = new osgART::VideoLayer();
	//_layer->setSize(*video);
	osgART::VideoGeode* _geode = new osgART::VideoGeode(video, NULL, 1, 1, 20, 20, 
		useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D);
	//addTexturedQuad(*_geode,video->s(),video->t());
	_layer->addChild(_geode);
	return _layer;
}


int main(int argc, char* argv[]) 
{

	// preload the video
	int _video_id = osgART::PluginManager::instance()->load("osgart_video_artoolkit2");
	
	// Set up the osgART viewer (a wrapper around osgProducer or osgViewer).
	osgViewer::Viewer viewer;

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// create a root node
	osg::ref_ptr<osg::Group> root = new osg::Group;

	// connect the scenegraph with the viewer
	viewer.setSceneData(root.get());

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = 
		dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get(_video_id));

	// check if an instance of the video stream could be started
	if (!video.valid()) 
	{   
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
		exit(-1);
	}

	// found video - configure now
	osgART::VideoConfiguration* _config = video->getVideoConfiguration();

	// if the configuration is existing
	if (_config) 
	{
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
	video->setStats(new osg::Stats("artoolkit2_video"));

	
	// Creating a video background
	osg::ref_ptr<osg::Group> videoBackground = createImageBackground(video.get());

	root->addChild(videoBackground.get());
	
	video->start();

	return viewer.run();

	
}
