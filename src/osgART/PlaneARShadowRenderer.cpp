/*
 *	osgART/PlaneARShadowRenderer
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

#include <osgART/PlaneARShadowRenderer>
#include <osgART/VideoTexture>
#include <osgART/VideoManager>
#include <osgART/VideoTextureRectangle>
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

#include <osgART/TrackerManager>

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
		dynamic_cast<osg::Group*>(m_shadowedScene)->addChild(osgDB::readNodeFile("models/myplane.ive"));
		m_shadowedScene->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
	}

}
