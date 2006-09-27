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

	GenericVideoObject::GenericVideoObject(int _videoId) :
		osg::Group(),
		m_videoId(_videoId),
		m_vShader(0L),
		m_vTexture(0L),
		m_distortionMode(NO_CORRECTION),
		m_textureMode(USE_TEXTURE_RECTANGLE),
		m_videoimage(new osg::Image)
	{
	}

	GenericVideoObject::~GenericVideoObject()
	{
	}

}; // namespace osgART
