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

#include "osgART/PluginManager"

#include <osg/Version>
#include <osg/Notify>

#include <osgDB/Registry>
#include <osgDB/FileUtils>

#include <sstream>
#include <cstdlib>

#if defined(_WIN32)||(_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	#define SLASH "\\"
#else
	#define SLASH "/"
#endif


std::string getCurrentExecutablePath()
{
	std::string result;
	
#if defined(_WIN32)

	static TCHAR lpFname[MAX_PATH];
	DWORD ret = GetModuleFileName( NULL, lpFname, MAX_PATH );
	size_t offset = strrchr(lpFname,SLASH[0]) - &lpFname[0];
	result.assign(&lpFname[0],offset);

#endif
	return result;
}



namespace osgART {

	PluginManager::PluginManager() 
	: osg::Referenced()
	{
		// add local executable path
		osgDB::Registry::instance()->getLibraryFilePathList().push_back(getCurrentExecutablePath()+SLASH+"osgPlugins-" + osgGetVersion()+SLASH);

		// only do if environment variable exists
		if (getenv("OSGART_ROOT"))
		{
			osgDB::getLibraryFilePathList().push_front(std::string(getenv("OSGART_ROOT"))+SLASH+"bin"+SLASH+"osgPlugins-" + osgGetVersion()+SLASH);
			osg::notify() << "osgART::PluginManager::PluginManager() added osgART root path:" << osgDB::getLibraryFilePathList().front() << std::endl;
		}

		if (getenv("OSGART_PLUGIN_PATH"))
		{
			osgDB::getLibraryFilePathList().push_front(std::string(getenv("OSGART_PLUGIN_PATH"))+SLASH);
			osg::notify() << "osgART::PluginManager::PluginManager() added osgART plugins path:" <<  osgDB::getLibraryFilePathList().front() << std::endl;
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
		// iterate over registered plugins
		for (PluginInterfaceMap::iterator i = m_plugininterfaces.begin();
			i != m_plugininterfaces.end();
			++i)
		{
			// check if this is the same reference
			if (ref == i->second) 
			{
				// just need to remove the reference from PluginManager
				osg::notify() << "osgART::PluginManager::remove() unregistered " << i->first << std::endl;
				i->second = 0L; 
			}		
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
			OSG_WARN << "Plugin '" << name << "' unknown!" << std::endl;
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
	PluginManager::load(const std::string& pluginname, bool resolveName)
	{
		std::string fullName;

		#if defined(__unix) || defined(__APPLE__)
					fullName = pluginname + ".so";
		#elif defined(_WIN32)
				// If we are on Windows, then the file extension is .dll, but we also 
				// distinguish between release and debug versions.
				#if defined(NDEBUG)
					fullName = pluginname + ".dll";
				#else
					fullName = pluginname + "_debug.dll";
				#endif
		#endif

		osgDB::DynamicLibrary* lib =  osgDB::DynamicLibrary::loadLibrary(fullName);

		return ( 0 != lib); 
	}
}
