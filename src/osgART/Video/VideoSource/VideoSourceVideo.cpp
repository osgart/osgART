///////////////////////////////////////////////////////////////////////////////
// File name : VideoSourceVideo.C
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

#include "VideoSourceVideo"
#include <OpenThreads/Thread>

#include <iniparser.h>

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

VideoSourceVideo::VideoSourceVideo(const char* config_file):
GenericVideo(),
videoConfig(config_file)
{
	pixelsize=3;
	pixelformat=VIDEOFORMAT_RGB24;
}

/*
VideoSourceVideo::VideoSourceVideo(const VideoSourceVideo &)
{
    
}*/

VideoSourceVideo::~VideoSourceVideo(void)
{
    
}

VideoSourceVideo& 
VideoSourceVideo::operator=(const VideoSourceVideo &)
{
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void
VideoSourceVideo::open()
{
	 // Create the video source:

	// 1) construct .ini parser
	IniParser parser(0);

	// 2) parse .ini file
    VideoSourceFactory::instance()->registerParameters(&parser);

	if (!parser.parse(videoConfig.c_str())) {
		std::cerr<<"ERROR:can't parse your video file.."<<std::endl;
	}
	// 3) try to construct a valid video source 
    if (!(vs=VideoSourceFactory::instance()->construct())) {
		std::cerr <<"ERROR:Unable to open video source!"<<std::endl;
    }

	vs->getSize(xsize,ysize);

	m_ipl_image = cvCreateImage(cvSize(xsize,ysize), IPL_DEPTH_8U, vs->getChannels());
}

void
VideoSourceVideo::close()
{
	cvReleaseImage(&m_ipl_image);
}

void
VideoSourceVideo::start()
{
  vs->start();
}

void
VideoSourceVideo::stop()
{
	vs->stop();
}

void
VideoSourceVideo::update()
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);

	vs->getFrame(m_ipl_image);
	if ((m_ipl_image!=NULL) && m_image.valid()) 
		m_image->setImage(this->xsize, this->ysize, 1, GL_RGB, GL_RGB, 
			GL_UNSIGNED_BYTE, (unsigned char*)m_ipl_image->imageData, osg::Image::NO_DELETE, 1);	

}



///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////
