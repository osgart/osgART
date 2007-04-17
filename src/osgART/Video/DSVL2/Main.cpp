#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"
#include "DSVLVideo"


DLL_API osgART::GenericVideo* osgart_createvideo()
{
	return new osgART::DSVL2Video();
}

OSGART_PLUGIN_ENTRY()



