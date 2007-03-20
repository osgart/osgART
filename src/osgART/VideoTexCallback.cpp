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
			osg::notify(osg::NOTICE) << m_videotexture->getVideo()->getWidth() <<
			"x" << m_videotexture->getVideo()->getHeight() << " : "
			<< std::endl;
	}
	
	/*virtual*/ 
	void 
	VideoTexCallback::load(const osg::Texture2D&, osg::State&) const {

		GLenum internalformat_GL;
		GLenum format_GL;
		GLenum type_GL;
		
		if (m_videotexture->getAlphaBias() >= 0.0f) glPixelTransferf(GL_ALPHA_BIAS, m_videotexture->getAlphaBias());

		m_video->getGLPixelFormat(&internalformat_GL, &format_GL, &type_GL);
		
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat_GL, m_texWidth, m_texHeight, 0, format_GL, type_GL, NULL);
		
		if (m_videotexture->getAlphaBias() >= 0.0f) glPixelTransferf(GL_ALPHA_BIAS, 0.0f);
	}
	
	/*virtual */ 
	void 
	VideoTexCallback::subload(const osg::Texture2D&, osg::State&) const {
	
		GLenum internalformat_GL;
		GLenum format_GL;
		GLenum type_GL;

		//IWA
		unsigned char* frame = m_video->getImageRaw();

		if (frame == 0L) return;

		if (m_videotexture->getAlphaBias() >= 0.0f) glPixelTransferf(GL_ALPHA_BIAS, m_videotexture->getAlphaBias());

		m_video->getGLPixelFormat(&internalformat_GL, &format_GL, &type_GL);
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_vidWidth, m_vidHeight, format_GL, type_GL, frame);

		if (m_videotexture->getAlphaBias() >= 0.0f) glPixelTransferf(GL_ALPHA_BIAS, 0.0f);
	}
};
