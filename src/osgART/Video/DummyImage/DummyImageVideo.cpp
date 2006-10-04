///////////////////////////////////////////////////////////////////////////////
// File name : DummyImageVideo.C
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

#include "osgART/VideoConfig"
#include "DummyImageVideo"
#include "OpenThreads/Thread"

#include <osgDB/Registry>
#include <osgDB/ReadFile>

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

DummyImageVideo::DummyImageVideo(const char* image):videoName(image)
{
	xsize=-1;
	ysize=-1;
	pixelsize=3;
	pixelformat=VIDEOFORMAT_RGB24;
}

/*
DummyImageVideo::DummyImageVideo(const DummyImageVideo &)
{
    
}*/

DummyImageVideo::~DummyImageVideo(void)
{
    
}

DummyImageVideo& 
DummyImageVideo::operator=(const DummyImageVideo &)
{
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void
DummyImageVideo::open()
{
	m_image = osgDB::readImageFile(videoName.c_str());
	xsize=m_image->s();
	ysize=m_image->t();

	if (osg::Image::computeNumComponents(m_image->getPixelFormat())==3)
	{
		m_image->setPixelFormat(GL_RGB);
		pixelsize=3;
		pixelformat=VIDEOFORMAT_RGB24;
	}
	else
	{
		if (osg::Image::computeNumComponents(m_image->getPixelFormat())==4)
		{
			m_image->setPixelFormat(GL_RGBA);
			pixelsize=4;
			pixelformat=VIDEOFORMAT_RGBA32;
		}
		else
		{
			std::cerr<<"ERROR:can't load the image, format not supported."<<std::endl;
			exit(-1);
		}
	}
	m_image->flipVertical();
}

void
DummyImageVideo::close()
{

}

void
DummyImageVideo::start()
{

}

void
DummyImageVideo::stop()
{

}

void
DummyImageVideo::update()
{
	/*
#if 0
	unsigned char* newImage = NULL;

	newImage=g_Image->data();

	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);

	if (!newImage) {
			image = NULL;
	} else {
			image = newImage;
		}
		
#endif

*/
}



///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////
