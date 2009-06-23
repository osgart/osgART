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

#include <osgDB/FileUtils>

#include "MultiMarker"

#include <AR/gsub_lite.h>

namespace osgART {

	MultiMarker::MultiMarker() : Marker(), m_multi(0L) {
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
	MultiMarker::initialise(const std::string& multiFile) {

		std::string actualMultiFile = osgDB::findDataFile(multiFile);

		// Check if multifile exists!!!
		m_multi = arMultiReadConfigFile(actualMultiFile.c_str());
		if (m_multi == NULL) return false;
		
		setName(multiFile);
		setActive(false);
		
		return true;
	}

	void
	MultiMarker::setActive(bool a) {
		m_active = a;
		
		if (m_active) arMultiActivate(m_multi);
		else arMultiDeactivate(m_multi);
	}

	void 
	MultiMarker::update(ARMarkerInfo* markerInfo, int markerCount) 
	{
		m_valid = (arMultiGetTransMat(markerInfo, markerCount, m_multi) >= 0);
		if (m_valid) {
			double modelView[16];
			arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			updateTransform(tmp);
		}
	}
	
};
