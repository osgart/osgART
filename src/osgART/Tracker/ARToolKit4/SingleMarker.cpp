#include "SingleMarker"

#include <AR/gsub_lite.h>

namespace osgART {

	SingleMarker::SingleMarker() : Marker(),
		patt_id(-1),
		m_arPattHandle(NULL)
	{
			m_fields["confidence"] = new TypedField<double>(&m_confidence);
	}

	SingleMarker::~SingleMarker()
	{
		// jcl64: Free the pattern
		if (patt_id >= 0) arPattFree(m_arPattHandle, patt_id);
		patt_id = -1;
		m_arPattHandle = NULL;
	}

	Marker::MarkerType SingleMarker::getType() const
	{
		return Marker::ART_SINGLE;
	}

	bool SingleMarker::initialise(ARPattHandle *pattHandle, const std::string& pattFile, double width)
	{
		if (patt_id >= 0) return (false);
		if (m_arPattHandle) return (false);
		patt_id = arPattLoad(pattHandle, pattFile.c_str());
		if (patt_id < 0) return false;
		patt_width = width;
		setName(pattFile);
		setActive(false);
		m_arPattHandle = pattHandle;
		return true;
	}

	void SingleMarker::update(AR3DHandle *handle, ARMarkerInfo* markerInfo)
	{
		double err;

		if (markerInfo == NULL) {
			m_valid = false;
			m_seen = false;
		} else {
			m_valid = true;
			err = arGetTransMatSquare(handle, markerInfo, patt_width, patt_trans);
			m_confidence = markerInfo->cf;
			double modelView[16];
			arglCameraViewRH(patt_trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			updateTransform(tmp);
		}
	}

	void SingleMarker::setActive(bool a)
	{
		m_active = a;
		
		if (m_active) arPattActivate(m_arPattHandle, patt_id);
		else arPattDeactivate(m_arPattHandle, patt_id);
	}

	int SingleMarker::getPatternID()
	{
		return patt_id;
	}

	double SingleMarker::getPatternWidth()
	{
		return patt_width;
	}
		
};