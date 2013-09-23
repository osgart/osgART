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

#include <osg/PolygonMode>
#include <osg/PositionAttitudeTransform>
#include <osg/LineWidth>


class TargetProximityUpdateCallback : public osg::NodeCallback {

private:

	osg::Vec3 hotSpotAPos;
	osg::Vec3 hotSpotBPos;

	osg::ShapeDrawable* hotSpotA;
	osg::ShapeDrawable* hotSpotB;

	osgART::Target* targetA;
	osgART::Target* targetB;

	osg::MatrixTransform* arTransformA;
	osg::MatrixTransform* arTransformB;

	osg::PositionAttitudeTransform* contentA;
	osg::PositionAttitudeTransform* contentB;

public:

	TargetProximityUpdateCallback(
		osgART::Target* ltargetA,osgART::Target* ltargetB,
		osg::MatrixTransform* larTransformA,osg::MatrixTransform* larTransformB,
		osg::ShapeDrawable* lhotSpotA, osg::ShapeDrawable* lhotSpotB,
		osg::Vec3 lhotSpotAPos,osg::Vec3 lhotSpotBPos,
		osg::PositionAttitudeTransform* lcontentA,osg::PositionAttitudeTransform* lcontentB
		) : osg::NodeCallback(), 
		targetA(ltargetA),targetB(ltargetB),arTransformA(larTransformA),arTransformB(larTransformB),
		hotSpotA(lhotSpotA),hotSpotB(lhotSpotB),hotSpotAPos(lhotSpotAPos),hotSpotBPos(lhotSpotBPos),
		contentA(lcontentA),contentB(lcontentB)
	{ }                                                       

			virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

		//	{

				// Matrix that rotates from the marker grid to paddle coordinate system
				osg::Matrix rotationMatrix = osg::Matrix(arTransformB->getMatrix().getRotate());

				osg::Vec3 globalUp = osg::Z_AXIS;							// z-axis on the marker grid
				osg::Vec3 localUp = globalUp * rotationMatrix;				// z-axis on the paddle
				osg::Vec3 projection = globalUp ^ (localUp ^ globalUp);		// projection of paddle up vector onto reference plane
				float magnitude = projection.length();						// length of projection

				std::cout << projection << ", " << magnitude << std::endl;

				osg::Vec3 paddlePos = arTransformB->getMatrix().getTrans();


				osg::Vec3 offsetA = paddlePos - hotSpotAPos;
				osg::Vec3 offsetB = paddlePos - hotSpotBPos;

				float scale = 1 + projection.x();

				if (offsetA.length() < 80) {
					hotSpotA->setColor(osg::Vec4(0, 0, 1, 1));
					contentA->setScale(osg::Vec3(scale, scale, scale));
				} else {
					hotSpotA->setColor(osg::Vec4(0.4, 0.4, 0.4, 1));
				}


				if (offsetB.length() < 80) {
					hotSpotB->setColor(osg::Vec4(0, 0, 1, 1));
					contentB->setScale(osg::Vec3(scale, scale, scale));
				} else {
					hotSpotB->setColor(osg::Vec4(0.4, 0.4, 0.4, 1));
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

	//first target: multi target
	osg::ref_ptr<osgART::Target> targetA = tracker->addTarget("multi;data/artoolkit2/multi/marker.dat");
	if (!targetA.valid()) 
	{
		// Without target an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not add target!" << std::endl;
		exit(-1);
	}

	targetA->setActive(true);

	//second target: interaction target
	osg::ref_ptr<osgART::Target> targetB = tracker->addTarget("single;data/artoolkit2/patt.paddle;40;0;0");
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

	//add a geode to transform A
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();


	osg::Vec3 hotSpotAPos = osg::Vec3(-100, -50, 30);
	osg::Vec3 hotSpotBPos = osg::Vec3(100, -50, 30);

	osg::ref_ptr<osg::ShapeDrawable> hotSpotA= new osg::ShapeDrawable(new osg::Sphere(hotSpotAPos, 10));
	hotSpotA->setColor(osg::Vec4(0.4, 0.4, 0.4, 1));
	geode->addDrawable(hotSpotA.get());

	osg::ref_ptr<osg::ShapeDrawable> hotSpotB = new osg::ShapeDrawable(new osg::Sphere(hotSpotBPos, 10));
	hotSpotB->setColor(osg::Vec4(0.4, 0.4, 0.4, 1));
	geode->addDrawable(hotSpotB.get());

	arTransformA->addChild(geode.get());

	//and two objects

	osg::ref_ptr<osg::PositionAttitudeTransform> contentA = new osg::PositionAttitudeTransform();

	contentA->setPosition(osg::Vec3(-100, 50, 0));
	contentA->setScale(osg::Vec3(1, 1, 1));
	contentA->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

	arTransformA->addChild(contentA.get());

	//first model
	contentA->addChild(osgDB::readNodeFile("media/models/gist_logo.osg"));

	osg::ref_ptr<osg::PositionAttitudeTransform> contentB = new osg::PositionAttitudeTransform();
	contentB->setPosition(osg::Vec3(100, 50, 0));
	contentB->setScale(osg::Vec3(1, 1, 1));
	contentB->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

	arTransformA->addChild(contentB.get());

	//second model
	contentB->addChild(osgDB::readNodeFile("media/models/hitl_logo.osg"));


	osg::ref_ptr<osg::MatrixTransform> arTransformB = new osg::MatrixTransform();

	arTransformB->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	//here we will use a local transformation from targetA
	osgART::addEventCallback(arTransformB.get(), new osgART::LocalTransformationCallback(targetA.get(), targetB.get()));

	//and add it to transform A
	arTransformA->addChild(arTransformB.get());

	//add a cube to the targetB transform
	arTransformB->addChild(osgART::testCube(40.0f));
	arTransformB->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	arTransformB->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(2));
	arTransformB->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osgART::addEventCallback(cam.get(), new TargetProximityUpdateCallback(targetA,targetB,arTransformA.get(),arTransformB.get(),
			hotSpotA.get(),hotSpotB.get(),hotSpotAPos,hotSpotBPos,contentA.get(),contentB.get()));

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