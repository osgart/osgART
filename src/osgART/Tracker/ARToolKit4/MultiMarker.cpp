/*
 *	osgART/Tracker/ARToolKit4/MultiMarker
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

#include "MultiMarker"

#include <AR/gsub_lite.h>

namespace osgART {

	MultiMarker::MultiMarker() //ARPattHandle  *_PattHdl
			: Marker()/*,
			AR4_PattList(),
			arHandle()*/
	{
	}

	MultiMarker::~MultiMarker() {   
		// jcl64: Free the multimarker
		if (m_multi) arMultiFreeConfig(m_multi);
	}

	/* virtual */
	Marker::MarkerType MultiMarker::getType() const {
		return Marker::ART_MULTI;
	}

	bool 
	MultiMarker::initialise(const std::string& multiFile)//ARHandle		*_arHandle,
	{
		// Check if multifile exists!!!
		m_multi = arMultiReadConfigFile(multiFile.c_str());
		if (m_multi == NULL) return false;
		
		setName(multiFile);
		setActive(false);
		
		return true;
	}

	void
	MultiMarker::setActive(bool a) {
		m_active = a;
		
	//	if (m_active) arMultiActivate(m_multi);
	//	else arMultiDeactivate(m_multi);

	}

	void 
	MultiMarker::update(AR3DHandle *_gAR3DHandle, ARMarkerInfo* markerInfo, int markerCount) 
	{
		m_valid = (arGetTransMatMultiSquare(_gAR3DHandle, markerInfo, markerCount, m_multi) >= 0);
		if (m_valid) {
			double modelView[16];
			arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			updateTransform(tmp);
		} else {
			m_seen = false;
		}
		/*
		arGetTransMatMultiSquare(
		
		m_valid = arMultiGetTransMat(markerInfo, markerCount, m_multi) >= 0);
		double modelView[16];
		arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
		osg::Matrix tmp(modelView);
		updateTransform(tmp);
		*/
	}
	
};
