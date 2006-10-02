#include "SingleMarker"

namespace osgART {

	SingleMarker::SingleMarker() : Marker() 	{
	}

	SingleMarker::~SingleMarker() {
		// jcl64: Free the pattern
		if (patt_id > 0) arFreePatt(patt_id);
	}

	Marker::MarkerType SingleMarker::getType() const {
		return Marker::ART_SINGLE;
	}

	bool SingleMarker::initialise(const std::string& pattFile, double width, double center[2]) {
		patt_id = arLoadPatt(pattFile.c_str());
		if (patt_id < 0) return false;
		patt_width = width;
		patt_center[0] = center[0];
		patt_center[1] = center[1];
		setName(pattFile);
		setActive(false);
		return true;
	}

	void SingleMarker::update(ARMarkerInfo* markerInfo) {
		
		if (markerInfo == NULL) {
			m_valid = false;
		} else {
			//arGetTransMatCont(markerInfo, patt_trans, patt_center, patt_width, patt_trans);
			arGetTransMat(markerInfo, patt_center, patt_width, patt_trans);
			
			m_valid = true;

		}

		updateTransform(patt_trans);

	}

	void SingleMarker::setActive(bool a) {
		m_active = a;
		
		if (m_active) arActivatePatt(patt_id);
		else arDeactivatePatt(patt_id);

	}

	int SingleMarker::getPatternID() {
		return patt_id;
	}

	double SingleMarker::getPatternWidth() {
		return patt_width;
	}
		
	double* SingleMarker::getPatternCenter() {
		return patt_center;
	}
};