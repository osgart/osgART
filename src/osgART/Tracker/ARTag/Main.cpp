#include "osgART/ExportPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"

#include "ARTagTracker"

DLL_API osgART::GenericTracker* osgart_create_tracker()
{
	return new osgART::ARTagTracker();
}

OSGART_PLUGIN_ENTRY()
