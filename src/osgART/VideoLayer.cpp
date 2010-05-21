/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
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
