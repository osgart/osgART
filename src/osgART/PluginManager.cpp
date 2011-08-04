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

#include "osgART/PluginManager"

#include <osg/Version>
#include <osg/Notify>

#include <osgDB/Registry>
#include <osgDB/FileUtils>

#include <sstream>
#include <cstdlib>

namespace osgART {

	PluginManager::PluginManager() 
	: osg::Referenced()
	{
			osgDB::FilePathList& fpl = osgDB::Registry::instance()->getDataFilePathList();
			
			// only do if environment variable exists
			if (getenv("OSGART_PLUGIN_DIR"))
			{
				osgDB::getLibraryFilePathList().push_front(getenv("OSGART_PLUGIN_DIR"));
				osg::notify() << "Added osgART specific paths" << std::endl;
			}
	}

	PluginManager::~PluginManager()
	{
	}

	void 
	PluginManager::add(const std::string& name, osg::Referenced* ref)
	{
		this->m_plugininterfaces[name] = ref;
		osg::notify() << "osgART::PluginManager::add() registered " << name << std::endl; 
	}

	void 
	PluginManager::remove(osg::Referenced* ref)
	{
		osg::notify() << "osgART::PluginManager::remove() unregistered " << std::endl;

		for (PluginInterfaceMap::iterator i = m_plugininterfaces.begin();
			i != m_plugininterfaces.end();
			++i)
		{
			if (ref == i->second) { m_plugininterfaces.erase(i); }		
		}
	}

	osg::Referenced* 
	PluginManager::operator[]( const std::string& name)
	{
		return this->get(name);
	}

	osg::Referenced* 
	PluginManager::get( const std::string& name )
	{
		if (m_plugininterfaces.find(name) == m_plugininterfaces.end())
		{
			osg::notify(osg::WARN) << "Plugin '" << name << "' unknown!" << std::endl;
			return 0L;
		}

		return ((*m_plugininterfaces.find(name)).second.get());
	}


	PluginManager* 
	PluginManager::instance(bool erase /* = false */) 
	{
		
		static osg::ref_ptr<PluginManager> s_pluginmanager = new PluginManager();
		
		if (erase)
		{
			s_pluginmanager = 0L;
		}		
		
		return s_pluginmanager.get();
	}

	bool 
	PluginManager::load(const std::string& pluginname, bool resolveName /*= true*/)
	{

		std::string localLibraryName = pluginname;

		std::string pathSeparator = 
#if defined(_WINDOWS)
			std::string("\\");
#else
			std::string("/");
#endif

		if (resolveName)
		{

			// actually some platform are using other extensions like Apple (.dylib)
			// however OpenSceneGraph uses the .so extension - we just follow this
#if defined(__unix) || defined(__APPLE__)
			localLibraryName = pluginname + ".so";
#elif defined(_WINDOWS)

		// If we are on Windows, then the file extension is .dll, but we also 
		// distinguish between release and debug versions.
		#if defined(NDEBUG)
			localLibraryName = pluginname + ".dll";
		#else
			localLibraryName = pluginname + "_debug.dll";
		#endif

	
#endif
		}
		std::stringstream pluginDirName;

		pluginDirName << "osgPlugins-" << osgGetVersion() << pathSeparator;

		osgDB::FilePathList::const_iterator i = osgDB::Registry::instance()->getLibraryFilePathList().begin();

		osgDB::DynamicLibrary* lib = 0L;

		while (i != osgDB::Registry::instance()->getLibraryFilePathList().end())
		{
#if defined(_WINDOWS)
			if ( lib = osgDB::DynamicLibrary::loadLibrary((*i) + pathSeparator + localLibraryName) )
#else
			if ( lib = osgDB::DynamicLibrary::loadLibrary((*i) + pathSeparator + pluginDirName.str() + localLibraryName) )
#endif
			{
				return true;
			}

			i++;
		}

		return false;
	}
}
