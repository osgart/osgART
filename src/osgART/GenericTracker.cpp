#include "osgART/GenericTracker"
#include "osgART/Marker"
#include "osgART/GenericVideo"

#include <AR/gsub_lite.h>


#include <iostream>


namespace osgART {

	int GenericTracker::trackerNum = 0;


	GenericTracker::GenericTracker() : osg::Referenced(),
		trackerId(GenericTracker::trackerNum++),
		m_imageptr(0L)
	{
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

	const CameraParameter& 
	GenericTracker::getIntrinsicParameters() const
	{
		return cparam;
	}

	unsigned int 
	GenericTracker::getMarkerCount() const 
	{
		return (unsigned int)m_markerlist.size();
	}


	void 
	GenericTracker::setImageRaw(unsigned char* grabbed_image,
		PixelFormatType format /*= VIDEOFORMAT_BGRA32*/)
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

};
