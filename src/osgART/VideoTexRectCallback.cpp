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
		osg::notify(osg::NOTICE) << m_videotexture->getVideo()->getWidth() <<
			"x" << m_videotexture->getVideo()->getHeight() << " : "
			<< std::endl;
	}

	/* virtual*/
	void VideoTexRectCallback::load(const osg::TextureRectangle&, 
		osg::State&) const 
	{

		GLenum internalformat_GL;
		GLenum format_GL;
		GLenum type_GL;

		if (m_videotexture->getAlphaBias() >= 0.0f) glPixelTransferf(GL_ALPHA_BIAS, m_videotexture->getAlphaBias());

		m_video->getGLPixelFormat(&internalformat_GL, &format_GL, &type_GL);

		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, internalformat_GL, m_vidWidth, m_vidHeight, 0, format_GL, type_GL, NULL);

		if (m_videotexture->getAlphaBias() >= 0.0f) glPixelTransferf(GL_ALPHA_BIAS, 0.0f);
	}
	
	/* virtual */ 
	void VideoTexRectCallback::subload(const osg::TextureRectangle&, 
		osg::State&) const
	{
		GLenum internalformat_GL;
		GLenum format_GL;
		GLenum type_GL;
		
		//IWA
		unsigned char* frame = m_video->getImageRaw();

		if (frame == 0L) return;

		if (m_videotexture->getAlphaBias() >= 0.0f) glPixelTransferf(GL_ALPHA_BIAS, m_videotexture->getAlphaBias());

		m_video->getGLPixelFormat(&internalformat_GL, &format_GL, &type_GL);

		glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, m_vidWidth, m_vidHeight, format_GL, type_GL, frame);

		if (m_videotexture->getAlphaBias() >= 0.0f) glPixelTransferf(GL_ALPHA_BIAS, 0.0f);
	};


};
