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
#include "osgART/VideoStream"



/**
 * \file osgART\VideoStream.cpp
 * \author Hartmut Seichter <hartmut@technotecture.com>
 * \brief implements a video sources using osg::ImageStream
 */
namespace osgART 
{

	VideoStream::VideoStream() : osg::ImageStream()
	{
		this->setPixelBufferObject(new osg::PixelBufferObject(this));
	}

	VideoStream::VideoStream(const VideoStream& video,
		const osg::CopyOp& copyop) : 
		osg::ImageStream(video,copyop)
	{
	}

	VideoStream::~VideoStream()
	{
		//this->close(false);
		this->setPixelBufferObject(NULL);
	}
	
	
	// virtual 
	void
	VideoStream::play() 
	{
		osg::ImageStream::play();
		//this->play();
	}
	
	//virtual
	void
	VideoStream::pause()
	{
		osg::ImageStream::pause();
		//this->pause();
	}
	
	/*
	//virtual 
	bool
	VideoStream::open() 
	{
		return false;
	}

	//virtual
	void
	VideoStream::close(bool waitForThread) 
	{
		this->quit(waitForThread);
	}
	*/
	
}
