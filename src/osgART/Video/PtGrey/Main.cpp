#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "PtGreyVideo"


DLL_API osgART::GenericVideo* osgart_createvideo()
{
	return new PtGreyVideo();
}

OSGART_PLUGIN_ENTRY()
