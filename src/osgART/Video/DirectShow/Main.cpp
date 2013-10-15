#include "osgART/ExportPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "IntranelStreamVideo"


DLL_API osgART::GenericVideo* osgart_createvideo(const osgART::VideoConfiguration& config)
{
	return new IntranelStreamVideo();
}

OSGART_PLUGIN_ENTRY()
