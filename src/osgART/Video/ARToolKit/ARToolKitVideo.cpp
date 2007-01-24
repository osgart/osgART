/*
 *	osgART/Video/ARToolKit/ARToolKitVideo
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

#include "ARToolKitVideo"

#include "osgART/VideoConfig"

namespace osgART {

	ARToolKitVideo::ARToolKitVideo() : GenericVideo(),
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
		char* config = 0;
		int xsize = 0;
		int ysize = 0;

		if (m_config.deviceconfig != "") 
		{
			config = (char*)&m_config.deviceconfig.c_str()[0];
		}

		// open the video capture device
		video = ar2VideoOpen(config);
		
		// check if the video was successfully opened
		if (video) 
		{
			// get the video size
			ar2VideoInqSize(video, &xsize, &ysize);
		}

		// create an image that same size (packing set to 1)
		this->allocateImage(xsize,ysize,1,
			GL_BGRA,GL_UNSIGNED_BYTE, 1);
	}

	void
	ARToolKitVideo::close(bool waitForThread)
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

		if (video) 
		{

			OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());
			
			ar2VideoCapNext(video);

			newImage = (unsigned char*)ar2VideoGetImage(video);

			if (newImage)
				dynamic_cast<osg::Image*>(this)->setImage(this->s(), this->t(), 
				1, GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE, newImage, 
				osg::Image::NO_DELETE, 1);	

		}
	}

	VideoConfiguration* 
	ARToolKitVideo::getVideoConfiguration() 
	{
		return &m_config;
	}


	void ARToolKitVideo::releaseImage() 
	{
	}
}
