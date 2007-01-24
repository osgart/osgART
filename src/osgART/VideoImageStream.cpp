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