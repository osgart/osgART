/*
 *	osgART/VideoBillboard
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

#include "osgART/VideoBillboard"
#include "osgART/VideoManager"

#include <osg/Group>
#include <osg/Node>
#include <osg/AutoTransform>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/TextureRectangle>
#include <osg/Depth>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Notify>

namespace osgART {

	VideoBillboard::VideoBillboard(GenericVideo* video) : VideoPlane(video)
	{
	}

	VideoBillboard::~VideoBillboard()
	{	    
	}

	osg::Node*
	VideoBillboard::buildObject() 
	{
		osg::MatrixTransform* modelview = new osg::MatrixTransform();
		modelview->setMatrix(osg::Matrix::identity());
		// modelview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

		//osg::Depth* depth = new osg::Depth;
		//depth->setFunction(osg::Depth::ALWAYS);
		//depth->setRange(1.0, 1.0);

		osg::StateSet* backgroundStateSet = new osg::StateSet();        
		backgroundStateSet->setRenderBinDetails(1, "RenderBin");
		//backgroundStateSet->setAttribute(depth);
		
		osg::Group* backgroundGroup = new osg::Group();
		backgroundGroup->setStateSet(backgroundStateSet);

		osg::AutoTransform* billboardTransform = new osg::AutoTransform;
		billboardTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

		billboardTransform->addChild(buildGeometry());
		backgroundGroup->addChild(billboardTransform);

		modelview->addChild(backgroundGroup);

		return modelview;
	}
};
