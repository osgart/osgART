#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"

#include "ARToolKit4Tracker"

DLL_API osgART::GenericTracker* osgart_create_tracker()
{
	return new osgART::ARToolKit4Tracker();
}

OSGART_PLUGIN_ENTRY()
