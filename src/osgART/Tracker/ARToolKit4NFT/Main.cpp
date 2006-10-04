#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"

#include "ARToolKit4NFTTracker"

DLL_API osgART::GenericTracker* osgart_create_tracker()
{
	return new osgART::ARToolKit4NFTTracker();
}

OSGART_PLUGIN_ENTRY()
