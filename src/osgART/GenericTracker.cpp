#include "osgART/GenericTracker"
#include "osgART/Marker"
#include "osgART/GenericVideo"

#include <AR/gsub_lite.h>
#include <iostream>

namespace osgART {

	int GenericTracker::trackerNum = 0;


	GenericTracker::GenericTracker() : 
		trackerId(GenericTracker::trackerNum++),
		patternNum(0),
		image(0)
	{
	}

	GenericTracker::~GenericTracker(void)
	{
		// Should delete list of markers...
	}
	int
	GenericTracker::getId()
	{
		return trackerId;
	}

	Marker* GenericTracker::getMarker(int id) 
	{
		// hse25: Bounds checked!
		Marker *_m = (Marker*)0L;

		try {
			// get the marker with id (removed .at() method - not defined in STL
			 _m = pattList[id];

		} catch(...) {

			// Debug message
			std::cerr << "No Marker with ID: " << id << std::endl;

		}

		// return the Marker
		return _m;
	}

	const CameraParameter& GenericTracker::getIntrinsicParameters() const
	{
		return cparam;
	}

	unsigned int GenericTracker::getMarkerCount() {
		return pattList.size();
	}


	void GenericTracker::setImageRaw(unsigned char* grabbed_image)
	{
		image = grabbed_image;
	}

	void GenericTracker::setImage(GenericVideo* video)
	{
		if (video) this->setImageRaw(video->getImageRaw());
	}


	const double* GenericTracker::getProjectionMatrix() const 
	{
		return m_projectionMatrix;
	}



};
