#include <osgART/ARTTransform>
#include <osgART/ARTTransformCallback>
#include <osgART/GenericTracker>
#include <osgART/TrackerManager>
#include <osgART/Marker>

namespace osgART {

	ARTTransform::ARTTransform(int markerID,int trackerId): osg::MatrixTransform()
	{
		m_marker = TrackerManager::getInstance()->getTracker(trackerId)->getMarker(markerID);
		if (m_marker) {
			this->setUpdateCallback(new ARTTransformCallback(m_marker));
		}
	}

	ARTTransform::~ARTTransform() {
	    
	}

	ARTTransform& 
	ARTTransform::operator=(const ARTTransform &)
	{
		return *this;
	}


	Marker* ARTTransform::getMarker() {
		return m_marker;
	}

};
