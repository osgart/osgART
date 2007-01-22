/*
 *	osgART/GenericVideo
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

#include "osgART/GenericVideo"

#include "osgART/VideoManager"

#include <OpenThreads/ScopedLock>
#include <osg/Notify>

namespace osgART {

	// static id counter
	int GenericVideo::videoNum = 0;


	GenericVideo::GenericVideo() 
		: osg::Referenced(), 
		m_image(new osg::Image),
		xsize(-1), ysize(-1),
		pixelsize(4), 
		pixelformat(VIDEOFORMAT_RGB24),
		framerate(VIDEOFRAMERATE_30),
		m_video_id(GenericVideo::videoNum++)		
	{
	}

	GenericVideo::GenericVideo(const GenericVideo& video) : 
		osg::Referenced(),
		FieldContainer<GenericVideo>(), 
		m_image(video.m_image.get())
	{
	}

	GenericVideo::~GenericVideo()
	{	    
		osg::notify(osg::INFO) << "Shut down video" << std::endl;
	}

	GenericVideo& 
	GenericVideo::operator=(const GenericVideo &)
	{
		return *this;
	}

	int
	GenericVideo::getID() const
	{
		return m_video_id;
	}

	unsigned char*
	GenericVideo::getImageRaw() 
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);
		return m_image->data();
	}

	osg::ref_ptr<osg::Image> GenericVideo::getImage() const 
	{		
		return m_image;
	}

	void
	GenericVideo::setImage(osg::Image* image) 
	{
		m_image = image;
	}

	Field*
	GenericVideo::get(const std::string& name)
	{
		FieldMap::iterator _found = m_fields.find(name);
		// return 0 if the field is not existant
		return (_found != m_fields.end()) ? _found->second.get() : 0L;
	}

	/* virtual */
	VideoConfiguration* 
	GenericVideo::getVideoConfiguration()
	{
		return 0L;
	}

	

	VideoContainer::VideoContainer(GenericVideo* video) 
		: GenericVideo(*video),
		m_encapsulated(video)
	{
	}

	VideoContainer::VideoContainer(const VideoContainer& container,
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) :
		GenericVideo(*container.m_encapsulated.get())
	{
	}



	VideoConfiguration* VideoContainer::getVideoConfiguration() 
	{
		return (m_encapsulated.valid()) ? m_encapsulated->getVideoConfiguration() : 0L;
	}

	    
		
	void 
	VideoContainer::open() 
	{
		if (m_encapsulated.valid()) 
		{			
			m_encapsulated->open();	
			this->xsize = m_encapsulated->getWidth();
			this->ysize = m_encapsulated->getHeight();

			this->pixelformat = m_encapsulated->getPixelFormat(false);
			this->internalpixelformat = m_encapsulated->getPixelFormat(true);

			this->setImage(m_encapsulated->getImage().get());
		}
	}
		
	void
	VideoContainer::close() 
	{
		if (m_encapsulated.valid()) m_encapsulated->close();		
	}
		
	void
	VideoContainer::start()
	{
		if (m_encapsulated.valid()) m_encapsulated->start();		
	}
		

	void
	VideoContainer::stop() 
	{
		if (m_encapsulated.valid()) m_encapsulated->stop();		
	}
		
	void
	VideoContainer::update()
	{
		if (m_encapsulated.valid()) m_encapsulated->update();		
	}


	VideoContainer::~VideoContainer()
	{
	}

	void
	VideoContainer::releaseImage() 
	{
	}


	void
	VideoContainer::load(const std::string& plugin)
	{
		if (m_encapsulated.valid()) {
			// do we need to stop?
		}
        
		m_encapsulated = VideoManager::createVideoFromPlugin(plugin);

	}


	


};
