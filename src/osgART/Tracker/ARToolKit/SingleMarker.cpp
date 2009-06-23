/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2008 Human Interface Technology Laboratory New Zealand
 * 
 * This file is part of osgART 2.0
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "SingleMarker"

#include <osgDB/FileUtils>
#include <AR/gsub_lite.h>



namespace osgART {

	SingleMarker::SingleMarker() : Marker(),
		patt_id(-1)
	{
			// moved to a real 
			//m_fields["confidence"] = new TypedField<double>(&_confidence);
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
		if (patt_id >= 0) 
			return (false);

		std::string actualPattFile = osgDB::findDataFile(pattFile);

		patt_id = arLoadPatt(actualPattFile.c_str());
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
			_confidence = markerInfo->cf;
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


