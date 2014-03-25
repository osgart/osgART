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
#include "osgART/TrackerUtils"



namespace osgART {

	osg::Camera*
	createBasicCamera(osgART::CameraConfiguration* calib)
	{
		osg::Camera* cam = new osg::Camera();
		cam->setRenderOrder(osg::Camera::NESTED_RENDER);
		cam->setViewMatrix(osg::Matrix::identity());
		cam->setProjectionMatrix(calib->getProjectionMatrix());
		cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		cam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

		return cam;		
	}

}
