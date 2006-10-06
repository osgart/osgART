///////////////////////////////////////////////////////////////////////////////
// File name : VideoWrapperVideo.C
//
// Creation : YYY
//
// Version : YYY
//
// Author : Raphael Grasset
//
// email : Raphael.Grasset@imag.fr
//
// Purpose : ??
//
// Distribution :
//
// Use :
//	??
//
// Todo :
//	O add more video formats
// 
//	/
//	X
//
// History :
//	YYY : Mr Grasset : Creation of the file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include file
///////////////////////////////////////////////////////////////////////////////

#include "VideoWrapperVideo"
#include <OpenThreads/Thread>

#include <process.h>

using namespace std;
using namespace osgART;

///////////////////////////////////////////////////////////////////////////////
// Macro 
///////////////////////////////////////////////////////////////////////////////

#if defined(NO_DEBUG)
#define ASSERT(x)
#else //defined(NO_DEBUG)
#define ASSERT(x) if(!(x)) \
    { cerr << "Assertion failed : (" << #x << ')' << endl \
    << "In file : " << __FILE__ << "at line #" << __LINE__ << endl \
    << "Compiled the " << __DATE__ << " at " << __TIME__ << endl; abort();}
#endif // else defined(NO_DEBUG)

const char* const Video_RCS_ID = "@(#)class Video definition.";

///////////////////////////////////////////////////////////////////////////////
// class Video
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Static variable
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PUBLIC: Standard services 
///////////////////////////////////////////////////////////////////////////////

VideoWrapperVideo::VideoWrapperVideo(const char* config):videoConfig(config)
{
	g_hVideo= 0;
	xsize=-1;
	ysize=-1;
	pixelformat=VIDEOFORMAT_RGB24;
}

/*
VideoWrapperVideo::VideoWrapperVideo(const VideoWrapperVideo &)
{
    
}*/

VideoWrapperVideo::~VideoWrapperVideo(void)
{
    
}

VideoWrapperVideo& 
VideoWrapperVideo::operator=(const VideoWrapperVideo &)
{
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////
void
VideoWrapperVideo::open()
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
	CoInitialize(NULL);
#endif

	VWResult result;

	// open camera
	if ((result=VIDEO_openVideo((char*)videoConfig.c_str(), &g_hVideo))!= VW_SUCCESS)
	{
		std::cerr<<"ERROR:VIDEO_openVideo failed."<<VIDEO_getErrDescription(result)<<std::endl;
	}

	 // get video size
	if ((result=VIDEO_getWidth(g_hVideo, &xsize))!=VW_SUCCESS)
	{
		std::cerr<<"ERROR:VIDEO_getWidth failed."<<VIDEO_getErrDescription(result)<<std::endl;
	}

	if ((result=VIDEO_getHeight(g_hVideo, &ysize))!=VW_SUCCESS)
	{
		std::cerr<<"ERROR:VIDEO_getHeight failed."<<VIDEO_getErrDescription(result)<<std::endl;
	}

	// set loop-- this only has an effect on the "replay" camera
    // used to play back captured video.
	VIDEO_replaySetPlaybackLoop( g_hVideo, TRUE);
}

void
VideoWrapperVideo::close()
{
   VWResult result;
   if ((result=VIDEO_close(g_hVideo))!=VW_SUCCESS)
   {
		std::cerr<<"ERROR:VIDEO_closeVideo failed."<<VIDEO_getErrDescription(result)<<std::endl;
   }
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
	CoUninitialize();
#endif
}

void
VideoWrapperVideo::start()
{
  VWResult result;
   if ((result=VIDEO_startVideo(g_hVideo))!=VW_SUCCESS)
   {
		std::cerr<<"ERROR:VIDEO_startVideo failed."<<VIDEO_getErrDescription(result)<<std::endl;
   }
}

void
VideoWrapperVideo::stop()
{
    VWResult result;
	// Always disconnect from the device when you're done
   if ((result=VIDEO_stopVideo(g_hVideo))!=VW_SUCCESS)
   {
		std::cerr<<"ERROR:VIDEO_stopVideofailed."<<VIDEO_getErrDescription(result)<<std::endl;
   }
}

void
VideoWrapperVideo::update()
{
	timeval timestamp;

	unsigned char* newImage = NULL;

	if (g_hVideo) {
		
		int result=VIDEO_getFrame(g_hVideo, &newImage, &timestamp);
		
		OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);

		if ((result==VW_SUCCESS) && m_image.valid())
			m_image->setImage(this->xsize, this->ysize, 1, GL_RGB, GL_RGB, 
				GL_UNSIGNED_BYTE, newImage, osg::Image::NO_DELETE, 1);
		//after passed it to m_image, video can't be release
		VIDEO_releaseFrame(g_hVideo);
	}
}

void
VideoWrapperVideo::releaseImage()
{
	//DOESN'T WORK
	//VIDEO_releaseFrame(g_hVideo);
}



///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////
