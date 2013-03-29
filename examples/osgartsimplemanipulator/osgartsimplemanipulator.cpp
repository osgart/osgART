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

#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/FileUtils>

#include <osg/MatrixTransform>

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

class MouseManipulationEventHandler : public osgGA::GUIEventHandler {

	osg::Camera* mCamera;
public:
	MouseManipulationEventHandler(osg::Camera* camera) : osgGA::GUIEventHandler() { mCamera=camera;}                                                       

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

	// Read the filename from the command line or use a default model
	std::string filename = "media/models/osgart.ive";

	if (argc > 1) filename = std::string(argv[1]);
	std::cout << "Using model: " << filename << std::endl;


	osgART::PluginManager::instance()->load("osgart_video_dummyvideo");
	osgART::PluginManager::instance()->load("osgart_tracker_dummytracker");

	osgViewer::Viewer viewer;
	
	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::ThreadingHandler);
	viewer.addEventHandler(new osgViewer::HelpHandler);


	osgART::Scene* scene = new osgART::Scene();

	viewer.setUpViewInWindow(0,0,1400,1400);

	scene->addVideoBackground("osgart_video_dummyvideo","Data/dummyvideo/dummyvideo.png");
	scene->addTracker("osgart_tracker_dummytracker","","mode=0;");

	osg::MatrixTransform* mt = scene->addTrackedTransform("test.pattern;35.2;22.0;0.3");

	osg::ref_ptr<osg::Camera> camera=scene->getCamera(); //call after addTracker

	camera->setViewport(0,0,1400,1400); //RESOLUTION same as VIEWER
	viewer.addEventHandler(new MouseManipulationEventHandler(camera.get()));

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


	viewer.setSceneData(scene);

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();
	
}
