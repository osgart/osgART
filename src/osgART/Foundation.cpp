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
#endif

//
// Private globals.
//
#ifdef __APPLE__
static char g_cwd[MAXPATHLEN] = "";
#endif __APPLE__

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
#endif __APPLE__
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
		// On Mac OS X, ARToolKit Data is in "Resources" directory inside the application bundle.
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
#endif __APPLE__

}

