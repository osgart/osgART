/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

// std include

// OpenThreads include

// OSG include

// local include
#include "osgART/Utils"



#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif


#if !defined(_MSC_VER) || !defined(_X86_)
#  include <cmath>
#endif
namespace osgART {

	unsigned int equalOrGreaterPowerOfTwo(unsigned int x)
	{
		// First check if it is a power of two...
		if ((x & (x-1))==0) return x;

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

	osg::Vec2 getTextureMaxUV(const osg::Texture* texture)
	{
		osg::Vec2 ret(1.0,1.0);

		const osg::Texture2D *tx2d = dynamic_cast<const osg::Texture2D*>(texture);
		const osg::TextureRectangle *txrec = dynamic_cast<const osg::TextureRectangle*>(texture);
		if (tx2d)
		{
			ret._v[0] = tx2d->getImage()->s() / (float)equalOrGreaterPowerOfTwo((unsigned int)tx2d->getImage()->s());
			ret._v[1] = tx2d->getImage()->t() / (float)equalOrGreaterPowerOfTwo((unsigned int)tx2d->getImage()->t());			
		} else if (txrec)
		{
			ret._v[0] = txrec->getImage()->s();
			ret._v[1] = txrec->getImage()->t();
		}

		return ret;
	}

	std::string trim(std::string& s,const std::string& drop/* = " "*/)
	{
		std::string r=s.erase(s.find_last_not_of(drop)+1);
		return r.erase(0,r.find_first_not_of(drop));
	}

	std::vector<std::string> tokenize(const std::string& str, const std::string& delimiters)
	{
		std::vector<std::string> tokens;
	    	
		// skip delimiters at beginning.
		std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	    	
		// find first "non-delimiter".
		std::string::size_type pos = str.find_first_of(delimiters, lastPos);

		while (std::string::npos != pos || std::string::npos != lastPos)
    	{
        	// found a token, add it to the vector.
        	tokens.push_back(str.substr(lastPos, pos - lastPos));
		
        	// skip delimiters.  Note the "not_of"
        	lastPos = str.find_first_not_of(delimiters, pos);
		
        	// find next "non-delimiter"
        	pos = str.find_first_of(delimiters, lastPos);
    	}

		return tokens;
	}
	

	osg::Matrix GenerateMatrix(const osg::Vec3d & _trans, const osg::Vec3d & _rot)
	{
		osg::Matrix tempmatt;
		tempmatt = osg::Matrixd::rotate(
			_rot[0], osg::Vec3f(1,0,0),
			_rot[1], osg::Vec3f(0,1,0),
			_rot[2], osg::Vec3f(0,0,1));

		tempmatt *= osg::Matrixd::translate(_trans);
		return tempmatt;
	}

	osg::Matrix DiffMatrix(const osg::Matrix & _base, const osg::Matrix & _m2)
	{
		osg::Matrix Base;
		Base.invert(_base);
		return _m2 * Base;
	}



};//namespace osgART
