/*
 *	osgART/Tracker/ARToolKit4/SingleMarker
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
//#include "ar/ar.h"

//#include <SG_TLS_exceptions.h>

namespace osgART {

	SingleMarker::SingleMarker(ARPattHandle  *_PattHdle) : Marker(),
		AR4_PattList(_PattHdle),
		patt_id(-1),
		patt_width(80),
		m_confidence(0),
		arHandle(NULL)
	{
			m_fields["confidence"] = new TypedField<double>(&m_confidence);
	}

	SingleMarker::~SingleMarker()
	{
		// jcl64: Free the pattern
		if (patt_id > 0)// arFreePatt(patt_id);
			arPattFree(AR4_PattList,patt_id );
		patt_id = -1;
		arHandle = NULL;
	}

	Marker::MarkerType SingleMarker::getType() const
	{
		return Marker::ART_SINGLE;
	}

	bool SingleMarker::initialise(ARHandle		*_arHandle,const std::string& pattFile, double width, double center[2])
	{
		if (!_arHandle)	return false;
		if (patt_id >= 0) return (false);
		if (arHandle) return (false);

		
		patt_id = arPattLoad(AR4_PattList, pattFile.c_str());
		if (patt_id < 0) return false;
		//arPattAttach(arHandle, AR4_PattList);//>????

		patt_width = width;
		patt_center[0] = center[0];
		patt_center[1] = center[1];
		setName(pattFile);
		setActive(true);
		arHandle = _arHandle;
		return true;
	}

	void SingleMarker::update(ARMarkerInfo* _markerInfo,AR3DHandle *_gAR3DHandle)
	{
		
		if (_markerInfo == NULL || _gAR3DHandle == NULL || _markerInfo->id ==-1) {
			m_valid = false;
			// We won't call update in the parent class if marker is not
			// valid. So set m_seen to false here instead.
			m_seen = false;
		} else {
			m_valid = true;
			arGetTransMatSquare(_gAR3DHandle, _markerInfo, patt_width, patt_trans);
			m_confidence = _markerInfo->cf;
			double modelView[16];
			arglCameraViewRH(patt_trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			updateTransform(tmp);
		}
	}

	void SingleMarker::setActive(bool a)
	{
		m_active = a;
		if (m_active) arPattActivate(AR4_PattList, patt_id);
		else arPattDeactivate(AR4_PattList, patt_id);
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
