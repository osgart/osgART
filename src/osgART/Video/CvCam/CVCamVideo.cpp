///////////////////////////////////////////////////////////////////////////////
// File name : CVCamVideo.C
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

#include "CVCamVideo"
#include <OpenThreads/Thread>

#include <process.h>

#include <cv.h>

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

unsigned char* CVCamVideo::newImage=NULL;

void imageCallback(IplImage* image)
{
	CVCamVideo::newImage=(unsigned char*)image->imageData;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC: Standard services 
///////////////////////////////////////////////////////////////////////////////

CVCamVideo::CVCamVideo()
{
	camIndex=1;
	pixelsize=3;
	pixelformat=VIDEOFORMAT_RGB24;
	/*
	xsize=_xsize;
	ysize=_ysize;

	gFrameNum=0;
	gDevNum=0;

	switch (pft)
	{
	case VIDEOFORMAT_RGB24:
		pixelsize=3;
		pixelformat=VIDEOFORMAT_RGB24;
		framerate=frt;
		vidFormat=VIDCAP_FORMAT_RGB24;
		switch (frt)
		{
		case VIDEOFRAMERATE_15:vidFrameRate=15;break;
		case VIDEOFRAMERATE_30:vidFrameRate=30;break;
		default:std::cerr<<"ERROR:Frame Rate not supported.."<<std::endl; exit(-1);break;
		}
		break;
	default: std::cerr<<"ERROR:Format not supported.."<<std::endl; exit(-1);break;
	}
	isRunning=false;
	haveNewImage=false;
	lastCVImage2=NULL;
	lastCVImage=NULL;*/
}

/*
CVCamVideo::CVCamVideo(const CVCamVideo &)
{
    
}*/

CVCamVideo::~CVCamVideo(void)
{
    
}

CVCamVideo& 
CVCamVideo::operator=(const CVCamVideo &)
{
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void
CVCamVideo::open()
{
	int* out;
	cvcamSelectCamera(&out);

	camIndex=out[0];
	cvcamSetProperty(camIndex, CVCAM_RNDWIDTH, &xsize);
	cvcamSetProperty(camIndex, CVCAM_RNDHEIGHT, &ysize);
	cvcamSetProperty(camIndex, CVCAM_PROP_ENABLE, CVCAMTRUE); 
	cvcamSetProperty(camIndex, CVCAM_PROP_RENDER, CVCAMTRUE); 
	cvcamSetProperty(0, CVCAM_PROP_CALLBACK, imageCallback);
	cvcamInit();
}

void
CVCamVideo::close()
{
  cvcamExit(); 
}

void
CVCamVideo::start()
{
   cvcamStart();
}

void
CVCamVideo::stop()
{
	cvcamStop();
}

void
CVCamVideo::update()
{

	IplImage* ipl_image;

	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);

	cvcamGetProperty(0,"raw_image",&ipl_image);

	if (ipl_image && m_image.valid()) 
		m_image->setImage(this->xsize, this->ysize, 1, GL_BGRA, GL_BGRA, 
			GL_UNSIGNED_BYTE, (unsigned char*)ipl_image->imageData, osg::Image::NO_DELETE, 1);	
}

void 
CVCamVideo::releaseImage()
{
	cvcamResume();
}


///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////
