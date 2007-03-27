/*
 *	osgART/VideoManager
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

#include "osgART/VideoManager"

#include <osg/Notify>
#include <OpenThreads/ScopedLock>
#include <iostream>

namespace osgART {

	VideoManager* VideoManager::_instance = 0L;
	
	VideoManager::PluginMap VideoManager::s_plugins;

	VideoManager* VideoManager::getInstance() {
		
		if (_instance == 0L) {
			_instance = new VideoManager();
		}
		return _instance;
	}

	VideoManager::~VideoManager() {

		// remove all videos 
		m_videomap.clear();

		// remove all video plugins
		VideoManager::s_plugins.clear();
	}

	int 
	VideoManager::addVideoStream(GenericVideo* video)
	{
		if (video) 
		{
			m_videomap[video->getID()] = video;

			osg::notify(osg::INFO) << "Added a osgART::GenericVideo with ID:" << video->getID() << " to the VideoManager" << std::endl;

			return numVideoStream++;
		}

		osg::notify(osg::WARN) << "osgART::VideoManager::addVideoStream(video) should receive a valid video stream" << std::endl;

		return -1;
	}

	void
	VideoManager::removeVideoStream(GenericVideo* video)
	{
		if (video) 
		{
			try {
			
				m_videomap[video->getID()] = 0L;
		
			} catch (...) {

				osg::notify(osg::WARN) << "osgART::VideoManager:removeVideoStream(video): "
					"Could not remove video with ID:" << video->getID() << std::endl;				
				return;
			}
			numVideoStream--;
		}
	}

	/*static*/
	void
	VideoManager::destroy() 
	{
		try
		{
			delete VideoManager::_instance;
			VideoManager::_instance = 0L;

		} 
		catch(...)
		{
			osg::notify(osg::WARN) << "Could not delete instance of VideoManager!" << std::endl;
		}
	}

	GenericVideo* 
	VideoManager::getVideo(int id)
	{
		/* \TODO: fix this in order to take advantage of the std::map :) */
		if (id<=numVideoStream)
		{
			return m_videomap[id].get();
		}
		else
		{
			osg::notify(osg::WARN) << "osgART::VideoManager::getVideo(id): Video with ID:" << id << 
				"doesn't exist." << std::endl;

			// return here
			return 0L;
		}
	}	


	VideoManager::VideoManager() : numVideoStream(0) 
	{
	}

	VideoManager::p_VideoCreateFunc 
	VideoManager::createFunc(const std::string& filename)
	{
		std::string localLibraryName;
#ifdef _WIN32
		localLibraryName = filename;
#else
		localLibraryName = filename + ".so";
#endif
		osgDB::DynamicLibrary* _lib = 0L;

		PluginMap::iterator _plug = s_plugins.find(filename);

		if (_plug == s_plugins.end()) 
		{
			_lib = osgDB::DynamicLibrary::loadLibrary(localLibraryName);

			if (_lib) {

				s_plugins[filename] = _lib;				

			} else {
				
				osg::notify(osg::WARN) << "osgART::VideoManager::createFunc(fileName) could not open " << filename << std::endl;

				return 0L;
			}
		} else 
		{
			osg::notify(osg::INFO) << "Video plugin '" << filename << "' was loaded in advance" << std::endl;

			// reaccess 
			_lib = (*_plug).second.get();

		}

		return (VideoManager::p_VideoCreateFunc)(_lib->getProcAddress("osgart_createvideo"));
		
	}

	GenericVideo* 
	VideoManager::createVideoFromPlugin(const std::string& plugin) 
	{
			GenericVideo* _ret = 0L;			
			
			p_VideoCreateFunc _createfunc = createFunc(plugin);

			_ret = (_createfunc) ? _createfunc() : 0L; 

			if (_ret) 
			{		

				osg::notify(osg::INFO) << "VideoManager::createVideoFromPlugin(plugin): " <<
					"Plugin '" << plugin << "' successfully instantiated a video handler" << std::endl;

				VideoManager::getInstance()->addVideoStream(_ret);

			} else 
			{
				osg::notify(osg::WARN) << "VideoManager::createVideoFromPlugin(plugin): " <<
					"Plugin '" << plugin << "' could not be loaded!" << std::endl;
			}

			return _ret;

	}


	
};
