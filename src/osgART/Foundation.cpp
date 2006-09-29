/*
 * osgART / AR Toolkit for OpenSceneGraph
 * (C) 2004-2006 HIT Lab NZ, University of Canterbury
 *
 * Licensing is governed by the LICENSE.txt which is 
 * part of this library distribution.
 *
 */

//
// Includes.
//
#include "osgART/Foundation"

#include <stdlib.h>

#ifdef __APPLE__
#  include <Carbon/Carbon.h>
#  include <unistd.h>
#  include <sys/param.h>
#  include <OpenGL/OpenGL.h>
#  include <osgText/Version>
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
	// Register exit function before anything else.
	atexit(osgARTFinal);
	
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
					printf("WARNING: Found a dodgy OpenGL driver for this CPU and Mac OS X version.\n");
					if (strncmp(osgTextGetVersion(), "1.1", 3) == 0) {
						printf("You MUST upgrade to OpenSceneGraph version 1.2 to avoid a crash in osgText!!\n");
						sleep(10);
					} else {
						printf("Using workaround in osgText.\n");
						putenv("OSG_TEXT_INCREMENTAL_SUBLOADING=OFF");
					}
				}
			}
		}
	}	
#endif	
}

