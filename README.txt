Read me for osgART 2.0
=======================


Contents.
---------
About this archive.
About osgART.

The osgART license

Building on Windows.
Building on Linux.
Building on Mac OS X.
Changes in this release.
Known issues in this release.
Changes in earlier releases.


About this archive.
-------------------
This archive contains the osgART 2.0 GPL 3.0 version

This archive was assembled by:

	Hartmut Seichter
	Human Interface Technology Laboratory New Zealand (HIT Lab NZ)
	University of Canterbury
	Private Bag 4800
	8041 Christchurch
	


About osgART.
-------------
osgART is a library that simplifies the development of Augmented Reality or Mixed Reality applications by combining the well-known ARToolKit tracking library with OpenSceneGraph. But rather than acting just as a simple nodekit, the library offers 3 main functionalities: high level integration of video input (video object, shaders), spatial registration (marker-based, multiple trackers), and photometric registration (occlusion, shadow).

With osgART, users gain the benefit of all the features of OpenSceneGraph (high quality renderer, multiple file type loaders, community nodekits like osgAL, etc.) directly in their augmented reality (AR), mixed reality (MR) or mediated reality applications.
As with the standard ARToolKit, the user can thus develop and prototype interactive applications that can use tangible interaction (in C++, Python, Lua, Ruby etc.).

The latest version of osgART 2.0 is always available from http://www.osgart.org

For the commercial version of osgART 1.1 please visit http://artoolworks.com

osgART 2.0 is supported on the Linux, Windows, and Macintosh OS X platforms.


The osgART 2.0 license
----------------------

osgART 2.0 is licensed under the terms of the GPL 3.0 which is included in this distribution as LICENSE.txt
Before you are using osgART 2.0. For other licensing options please contact the Human Interface Laboratory New Zealand (HIT Lab NZ) or write directly to licensing@osgart.org

ARToolworks has exclusive rights for licensing osgART 1.1, please visit http://www.artoolworks.com


Building on Windows / Mac OS X / Linux.
---------------------------------------

Please visit http://www.osgart.org for tutorials on how to use osgART 2.0



Changes in version 2.0 (this release)
-------------------------------------

Too many to list



Changes in version 1.1 (2007-04-11).
---------------------------------------------------
Although osgART functionality remains similar, version 1.1 includes major architectural changes over version 1.0.
- The ARToolKit tracker is now self-contained in a plugin, allowing different versions of ARToolKit (open source v2.x vs. Professional v4.x, NFT) to be used, as well as other tracker libraries.
- Video plugins now return frames in the standard osg::Image type, allowing video to be more easily utilised elsewhere in the scene graph.
- Tracker plugins now accept the standard osg::Image type.
- Connection of the video and the tracker is achieved using a new OSG node type, ARSceneNode, which is subclassed from osg::Group, eliminating the need for a separate run loop inside osgART.
- The video background functionality has been restructured, eliminating some small utility classes in favour of achieving the same effect with standard OSG calls.
- Passing of data to and from the video and tracker plugins is now managed through fields, dynamically typed accessors which allow for plugins to accept and supply a variety of different data types.



Known issues in this release.
-----------------------------
- This release will break previously working code targeted at osgART-1.0. Please use the osgARTTest.cpp sample to update your usage.
- Due to an issue in OpenSceneGraph, YCbCr-format video streams are not yet supported. On Mac OS X, make sure that ARToolKit's default video format is ARGB.
- The Mac osgARTTest opens in multiple windows on a multi-display system.
- The Mac XCode project file includes targets for only the osgARTTest example. The other exmples will be added later.


Changes in earlier releases.
----------------------------
Please see the file ChangeLog.txt.


--
EOF
