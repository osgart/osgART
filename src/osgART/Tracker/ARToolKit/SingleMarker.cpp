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

	SingleMarker::SingleMarker() : Target(),
		patt_id(-1),
		mInitialData(false)
	{			
	}

	SingleMarker::~SingleMarker()
	{
		if (patt_id >= 0) arFreePatt(patt_id);
	}

	Target::MarkerType SingleMarker::getType() const
	{
		return Target::ART_SINGLE;
	}

	bool SingleMarker::initialise(const std::string& pattFile, double width, double center[2])
	{
		// Check if this marker has already been initialised
		if (patt_id >= 0) 
		{
			osg::notify(osg::WARN) << "SingleMarker: Cannot initialise Marker. Already initialised." << std::endl;
			return false;
		}

		// Locate the specified pattern file
		std::string actualPattFile = osgDB::findDataFile(pattFile);
		if (!osgDB::fileExists(actualPattFile)) 
		{
			osg::notify(osg::WARN) << "SingleMarker: Cannot find pattern file: " << pattFile << std::endl;
			return false;
		}

		// Attempt to load pattern file
		patt_id = arLoadPatt(actualPattFile.c_str());
		if (patt_id < 0) 
		{
			osg::notify(osg::WARN) << "SingleMarker: Error loading pattern file: " << pattFile << std::endl;
			return false;
		}
		
		patt_width = width;
		patt_center[0] = center[0];
		patt_center[1] = center[1];
		
		setName(pattFile);
		setActive(false);
		_valid = false;

		return true;
	}

	void SingleMarker::update(ARMarkerInfo* markerInfo, bool useHistory)
	{

		if (_active == false) 
		{
			// If the marker isn't active, then it can't be valid, and should not be updated either.
			_valid = false;
			return;
		}

		if (markerInfo == 0L) 
		{
			// Invalid marker info cannot be used for update
			_valid = false;
			return;
		} 

		// Valid marker info means the tracker detected and tracked the marker
		_valid = true;

		// Use history-based arGetTransMatCont if flag is set and we have inital data from a call to arGetTransMat
		if (useHistory && mInitialData) 
		{
			arGetTransMatCont(markerInfo, patt_trans, patt_center, patt_width, patt_trans);
		} 
		else 
		{
			arGetTransMat(markerInfo, patt_center, patt_width, patt_trans);
			mInitialData = true; // Need to get inital data before arGetTransMatCont can be used
		}

		_confidence = markerInfo->cf;

		double modelView[16];
		arglCameraViewRH(patt_trans, modelView, 1.0f);
		updateTransform(osg::Matrix(modelView));

	}

	void SingleMarker::setActive(bool a)
	{
		_active = a;
		
		if (_active) arActivatePatt(patt_id);
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


