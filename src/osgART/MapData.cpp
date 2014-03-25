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

// local include
#include "osgART/MapData"

namespace osgART {

    MapData::MapData()
		: osg::Object()
	{
	}

    MapData::MapData(const MapData& Mapdata, const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/)
        : osg::Object(Mapdata,copyop)
	{
        if (this != &Mapdata)
		{

		}
	}

    MapData::~MapData()
	{
	}

	/*virtual*/
    MapData::MapDataType
    MapData::getType() const
	{
        return MAPDATA_UNKNOWN;
	}

	//virtual 
	bool
    MapData::valid() const
	{
		return _valid;
	}

	//virtual 
	bool
    MapData::active() const
	{
		return _active;
	}

	//virtual
	void
    MapData::setActive(bool active /*= true*/)
	{
		_active = active;
	}

}
