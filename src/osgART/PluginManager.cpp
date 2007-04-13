#include "osgART/PluginManager"

#include <osg/Notify>

namespace osgART {

	PluginManager::PluginManager() : osg::Referenced()
	{
	}

	PluginManager::~PluginManager()
	{
	}

	void PluginManager::add(const std::string& name, osg::Referenced* ref)
	{
		this->m_plugininterfaces[name] = ref;
	}

	osg::Referenced* PluginManager::get(const std::string& name)
	{
		if (m_plugininterfaces.find(name) == m_plugininterfaces.end()) 
			return 0L;
		return ((*m_plugininterfaces.find(name)).second.get());
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

	void PluginManager::load(const std::string& pluginname, bool resolveName /*= true*/)
	{
		std::string localLibraryName = pluginname;

		if (resolveName)
		{
#if !defined(NDEBUG)
			localLibraryName += "_debug";
#endif
			// actually some platform are using .dylib (Apple)
#if defined(__unix) || defined(__APPLE__)
			localLibraryName = filename + ".so";
#endif
		}

		osgDB::DynamicLibrary* lib = 0L;

		// some plugins behave badly when loaded into memory
		try {
			
			lib = osgDB::DynamicLibrary::loadLibrary(localLibraryName);

		} catch (...)
		{
			osg::notify(osg::WARN) << "Loading plugin '" << pluginname << "' failed!" << std::endl;

			return;
		}

		if (lib) m_plugins.push_back(lib);
	}
}
