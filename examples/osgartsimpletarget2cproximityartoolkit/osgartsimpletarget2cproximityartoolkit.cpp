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

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/FileUtils>

#include <osgART/Scene>
#include <osgART/GeometryUtils>

#include <osgART/PluginManager>


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

	//AR INIT

	//AR SCENEGRAPH INIT

	osgART::Scene* scene = new osgART::Scene();

	scene->addVideoBackground("osgart_video_artoolkit2");
	scene->addTracker("osgart_tracker_artoolkit2","data/artoolkit2/camera_para.dat");
	
	osg::ref_ptr<osg::MatrixTransform> mt = scene->addTrackedTransform("single;data/artoolkit2/patt.hiro;80;0;0");
	
	// Load some models
	osg::ref_ptr<osg::Node> farNode = osgDB::readNodeFile("media/models/far.osg");
	osg::ref_ptr<osg::Node> closerNode = osgDB::readNodeFile("media/models/closer.osg");
	osg::ref_ptr<osg::Node> nearNode = osgDB::readNodeFile("media/models/near.osg");

	// Use a Level-Of-Detail node to show each model at different distance ranges.
	osg::ref_ptr<osg::LOD> lod = new osg::LOD();		
	lod->addChild(farNode.get(), 500.0f, 10000.0f);			// Show the "far" node from 50cm to 10m away
	lod->addChild(closerNode.get(), 200.0f, 500.0f);		// Show the "closer" node from 20cm to 50cm away
	lod->addChild(nearNode.get(), 0.0f, 200.0f);			// Show the "near" node from 0cm to 2cm away

	//you can also do both of these steps in one (if you don't consider to modify your far/closer/nearNode)
	//lod->addChild(osgDB::readNodeFile("media/far.osg"), 500.0f, 10000.0f);		// Show the "far" node from 50cm to 10m away
	//lod->addChild(osgDB::readNodeFile("media/closer.osg"), 200.0f, 500.0f);		// Show the "closer" node from 20cm to 50cm away
	//lod->addChild(osgDB::readNodeFile("media/near.osg"), 0.0f, 200.0f);			// Show the "near" node from 0cm to 2cm away

	mt->addChild(lod.get());

	//APPLICATION INIT

	//BOOTSTRAP INIT

	viewer.setSceneData(scene);

	//MAIN LOOP & EXIT CLEANUP

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();	
}
