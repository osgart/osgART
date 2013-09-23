/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include <osg/MatrixTransform>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osgART/Scene>
#include <osgART/PluginManager>
#include <osgART/GeometryUtils>


int main(int argc, char* argv[])  {

	//ARGUMENTS INIT

	//VIEWER INIT

	//create a default viewer
	osgViewer::Viewer viewer;

	//setup default threading mode
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);//stats, press 's'
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);//resize, fullscreen 'f'
	viewer.addEventHandler(new osgViewer::ThreadingHandler);//threading mode, press 't'
	viewer.addEventHandler(new osgViewer::HelpHandler);//help menu, press 'h'

	//AR SCENEGRAPH INIT

	osgART::Scene* scene = new osgART::Scene();

	scene->addVideoBackground("osgart_video_artoolkit2");
	scene->addTracker("osgart_tracker_artoolkit2","data/artoolkit2/camera_para.dat");
	scene->addTrackedTransform("single;data/artoolkit2/patt.hiro;80;0;0")->addChild(osgART::testCube(80));

	//or for being able to further add/modify the target transformation:
	//osg::ref_ptr<osg::MatrixTransform> mt = scene->addTrackedTransform("single;data/artoolkit2/patt.hiro;80;0;0");
	//mt->addChild(osgART::testCube(80));


	//APPLICATION INIT

	//BOOTSTRAP INIT

	viewer.setSceneData(scene);


	//MAIN LOOP & EXIT CLEANUP

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();
	
}
