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

// A simple example to demonstrate picking with osgART

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
#include <osgART/ImageStreamCallback>

#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <iostream>
#include <sstream>

class PickEventHandler : public osgGA::GUIEventHandler {

public:
	PickEventHandler() : osgGA::GUIEventHandler() { }

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) {

		switch (ea.getEventType()) {

			case osgGA::GUIEventAdapter::PUSH:

				osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
				osgUtil::LineSegmentIntersector::Intersections intersections;

				if (view && view->computeIntersections(ea.getX(), ea.getY(), intersections)) {
					for (osgUtil::LineSegmentIntersector::Intersections::iterator iter = intersections.begin(); iter != intersections.end(); iter++) {
						if (iter->nodePath.back()->getName() == "target") {
							std::cout << "HIT!" << std::endl;
							return true;
						}
					}
				}

				break;
		}
		return false;
	}
};


int main(int argc, char* argv[])  {

	osgViewer::Viewer viewer;

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);


	// preload the video and tracker
	osgART::PluginManager::instance()->load("osgart_video_dummyvideo");
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
	osgART::VideoConfiguration* _configvideo = video->getConfiguration();

	// if the configuration is existing
	if (_configvideo)
	{
		// it is possible to configure the plugin before opening it
		_configvideo->config="Data/dummyvideo/dummyvideo.png";
	}

	//you can also configure the plugin using fields
	//before/after open/start in function of the specific field variable/function
	video->getField < osgART::TypedField<bool>* >("flip_vertical")->set(true);	

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for connecting a tracker
	// Note: configuration should be defined before opening the video
	video->open();

	osg::ref_ptr<osgART::Tracker> tracker 
		= dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get("osgart_tracker_dummytracker"));

	if (!tracker.valid())
	{
		// Without tracker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plug-in!" << std::endl;

		return -1;

	}

	// found tracker - configure now
	osgART::TrackerConfiguration* _configtracker = tracker->getConfiguration();

	// if the configuration is existing
	if (_configtracker)
	{
		// it is possible to configure the plugin before opening it
		_configtracker->config="mode=0;";
	}

	// get the tracker calibration object
	osg::ref_ptr<osgART::Calibration> calibration = tracker->getOrCreateCalibration();
	calibration->load("");


	// setup one target
	osg::ref_ptr<osgART::Target> target = tracker->addTarget("test.pattern;35.2;22.0;0.3");

	target->setActive(true);

	tracker->setImage(video.get());

	tracker->init();

	// AR SCENE GRAPH INIT
	osg::ref_ptr<osg::Group> root = new osg::Group;

	if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(video.get())) {
		osgART::addEventCallback(root.get(), new osgART::ImageStreamCallback(imagestream));
	}

	osgART::TrackerCallback::addOrSet(root.get(),tracker.get());

	osg::ref_ptr<osg::Group> videoBackground = osgART::createBasicVideoBackground(video.get());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
	root->addChild(videoBackground.get());

	osg::ref_ptr<osg::Camera> cam = osgART::createBasicCamera(calibration);
	root->addChild(cam.get());

	osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();

	//we don't use the default callback
	osgART::attachDefaultEventCallbacks(arTransform.get(), target.get());

	//but specify it by hand
	//osgART::addEventCallback(arTransform.get(), new osgART::TargetTransformCallback(target.get()));
	//osgART::addEventCallback(arTransform.get(), new osgART::TargetVisibilityCallback(target.get()));
	//osgART::addEventCallback(arTransform.get(), new osgART::TransformFilterCallback());


	//and we add a pick handler
	viewer.addEventHandler(new PickEventHandler());

	//create cube
	osg::Geode* cube = osgART::testCube(20);

	//name the cube
	cube->setName("target");
	arTransform->addChild(cube);

	arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");
	cam->addChild(arTransform.get());

	//APPLICATION INIT

	//for the demo we activate notification level to debug
	//to see log of video call
	//osg::setNotifyLevel(osg::DEBUG_INFO);

	//BOOTSTRAP INIT
	viewer.setSceneData(root.get());

	viewer.realize();

	video->start();

	tracker->start();

	while (!viewer.done()) {
		viewer.frame();
	}

	return 0;

}

/*#if OSGART_DEPRECATED_FIELDS

	osg::ref_ptr< osgART::TypedField<bool> > historyField = reinterpret_cast< osgART::TypedField<bool>* >(tracker->get("use_history"));
	historyField->set(true);

#endif*/
