/*
 *	osgART/ARTTransform
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

#include <osgART/ARTTransform>
#include <osgART/ARTTransformCallback>
#include <osgART/GenericTracker>
#include <osgART/TrackerManager>
#include <osgART/Marker>

#include <osg/Notify>

namespace osgART {

	ARTTransform::ARTTransform(Marker* marker) :  
		osg::MatrixTransform(),
		m_marker(marker)
	{	
		if (m_marker.valid()) 
		{
			this->setUpdateCallback(new ARTTransformCallback(m_marker.get()));
		} else 
		{
			osg::notify(osg::FATAL) << "ARTTransform::ARTTransform(marker): " <<
				"Could not create a transform callback" << std::endl;
		}
	}

	/* virtual */
	ARTTransform::~ARTTransform() 
	{
		this->setUpdateCallback(0L);
	}

	ARTTransform& 
	ARTTransform::operator=(const ARTTransform &)
	{
		return *this;
	}

	Marker* 
	ARTTransform::getMarker() 
	{
		return m_marker.get();
	}

};
