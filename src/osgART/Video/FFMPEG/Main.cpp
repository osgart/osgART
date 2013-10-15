#include "osgART/ExportPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "Video.h"


DLL_API osgART::GenericVideo* osgart_createvideo(const osgART::VideoConfiguration& config)
{
	return new FFMPEGVideo();
}

OSGART_PLUGIN_ENTRY()
