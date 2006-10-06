#include "osgART/GenericVideo"

#include "OpenThreads/ScopedLock"

namespace osgART {


	// static id counter
	int GenericVideo::videoNum = 0;


	GenericVideo::GenericVideo() 
		: osg::Referenced(), 
		xsize(-1), ysize(-1),
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
		return m_image->data();
	}

	osg::ref_ptr<osg::Image> GenericVideo::getImage() const {		
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
		return (_found != m_fields.end()) ? _found->second.get() : 0L;
	}


};
