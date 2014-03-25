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
#include <osg/Notify>
#include <osg/ValueObject>
#include <osg/UserDataContainer>

// local include
#include "osgART/Export"
#include "osgART/GPSInertialTracker"
#include "osgART/Target"
#include "osgART/Video"



namespace osgART {

    ///

	GPSInertialTracker::GPSInertialTracker() :
        Tracker()
	{

    }

    GPSInertialTracker::GPSInertialTracker(const GPSInertialTracker &container, 
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) :
		Tracker()
    {
    }

	GPSInertialTracker::~GPSInertialTracker() 
	{
		//~Tracker();
	}
	
	GPSInertialTracker& 
	GPSInertialTracker::operator=(const GPSInertialTracker &)
	{
		return *this;
	}

	// static 
	GPSInertialTracker* GPSInertialTracker::cast(osg::Referenced* instance)
	{ 
		return reinterpret_cast<GPSInertialTracker*>(instance);
	}
	
};
