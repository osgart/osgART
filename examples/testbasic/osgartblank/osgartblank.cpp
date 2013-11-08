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

#include <osgGA/TrackballManipulator>

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
		
	osgGA::TrackballManipulator* tb = new osgGA::TrackballManipulator;
	
	//gl style
	tb->setHomePosition( osg::Vec3f(0,0,20), osg::Vec3f(0,0,0), osg::Vec3f(0,1,0) );
	
	//standard
	//tb->setHomePosition( osg::Vec3f(0,-20,0), osg::Vec3f(0,0,0), osg::Vec3f(0,0,1) );
	viewer.setCameraManipulator(tb);

	viewer.setUpViewInWindow(0,0,800,600);
	//AR INIT


	//AR SCENEGRAPH INIT

	// Create a root node
	osg::ref_ptr<osg::Group> root = new osg::Group;


	//APPLICATION INIT
	//root->addChild(osgART::createTopCube(10.));
	//root->addChild(osgART::createTopSphere(10.));
	//root->addChild(osgART::createCube(10.));
	//root->addChild(osgART::createSphere(10.));
	root->addChild(osgART::create3DAxis(10.,5.));
	root->addChild(osgART::translateModel(osgART::create2DAxisRH(10.,5.),osg::Vec3f(0.,0.,-5.)));
	root->addChild(osgART::createFiniteGrid());

	//BOOTSTRAP INIT

	// Attach root node to the viewer
	viewer.setSceneData(root.get());


	//-- SOLUTION 1 (simple) :run call --
	//you can leave to OSG the control
	//to realize the viewer, initialize opengl, viewport, etc
	//by calling

	//MAIN LOOP & EXIT CLEANUP
	// return viewer.run();
	 


	//-- SOLUTION 2 (interm): frame loop --
	// OR doing it step by step, which give you control
	// into the main loop

	//create,display the view
	viewer.realize();


	//MAIN LOOP

	//viewer.done exit on a press key
	while (!viewer.done()) {

		//update here anything before graph traversal

		// update, culling, and draw traversal
		viewer.frame();
	}
	
	//EXIT CLEANUP

}
