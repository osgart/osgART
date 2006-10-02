#include "osgART/VideoTexCallback"

#include "osgART/VideoManager"
#include "osgART/TrackerManager"


#include <iostream>

namespace osgART {

	VideoTexCallback::VideoTexCallback(int video,int vw, int vh, int tw, int th) :
		m_alphabias(1.0f),
		videoId(video),
		m_vidWidth(vw),
		m_vidHeight(vh),
		m_texWidth(tw),
		m_texHeight(th)	
	{
	}
	
	void VideoTexCallback::setAlphaBias(float alphabias) {
		m_alphabias = osg::clampBetween<float>(alphabias,0.0f,1.0f);
	}
	float VideoTexCallback::getAlphaBias() const {
		return m_alphabias;
	}

	/*virtual*/ 
	void VideoTexCallback::load(const osg::Texture2D& texture, osg::State& state) const {

#ifdef OSGART_PIXELBIAS_NO_PERFORMANCE
		glPixelTransferf(GL_ALPHA_BIAS, 1.0f);
#endif

	switch (VideoManager::getInstance()->getVideo(videoId)->pixelFormat())
	{
		case VIDEOFORMAT_RGB24:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			break;
		case VIDEOFORMAT_BGR24:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
			break;
		case VIDEOFORMAT_RGB32:
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
			std::cerr<<"Due to an unidentified issue, AR_PIX_FORMAT_2vuy is not supported in osgART yet. Use AR_PIX_FORMAT_ARGB instead.\n"<<std::endl;
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
			default: std::cerr<<"ERROR:format not supported for texture mapping.."<<std::endl;
			 break;
		}
#ifdef OSGART_PIXELBIAS_NO_PERFORMANCE
		glPixelTransferf(GL_ALPHA_BIAS, 0.0f);
#endif
	}
	
	/*virtual */ 
	void 
	VideoTexCallback::subload(const osg::Texture2D& texture, osg::State& state) const {
	
		unsigned char* frame = VideoManager::getInstance()->getVideo(videoId)->getImageRaw();

		if (frame == NULL) return;

#ifdef OSGART_PIXELBIAS_NO_PERFORMANCE
		glPixelTransferf(GL_ALPHA_BIAS, 1.0f);
#endif

	switch (VideoManager::getInstance()->getVideo(videoId)->pixelFormat())
	{
		case VIDEOFORMAT_RGB24:
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_RGB, GL_UNSIGNED_BYTE, frame);
			break;
		case VIDEOFORMAT_BGR24:			
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, GL_BGR, GL_UNSIGNED_BYTE, frame);
			break;
		case VIDEOFORMAT_RGB32:
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
			std::cerr<<"Due to an unidentified issue, AR_PIX_FORMAT_2vuy is not supported in osgART yet. Use AR_PIX_FORMAT_ARGB instead.\n"<<std::endl;
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
	
		default: std::cerr<<"ERROR:format not supported for texture mapping.."<<std::endl;
			break;
	}
#ifdef OSGART_PIXELBIAS_NO_PERFORMANCE
		glPixelTransferf(GL_ALPHA_BIAS, 0.0f);
#endif

	}
};
