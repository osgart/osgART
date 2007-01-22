/*
 *	osgART/VideoBackground
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

#include "osgART/VideoBackground"

#include "osgART/VideoTextureRectangle"

namespace osgART {
	VideoBackground::VideoBackground(GenericVideo* video /* = 0L */) 
		: VideoLayer(video, 1)
	{

	}

	VideoBackground::~VideoBackground() 
	{
	}
};
