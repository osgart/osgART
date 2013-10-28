/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
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

#include "osgART/Video"

#include <osg/Notify>

const int max_video_number=32;

namespace osgART {

	Video::Video() :
		osgART::Object(),
		FieldContainer<Video>(),
		_videoConfiguration(0L),
		_horizontalFlip(false),
		_verticalFlip(false)

	{
		_stats=new osg::Stats("video");
		_videoStreamList.reserve(max_video_number);
	}

	Video::Video(const Video& container,
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) :
		osgART::Object(),
		FieldContainer<Video>(),
		_videoConfiguration(0L),
		_horizontalFlip(false),
		_verticalFlip(false)
	{
		
	}
	
	Video::~Video()
	{	    
	}

	Video& 
	Video::operator=(const Video &)
	{
		return *this;
	}

	// static
	Video* Video::cast(osg::Referenced* instance)
	{ 
		return reinterpret_cast<Video*>(instance);
	}

	Field*
	Video::get(const std::string& name)
	{
		FieldMap::iterator _found = _fields.find(name);
		// return 0 if the field is not existant
		return (_found != _fields.end()) ? _found->second.get() : 0L;
	}

	// virtual
	VideoConfiguration* 
	Video::getOrCreateConfiguration()
	{
		if (!_videoConfiguration)
		{
			_videoConfiguration=new osgART::VideoConfiguration();
		}
		return _videoConfiguration;
	}

	// virtual
	void 
	Video::setConfiguration(VideoConfiguration* config)
	{
		*_videoConfiguration=*config;
	}
	
	// virtual 
	VideoStream* 
	Video::getStream(size_t i /* =0 */) 
	{
		//todo check stream id exist
		return _videoStreamList[i];
	}
	
	void 
	Video::setFlip(bool horizontal,
		bool vertical) 
	{
		_horizontalFlip = horizontal;
		_verticalFlip = vertical;
	}


};
