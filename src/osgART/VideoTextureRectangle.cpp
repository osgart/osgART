/*
 *	osgART/VideoTextureRectangle
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

#include "osgART/VideoTextureRectangle"
#include "osgART/VideoManager"
#include "osgART/VideoTexRectCallback"
#include "osgART/VideoConfig"

#include <osg/Notify>

#include <iostream>


namespace osgART {


	VideoTextureRectangle::VideoTextureRectangle(GenericVideo* video) 
		: VideoTextureBase(video)
	{

		m_vidWidth = video->getWidth();
		m_vidHeight = video->getHeight();

		this->setTextureSize(m_vidWidth, m_vidHeight);
		this->setFilter(osg::TextureRectangle::MIN_FILTER, osg::TextureRectangle::LINEAR);
		this->setFilter(osg::TextureRectangle::MAG_FILTER, osg::TextureRectangle::LINEAR);
		this->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::CLAMP);
		this->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::CLAMP);

		m_callback = new VideoTexRectCallback(this, m_vidWidth, m_vidHeight);

		this->setSubloadCallback(m_callback.get());
		
	}

	VideoTextureRectangle::~VideoTextureRectangle(void)
	{	    
	}

}
