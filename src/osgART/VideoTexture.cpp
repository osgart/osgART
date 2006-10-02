///////////////////////////////////////////////////////////////////////////////
// File name : VideoTexture.C
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
//	O ??
//	/
//	X
//
// History :
//	YYY : Mr Grasset : Creation of the file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include file
///////////////////////////////////////////////////////////////////////////////

#include <osgART/VideoManager>
#include <osgART/VideoTexture>
#include <osgART/VideoTexCallback>
#include <osgART/VideoBackground>


// using namespace std;
namespace osgART {

	VideoTexture::VideoTexture(int video) {


		this->setDataVariance(osg::Object::DYNAMIC);

		videoId = video;

		m_vidWidth = VideoManager::getInstance()->getVideo(videoId)->getWidth();
		m_vidHeight = VideoManager::getInstance()->getVideo(videoId)->getHeight();

		m_texWidth = GenericVideoObject::mathNextPowerOf2(m_vidWidth);
		m_texHeight = GenericVideoObject::mathNextPowerOf2(m_vidHeight);

		switch (VideoManager::getInstance()->getVideo(videoId)->pixelFormat())
		{
		case VIDEOFORMAT_RGB24:
				this->setInternalFormat(GL_RGB);
				break;
		case VIDEOFORMAT_BGR24:
				this->setInternalFormat(GL_RGB);
				break;
		case VIDEOFORMAT_RGBA32:
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
		default: 
			std::cerr<<"ERROR:format not supported for texture mapping.."<<std::endl;
			break;
		}

		this->setTextureSize(m_texWidth,m_texHeight);
		this->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		this->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		this->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP);
		this->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP);

		this->setSubloadCallback(new VideoTexCallback(videoId, 
			m_vidWidth, m_vidHeight, m_texWidth, m_texHeight));

	}

	VideoTexture::~VideoTexture(void)
	{
	    
	};

};