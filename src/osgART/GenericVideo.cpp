#include "osgART/GenericVideo"

#include "OpenThreads/ScopedLock"
//Yannick 31/10/06, to add take snapshot()
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Image>
//===============================
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
		//update by yannick
		if (m_image.valid())
			return m_image->data();
		else
			return NULL;
		//==================
	}

	osg::ref_ptr<osg::Image> GenericVideo::getImage() const {		
		return m_image;
	}

	void
	GenericVideo::setImage(osg::Image* image) 
	{
		m_image = image;
	}

//Yannick 31/10/06===	
	bool 
	GenericVideo::takeSnapShot(const std::string & filename)
	{
		if (!m_image.valid())
			osg::notify(osg::WARN) << "GenericVideo::takeSnapShot(): Image object is not valid" << std::endl;
		else if (filename == "")
			osg::notify(osg::WARN) << "GenericVideo::takeSnapShot(): FileName is empty" << std::endl;	
		else if (!m_image->data())
			osg::notify(osg::WARN) << "GenericVideo::takeSnapShot(): Image object has no data" << std::endl;
		else
		{
			m_image->flipVertical();
			return osgDB::writeImageFile(*m_image,  filename);
		}
		return false;
	}
//yannick============

	Field*
	GenericVideo::get(const std::string& name)
	{
		FieldMap::iterator _found = m_fields.find(name);
		return (_found != m_fields.end()) ? _found->second.get() : 0L;
	}


};
