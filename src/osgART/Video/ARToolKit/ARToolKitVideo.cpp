#include "ARToolKitVideo"

#include "osgART/VideoConfig"

using namespace osgART;


ARToolKitVideo::ARToolKitVideo(const char *name) : osgART::GenericVideo(),
	videoName(name),
	video(0L)
{

#if (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_RGBA)
		pixelsize=4;
		pixelformat=VIDEOFORMAT_RGB32;
#elif (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_ABGR)
		pixelsize=4;
		pixelformat=VIDEOFORMAT_ABGR32;
#elif (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_BGRA)
		pixelsize=4;
		pixelformat=VIDEOFORMAT_BGRA32;
#elif (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_ARGB)
		pixelsize=4;
		pixelformat=VIDEOFORMAT_ARGB32;
#elif (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_RGB)
		pixelsize=3;
		pixelformat=VIDEOFORMAT_RGB24;
#elif (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_BGR)
		pixelsize=3;
		pixelformat=VIDEOFORMAT_BGR24;
#elif (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_2vuy)
		pixelsize=3;	
		pixelformat=VIDEOFORMAT_YUV422;
#elif defined(AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_yuvs)
		pixelsize=3;	
		pixelformat=VIDEOFORMAT_YUV422;
#else
		#error "osgART: Unknown pixel format in config.h of your AR Toolkit installation
#endif

}

ARToolKitVideo::ARToolKitVideo(const ARToolKitVideo &)
{	    
}

ARToolKitVideo::~ARToolKitVideo() 
{
	this->close();
}

ARToolKitVideo& 
ARToolKitVideo::operator=(const ARToolKitVideo &)
{
	return *this;
}


void
ARToolKitVideo::open()
{
	video = ar2VideoOpen((char*)videoName.c_str());
	if (video) {
		ar2VideoInqSize(video, &xsize, &ysize);
	}
}

void
ARToolKitVideo::close()
{
	if (NULL != video) {
		int _ret = ar2VideoClose(video);
		
		if (0 == _ret) {
			video = NULL;
		}
	}
}

void
ARToolKitVideo::start()
{
	if (video) {
		ar2VideoCapStart(video);
		ar2VideoGetImage(video);
	}
}

void
ARToolKitVideo::stop()
{
	if (video) {
		ar2VideoCapStop(video);
	}
}

void
ARToolKitVideo::update()
{
	unsigned char* newImage = NULL;

	if (video) {

		OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());
		
		ar2VideoCapNext(video);

		newImage = (unsigned char*)ar2VideoGetImage(video);

		if (newImage && m_image.valid())
			m_image->setImage(this->xsize, this->ysize, 1, GL_BGRA, GL_BGRA, 
				GL_UNSIGNED_BYTE, newImage, osg::Image::NO_DELETE, 1);	

		/*
		if (!newImage) {
			image = NULL;
		} else {
			image = newImage;
		}
		*/

		// image = (unsigned char*)ar2VideoGetImage(video);

		
	}
}


void ARToolKitVideo::releaseImage() 
{
}

