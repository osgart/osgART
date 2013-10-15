#include "osgART/ExportPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"

#include "ARToolKitMMAutoTracker"

DLL_API osgART::GenericTracker* osgart_create_tracker()
{
	return new osgART::ARToolKitMMAutoTracker();
}

OSGART_PLUGIN_ENTRY()
