/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
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

#include "MultiTarget"

#include <AR/arGLUtils.h>

namespace osgART {

	MultiTarget::MultiTarget() : Target(), m_multi(0L) 
	{
	}

	MultiTarget::~MultiTarget() 
	{   	
		if (m_multi) arMultiFreeConfig(m_multi);
	}

	/* virtual */
	Target::TargetType MultiTarget::getType() const 
	{
		return Target::ART_MULTI;
	}

	bool 
	MultiTarget::initialise(const std::string& multiFile) {

		if (m_multi) 
		{
			osg::notify(osg::WARN) << "MultiTarget: Cannot initialise Target. Already initialised." << std::endl;
			return false;
		}

		std::string actualMultiFile = osgDB::findDataFile(multiFile);

		// Check if multi-markre configuration file exists
		if (actualMultiFile.empty() || !osgDB::fileExists(actualMultiFile)) 
		{
			osg::notify(osg::WARN) << "MultiTarget: Cannot initialise MultiTarget. File " << multiFile << " not found" << std::endl;
			return false;
		}
		
		m_multi = arMultiReadConfigFile(actualMultiFile.c_str());
		if (m_multi == NULL) 
		{
			osg::notify(osg::WARN) << "MultiTarget: Cannot initialise MultiTarget. Error reading config file." << std::endl;
			return false;
		}
		
		setName(multiFile);
		setActive(false);
		_valid = false;
		
		return true;
	}

	void
	MultiTarget::setActive(bool a) 
	{
		_active = a;
		
		if (_active) arMultiActivate(m_multi);
		else arMultiDeactivate(m_multi);
	}

	void 
	MultiTarget::update(ARMarkerInfo* targetInfo, int targetCount) 
	{

		if (_active == false) 
		{
			// If the target isn't active, then it can't be valid, and should not be updated either.
			_valid = false;
			return;
		}

		// Sanity check
		if (!targetInfo) 
		{
			_valid = false;
			return;
		}

		_valid = (arMultiGetTransMat(targetInfo, targetCount, m_multi) >= 0);

		if (_valid) {
			double modelView[16];
			arglCameraViewRH(m_multi->trans, modelView, 1.0f);
			updateTransform(osg::Matrix(modelView));
		}
	}
	
};
