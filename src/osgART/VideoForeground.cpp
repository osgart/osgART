/*
 *	osgART/VideoForeground
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

#include "osgART/VideoForeground"

#include "osgART/VideoTextureRectangle"
#include <osg/BlendFunc>

namespace osgART {
	VideoForeground::VideoForeground(GenericVideo* video) 
		: VideoLayer(video, 1000)
	{
	}

	VideoForeground::~VideoForeground() 
	{
	}
};
