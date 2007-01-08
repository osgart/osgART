#include <osgART/TrackerManager>

#include <osgDB/DynamicLibrary>

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
		return m_trackercount++;
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
			
			m_trackercount--;

		}
	}

	GenericTracker* 
	TrackerManager::getTracker(int id)
	{
		if (id <= m_trackercount)
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

	int
	TrackerManager::GetCount()
	{
		return m_trackercount;
	}


	TrackerManager::TrackerManager() : m_trackercount(0)
	{
	}

	/* static */
	GenericTracker* TrackerManager::createTrackerFromPlugin(const std::string& plugin) 
	{
		GenericTracker* _ret = 0L;
		std::string localLibraryName;

#ifdef _WIN32
		localLibraryName = plugin;
#else
		localLibraryName = plugin + ".so";
#endif

		osgDB::DynamicLibrary *_lib = osgDB::DynamicLibrary::loadLibrary(localLibraryName);

		if (_lib) {

			p_TrackerCreateFunc _createfunc = (p_TrackerCreateFunc)_lib->getProcAddress("osgart_create_tracker");

			_ret = (_createfunc) ? _createfunc() : 0L; 

			if (_ret) {
				
				TrackerManager::getInstance()->addTracker(_ret);

			} else {
				std::cerr << "Could not create TrackerPlugin " << std::endl;
			}
		}

		return _ret;

	}
};
