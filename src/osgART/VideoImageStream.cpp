/*
*	osgART/VideoImageStream
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

#include "osgART/VideoImageStream"

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
	
}