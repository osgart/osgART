/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include "SingleTarget"

#include <osgDB/FileUtils>
#include <AR/arGLUtils.h>



namespace osgART {

	SingleTarget::SingleTarget() : Target(),
		patt_id(-1),
		mInitialData(false)
	{			
	}

	SingleTarget::~SingleTarget()
	{
		if (patt_id >= 0) arFreePatt(patt_id);
	}

	Target::TargetType SingleTarget::getType() const
	{
		return Target::TARGET_SINGLE;
	}

	bool SingleTarget::initialise(const std::string& pattFile, double width, double center[2])
	{
		// Check if this target has already been initialised
		if (patt_id >= 0) 
		{
			osg::notify(osg::WARN) << "SingleTarget: Cannot initialise Target. Already initialised." << std::endl;
			return false;
		}

		// Locate the specified pattern file
		std::string actualPattFile = osgDB::findDataFile(pattFile);
		if (!osgDB::fileExists(actualPattFile)) 
		{
			osg::notify(osg::WARN) << "SingleTarget: Cannot find pattern file: " << pattFile << std::endl;
			return false;
		}

		// Attempt to load pattern file
		patt_id = arLoadPatt(actualPattFile.c_str());
		if (patt_id < 0) 
		{
			osg::notify(osg::WARN) << "SingleTarget: Error loading pattern file: " << pattFile << std::endl;
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

	void SingleTarget::update(ARMarkerInfo* targetInfo, bool useHistory)
	{

		if (_active == false) 
		{
			// If the target isn't active, then it can't be valid, and should not be updated either.
			_valid = false;
			return;
		}

		if (targetInfo == 0L) 
		{
			// Invalid target info cannot be used for update
			_valid = false;
			return;
		} 

		// Valid target info means the tracker detected and tracked the target
		_valid = true;

		// Use history-based arGetTransMatCont if flag is set and we have inital data from a call to arGetTransMat
		if (useHistory && mInitialData) 
		{
			arGetTransMatCont(targetInfo, patt_trans, patt_center, patt_width, patt_trans);
		} 
		else 
		{
			arGetTransMat(targetInfo, patt_center, patt_width, patt_trans);
			mInitialData = true; // Need to get inital data before arGetTransMatCont can be used
		}

		_confidence = targetInfo->cf;

		double modelView[16];
		arglCameraViewRH(patt_trans, modelView, 1.0f);
		updateTransform(osg::Matrix(modelView));

	}

	void SingleTarget::setActive(bool a)
	{
		_active = a;
		
		if (_active) arActivatePatt(patt_id);
		else arDeactivatePatt(patt_id);

	}

	int SingleTarget::getPatternID()
	{
		return patt_id;
	}

	double SingleTarget::getPatternWidth()
	{
		return patt_width;
	}
		
	double* SingleTarget::getPatternCenter()
	{
		return patt_center;
	}
};


