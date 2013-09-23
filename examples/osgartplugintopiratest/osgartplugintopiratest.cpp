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

// A simple example to demonstrate OPIRA with osgART

#include <osgART/Foundation>
#include <osgART/VideoLayer>
#include <osgART/PluginManager>
#include <osgART/VideoGeode>
#include <osgART/Utils>
#include <osgART/GeometryUtils>
#include <osgART/TargetCallback>
#include <osgART/TransformFilterCallback>
#include <osgART/ImageStreamCallback>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>

#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

osg::Group* createImageBackground(osg::Image* video, osgART::Calibration* calibration = NULL, bool useTextureRectangle = false) {
	osgART::VideoLayer* _layer = new osgART::VideoLayer();
	osgART::VideoGeode* _geode = new osgART::VideoGeode(video, calibration, 1, 1, 20, 20,
		useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D);
	_layer->addChild(_geode);
	return _layer;
}


int main(int argc, char* argv[])  {

	osgViewer::Viewer viewer;
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);

	std::string policy = "opira";
	std::string image = "MagicLand.bmp";
	std::string featureDetector = "OCVSURF";

	osg::ArgumentParser parser(&argc, argv);
	parser.read("-policy", policy);
	parser.read("-image", image);
	parser.read("-fd", featureDetector);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	viewer.setSceneData(root.get());

	osgART::PluginManager::instance()->load("osgart_video_artoolkit2");//videoinput");

	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_artoolkit2"));

	// check if an instance of the video stream could be started
	if (!video.valid())  {
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
		exit(-1);
	}

	//video->getVideoConfiguration()->deviceconfig = "0;320;240;30";


	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();


	osgART::PluginManager::instance()->load("osgart_tracker_opira");

	osg::ref_ptr<osgART::Tracker> tracker = dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get("osgart_tracker_opira"));

	if (!tracker.valid())  {
		// Without tracker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plugin!" << std::endl;
		exit(-1);
	}

#if OSGART_DEPRECATED_FIELDS

	osg::ref_ptr< osgART::TypedField<std::string> > registrationPolicyField = reinterpret_cast< osgART::TypedField<std::string>* >(tracker->get("registration_policy"));
	if (!registrationPolicyField.valid()) {
		std::cout << "Registration policy field not available from tracker" << std::endl;
		return -1;
	}
	registrationPolicyField->set(policy);

	osg::ref_ptr< osgART::TypedField<std::string> > featureDetectorField = reinterpret_cast< osgART::TypedField<std::string>* >(tracker->get("feature_detector"));
	if (!featureDetectorField.valid()) {
		std::cout << "Feature detector field not available from tracker" << std::endl;
		return -1;
	}
	featureDetectorField->set(featureDetector);

#endif

	// get the tracker calibration object
	osg::ref_ptr<osgART::Calibration> calibration = tracker->getOrCreateCalibration();

	// load a calibration file
	if (!calibration->load("camera.yml")) {
		// the calibration file was non-existing or couldnt be loaded
		osg::notify(osg::FATAL) << "Non existing or incompatible calibration file" << std::endl;
		exit(-1);
	}

	tracker->setImage(video.get());

	osgART::TrackerCallback::addOrSet(root.get(),tracker.get());

	if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(video.get())) {
		osgART::addEventCallback(root.get(), new osgART::ImageStreamCallback(imagestream));
	}


	osg::ref_ptr<osgART::Target> marker = tracker->addTarget(image);

	if (!marker.valid()) {
		// Without marker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not add marker!" << std::endl;
		exit(-1);
	}

	marker->setActive(true);

	osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();
	arTransform->setUpdateCallback(new osgART::TargetTransformCallback(marker.get()));
	arTransform->getUpdateCallback()->setNestedCallback(new osgART::TargetVisibilityCallback(marker.get()));
	//arTransform->getUpdateCallback()->getNestedCallback()->setNestedCallback(new osgART::TransformFilterCallback());


	osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
	pat->setScale(osg::Vec3(30, 30, 30));
	pat->setPosition(osg::Vec3(116, -165, 100));
	pat->getOrCreateStateSet()->setMode(GL_NORMALIZE, GL_TRUE);
	pat->addChild(osgDB::readNodeFile("D:\\Content\\Models\\Vehicles\\Military\\Military Ambulance\\ambulance.ive"));

	//osg::Geode* g = new osg::Geode();
	//g->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(), osg::X_AXIS * 232, osg::Y_AXIS * -330));
	//arTransform->addChild(g);

	arTransform->addChild(pat);

	arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");


	osg::ref_ptr<osg::Group> videoBackground = createImageBackground(video.get());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

	osg::ref_ptr<osg::Camera> cam = calibration->createCamera();

	cam->addChild(arTransform.get());
	cam->addChild(videoBackground.get());
	root->addChild(cam.get());

	video->start();
	return viewer.run();

}
