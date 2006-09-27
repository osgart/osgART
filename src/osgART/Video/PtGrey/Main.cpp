#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "PtGreyVideo"


DLL_API osgART::GenericVideo* osgart_createvideo(const osgART::VideoConfiguration& config)
{
	return new PtGreyVideo(config.id,config.type,config.width,config.height,config.framerate);
}

OSGART_PLUGIN_ENTRY()
