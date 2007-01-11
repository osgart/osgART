#include "osgART/GenericTracker"
#include "osgART/Marker"
#include "osgART/GenericVideo"

#include <iostream>


namespace osgART {

	int GenericTracker::trackerNum = 0;


	GenericTracker::GenericTracker() : osg::Referenced(),
		trackerId(GenericTracker::trackerNum++),
		//yannick
		m_name(""),
		m_version(""),
		m_imageptr(0L),
		m_width(-1),
		m_height(-1),
		m_conversionBuff(NULL),
		m_conversionBuffSize(0),
		m_arInternalFormat(osgART::VIDEOFORMAT_ANY),
		m_imageptr_format(osgART::VIDEOFORMAT_ANY)
		//=============
	{
		//Add by Yannick
		//Add fields
		m_fields["name"]	= new TypedField<std::string>(&m_name);
		m_fields["version"]	= new TypedField<std::string>(&m_version);
		m_fields["width"]	= new TypedField<int>(&m_width);
		m_fields["height"]	= new TypedField<int>(&m_height);

		m_fields["arInternalFormat"]= new TypedField<PixelFormatType>(&m_arInternalFormat);
		m_fields["imageptr_format"]	= new TypedField<PixelFormatType>(&m_imageptr_format);
		m_fields["conversionBuffSize"]	= new TypedField<int>(&m_conversionBuffSize);
		//===========================
	}

	GenericTracker::~GenericTracker() 
	{
		// hse25: delete markers
		m_markerlist.clear();		
	}


	int
	GenericTracker::getId()
	{
		return trackerId;
	}

	Marker* 
	GenericTracker::getMarker(int id) 
	{
		// hse25: Bounds checked!
		Marker *_m = (Marker*)0L;

		try {
			// get the marker with id (removed .at() method - not defined in STL
			 _m = m_markerlist[id].get();

		} catch(...) {

			// Debug message
			std::cerr << "No Marker with ID: " << id << std::endl;

		}

		// return the Marker
		return _m;
	}

	unsigned int 
	GenericTracker::getMarkerCount() const 
	{
		return (unsigned int)m_markerlist.size();
	}

	/*virtual*/ 
	void GenericTracker::createUndistortedMesh(int,int,
		float,float,osg::Geometry&)
	{
	}


	/*virtual*/ 
	void 
	GenericTracker::setImageRaw(unsigned char* grabbed_image,
		PixelFormatType format)
	{
		m_imageptr = grabbed_image;
		m_imageptr_format = format;
	}

	void 
	GenericTracker::setImage(GenericVideo* video)
	{
		if (video) this->setImageRaw(video->getImageRaw(),
			video->getPixelFormat(false));
	}


	const double* 
	GenericTracker::getProjectionMatrix() const 
	{
		return m_projectionMatrix;
	}

	//yannick
	char GenericTracker::getPixelSize(PixelFormatType format) const 
	{
		switch (format) 
		{
			case VIDEOFORMAT_RGB24:	
			case VIDEOFORMAT_BGR24:		return 3;
			case VIDEOFORMAT_BGRA32: 
			case VIDEOFORMAT_RGBA32: 
			case VIDEOFORMAT_ARGB32: 
			case VIDEOFORMAT_ABGR32:	return 4;
			case VIDEOFORMAT_YUV422:	return 2;
			case VIDEOFORMAT_Y8:
			case VIDEOFORMAT_GREY8:		return 1;
		}
		osg::notify() << "Error in GenericTracker::getPixelSize(), unknown pixel format" << std::endl;
		return 0;
	}

	/*virtual*/
	PixelFormatType GenericTracker::ConvertARTPixelFormatToOSGART(int format) const
	{return VIDEOFORMAT_ANY;};
	
    /*virtual*/
	int GenericTracker::ConvertOSGARTPixelFormatToART(PixelFormatType format) const
	{return 0;};
        //=======================


	void GenericTracker::PrintOptions() const {
		osg::notify() << "osgART::GenericTracker::PrintOptions() : No description available!" << std::endl;
	}
	
	std::string GenericTracker::getLabel() const {
		std::string Result = m_name;
		Result += "-";
		Result += m_version;
		return Result;
	}

	


	

};
