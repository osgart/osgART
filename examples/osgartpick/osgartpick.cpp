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

// A simple example to demonstrate picking with osgART

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

osg::ref_ptr<osgART::VideoFlipper> flipper;

osg::ref_ptr<osgART::MatrixOffsetCallback> offsetCallback;

osg::Group* createImageBackground(osg::Image* video, osgART::Calibration* calibration = NULL, bool useTextureRectangle = false) {
	osgART::VideoLayer* _layer = new osgART::VideoLayer();
	//_layer->setSize(*video);
	osgART::VideoGeode* _geode = new osgART::VideoGeode(video, calibration, 1, 1, 20, 20,
		useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D);
	//addTexturedQuad(*_geode,video->s(),video->t());


	flipper = new osgART::VideoFlipper(false, false);

	flipper->addChild(_geode);

	_layer->addChild(flipper.get());


	//_layer->addChild(_geode);

	return _layer;
}

class PickEventHandler : public osgGA::GUIEventHandler {

public:
	PickEventHandler() : osgGA::GUIEventHandler() { }

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) {

		switch (ea.getEventType()) {

			case osgGA::GUIEventAdapter::KEYDOWN:
				switch (ea.getKey()) {
					case 'h':
						flipper->setFlipH(!flipper->getFlipH());
						offsetCallback->setupReflection(flipper->getFlipH(), flipper->getFlipV());
						break;
					case 'v':
						flipper->setFlipV(!flipper->getFlipV());
						offsetCallback->setupReflection(flipper->getFlipH(), flipper->getFlipV());
						break;
				}
				break;


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

	// create a root node
	osg::ref_ptr<osg::Group> root = new osg::Group;

	osgViewer::Viewer viewer;
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	viewer.addEventHandler(new PickEventHandler());
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);

	viewer.setSceneData(root.get());

	// preload the video and tracker
	osgART::PluginManager::instance()->load("osgart_video_artoolkit2");


	// Load a video plugin.
	osg::ref_ptr<osgART::Video> video =
		dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_artoolkit2"));

	// check if an instance of the video stream could be started
	if (!video.valid())
	{
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
		exit(-1);
	}

	// found video - configure now
	osgART::VideoConfiguration* _config = video->getVideoConfiguration();

	// if the configuration is existing
	if (_config)
	{
		// it is possible to configure the plugin before opening it

		//_config->deviceconfig = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		//	"<dsvl_input><avi_file use_reference_clock=\"true\" file_name=\"Data\\MyVideo.avi\" loop_avi=\"true\" render_secondary=\"true\">"
		//	"<pixel_format><RGB32/></pixel_format></avi_file></dsvl_input>";
	}


	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();

	osgART::PluginManager::instance()->load("osgart_tracker_artoolkit2");

	osg::ref_ptr<osgART::Tracker> tracker =
		dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get("osgart_tracker_artoolkit2"));

	if (!tracker.valid())
	{
		// Without tracker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plugin!" << std::endl;
		exit(-1);
	}

	// get the tracker calibration object
	osg::ref_ptr<osgART::Calibration> calibration = tracker->getOrCreateCalibration();

	// load a calibration file
	if (!calibration->load("data/camera_para.dat"))
	{

		// the calibration file was non-existing or couldnt be loaded
		osg::notify(osg::FATAL) << "Non existing or incompatible calibration file" << std::endl;
		exit(-1);
	}

	tracker->setImage(video.get());

	//osgART::addEventCallback(root.get(), new osgART::TrackerCallback(tracker.get()));

	osgART::TrackerCallback::addOrSet(root.get(),tracker.get());

	if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(video.get())) {
		osgART::addEventCallback(root.get(), new osgART::ImageStreamCallback(imagestream));
	}

#if OSGART_DEPRECATED_FIELDS

	osg::ref_ptr< osgART::TypedField<bool> > historyField = reinterpret_cast< osgART::TypedField<bool>* >(tracker->get("use_history"));
	historyField->set(true);

#endif


	osg::ref_ptr<osgART::Target> marker = tracker->addTarget("single;data/patt.hiro;80;0;0");
	if (!marker.valid())
	{
		// Without marker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not add marker!" << std::endl;
		exit(-1);
	}

	marker->setActive(true);

	osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();


	offsetCallback = new osgART::MatrixOffsetCallback();

	osgART::addEventCallback(arTransform.get(), new osgART::TargetTransformCallback(marker.get()));
	osgART::addEventCallback(arTransform.get(), new osgART::TargetVisibilityCallback(marker.get()));
	osgART::addEventCallback(arTransform.get(), offsetCallback.get());
	osgART::addEventCallback(arTransform.get(), new osgART::TransformFilterCallback());



	osg::Geode* cube = osgART::testCube(20);
	cube->setName("target");
	arTransform->addChild(cube);


	arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");


	osg::ref_ptr<osg::Group> videoBackground = createImageBackground(video.get());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

	osg::ref_ptr<osg::Camera> cam = calibration->createCamera();

	cam->addChild(arTransform.get());
	cam->addChild(videoBackground.get());
	root->addChild(cam.get());

	offsetCallback->setupReflection(flipper->getFlipH(), flipper->getFlipV());

	video->start();
	return viewer.run();

}
