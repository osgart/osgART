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
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

#include <osgART/PlaneARShadowRenderer>
#include <osg/Group>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/TextureRectangle>
#include <osg/Depth>
#include <osg/Geometry>
#include <osg/BlendFunc>

#include <osg/CameraNode>
#include <osg/PolygonOffset>
#include <osg/LightModel>
#include <osg/LightSource>
#include <osg/BlendFunc>

#include <osgDB/ReadFile>


namespace osgART {

	PlaneARShadowRenderer::PlaneARShadowRenderer(osg::Node* scene,osg::MatrixTransform* light,osg::Vec2f size,int marker) : 
ShadowRenderer(scene,light,marker),m_planeSize(size)
	{

	}

	PlaneARShadowRenderer::~PlaneARShadowRenderer()
	{	    
	}

	void 
	PlaneARShadowRenderer::init()
	{   
		ShadowRenderer::init();

		// hse25: should be replaced with a generic plane. no osgDB and hard wired filenames
		
		//HACK we need to construct a scene here from the size parameters
#if 0
		dynamic_cast<osg::Group*>(m_shadowedScene)->addChild(osgDB::readNodeFile("models/myplane.ive"));
		m_shadowedScene->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
#endif
	}

}
