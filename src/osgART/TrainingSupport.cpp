/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
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

#include "osgART/TrainingSupport"

namespace osgART {

	TrainingCandidate::TrainingCandidate() : osg::Referenced() {


	}


	TrainingCandidate::~TrainingCandidate() {



	}

	TrainingSet::TrainingSet() : osg::Referenced() {


	}


	TrainingSet::~TrainingSet() {

	}

	TrainingSet::TrainingCandidateList& TrainingSet::getTrainingCandidates() {
		return mCandidates;
	}



	TrainingSupport::TrainingSupport() : osg::Referenced(), 
		mEnabled(false) {

	}

	TrainingSupport::~TrainingSupport() {

	}

};
