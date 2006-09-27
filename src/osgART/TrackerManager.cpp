///////////////////////////////////////////////////////////////////////////////
// File name : TrackerManager.C
//
// Creation : YYY
//
// Version : YYY
//
// Author : Raphael Grasset
//
// email : Raphael.Grasset@imag.fr
//
// Purpose : ??
//
// Distribution :
//
// Use :
//	??
//
// Todo :
//	O ??
//	/
//	X
//
// History :
//	YYY : Mr Grasset : Creation of the file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include file
///////////////////////////////////////////////////////////////////////////////

#include <osgART/TrackerManager>

#include <iostream>

namespace osgART {


	TrackerManager* TrackerManager::getInstance() {
		static TrackerManager* instance = NULL;
		if (instance == NULL) {
			instance = new TrackerManager();
		}
		return instance;
	}

	TrackerManager::~TrackerManager(void)
	{
	    
	}

	int 
	TrackerManager::addTracker(GenericTracker* tracker)
	{
		trackerMap[tracker->getId()] = tracker;
		return numTracker++;
	}

	void 
	TrackerManager::removeTracker(GenericTracker* tracker)
	{
		if (tracker) {
			try {
				trackerMap[tracker->getId()] = 0L;
			} catch(...) {

				std::cerr << "osgART::TrackerManager: could not unregister tracker" << std::endl;

				// return here
				return;
			}
			
			numTracker--;

		}
	}

	GenericTracker* 
	TrackerManager::getTracker(int id)
	{
		if (id<=numTracker)
		{
			return trackerMap[id];
		}
		else
		{
			std::cerr << "osgART::TrackerManager: tracker with id:" << id << " doesn't exist" << std::endl;

			return 0L;
		}
	}


	TrackerManager::TrackerManager()
	{
		numTracker = 0;
	}

};