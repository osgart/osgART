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
#include <osg/AutoTransform>

#include <osgART/Foundation>
#include <osgART/VideoLayer>
#include <osgART/PluginManager>
#include <osgART/VideoGeode>
#include <osgART/Utils>

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>


void addTexturedQuad(osgART::VideoGeode& geode, float width, float height)
{

	float maxU(1.0);
	float maxV(1.0);
	
	if (geode.getTextureMode() == osgART::VideoGeode::USE_TEXTURE_RECTANGLE) 
	{
		maxU = width;
		maxV = height;
	} else 
	{
		maxU = width / (float)osgART::nextPowerOfTwo((unsigned int)width);
		maxV = height / (float)osgART::nextPowerOfTwo((unsigned int)height);
	}
	
	osg::Geometry* geometry = new osg::Geometry();
		
	osg::Vec3Array* vcoords = new osg::Vec3Array();
	geometry->setVertexArray(vcoords);

	osg::Vec2Array* tcoords = new osg::Vec2Array();
	geometry->setTexCoordArray(0, tcoords);

	vcoords->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vcoords->push_back(osg::Vec3(width, 0.0f, 0.0f));
	vcoords->push_back(osg::Vec3(width, height, 0.0f));
	vcoords->push_back(osg::Vec3(0.0f,  height, 0.0f));

	tcoords->push_back(osg::Vec2(0.0f, maxV));
	tcoords->push_back(osg::Vec2(maxU, maxV));
	tcoords->push_back(osg::Vec2(maxU, 0.0f));
	tcoords->push_back(osg::Vec2(0.0f, 0.0f));

	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	geode.addDrawable(geometry);
}



osg::Group* createImageBackground(osg::Image* video)
{

	osgART::VideoLayer* _layer = new osgART::VideoLayer();
	
	_layer->setSize(*video);

	osgART::VideoGeode* _geode = new osgART::VideoGeode(osgART::VideoGeode::USE_TEXTURE_2D, video);

	addTexturedQuad(*_geode,video->s(),video->t());

	_layer->addChild(_geode);

	return _layer;
}

osg::Node* createImageBillboard(osg::Image* video)
{
	osg::AutoTransform* node = new osg::AutoTransform;

	node->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	osgART::VideoGeode* _geode = new osgART::VideoGeode(osgART::VideoGeode::USE_TEXTURE_2D, video);
	
	addTexturedQuad(*_geode,video->s(),video->t());

	node->addChild(_geode);
	
	return node;

}

int main(int argc, char* argv[]) 
{

	// preload the video
	int _video_id = osgART::PluginManager::getInstance()->load("osgart_video_artoolkit");
	
	// Set up the osgART viewer (a wrapper around osgProducer or osgViewer).
	osgViewer::Viewer viewer;

	// create a root node
	osg::ref_ptr<osg::Group> root = new osg::Group;

	// connect the scenegraph with the viewer
	viewer.setSceneData(root.get());

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = 
		dynamic_cast<osgART::Video*>(osgART::PluginManager::getInstance()->get(_video_id));

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


	// Creating a video background
	root->addChild(createImageBackground(video.get()));

	// Creating a video billboard
	root->addChild(createImageBillboard(video.get()));

	video->start();

	return viewer.run();
	
}
