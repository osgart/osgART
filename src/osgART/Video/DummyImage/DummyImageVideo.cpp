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

#include <osg/Notify>

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

DummyImageVideo::DummyImageVideo(const char* image):
	osgART::GenericVideo(),
	videoName(image),
	m_flip_horizontal(false),
	m_flip_vertical(false)
{
	xsize=-1;
	ysize=-1;
	pixelsize=3;
	pixelformat=VIDEOFORMAT_RGB24;

	m_fields["flip_horizontal"] = new TypedField<bool>(&m_flip_horizontal);
	m_fields["flip_vertical"]	= new TypedField<bool>(&m_flip_vertical);
	m_fields["image_file"]		= new CallbackField<DummyImageVideo, std::string>(this,
		&DummyImageVideo::getImageFile,
		&DummyImageVideo::setImageFile);
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
//yannick
	osg::notify() << "DummyImageVideo::open()  open image : " << videoName << std::endl;
	if (videoName == "")
	{
		osg::notify(osg::WARN) << "Error in DummyImageVideo::open(), File name is empty!";
		return;
	}
//=====================	
	m_image = osgDB::readImageFile(videoName.c_str());
//yannick
	if (!m_image)
	{
		osg::notify(osg::WARN) << "Error in DummyImageVideo::open(), Could not open File!";
		return	;
	}
//=====================
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
	if (m_flip_vertical) {
		m_image->flipVertical();
	}

	if (m_flip_horizontal) {
        m_image->flipHorizontal();
	}
}

void
DummyImageVideo::close()
{
}


void DummyImageVideo::setImageFile(const std::string & _NewFile)
{
//relase previous image..???
	videoName = _NewFile;
	open();
}
	
std::string DummyImageVideo::getImageFile()const
{
	return videoName;
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
