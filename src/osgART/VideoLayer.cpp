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

#include "osgART/VideoLayer"
#include "osgART/Tracker"

#include <osg/Texture>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/Group>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Depth>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Notify>
#include <osg/Image>
#include <osg/GraphicsContext>

namespace osgART {


	// VideoLayer
	VideoLayer::VideoLayer() : osg::Camera()
	{	
		this->setProjectionMatrixAsOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
		this->setViewMatrix(osg::Matrix::identity());
		this->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		this->setRenderOrder(osg::Camera::NESTED_RENDER);

		this->getOrCreateStateSet()->setMode(GL_LIGHTING, GL_FALSE);

		this->setClearMask(GL_DEPTH_BUFFER_BIT);
		this->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, GL_FALSE);
		this->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0, 1, false));

		
	}

	VideoLayer::VideoLayer(const VideoLayer& videolayer,
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) : 
		osg::Camera(videolayer,copyop)
	{		
	}


	VideoLayer::~VideoLayer()
	{	    
	}
	
	void VideoLayer::setWindowSize(osg::Vec2i pos, osg::Vec2i size) {
			this->setViewport(new osg::Viewport(pos[0],pos[1],size[0],size[1]));		
	}

	void VideoLayer::setRelativeSize(osg::Vec2f pos, osg::Vec2f size, osg::Camera* main) {

			this->setGraphicsContext(main->getGraphicsContext());

			this->setViewport(new osg::Viewport(main->getGraphicsContext()->getTraits()->width*pos[0],
			main->getGraphicsContext()->getTraits()->height*pos[1],
			main->getGraphicsContext()->getTraits()->width*size[0],
			main->getGraphicsContext()->getTraits()->height*size[1]));

			this->setProjectionResizePolicy(osg::Camera::FIXED);
	}

	//void VideoLayer::setRelativeSize(osg::Vec2i pos, osg::Vec2i size) {
			//this->setViewport(new osg::Viewport(pos[0],pos[1],size[0],size[1]));
	//}
	
	VideoFlipper::VideoFlipper(bool flipH, bool flipV) : osg::MatrixTransform(),
		_flipH(flipH),
		_flipV(flipV)
	{
		updateFlip();
	}

	void VideoFlipper::updateFlip() 
	{
		this->setMatrix(osg::Matrix::scale(_flipH ? -1 : 1, _flipV ? -1 : 1, 1) * osg::Matrix::translate(_flipH ? 1 : 0, _flipV ? 1 : 0, 0));
	}

	void VideoFlipper::setFlipH(bool flipH) 
	{
		_flipH = flipH;
		updateFlip();
	}
	
	bool VideoFlipper::getFlipH()
	{
		return _flipH;
	}

	void VideoFlipper::setFlipV(bool flipV) 
	{
		_flipV = flipV;
		updateFlip();
	}
	
	bool VideoFlipper::getFlipV()
	{
		return _flipV;
	}

};
