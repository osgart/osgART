/*
 *	osgART/TrackerManager
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

#include <osgART/TrackerManager>
#include <osgDB/DynamicLibrary>
#include <osg/Notify>

namespace osgART {

	TrackerManager* TrackerManager::s_instance = 0L;


	TrackerManager::TrackerManager() : m_trackercount(0)
	{
	}


	TrackerManager* TrackerManager::getInstance() 
	{
		if (TrackerManager::s_instance == 0L) 
		{
			TrackerManager::s_instance = new TrackerManager();
		}
		return s_instance;
	}

	TrackerManager::~TrackerManager()
	{	    
		osg::notify() << "TrackerManager::~TrackerManager(): " <<
			"Deleting all tracker. Got " << m_trackermap.size() << " assigned tracker(s)"
			<< std::endl;

		this->m_trackermap.clear();
	}

	int 
	TrackerManager::addTracker(GenericTracker* tracker)
	{
		m_trackermap[tracker->getId()] = tracker;

		osg::notify() << "osgART::TrackerManager::addTracker(tracker): " <<
			"Added tracker with ID " << tracker->getId() << std::endl;

		return m_trackercount++;
	}

	void 
	TrackerManager::removeTracker(GenericTracker* tracker)
	{
		if (tracker) {

			try 
			{			
				m_trackermap[tracker->getId()] = 0L;
			
			} catch(...) 
			{

				osg::notify(osg::WARN) << "Warning! osgART::TrackerManager::removeTracker(tracker) "
					<< "Could not unregister tracker with ID " << tracker->getId() << std::endl;

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
			osg::notify(osg::WARN) << "Warning! osgART::TrackerManager::getTracker(id)"
				"tracker with ID:" << id << " doesn't exist" << std::endl;
		}
		return 0L;
	}

	/*static*/
	void
	TrackerManager::destroy()
	{
		osg::notify() << "osgART::TrackerManager::destroy() Delete the tracker manager"
			<< std::endl;

		delete s_instance;
		
		s_instance = 0L;
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

		if (_lib) 
		{

			p_TrackerCreateFunc _createfunc = (p_TrackerCreateFunc)_lib->getProcAddress("osgart_create_tracker");

			_ret = (_createfunc) ? _createfunc() : 0L; 

			if (_ret) 
			{
				
				TrackerManager::getInstance()->addTracker(_ret);

			} else 
			{
				osg::notify(osg::WARN) << "Warning! osgART::TrackerManager::createTrackerFromPlugin(plugin): "
					"Could not create Tracker from TrackerPlugin '" << plugin << "'" << std::endl;
			}
		}

		return _ret;

	}
};
