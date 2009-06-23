#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "CVCamVideo"


DLL_API osgART::GenericVideo* osgart_createvideo(const osgART::VideoConfiguration& config)
{
	//return new CVCamVideo(config.id,config.width,config.height,config.type,config.framerate);
	return new CVCamVideo();
}

OSGART_PLUGIN_ENTRY()
