#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "VideoSourceVideo"


DLL_API osgART::GenericVideo* osgart_createvideo(const osgART::VideoConfiguration& config)
{
	return new VideoSourceVideo(config.deviceconfig.c_str());

}

OSGART_PLUGIN_ENTRY()
