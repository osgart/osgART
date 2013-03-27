/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2008 Human Interface Technology Laboratory New Zealand
 * 
 * This file is part of osgART 2.0
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

#include <osgDB/FileUtils>

#include <osgART/Scene>
#include <osgART/GeometryUtils>

#include <osgART/PluginManager>


int main(int argc, char* argv[])  {


	osgART::PluginManager::instance()->load("osgart_video_dummyvideo");
	osgART::PluginManager::instance()->load("osgart_tracker_dummytracker");

	osgViewer::Viewer viewer;
	
	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::ThreadingHandler);
	viewer.addEventHandler(new osgViewer::HelpHandler);

	//osgART::Display* display=new osgART::Display(osgART::STANDARD_DISPLAY);
	//osgART::StereoDisplay* display=new osgART::Display
	//display->getView()->
	//display->setPosSize(0,0,640,480);
	//display->setPosSize(video.size());


	osgART::Scene* scene = new osgART::Scene();
	scene->addVideoBackground("osgart_video_dummyvideo");
	scene->addTracker("osgart_tracker_dummytracker");
	scene->addTrackedTransform("test.pattern;35.2;22.0;0.3")->addChild(osgART::testCube());
	
	//or:
	//osg::ref_ptr<osg::MatrixTransform> mt = scene->addTrackedTransform("single;data/patt.hiro;80;0;0");
	//mt->addChild(osgART::testCube());

	viewer.setSceneData(scene);

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();
	
}
