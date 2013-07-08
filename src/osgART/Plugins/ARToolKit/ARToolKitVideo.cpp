/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */



#include "ARToolKitVideo"


using namespace osgART;



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


// check for newer ARToolKit version
#ifndef AR_HAVE_HEADER_VERSION_2_72
#error ARToolKit v2.72 or later is required to build the OSGART ARToolKit tracker.
#endif



ARToolKitVideo::ARToolKitVideo() : osgART::Video(),
	video(0L)
{
}

ARToolKitVideo::ARToolKitVideo(const ARToolKitVideo &,
		const osg::CopyOp& copyop/* = osg::CopyOp::SHALLOW_COPY*/)
{
}

ARToolKitVideo::~ARToolKitVideo()
{
	//this->close(false);
}

ARToolKitVideo&
ARToolKitVideo::operator=(const ARToolKitVideo &)
{
	return *this;
}

bool
ARToolKitVideo::open()
{
	char* config = 0;
	int xsize = 0, ysize = 0;

	// Get the format only once for opening the video source
	if (getGLPixelFormatForARPixelFormat(AR_DEFAULT_PIXEL_FORMAT,
					&_internalformat_GL,
					&_format_GL,
					&_datatype_GL))
	{
		osg::notify(osg::FATAL) << "osgART::ARToolKitVideo::open() << unknown video format! " << std::endl;
		return false;
	}
	else
	{
		osg::notify() << "osgART::ARToolKitVideo::open() using format [GL (internal) " <<
			"0x" << std::hex << _internalformat_GL << ", " <<
			"0x" << std::hex << _format_GL << ", " <<
			"0x" << std::hex << std::uppercase << _datatype_GL << "]" << std::endl;
	}


	if (m_config.config != "") {
		config = (char*)&m_config.config.c_str()[0];
	}

	// open the video capture device
	video = ar2VideoOpen(config);

	// check if the video was successfully opened
	if (video)
	{
		// get the video size
		ar2VideoInqSize(video, &xsize, &ysize);

		float fps = 0.0f;

#ifdef WIN32
		ar2VideoInqFreq(video, &fps);
#endif

		// report the actual
		osg::notify() << std::dec << "ARToolKitVideo::open() size of video " <<
			xsize << " x " << ysize << ", fps: " << fps << std::endl;

		m_config.selectedWidth = xsize;
		m_config.selectedHeight = ysize;
		m_config.selectedFrameRate = fps;

	}

	// create an image that same size (packing set to 1)
	this->allocateImage(xsize, ysize, 1, _format_GL, _datatype_GL, 1);

	this->setDataVariance(osg::Object::DYNAMIC);

	return true;

}

void
ARToolKitVideo::close(bool waitForThread)
{

	// This code was fenced for Windows - if you experience
	// problems with this code you are using an outdated
	// version of ARToolKit!
//#if !defined( WIN32 )
	if (NULL != video) {

		this->pause();
		int _ret = ar2VideoClose(video);

		if (0 == _ret) {
			video = NULL;
		}
	}
//#endif
}

void
ARToolKitVideo::play()
{
	if (video)
	{
		ar2VideoCapStart(video);
		osg::ImageStream::play();
	}
}

void
ARToolKitVideo::pause()
{
	if (video) {
		ar2VideoCapStop(video);

		osg::ImageStream::pause();
	}
}

void
ARToolKitVideo::update(osg::NodeVisitor* nv)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(this->getMutex());

	osg::Timer t; t.setStartTick();

	if (video && (0 == ar2VideoCapNext(video)))
	{
		unsigned char* newImage = static_cast<unsigned char*>(ar2VideoGetImage(video));

		if (newImage)
		{
			memcpy(this->data(),newImage, this->getImageSizeInBytes());

			this->dirty();

			if (nv) {

				const osg::FrameStamp *framestamp = nv->getFrameStamp();

				if (framestamp && _stats.valid())
				{
					_stats->setAttribute(framestamp->getFrameNumber(),
										 "Capture time taken", t.time_m());
				}
			}
		}
	}
}

osgART::VideoConfiguration*
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

// initializer for dynamic loading
osgART::PluginProxy<ARToolKitVideo> g_artoolkitvideo("osgart_video_artoolkit2");


