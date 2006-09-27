#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "ARToolKitVideo"


DLL_API osgART::GenericVideo* osgart_createvideo(const osgART::VideoConfiguration& config)
{
	return new ARToolKitVideo(config.deviceconfig);
}

OSGART_PLUGIN_ENTRY()



