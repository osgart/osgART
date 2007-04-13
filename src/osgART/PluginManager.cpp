#include "osgART/PluginManager"

#include <osg/Notify>

namespace osgART {

	PluginManager::PluginManager() : osg::Referenced()
	{
	}

	PluginManager::~PluginManager()
	{
	}

	PluginManager* PluginManager::getInstance(bool erase /* = false */) 
	{
		
		static osg::ref_ptr<PluginManager> s_pluginmanager = new PluginManager;
		
		if (erase)
		{
			s_pluginmanager = 0;
		}		
		
		return s_pluginmanager.get();
	}

	void PluginManager::load(const std::string& pluginname)
	{
		std::string localLibraryName = pluginname;

#if !defined(NDEBUG)
		localLibraryName += "_debug";
#endif
		// actually some platform are using .dylib (Apple)
#if defined(__unix) || defined(__APPLE__)
		localLibraryName = filename + ".so";
#endif
		osgDB::DynamicLibrary* lib = 0L;

		// some plugins behave badly if loaded
		try {
			
			lib = osgDB::DynamicLibrary::loadLibrary(localLibraryName);

		} catch (...)
		{
			osg::notify(osg::WARN) << "Loading plugin '" << pluginname << "' failed!" << std::endl;
		}

		if (lib) m_plugins.push_back(lib);

	}
}
