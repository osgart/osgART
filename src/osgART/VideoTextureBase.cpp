/*
 *	osgART/VideoTextureBase
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

#include <osgART/VideoTextureBase>
#include <osg/Notify>


// using namespace std;
namespace osgART {

	VideoTextureBase::VideoTextureBase(GenericVideo* video) : 
		m_alphabias(-1.), 
		m_video(video) 
	{		
	}

	void 
	VideoTextureBase::setAlphaBias(float alphabias) 
	{
		m_alphabias = alphabias;
	}

	float
	VideoTextureBase::getAlphaBias() const 
	{
		return m_alphabias;
	}

	void 
	VideoTextureBase::setVideo(GenericVideo* video)
	{
		m_video = video;
	}

	GenericVideo*
	VideoTextureBase::getVideo() const 
	{
		return (GenericVideo*)(m_video.get());
	}

	VideoTextureBase::~VideoTextureBase()
	{	    
	}

};
