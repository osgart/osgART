#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "DSVLVideo"


DLL_API osgART::GenericVideo* osgart_createvideo(const osgART::VideoConfiguration& config)
{
	return new DSVLVideo(config.deviceconfig.c_str());
}

OSGART_PLUGIN_ENTRY()



