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

#include <osg/Notify>
#include <iostream>
#include <iomanip>

#include "osgART/VideoConfig"

// Make sure that required OpenGL constant definitions are available at compile-time.
// N.B. These should not be used unless the renderer indicates (at run-time) that it supports them.
// Define constants for extensions (not yet core).
#ifndef GL_APPLE_ycbcr_422
#  define GL_YCBCR_422_APPLE				0x85B9
#  define GL_UNSIGNED_SHORT_8_8_APPLE		0x85BA
#  define GL_UNSIGNED_SHORT_8_8_REV_APPLE	0x85BB
#endif
#ifndef GL_EXT_abgr
#  define GL_ABGR_EXT						0x8000
#endif
#ifndef GL_MESA_ycbcr_texture
#  define GL_YCBCR_MESA						0x8757
#  define GL_UNSIGNED_SHORT_8_8_MESA		0x85BA
#  define GL_UNSIGNED_SHORT_8_8_REV_MESA	0x85BB
#endif

namespace osgART {

	ARToolKitVideo::ARToolKitVideo() : GenericVideo(),
		video(0L)
	{
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

		// get the format only once for opening the video source
		if (getGLPixelFormatForARPixelFormat(AR_DEFAULT_PIXEL_FORMAT, 
						&_internalformat_GL, 
						&_format_GL, 
						&_datatype_GL))
		{
			osg::notify(osg::FATAL) << "osgART::ARToolKitVideo::open() << unknown video format! " << std::endl;
			return;

		} else 
		{
			osg::notify() << "osgART::ARToolKitVideo::open() using format [GL (internal) " <<
				"0x" << std::hex << _internalformat_GL << ", " <<
				"0x" << std::hex << _format_GL << ", " << 
				"0x" << std::hex << std::uppercase << _datatype_GL << "]" << std::endl;
		}


		if (m_config.deviceconfig != "") {
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
		this->allocateImage(xsize, ysize, 1, _format_GL, _datatype_GL, 1);
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
		if (video)
		{
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
			{
				std::cout << "SGGG" << std::endl;
				this->setImage(this->s(), this->t(), 
					1, _internalformat_GL, _format_GL, _datatype_GL, newImage , 
					osg::Image::NO_DELETE, 1);
			}
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

	int ARToolKitVideo::getGLPixelFormatForARPixelFormat(const int arPixelFormat, 
		GLint *internalformat_GL, GLenum *format_GL, GLenum *type_GL)
	{
		// Translate the internal pixelformat to an OpenGL texture2D triplet.
		switch (arPixelFormat) {
			case AR_PIXEL_FORMAT_RGB:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_RGB;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_BGR:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_BGR;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_RGBA:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_RGBA;
				*type_GL = GL_UNSIGNED_BYTE;
			case AR_PIXEL_FORMAT_BGRA:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_BGRA;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_ARGB:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_BGRA;
	#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_INT_8_8_8_8_REV;
	#else
				*type_GL = GL_UNSIGNED_INT_8_8_8_8;
	#endif
				break;
			case AR_PIXEL_FORMAT_ABGR:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_ABGR_EXT;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_MONO:
				*internalformat_GL = GL_LUMINANCE8;
				*format_GL = GL_LUMINANCE;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_2vuy:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_YCBCR_422_APPLE; //GL_YCBCR_MESA
	#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_SHORT_8_8_REV_APPLE; //GL_UNSIGNED_SHORT_8_8_REV_MESA
	#else
				*type_GL = GL_UNSIGNED_SHORT_8_8_APPLE; //GL_UNSIGNED_SHORT_8_8_MESA
	#endif
				break;
			case AR_PIXEL_FORMAT_yuvs:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_YCBCR_422_APPLE; //GL_YCBCR_MESA
	#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_SHORT_8_8_APPLE; //GL_UNSIGNED_SHORT_8_8_MESA
	#else
				*type_GL = GL_UNSIGNED_SHORT_8_8_REV_APPLE; //GL_UNSIGNED_SHORT_8_8_REV_MESA
	#endif
				break;
			default:
				return (-1);
				break;
		}
		return (0);
	}
}
