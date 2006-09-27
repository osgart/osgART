#include "osgART/MultiMarker"

namespace osgART {

	MultiMarker::MultiMarker() : Marker() {
	}

	MultiMarker::~MultiMarker() {    
	}

	/* virtual */
	Marker::MarkerType MultiMarker::getType() const {
		return Marker::ART_MULTI;
	}

	bool 
	MultiMarker::initialise(const std::string& multiFile) {

		// Check if multifile exists!!!
		m_multi = arMultiReadConfigFile(multiFile.c_str());
		if (m_multi == NULL) return false;
		
		setName(multiFile);
		setActive(false);
		
		return true;
	}

	void MultiMarker::setActive(bool a) {
		m_active = a;
		
		if (m_active) arMultiActivate(m_multi);
		else arMultiDeactivate(m_multi);
	}

	void 
	MultiMarker::update(ARMarkerInfo* markerInfo, int markerCount) 
	{
		m_valid = (arMultiGetTransMat(markerInfo, markerCount, m_multi) >= 0);
		updateTransform(m_multi->trans);
	}

};
