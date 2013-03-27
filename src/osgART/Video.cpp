/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
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
		: osgART::Object(),
		VideoImageStream(), 
		FieldContainer<Video>()
	{

		this->setPixelBufferObject(new osg::PixelBufferObject(this));
		
	}

	Video::Video(const Video& container,
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) :
		osgART::Object(),
		VideoImageStream(container),
		FieldContainer<Video>()
	{
		
	}

	Video* Video::cast(osg::Referenced* instance)
	{ 
		return reinterpret_cast<Video*>(instance);
	}

	
	Video::~Video()
	{	    
		this->setPixelBufferObject(NULL);
	}

	Video& 
	Video::operator=(const Video &)
	{
		return *this;
	}

	Field*
	Video::get(const std::string& name)
	{
		FieldMap::iterator _found = _fields.find(name);
		// return 0 if the field is not existant
		return (_found != _fields.end()) ? _found->second.get() : 0L;
	}

	/* virtual */
	VideoConfiguration* 
	Video::getConfiguration()
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
