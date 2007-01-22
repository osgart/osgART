/*
 *	osgART/GenericVideoObject
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

#include "osgART/GenericVideoObject"

#if defined(_WIN32)
#include <windows.h>
#endif


#if !defined(_MSC_VER) || !defined(_X86_)
#  include <cmath>
#endif


namespace osgART {

	/*static*/
	unsigned int GenericVideoObject::mathNextPowerOf2(unsigned int x)
	{
	#if defined(_MSC_VER) && defined(_X86_)
		unsigned int result = 0;
		_asm   bsr  ecx,x;      // Find the base 2 logarithm
		_asm   inc  ecx;        // Increase it by 1
		_asm   mov  eax,1;
		_asm   shl  eax,cl;     // Find the antilogarithm
		_asm   mov  result,eax;
		return result;          // Return the result
	#else
		return ((unsigned int)(exp2((double)((int)(log2((double)x)) + 1))));
	#endif
	}

	GenericVideoObject::GenericVideoObject(GenericVideo* video /* = 0L */) :
		osg::Group(),
		m_vShader(0L),
		m_vTexture(0L),
		m_video(video),
		m_distortionMode(NO_CORRECTION),
		m_textureMode(USE_TEXTURE_RECTANGLE),
		m_videoimage(new osg::Image)
	{
	}

	GenericVideoObject::~GenericVideoObject()
	{
	}

	void 
	GenericVideoObject::setVideo(GenericVideo* video)
	{
		this->m_video = video;
	}

	GenericVideo*
	GenericVideoObject::getVideo() const 
	{
		return this->m_video.get();
	}

}; // namespace osgART
