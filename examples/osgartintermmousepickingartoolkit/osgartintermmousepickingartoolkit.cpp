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
#include <osgART/ImageStreamCallback>

#include <iostream>
#include <sstream>


class HitTargetGeode : public osg::Geode {

private:

	osg::ref_ptr<osg::ShapeDrawable> mShapeDrawable;

public:

	HitTargetGeode(osg::Vec3 position, float size) : osg::Geode() {
		osg::Box* box = new osg::Box(position, size);
		mShapeDrawable = new osg::ShapeDrawable(box);
		this->addDrawable(mShapeDrawable.get());
		setSelected(false);
	}

	void setSelected(bool selected) {
		if (selected) mShapeDrawable->setColor(osg::Vec4(1, 0, 0, 1));
		else mShapeDrawable->setColor(osg::Vec4(1, 1, 1, 1));
	}

};


std::vector< osg::ref_ptr<HitTargetGeode> > hittargetlist;


class MousePickingEventHandler : public osgGA::GUIEventHandler {

public:
	MousePickingEventHandler() : osgGA::GUIEventHandler() { }                                                       

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) { 

		switch (ea.getEventType()) {

		case osgGA::GUIEventAdapter::PUSH:

			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			osgUtil::LineSegmentIntersector::Intersections intersections;

			// Clear previous selections
			for (unsigned int i = 0; i < hittargetlist.size(); i++) {
				hittargetlist[i]->setSelected(false);
			}

			// Find new selection based on click position
			if (view && view->computeIntersections(ea.getX(), ea.getY(), intersections)) {				
				for (osgUtil::LineSegmentIntersector::Intersections::iterator iter = intersections.begin(); iter != intersections.end(); iter++) {							
					if (HitTargetGeode* hittarget = dynamic_cast<HitTargetGeode*>(iter->nodePath.back())) {
						std::cout << "HIT!" << std::endl;	
						hittarget->setSelected(true);
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
	osgART::PluginManager::instance()->load("osgart_video_artoolkit2");
	//tracker plugin
	osgART::PluginManager::instance()->load("osgart_tracker_artoolkit2");

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_artoolkit2"));

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

		//artoolkit2 plugin will generate a default configuration for you
		//if you omit this line
		//here we use the default config file in the artoolkit2 data directory
		_configvideo->config="Data/artoolkit2/WDM_camera.xml";

		//you can also specify configuration file here:
		//_config->deviceconfig = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		//	"<dsvl_input><avi_file use_reference_clock=\"true\" file_name=\"Data\\MyVideo.avi\" loop_avi=\"true\" render_secondary=\"true\">"
		//	"<pixel_format><RGB32/></pixel_format></avi_file></dsvl_input>";

	}

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for connecting a tracker
	// Note: configuration should be defined before opening the video
	video->open();

	osg::ref_ptr<osgART::Tracker> tracker 
		= dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get("osgart_tracker_artoolkit2"));

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
		//artoolkit2: no configuration
		_configtracker->config="";
	}

	// get the tracker calibration object
	osg::ref_ptr<osgART::Calibration> calibration = tracker->getOrCreateCalibration();
	
	// load a calibration file
	if (!calibration->load("data/artoolkit2/camera_para.dat")) 
	{
		// the calibration file was non-existing or couldn't be loaded
		osg::notify(osg::FATAL) << "Non existing or incompatible calibration file" << std::endl;
		exit(-1);
	}

	// setup one target
	osg::ref_ptr<osgART::Target> target = tracker->addTarget("single;data/artoolkit2/patt.hiro;80;0;0");
	
	target->setActive(true);

	tracker->setImage(video.get());

	tracker->init();


	//AR SCENEGRAPH INIT
	//create root 
	osg::ref_ptr<osg::Group> root = new osg::Group;

	//add video update callback (update video stream)
	if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(video.get())) {
		osgART::addEventCallback(root.get(), new osgART::ImageStreamCallback(imagestream));
	}

	//add tracker update callback (update tracker from video stream)
	osgART::TrackerCallback::addOrSet(root.get(),tracker.get());

	//add a video background
	osg::ref_ptr<osg::Group> videoBackground = osgART::createBasicVideoBackground(video.get());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

	root->addChild(videoBackground.get());

	//add a virtual camera
	osg::ref_ptr<osg::Camera> cam = osgART::createBasicCamera(calibration);
	root->addChild(cam.get());

	//add a target transform callback (update transform from target information)
	osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();
	arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	osgART::attachDefaultEventCallbacks(arTransform.get(), target.get());

	cam->addChild(arTransform.get());

	//add a cube to the transform node
	//adding our mouse picking event handler
	viewer.addEventHandler(new MousePickingEventHandler());

	// Settings for a grid of targets
	float size = 40.0f;
	float space = 50.0f;
	unsigned int width = 5;
	unsigned int height = 5;

	// Create a grid of hit targets
	for (unsigned int x = 0; x < width; x++) {
		for (unsigned int y = 0; y < height; y++) {
			float px = -(width * space * 0.5f) + x * space;
			float py = -(height * space * 0.5f) + y * space;
			float pz = size * 0.5f;
			osg::ref_ptr<HitTargetGeode> hittarget = new HitTargetGeode(osg::Vec3(px, py, pz), size);
			hittargetlist.push_back(hittarget);
			arTransform->addChild(hittarget.get());
		}
	}

	//APPLICATION INIT


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

