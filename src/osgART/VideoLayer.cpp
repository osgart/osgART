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
	VideoLayer::VideoLayer() : 
		osg::Camera()
	{		
		this->setProjectionMatrixAsOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
		this->setViewMatrix(osg::Matrix::identity());
		this->setRenderOrder(osg::Camera::NESTED_RENDER);
		this->setClearMask(GL_DEPTH_BUFFER_BIT);
		this->getOrCreateStateSet()->setMode(GL_LIGHTING, GL_FALSE);
		this->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, GL_FALSE);
		this->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		//this->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
	}

	VideoLayer::VideoLayer(const VideoLayer& videolayer,
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) : 
		osg::Camera(videolayer,copyop)
	{		
	}


	VideoLayer::~VideoLayer()
	{	    
	}
	
};
