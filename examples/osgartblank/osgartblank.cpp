/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

int main(int argc, char* argv[])  {

	//VIEWER INIT

	//create a default viewer
	osgViewer::Viewer viewer;
	
	// Add relevant event handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	// You can setup threading mode directly 
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	// or consider a event handler ('t' key)
	//viewer.addEventHandler(new osgViewer::ThreadingHandler);

	//you can also optionally add a help handler ('h' key)
	//viewer.addEventHandler(new osgViewer::HelpHandler);

	//AR INIT

	// AR SCENE GRAPH INIT

	// Create a root node
	osg::ref_ptr<osg::Group> root = new osg::Group;

	//APPLICATION INIT

	//BOOTSTRAP INIT
	// Attach root node to the viewer
	viewer.setSceneData(root.get());

	//you can leave to OSG the control
	//to realize the viewer, initialize opengl, viewport, etc
	//by calling
	// return viewer.run();

	// OR doing it step by step, which give you control
	// into the main loop

	//create,display the 
	viewer.realize();

	//MAIN LOOP

	//viewer.done exit on a press key
	while (!viewer.done()) {

		//update here anything before graph traversal

		// update, culling, and draw traversal
		viewer.frame();
	}
	
}
