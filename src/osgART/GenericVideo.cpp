#include "osgART/GenericVideo"

#include "OpenThreads/ScopedLock"

namespace osgART {


	// static id counter
	int GenericVideo::videoNum = 0;


	GenericVideo::GenericVideo() 
		: osg::Referenced(), 
		xsize(-1), ysize(-1),
		image(0L),
		pixelsize(4), 
		videoId(GenericVideo::videoNum++),
		pixelformat(VIDEOFORMAT_RGB24),
		framerate(VIDEOFRAMERATE_30),
		m_image(new osg::Image)
	{
	}

	GenericVideo::GenericVideo(const GenericVideo &)
	{	    
	}

	GenericVideo::~GenericVideo(void)
	{	    
	}

	GenericVideo& 
	GenericVideo::operator=(const GenericVideo &)
	{
		return *this;
	}

	int
	GenericVideo::getId() const
	{
		return videoId;
	}

	unsigned char*
	GenericVideo::getImageRaw() {
		OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);
		return image; // this->getImage()->data();
	}

	osg::Image* GenericVideo::getImage() {		
		return m_image.get();
	}

};
