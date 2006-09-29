#include "osgART/VideoManager"

#include <OpenThreads/ScopedLock>

#include <iostream>

namespace osgART {

	VideoManager* VideoManager::_instance = NULL;
	
	VideoManager::PluginMap VideoManager::s_plugins;

	VideoManager* VideoManager::getInstance() {
		
		if (_instance == NULL) {
			_instance = new VideoManager();
		}
		return _instance;
	}

	VideoManager::~VideoManager() {
		m_videomap.clear();
		VideoManager::s_plugins.clear();
	}

	int 
	VideoManager::addVideoStream(GenericVideo* video)
	{
		m_videomap[video->getId()] = video;
		return numVideoStream++;
	}

	void
	VideoManager::removeVideoStream(GenericVideo* video)
	{
		if (video) 
		{
			try {
			
				m_videomap[video->getId()] = 0L;
		
			} catch (...) {

				std::cerr << "osgART::VideoManager: Could not remove " << video->getId() << std::endl;
				// return here! 
				return;
			}

			numVideoStream--;
		}
	}

	/*static*/
	void
	VideoManager::destroy() {
		delete VideoManager::_instance;
		VideoManager::_instance = NULL;
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
			std::cerr << "osgART::VideoManager: video with id:" << id << 
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

		if (/*_plug == s_plugins.end()*/1) {
		
			_lib = osgDB::DynamicLibrary::loadLibrary(localLibraryName);

			if (_lib) {

				s_plugins[filename] = _lib;				

			} else {
				
				std::cerr << "osgART::VideoManager could not open " << filename << std::endl;

				return 0L;
			}
		} else {

			_lib = (*_plug).second.get();

		}

		return (VideoManager::p_VideoCreateFunc)(_lib->getProcAddress("osgart_createvideo"));
		
	}

	GenericVideo* VideoManager::createVideoFromPlugin(const std::string& plugin,
		const VideoConfiguration& config) {

			GenericVideo* _ret = 0L;

			p_VideoCreateFunc _createfunc = createFunc(plugin);

			_ret = (_createfunc) ? _createfunc(config) : 0L; 

			if (_ret) {
				
				VideoManager::getInstance()->addVideoStream(_ret);

			} else {
				std::cerr << "Could not create VideoPlugin " << std::endl;
			}

			return _ret;

	}


	
};