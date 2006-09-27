/*
 * osgART / AR Toolkit for OpenSceneGraph
 * (C) 2004-2006 HIT Lab NZ, University of Canterbury
 *
 * Licensing is governed by the LICENSE.txt which is 
 * part of this library distribution.
 *
 */

#include "osgART/Foundation"

#ifdef __APPLE__
#  include <Carbon/Carbon.h>
#  include <unistd.h>
#  include <sys/param.h>
#endif

void osgARTInit(int *argcp, char **argv)
{
#ifdef __APPLE__
		// On Mac OS X, ARToolKit Data is in "Resources" directory inside the application bundle.
		CFURLRef pathCFURLRef = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle()); // Get relative path to resources directory.
		if (pathCFURLRef) {
			char *path = (char *)calloc(MAXPATHLEN, sizeof(char)); //getcwd(path, MAXPATHLEN);
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

