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
#include <osgART/GeometryUtils>
#include <osgART/PluginManager>

#include <osgManipulator/Selection>
#include <osgManipulator/CommandManager>
#include <osgManipulator/Dragger>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TrackballDragger>

osgManipulator::PointerInfo pointerInfo;
osgManipulator::Dragger* activeDragger = NULL;

typedef osgUtil::LineSegmentIntersector::Intersections::iterator intersectIter;
typedef osg::NodePath::iterator npIter;

class MouseManipulatorEventHandler : public osgGA::GUIEventHandler {

	osg::Camera* mCamera;
public:
	MouseManipulatorEventHandler(osg::Camera* camera) : osgGA::GUIEventHandler() { mCamera=camera;}                                                       

	virtual bool handle(const osgGA::GUIEventAdapter& ea,
		osgGA::GUIActionAdapter& aa, 
		osg::Object* obj, 
		osg::NodeVisitor* nv) { 

			bool somethingChanged = false;

			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) {

				switch (ea.getEventType()) {

				case osgGA::GUIEventAdapter::PUSH: {

					osgUtil::LineSegmentIntersector::Intersections intersections;
					pointerInfo.reset();

					if (view->computeIntersections(ea.getX(), ea.getY(), intersections)) {

						pointerInfo.setCamera(mCamera);
						pointerInfo.setMousePosition(ea.getX(), ea.getY());

						for (intersectIter iter = intersections.begin(); 
							iter != intersections.end(); 
							++iter) {
								pointerInfo.addIntersection(iter->nodePath, iter->getLocalIntersectPoint());
						}

						for (npIter iter = pointerInfo._hitList.front().first.begin(); 
							iter != pointerInfo._hitList.front().first.end(); 
							++iter) {	

								if (osgManipulator::Dragger* dragger = 
									dynamic_cast<osgManipulator::Dragger*>(*iter)) {
										dragger->handle(pointerInfo, ea, aa);
										activeDragger = dragger;
										return false;
								}
						}
					}
												   }
												   break;

				case osgGA::GUIEventAdapter::RELEASE:
				case osgGA::GUIEventAdapter::DRAG:

					if (activeDragger) {
						pointerInfo._hitIter = pointerInfo._hitList.begin();
						pointerInfo.setCamera(mCamera);
						pointerInfo.setMousePosition(ea.getX(), ea.getY());
						activeDragger->handle(pointerInfo, ea, aa);
						return false;
					}
					break;

				} 
			}
			return false;
	}
};


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

	scene->addVideoBackground("osgart_video_artoolkit");
	scene->addTracker("osgart_tracker_artoolkit","data/artoolkit/camera_para.dat");

	osg::ref_ptr<osg::MatrixTransform> mt = scene->addTrackedTransform("single;data/artoolkit/patt.hiro;80;0;0");

	osg::ref_ptr<osg::Camera> cam=scene->getCamera(); //call after addTracker

	//adjust window size
	viewer.setUpViewInWindow(0,0,800,800);

	//adjust camera viewport
	cam->setViewport(0,0,800,800); //RESOLUTION same as VIEWER

	//add our manipulator handle
	viewer.addEventHandler(new MouseManipulatorEventHandler(cam.get()));

	osg::ref_ptr<osg::MatrixTransform> geom1 = 
		new osg::MatrixTransform(osg::Matrixd::scale(osg::Vec3f(8.0,4.0,8.0)));
	geom1->addChild(osgDB::readNodeFile("media/models/cow.ive"));

	osg::ref_ptr<osg::Group> group = new osg::Group();
	mt->addChild(group.get());

	osg::ref_ptr<osgManipulator::Selection> selection = 
		new osgManipulator::Selection();
	group->addChild(selection.get());
	selection->addChild(geom1);

	osg::ref_ptr<osgManipulator::Dragger> dragger = 
		new osgManipulator::TrackballDragger();
	dragger->setupDefaultGeometry();
	group->addChild(dragger.get());

	float scale = geom1->getBound().radius() * 1.5f;
	osg::Matrix mat = osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(geom1->getBound().center());
	dragger->setMatrix(mat);

	osg::ref_ptr<osgManipulator::CommandManager> commandManager = 
		new osgManipulator::CommandManager();
	commandManager->connect(*(dragger.get()), *(selection.get()));

	/** The Geometry for the TabBoxDragger  **/
	osg::ref_ptr<osg::Geode> geom2 = new osg::Geode();
	osg::ref_ptr<osg::ShapeDrawable> shape = 
		  new osg::ShapeDrawable(new osg::Cone(osg::Vec3(-100.0,0.0,30.0), 20.0f, 50.0f));
	shape->setColor(osg::Vec4(0.2f, 0.6f, 0.2f, 1.0f));
	geom2->addDrawable(shape.get());
 
	/** osg::Group node **/
	group = new osg::Group();
	mt->addChild(group.get());    
 
	/** Selection Node **/
	selection =  new osgManipulator::Selection();
	group->addChild(selection.get());
	selection->addChild(geom2);
 
	/** Dragger Node:
	  * This one is a TabBoxDragger for both scaling and translation **/
	dragger = new osgManipulator::TabBoxDragger();
	dragger->setupDefaultGeometry();
	group->addChild(dragger.get());
 
	/** Starting matrix for the Dragger **/
	scale = geom2->getBound().radius() * 1.5f;
	mat = osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(geom2->getBound().center());
	dragger->setMatrix(mat);
 
	/** Command Manager - connects Dragger objects with Selection objects **/
	commandManager->connect(*(dragger.get()), *(selection.get()));

	//APPLICATION INIT

	//BOOTSTRAP INIT

	viewer.setSceneData(scene);

	//MAIN LOOP & EXIT CLEANUP

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();	
}
