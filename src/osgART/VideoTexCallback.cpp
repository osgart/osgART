/*
 *	osgART/VideoTexCallback
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

#include "osgART/VideoTexCallback"

#include "osgART/VideoManager"
#include "osgART/TrackerManager"

#include <osg/Notify>

#include <iostream>

namespace osgART {

	VideoTexCallback::VideoTexCallback(VideoTextureBase* videotexture,
		int vw, int vh, int tw, int th) :
		m_vidWidth(vw),
		m_vidHeight(vh),
		m_texWidth(tw),
		m_texHeight(th),
		m_video(videotexture->getVideo()),
		m_videotexture(videotexture)
	{
	}
	
	/*virtual*/ 
	void 
	VideoTexCallback::load(const osg::Texture2D&, osg::State&) const {

		if (m_videotexture->getAlphaBias() >= 0.0f) 
			glPixelTransferf(GL_ALPHA_BIAS, m_videotexture->getAlphaBias());


	switch (m_video->pixelFormat())
	{
		case VIDEOFORMAT_RGB24:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			break;
		case VIDEOFORMAT_BGR24:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
			break;
		case VIDEOFORMAT_RGBA32:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texWidth, m_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			break;
		case VIDEOFORMAT_ABGR32:
#ifdef GL_ABGR_EXT 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texWidth, m_texHeight, 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, NULL);
#endif
			break;
		case VIDEOFORMAT_BGRA32:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texWidth, m_texHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);			
			break;
		case VIDEOFORMAT_ARGB32:
#ifdef __BIG_ENDIAN__
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texWidth, m_texHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texWidth, m_texHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, NULL);
#endif
			break;
		case VIDEOFORMAT_YUV422:
			osg::notify(osg::WARN) << "Due to lack of support in OpenSceneGraph, AR_PIX_FORMAT_2vuy is not supported in osgART yet. Use AR_PIX_FORMAT_ARGB instead.\n"<<std::endl;
			/*
#ifdef __APPLE__
#ifdef __BIG_ENDIAN__
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE, NULL);
#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, NULL);
#endif
#endif
			 */
			break;
		default: 
			osg::notify(osg::WARN) << "VideoTexCallback::VideoTexCallback(): "
				"format not supported for texture mapping.." << std::endl;

			break;
		}
		
		if (m_videotexture->getAlphaBias() >= 0.0f) 
			glPixelTransferf(GL_ALPHA_BIAS, 0.0f);
	}
	
	/*virtual */ 
	void 
	VideoTexCallback::subload(const osg::Texture2D&, osg::State&) const {
	
		//IWA
		unsigned char* frame = ((osgART::GenericVideo*)(m_video.get()))->getImageRaw();

		if (frame == 0L) return;

		if (m_videotexture->getAlphaBias() >= 0.0f) 
			glPixelTransferf(GL_ALPHA_BIAS, m_videotexture->getAlphaBias());

	switch (m_video->pixelFormat())
	{
		case VIDEOFORMAT_RGB24:
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_RGB, GL_UNSIGNED_BYTE, frame);
			break;
		case VIDEOFORMAT_BGR24:			
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_BGR, GL_UNSIGNED_BYTE, frame);
			break;
		case VIDEOFORMAT_RGBA32:
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_RGBA, GL_UNSIGNED_BYTE, frame);
			break;
		case VIDEOFORMAT_ABGR32:
#ifdef GL_ABGR_EXT
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, frame);
			// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_ABGR_EXT, GL_UNSIGNED_BYTE, frame);
#endif
			break;
		case VIDEOFORMAT_BGRA32:
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, 
				GL_BGRA, GL_UNSIGNED_BYTE, frame);
			break;
		case VIDEOFORMAT_ARGB32:
#ifdef __BIG_ENDIAN__
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, frame);
			// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, frame);
#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, frame);
			// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, frame);
#endif
			break;
		case VIDEOFORMAT_YUV422:
			osg::notify(osg::WARN) << "Due to lack of support in OpenSceneGraph, AR_PIX_FORMAT_2vuy is not supported in osgART yet. Use AR_PIX_FORMAT_ARGB instead.\n"<<std::endl;
			/*
#ifdef __APPLE__
#ifdef __BIG_ENDIAN__
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_vidWidth, m_vidHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE, frame);
			// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE, frame);
#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_vidWidth, m_vidHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, frame);
			// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, frame);
#endif
#endif
			 */
			break;
	

		default: 
			osg::notify(osg::WARN) << "osgART::VideoTexCallback::subload() format "
				"not supported for texture mapping!" << std::endl;

			break;
	}
		if (m_videotexture->getAlphaBias() >= 0.0f) 
			glPixelTransferf(GL_ALPHA_BIAS, 0.0f);

	}
};
