#include "osgART/VideoManager"
#include "osgDB/DynamicLibrary"

#include <iostream>

namespace osgART {

	std::map<unsigned int, VideoManager::p_VideoCreateFunc> VideoManager::gs_videocreate;

	VideoManager* VideoManager::getInstance() {
		static VideoManager* instance = NULL;
		if (instance == NULL) {
			instance = new VideoManager();
		}
		return instance;
	}

	VideoManager::~VideoManager(void)
	{	    
	}

	int 
	VideoManager::addVideoStream(GenericVideo* video)
	{
		videoMap[video->getId()] = video;
		return numVideoStream++;
	}

	void
	VideoManager::removeVideoStream(GenericVideo* video)
	{
		if (video) 
		{
			try {
			
				videoMap[video->getId()] = 0L;
		
			} catch (...) {

				std::cerr << "osgART::VideoManager: Could not remove " << video->getId() << std::endl;
				// return here! 
				return;
			}

			numVideoStream--;
		}
	}

	GenericVideo* 
	VideoManager::getVideo(int id)
	{
		/* \TODO: fix this in order to take advantage of the std::map :) */
		if (id<=numVideoStream)
		{
			return videoMap[id];
		}
		else
		{
			std::cerr << "osgART::VideoManager: video with id:" << id << 
				"doesn't exist." << std::endl;

			// return here
			return 0L;
		}
	}	

	/* static */
	void 
	VideoManager::registerVideo(unsigned int guid, p_VideoCreateFunc func)
	{
		VideoManager::gs_videocreate[guid] = func;
	}

	GenericVideo*
	VideoManager::createVideo(unsigned int guid, const VideoConfiguration& cfg) 
	{
		return VideoManager::gs_videocreate[guid](cfg);
	}

	VideoManager::VideoManager() : numVideoStream(0) 
	{
	}

	VideoManager::p_VideoCreateFunc createFunc(const std::string& filename)
	{
		std::string localLibraryName;
#ifdef _WIN32
		localLibraryName = filename;
#else
		localLibraryName = filename + ".so";
#endif
		osgDB::DynamicLibrary *_lib = osgDB::DynamicLibrary::loadLibrary(localLibraryName);

		if (_lib) {

			return (VideoManager::p_VideoCreateFunc)(_lib->getProcAddress("osgart_createvideo"));
		}

		std::cerr << "osgART::VideoManager could not open " << filename << std::endl;

		return 0L;
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