#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "VidCaptureVideo"


DLL_API osgART::GenericVideo* osgart_createvideo(const osgART::VideoConfiguration& config)
{
	return new VidCaptureVideo(config.id,config.width,config.height,config.type,config.framerate);

}

OSGART_PLUGIN_ENTRY()
