#include "osgART/ExportPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"

#include "BazARTracker"

DLL_API osgART::GenericTracker* osgart_create_tracker()
{
	return new osgART::BazARTracker();
}

OSGART_PLUGIN_ENTRY()
