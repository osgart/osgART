#include <osgART/TrackerManager>

#include <iostream>

namespace osgART {

	TrackerManager* TrackerManager::s_instance = NULL;

	TrackerManager* TrackerManager::getInstance() {
		if (TrackerManager::s_instance == NULL) {
			TrackerManager::s_instance = new TrackerManager();
		}
		return s_instance;
	}

	TrackerManager::~TrackerManager()
	{	    
		// hse25: remove all Tracker
		this->m_trackermap.clear();
	}

	int 
	TrackerManager::addTracker(GenericTracker* tracker)
	{
		m_trackermap[tracker->getId()] = tracker;
		return numTracker++;
	}

	void 
	TrackerManager::removeTracker(GenericTracker* tracker)
	{
		if (tracker) {
			try {
				m_trackermap[tracker->getId()] = 0L;
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
			return m_trackermap[id].get();
		}
		else
		{
			std::cerr << "osgART::TrackerManager: tracker with id:" << id << " doesn't exist" << std::endl;

			return 0L;
		}
	}

	/*static*/
	void
	TrackerManager::destroy()
	{
		delete s_instance;
		s_instance = NULL;
	}


	TrackerManager::TrackerManager()
	{
		numTracker = 0;
	}

};