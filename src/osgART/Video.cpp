/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * 
 * This file is part of osgART 2.0
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "osgART/Video"

#include <osg/Notify>

namespace osgART {

	Video::Video() 
		: VideoImageStream(), 
		FieldContainer<Video>()
	{
		osg::notify(osg::INFO) << "Generic Video c'tor" << std::endl;
	}

	Video::Video(const Video& container,
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) :
		VideoImageStream(container),
		FieldContainer<Video>()
	{
		osg::notify(osg::INFO) << "Copy C'tor" << std::endl;
	}

	Video* Video::cast(osg::Referenced* instance)
	{ 
		return reinterpret_cast<Video*>(instance);
	}

	
	Video::~Video()
	{	    
		osg::notify(osg::INFO) << "Shut down video" << std::endl;
	}

	Video& 
	Video::operator=(const Video &)
	{
		return *this;
	}

	Field*
	Video::get(const std::string& name)
	{
		FieldMap::iterator _found = m_fields.find(name);
		// return 0 if the field is not existant
		return (_found != m_fields.end()) ? _found->second.get() : 0L;
	}

	/* virtual */
	VideoConfiguration* 
	Video::getVideoConfiguration()
	{
		return 0L;
	}

	void 
	Video::setFlip(bool horizontal,
		bool vertical) 
	{
		m_horizontal_flip = horizontal;
		m_vertical_flip = vertical;

	}

	


};
