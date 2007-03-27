/*
 *	osgART/Foundation
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@


#include <osg/Version>
#include <osg/Notify>
//
// Includes.
//
#include "osgART/Foundation"
#include "osgART/VideoManager"
#include "osgART/TrackerManager"


#include <cstdlib>

#ifdef __APPLE__
#  include <Carbon/Carbon.h>
#  include <unistd.h>
#  include <sys/param.h>
#  include <OpenGL/OpenGL.h>
#endif


//
// Private globals.
//
#ifdef __APPLE__
static char g_cwd[MAXPATHLEN] = "";
#endif

//
// Private functions.
//
extern "C" void osgARTFinal(void)
{
	// Add OS-specific cleanup here.
#ifdef __APPLE__
	// Restore working directory to what it was when we were exec'ed.
	if (g_cwd[0]) {
		chdir(g_cwd);
		g_cwd[0] = '\0';
	}
#endif
}

//
// Public functions.
//
void osgARTInit(int *argcp, char **argv)
{

	// silence the compiler
	if ((argcp == 0) || (&argv[0] == 0)) {
	};
	
	// Add OS-specific setup here.

#ifdef __APPLE__
	// On Mac OS X, ARToolKit 'Data' is in "Resources" directory inside the application bundle.
	if (!g_cwd[0]) getcwd(g_cwd, MAXPATHLEN);
	CFURLRef pathCFURLRef = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle()); // Get relative path to resources directory.
	if (pathCFURLRef) {
		char *path = (char *)calloc(MAXPATHLEN, sizeof(char));
		if (CFURLGetFileSystemRepresentation(pathCFURLRef, true, (UInt8*)path, MAXPATHLEN)) { // true in param 2 resolves against base.
			if (chdir(path) != 0) {
				fprintf(stderr, "Unable to change working directory to %s.\n", path);
				perror(NULL);
			} // else fprintf(stderr,"Current working directory is %s\n", path);
		}
		free(path);
		CFRelease(pathCFURLRef);
	}
	
	// Workaround for the Radeon X1600 driver bug, which gets tickled by osgText.
	// See if the ATi Radeon X1600 OpenGL engine is loaded.
	CGLRendererInfoObj rend;
	long rendIndex, rendCount, rendProp;
	if (CGLQueryRendererInfo(0xffffffff, &rend, &rendCount) == kCGLNoError) {
		for (rendIndex = 0; rendIndex < rendCount; rendIndex++) {
			if (CGLDescribeRenderer(rend, rendIndex, kCGLRPRendererID, &rendProp) == kCGLNoError) {
				if (rendProp == 0x00021901) break; // kCGLRendererATIRadeonX1600ID = 0x00021901.
			}
		}
		CGLDestroyRendererInfo(rend);
		if (rendIndex != rendCount) {
			// If it is loaded, check if this is this is the Mac OS X version with a dodgy version of this renderer.
			SInt32 MacVersion;
			if (Gestalt(gestaltSystemVersion, &MacVersion) == noErr) {
				if (MacVersion == 0x1047) { // Mac OS X 10.4.7.
					osg::notify() << "WARNING: Found a dodgy OpenGL driver for this CPU and Mac OS X version" << std::endl;
					if (strncmp(osgGetVersion(), "1.1", 3) == 0) {
						osg::notify() << "You MUST upgrade to OpenSceneGraph version 1.2 to avoid a crash in osgText!" << std::endl;
						sleep(10);
					} else {
						osg::notify() << "Installing workaround in osgText." << std::endl;
						
						putenv("OSG_TEXT_INCREMENTAL_SUBLOADING=OFF");
					}
				}
			}
		}
	}	
#else

#endif


}

namespace osgART {

	class Initializer 
	{
	public:

		Initializer()
		{
			osg::notify() << "osgART::Initializer()" << std::endl;

			osgARTInit(0,0);
		}

		~Initializer() 
		{
			// TrackerManager Singleton needs an explicit D'tor
			osgART::TrackerManager::destroy();


			// VideoManager Singleton needs an explicit D'tor
			osgART::VideoManager::destroy();
		}

	};
}


osgART::Initializer gs_Initializer;
