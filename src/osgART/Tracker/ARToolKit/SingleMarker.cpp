/*
 *	osgART/Tracker/ARToolKit/SingleMarker
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

#include "SingleMarker"

#include <AR/gsub_lite.h>

namespace osgART {

	SingleMarker::SingleMarker() : Marker(),
		patt_id(-1)
	{
			m_fields["confidence"] = new TypedField<double>(&m_confidence);
	}

	SingleMarker::~SingleMarker()
	{
		// jcl64: Free the pattern
		if (patt_id >= 0) arFreePatt(patt_id);
		patt_id = -1;
	}

	Marker::MarkerType SingleMarker::getType() const
	{
		return Marker::ART_SINGLE;
	}

	bool SingleMarker::initialise(const std::string& pattFile, double width, double center[2])
	{
		if (patt_id >= 0) return (false);
		patt_id = arLoadPatt(pattFile.c_str());
		if (patt_id < 0) return false;
		patt_width = width;
		patt_center[0] = center[0];
		patt_center[1] = center[1];
		setName(pattFile);
		setActive(false);
		return true;
	}

	void SingleMarker::update(ARMarkerInfo* markerInfo)
	{
		if (markerInfo == 0L) {
			m_valid = false;
			// We won't call update in the parent class if marker is not
			// valid.
		} else {
			m_valid = true;
			//arGetTransMatCont(markerInfo, patt_trans, patt_center, patt_width, patt_trans);
			arGetTransMat(markerInfo, patt_center, patt_width, patt_trans);
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
		
		if (m_active) arActivatePatt(patt_id);
		else arDeactivatePatt(patt_id);

	}

	int SingleMarker::getPatternID()
	{
		return patt_id;
	}

	double SingleMarker::getPatternWidth()
	{
		return patt_width;
	}
		
	double* SingleMarker::getPatternCenter()
	{
		return patt_center;
	}
};
