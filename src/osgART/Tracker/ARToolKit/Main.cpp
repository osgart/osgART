/*
 *	osgART/Tracker/ARToolKit/Main
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

#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"

#include "ARToolKitTracker"

DLL_API osgART::GenericTracker* osgart_create_tracker()
{
	return new osgART::ARToolKitTracker();
}

OSGART_PLUGIN_ENTRY()
