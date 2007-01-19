/*
 *	osgART/VideoTexRectCallback
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

#include "osgART/VideoTexRectCallback"
#include "osgART/TrackerManager"
#include "osgART/VideoManager"

#include <osg/Version>
#include <osg/Notify>

namespace osgART {

	VideoTexRectCallback::VideoTexRectCallback(VideoTextureBase* parent, int vw, int vh) :
		m_vidWidth(vw),
		m_vidHeight(vh),
		m_video(parent->getVideo()),
		m_videotexture(parent)
	{
	}

	/* virtual*/
	void VideoTexRectCallback::load(const osg::TextureRectangle&, 
		osg::State&) const {

		if (m_videotexture->getAlphaBias() >= 0.0f) 
			glPixelTransferf(GL_ALPHA_BIAS, m_videotexture->getAlphaBias());


		switch (m_video->pixelFormat()) {
		case VIDEOFORMAT_RGB24:
				glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, m_vidWidth, m_vidHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				break;
		case VIDEOFORMAT_BGR24:
				glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, m_vidWidth, m_vidHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
				break;
		case VIDEOFORMAT_RGBA32:
				glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				break;
		case VIDEOFORMAT_ABGR32:
	#ifdef GL_ABGR_EXT 
				glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, NULL);
	#endif
				break;
		case VIDEOFORMAT_BGRA32:
				glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
				break;
		case VIDEOFORMAT_ARGB32:
	#ifdef __BIG_ENDIAN__
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
	#else
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, NULL);
	#endif
				break;
		case VIDEOFORMAT_YUV422:
			osg::notify(osg::WARN) << "Due to an lack of support in OpenSceneGraph," 
				"AR_PIX_FORMAT_2vuy is not supported in osgART yet. " 
				"Use AR_PIX_FORMAT_ARGB instead." << std::endl;
			/*
#if defined(__APPLE__)
#ifdef __BIG_ENDIAN__
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, m_vidWidth, m_vidHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE, NULL);
#else
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, m_vidWidth, m_vidHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, NULL);
#endif
#endif
			*/
				break;
		default: std::cerr<<"ERROR:format not supported for texture mapping.."<<std::endl;
				break;
		}

		if (m_videotexture->getAlphaBias() >= 0.0f) 
			glPixelTransferf(GL_ALPHA_BIAS, 0.0f);

	}
	
	/* virtual */ 
	void VideoTexRectCallback::subload(const osg::TextureRectangle&, 
		osg::State&) const {

		//IWA
		unsigned char* frame = ((osgART::GenericVideo*)(m_video.get()))->getImageRaw();

		if (frame == 0L) return;

		if (m_videotexture->getAlphaBias() >= 0.0f) 
			glPixelTransferf(GL_ALPHA_BIAS, m_videotexture->getAlphaBias());

	switch (m_video->pixelFormat())
	{
	case VIDEOFORMAT_RGB24:
			glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, m_vidWidth, m_vidHeight, 
				GL_RGB, GL_UNSIGNED_BYTE, frame);
			break;
	case VIDEOFORMAT_BGR24:
			glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, m_vidWidth, m_vidHeight, 
				GL_BGR, GL_UNSIGNED_BYTE, frame);
			break;
	case VIDEOFORMAT_RGBA32:
			glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, m_vidWidth, m_vidHeight, 
				GL_RGBA, GL_UNSIGNED_BYTE, frame);	
			break;
	case VIDEOFORMAT_BGRA32:			
			glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, m_vidWidth, m_vidHeight, 
				GL_BGRA, GL_UNSIGNED_BYTE, frame);
			break;
	case VIDEOFORMAT_ARGB32:
#ifdef __BIG_ENDIAN__
			// glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, frame);
		glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, m_vidWidth, m_vidHeight, 
				GL_BGRA, GL_UNSIGNED_BYTE, frame);
#else
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, frame);
#endif
		break;
	case VIDEOFORMAT_YUV422:
#ifdef __APPLE__
		std::cerr<<"Due to lack of support in OpenSceneGraph, AR_PIX_FORMAT_2vuy is not supported in osgART yet. Use AR_PIX_FORMAT_ARGB instead.\n"<<std::endl;
/*
#ifdef __BIG_ENDIAN__
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, m_vidWidth, m_vidHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE, frame);
#else
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, m_vidWidth, m_vidHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, frame);
#endif
*/
#endif
			break;

#ifdef GL_ABGR_EXT 
	case VIDEOFORMAT_ABGR32:
			// glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_vidWidth, m_vidHeight, 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, frame);
			glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, m_vidWidth, m_vidHeight, 
				GL_RGBA, GL_UNSIGNED_BYTE, frame);
			break;
#endif
	default:
		osg::notify(osg::WARN) << "Warning: format not supported for texture mapping!" << std::endl;
			break;

	}


	if (m_videotexture->getAlphaBias() >= 0.0f) 
			glPixelTransferf(GL_ALPHA_BIAS, 0.0f);

	};


};
