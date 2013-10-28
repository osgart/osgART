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

#include <osg/PositionAttitudeTransform>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgART/Foundation>
#include <osgART/VideoLayer>
#include <osgART/PluginManager>
#include <osgART/VideoGeode>

#include <osgART/Utils>
#include <osgART/GeometryUtils>
#include <osgART/TrackerUtils>
#include <osgART/VideoUtils>

#include <osgART/TrackerCallback>
#include <osgART/TargetCallback>
#include <osgART/TransformFilterCallback>
#include <osgART/VideoCallback>

#include <osgManipulator/Selection>
#include <osgManipulator/CommandManager>
#include <osgManipulator/Dragger>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TrackballDragger>

#include <iostream>
#include <sstream>

#include <osgART/VisualTracker>

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

	//preload plugins
	//video plugin
	osgART::PluginManager::instance()->load("osgart_video_dummyvideo");
	//tracker plugin
	osgART::PluginManager::instance()->load("osgart_tracker_dummytracker");

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_dummyvideo"));

	// check if an instance of the video stream could be started
	if (!video.valid())
	{
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plug-in!" << std::endl;
	}


	// found video - configure now
	osgART::VideoConfiguration* _configvideo = video->getOrCreateConfiguration();

	// if the configuration is existing
	if (_configvideo)
	{
		// it is possible to configure the plugin before opening it
		_configvideo->config="data/dummyvideo/dummyvideo.png";
	}

	//you can also configure the plugin using fields
	//before/after open/start in function of the specific field variable/function
	video->getField < osgART::TypedField<bool>* >("flip_vertical")->set(true);	

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for connecting a tracker
	// Note: configuration should be defined before opening the video
	video->init();

	osg::ref_ptr<osgART::VisualTracker> tracker 
		= dynamic_cast<osgART::VisualTracker*>(osgART::PluginManager::instance()->get("osgart_tracker_dummytracker"));

	if (!tracker.valid())
	{
		// Without tracker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plug-in!" << std::endl;

		return -1;

	}

	// found tracker - configure now
	osgART::TrackerConfiguration* _configtracker = tracker->getOrCreateConfiguration();

	// if the configuration is existing
	if (_configtracker)
	{
		// it is possible to configure the plugin before opening it
		_configtracker->config="mode=0;";
	}

	// get the tracker camera configuration object
	osg::ref_ptr<osgART::CameraConfiguration> cameraconfig = tracker->getOrCreateCameraConfiguration();
	cameraconfig->load("");


	// setup one target
	osg::ref_ptr<osgART::Target> target = tracker->addTarget("test.pattern;35.2;22.0;0.3");
	
	target->setActive(true);

	tracker->setImage(video->getStream());

	tracker->init();


	//AR SCENEGRAPH INIT
	
	//create root 
	osg::ref_ptr<osg::Group> root = new osg::Group;

	//add video update callback (update video + video stream)
	osgART::VideoUpdateCallback::addOrSet(root.get(),video.get());

	//add tracker update callback (update tracker from video stream)
	osgART::TrackerUpdateCallback::addOrSet(root.get(),tracker.get());

	//add a video background
	osg::ref_ptr<osg::Group> videoBackground = osgART::createBasicVideoBackground(video->getStream());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

	root->addChild(videoBackground.get());

	//add a virtual camera
	osg::ref_ptr<osg::Camera> cam = osgART::createBasicCamera(cameraconfig);
	root->addChild(cam.get());

	//add a target transform callback (update transform from target information)
	osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();
	arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	osgART::attachDefaultTargetCallbacks(arTransform.get(), target.get());

	cam->addChild(arTransform.get());

	//adjust window size
	//viewer.setUpViewInWindow(0,0,800,800);

	//adjust camera viewport
	//cam->setViewport(0,0,800,800); //RESOLUTION same as VIEWER

	//add our manipulator handle
	viewer.addEventHandler(new MouseManipulatorEventHandler(cam.get()));

	osg::ref_ptr<osg::MatrixTransform> geom1 = 
		new osg::MatrixTransform(osg::Matrixd::scale(osg::Vec3f(1.0,1.0,1.0)));
	geom1->addChild(osgDB::readNodeFile("media/models/cow.ive"));

	osg::ref_ptr<osg::Group> group = new osg::Group();
	arTransform->addChild(group.get());

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
		new osg::ShapeDrawable(new osg::Cone(osg::Vec3(-10.0,0.0,3.0), 2.0f, 5.0f));
	shape->setColor(osg::Vec4(0.2f, 0.6f, 0.2f, 1.0f));
	geom2->addDrawable(shape.get());
 
	/** osg::Group node **/
	group = new osg::Group();
	arTransform->addChild(group.get());    
 
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


	//BOOTSTRAP INIT
	viewer.setSceneData(root.get());

	viewer.realize();

	//video start
	video->start();

	//tracker start
	tracker->start();


	//MAIN LOOP
	while (!viewer.done()) {
		viewer.frame();
	}

	//EXIT CLEANUP

	//tracker stop
	tracker->stop();

	//video stop
	video->stop();

	//tracker open
	tracker->close();

	//video open
	video->close();

	return 0;
}
