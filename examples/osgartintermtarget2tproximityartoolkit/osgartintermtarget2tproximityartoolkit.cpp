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
#include <osgART/ImageStreamCallback>

#include <iostream>
#include <sstream>


class TargetProximityUpdateCallback : public osg::NodeCallback {
 
	private:
		osg::MatrixTransform* mtA;
		osg::MatrixTransform* mtB;
 
		osg::Switch* mSwitchA;
		osg::Switch* mSwitchB;
 
		float mThreshold;
 
	public:
 
		TargetProximityUpdateCallback(
		osg::MatrixTransform* mA, osg::MatrixTransform* mB, 
			osg::Switch* switchA, osg::Switch* switchB,	float threshold) : 
			                     osg::NodeCallback(), 
					    mtA(mA), mtB(mB),
					    mSwitchA(switchA), mSwitchB(switchB),
					    mThreshold(threshold) { }
 
 
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
 
			/** CALCULATE INTER-TARGET PROXIMITY:
				Here we obtain the current position of each target, and the
				distance between them by examining
				the translation components of their parent transformation 
				matrices **/
			osg::Vec3 posA = mtA->getMatrix().getTrans();
			osg::Vec3 posB = mtB->getMatrix().getTrans();
			osg::Vec3 offset = posA - posB;
			float distance = offset.length();
 
			/** LOAD APPROPRIATE MODELS:
				Here we use each target's OSG Switch node to swap between
				models, depending on the inter-target distance we have just 
				calculated. **/
			if (distance <= mThreshold) {
				if (mSwitchA->getNumChildren() > 1) mSwitchA->setSingleChildOn(1);
				if (mSwitchB->getNumChildren() > 1) mSwitchB->setSingleChildOn(1);
			} else {
				if (mSwitchA->getNumChildren() > 0) mSwitchA->setSingleChildOn(0);
				if (mSwitchB->getNumChildren() > 0) mSwitchB->setSingleChildOn(0);
			}
 
			traverse(node,nv);
 
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

	// setup two targets

	//first target
	osg::ref_ptr<osgART::Target> targetA = tracker->addTarget("single;data/artoolkit2/patt.hiro;80;0;0");
	if (!targetA.valid()) 
	{
		// Without target an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not add target!" << std::endl;
		exit(-1);
	}

	targetA->setActive(true);

	//second target
	osg::ref_ptr<osgART::Target> targetB = tracker->addTarget("single;data/artoolkit2/patt.kanji;80;0;0");
	if (!targetB.valid()) 
	{
		// Without target an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not add target!" << std::endl;
		exit(-1);
	}

	targetB->setActive(true);

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

	//add two transforms: one for each target
	osg::ref_ptr<osg::MatrixTransform> arTransformA = new osg::MatrixTransform();

	arTransformA->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");
	osgART::attachDefaultEventCallbacks(arTransformA.get(), targetA.get());

	cam->addChild(arTransformA.get());

	//add a switch to the targetA transform
	osg::ref_ptr<osg::Switch> switchA = new osg::Switch();
	switchA->addChild(osgDB::readNodeFile("media/models/voltmeter_low.osg"), true);
	switchA->addChild(osgDB::readNodeFile("media/models/voltmeter_high.osg"), false);
	arTransformA->addChild(switchA.get());

	osg::ref_ptr<osg::MatrixTransform> arTransformB = new osg::MatrixTransform();

	arTransformB->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");
	osgART::attachDefaultEventCallbacks(arTransformB.get(), targetB.get());

	cam->addChild(arTransformB.get());

	//add a switch to the targetB transform
	osg::ref_ptr<osg::Switch> switchB = new osg::Switch();
	switchB->addChild(osgDB::readNodeFile("media/models/battery.osg"), true);
	switchB->addChild(osgDB::readNodeFile("media/models/battery_spark.osg"), false);
	arTransformB->addChild(switchB.get());

	cam->setUpdateCallback(new TargetProximityUpdateCallback(arTransformA, arTransformB, switchA.get(), switchB.get(), 200.0f));

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