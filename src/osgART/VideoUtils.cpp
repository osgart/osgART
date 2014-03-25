/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2014 Raphael Grasset, Julian Looser, Hartmut Seichter
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

// OpenThreads include

// OSG include

// local include
#include "osgART/VideoUtils"

#include "osgART/VideoLayer"
#include "osgART/VideoGeode"
#include "osgART/VideoPlane"



namespace osgART {

	osg::Group*
	createBasicVideoBackground(osg::Image* video,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
        osgART::VideoGeode* _geode = new osgART::VideoPlane(video,
                                                            useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D,
                                                            1, 1, 20, 20);
		_layer->addChild(_geode);
		return _layer;
	}

	osg::Group*
    createUndistortVideoBackground(osg::Image* video,osgART::CameraConfiguration* cameraconfig,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
		_layer->setType(osgART::VideoLayer::VIDEO_BACKGROUND);

        osgART::VideoGeode* _geode = new osgART::UndistortedVideoPlane(video,
                                                            useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D,
                                                            cameraconfig,
                                                            1, 1, 20, 20);
        _layer->addChild(_geode);
		return _layer;
	}
	
	osg::Group*
	createBasicFixedVideoBackground(osg::Image* video, osg::Vec2i pos, osg::Vec2i size,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
		_layer->setType(osgART::VideoLayer::VIDEO_BACKGROUND);
		_layer->setWindowSize(pos,size);

        osgART::VideoGeode* _geode = new osgART::VideoPlane(video,
                                                            useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D,
                                                            1, 1, 20, 20);
        _layer->addChild(_geode);
		return _layer;
	}
	
	osg::Group*
	createBasicFloatingVideoBackground(osg::Image* video, osg::Vec2f pos, osg::Vec2f size,osg::Camera* main,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
		_layer->setType(osgART::VideoLayer::VIDEO_BACKGROUND);
		_layer->setRelativeSize(pos,size,main);

        osgART::VideoGeode* _geode = new osgART::VideoPlane(video,
                                                            useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D,
                                                            1, 1, 20, 20);
        _layer->addChild(_geode);
		return _layer;
	}

	osg::Group*
	createBasicVideoForeground(osg::Image* video,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
		_layer->setType(osgART::VideoLayer::VIDEO_FOREGROUND);

        osgART::VideoGeode* _geode = new osgART::VideoPlane(video,
                                                            useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D,
                                                            1, 1, 20, 20);
        _layer->addChild(_geode);
		return _layer;
	}

	osg::Group*
    createUndistortVideoForeground(osg::Image* video,osgART::CameraConfiguration* cameraconfig,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
		_layer->setType(osgART::VideoLayer::VIDEO_FOREGROUND);

        osgART::VideoGeode* _geode = new osgART::UndistortedVideoPlane(video,
                                                            useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D,
                                                            cameraconfig,
                                                            1, 1, 20, 20);
        _layer->addChild(_geode);
		return _layer;
	}
	
	osg::Group*
	createBasicFixedVideoForeground(osg::Image* video, osg::Vec2i pos, osg::Vec2i size,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
		_layer->setType(osgART::VideoLayer::VIDEO_FOREGROUND);
		_layer->setWindowSize(pos,size);

        osgART::VideoGeode* _geode = new osgART::VideoPlane(video,
                                                            useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D,
                                                            1, 1, 20, 20);
        _layer->addChild(_geode);
		return _layer;
	}
	
	osg::Group*
	createBasicFloatingVideoForeground(osg::Image* video, osg::Vec2f pos, osg::Vec2f size,osg::Camera* main,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
		_layer->setType(osgART::VideoLayer::VIDEO_FOREGROUND);
		_layer->setRelativeSize(pos,size,main);

        osgART::VideoGeode* _geode = new osgART::VideoPlane(video,
                                                            useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D,
                                                            1, 1, 20, 20);
        _layer->addChild(_geode);
		return _layer;
	}
	
	/*
	osg::Group* createImageBackground(osg::Image* video) {
	osgART::VideoLayer* _layer = new osgART::VideoLayer();
	_layer->setSize(*video);
	osgART::VideoGeode* _geode = new osgART::VideoGeode(osgART::VideoGeode::USE_TEXTURE_2D, video);
	addTexturedQuad(*_geode,video->s(),video->t());
	_layer->addChild(_geode);
	return _layer;
	}*/
}
