#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "DSVLVideo"


DLL_API osgART::GenericVideo* osgart_createvideo()
{
	return new DSVLVideo();
}

OSGART_PLUGIN_ENTRY()



