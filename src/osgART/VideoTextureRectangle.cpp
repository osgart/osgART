/*
 * osgART / AR Toolkit for OpenSceneGraph
 * (C) 2004-2006 HIT Lab NZ, University of Canterbury
 *
 * Licensing is governed by the LICENSE.txt which is 
 * part of this library distribution.
 *
 */

#include "osgART/VideoTextureRectangle"

#include "osgART/VideoManager"
#include "osgART/VideoTexRectCallback"
#include "osgART/VideoConfig"

#include <iostream>


namespace osgART {

	VideoTextureRectangle::VideoTextureRectangle(int video) {


		// Should check whether the video id is ok...!

		videoId = video;

		m_vidWidth = VideoManager::getInstance()->getVideo(videoId)->getWidth();
		m_vidHeight = VideoManager::getInstance()->getVideo(videoId)->getHeight();

		switch (VideoManager::getInstance()->getVideo(videoId)->pixelFormat())
		{
		case VIDEOFORMAT_RGB24:
				this->setInternalFormat(GL_RGB);
				break;
		case VIDEOFORMAT_BGR24:
				this->setInternalFormat(GL_RGB);
				break;
		case VIDEOFORMAT_RGB32:
				this->setInternalFormat(GL_RGBA);
				break;
		case VIDEOFORMAT_ABGR32:
				this->setInternalFormat(GL_RGBA);
				break;
		case VIDEOFORMAT_BGRA32:
				this->setInternalFormat(GL_RGBA);
				break;
		case VIDEOFORMAT_ARGB32:
				this->setInternalFormat(GL_RGBA);
				break;
		case VIDEOFORMAT_YUV422:
	#ifdef __APPLE__
	//# error Due to an unidentified issue, AR_PIX_FORMAT_2vuy is not supported in osgART yet. Use AR_PIX_FORMAT_ARGB instead.\n
			this->setInternalFormat(GL_RGB);
	#endif
				break;
		default: std::cerr<<"ERROR:format not supported for texture mapping.."<<std::endl;
		}

		this->setTextureSize(m_vidWidth, m_vidHeight);
		this->setFilter(osg::TextureRectangle::MIN_FILTER, osg::TextureRectangle::LINEAR);
		this->setFilter(osg::TextureRectangle::MAG_FILTER, osg::TextureRectangle::LINEAR);
		this->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::CLAMP);
		this->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::CLAMP);
		this->setSubloadCallback(new VideoTexRectCallback(videoId, m_vidWidth, m_vidHeight));

		
	}

	VideoTextureRectangle::~VideoTextureRectangle(void)
	{
	    
	}

}