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

#include "osgART/VideoImageStream"

#include <osg/Notify>

/**
 * \file osgART\VideoImageStream.cpp
 * \author Hartmut Seichter <hartmut@technotecture.com>
 * \brief implements a video sources using osg::ImageStream
 */
namespace osgART 
{

	VideoImageStream::VideoImageStream() : osg::ImageStream()
	{
	}

	VideoImageStream::VideoImageStream(const VideoImageStream& video,
		const osg::CopyOp& copyop) : 
		osg::ImageStream(video,copyop)
	{
	}

	/* virtual */
	void
	VideoImageStream::start() 
	{
		this->play();
	}
	
	/*virtual*/
	void
	VideoImageStream::stop()
	{
		this->pause();
	}
	
	/*virtual*/
	void
	VideoImageStream::open() 
	{
	}

	/*virtual*/
	void
	VideoImageStream::close(bool waitForThread/* = true*/) 
	{
		this->quit(waitForThread);
	}

	VideoImageStream::~VideoImageStream()
	{
		osg::notify() << "VideoImageStream D'tor" << std::endl; 

		this->close(false);
	}
	
}
