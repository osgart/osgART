#include "SingleMarker"

#include <AR/gsub_lite.h>


namespace osgART {

	SingleMarker::SingleMarker() : Marker(),
		m_patt_id(-1)
	{
			m_fields["confidence"] = new TypedField<double>(&m_confidence);
			m_fields["patt_id"] = new TypedField<int>(&m_patt_id);
	}

	SingleMarker::~SingleMarker()
	{
		// jcl64: Free the pattern
		if (m_patt_id >= 0) arFreePatt(m_patt_id);
		m_patt_id = -1;
	}

	Marker::MarkerType SingleMarker::getType() const
	{
		return Marker::ART_SINGLE;
	}

	bool SingleMarker::initialise(const std::string& pattFile, double width, double center[2])
	{
		if (m_patt_id >= 0) return (false);
		m_patt_id = arLoadPatt(pattFile.c_str());
		if (m_patt_id < 0) return false;
		m_patt_width = width;
		m_patt_center[0] = center[0];
		m_patt_center[1] = center[1];
		setName(pattFile);
		setActive(false);
		return true;
	}

	void SingleMarker::update(ARMarkerInfo* markerInfo)
	{
		if (markerInfo == NULL) {
			m_valid = false;
			m_seen = false;
		} else {
			m_valid = true;
			//arGetTransMatCont(markerInfo, patt_trans, patt_center, patt_width, patt_trans);
			arGetTransMat(markerInfo, m_patt_center, m_patt_width, m_patt_trans);
			m_confidence = markerInfo->cf;
			double modelView[16];
			arglCameraViewRH(m_patt_trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			//osgART::PrintMatrix("ARToolkit simple marker Matrix :", tmp);
			updateTransform(tmp);
		}
	}

	void SingleMarker::setActive(bool a)
	{
		m_active = a;
		
		if (m_active) arActivatePatt(m_patt_id);
		else arDeactivatePatt(m_patt_id);

	}

	int SingleMarker::getPatternID()
	{
		return m_patt_id;
	}

	double SingleMarker::getPatternWidth()
	{
		return m_patt_width;
	}
		
	double* SingleMarker::getPatternCenter()
	{
		return m_patt_center;
	}
};
