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


/* An example osgartviewer.cfg file:


#
# Config file
#

video artoolkit
tracker artoolkit

# hiro is an ID
target hiro single;data/patt.hiro;80;0;0

# the rest is for working on
translate hiro 0 0 0
scale hiro 3 3 3
#rotate hiro 1 10 10 10
model hiro thunderbird.lwo


*/





#include <osg/ProxyNode>
#include <osg/PositionAttitudeTransform>

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

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgDB/WriteFile>

#include <osgART/VisualTracker>

osg::Group* createBasicVideoBackground(osg::Image* video, bool useTextureRectangle = false) {
	osgART::VideoLayer* _layer = new osgART::VideoLayer();
	//_layer->setSize(*video);
	osgART::VideoGeode* _geode = new osgART::VideoGeode(video, NULL, 1, 1, 20, 20,
		useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D);
	//addTexturedQuad(*_geode,video->s(),video->t());
	_layer->addChild(_geode);
	return _layer;
}


int main(int argc, char* argv[])  {

	std::string config("osgartviewer.cfg");
	std::string dump2file;

	osgDB::appendPlatformSpecificResourceFilePaths(osgDB::getDataFilePathList());

/*
	std::cerr << "Starting" << std::endl;

	osgDB::FilePathList::iterator il = osgDB::getDataFilePathList().begin();
	while (il != osgDB::getDataFilePathList().end())
	{
		std::cerr << *il << std::endl;
		il++;
	}

	std::cerr << "Ending" << std::endl;
*/


	OpenThreads::Thread::Init();

	osg::ArgumentParser arguments(&argc,argv);

	bool useTextureRectangle(false);

	while (arguments.read("--config", config)) {
		osg::notify()  << argv[0] << " using config: '" << config << "'" << std::endl;
	}

	while (arguments.read("--use-texturerectangle", useTextureRectangle)) {
		osg::notify()  << argv[0] << " using texture rectangle: '" << useTextureRectangle << "'" << std::endl;
	}


	while (arguments.read("--dump2file", dump2file)) {
		osg::notify() << argv[0] << " dumping to file '" << dump2file << "'" << std::endl;
	}

    // add directory of the file path to support relative paths in the config file
    osgDB::getDataFilePathList().push_back(osgDB::getFilePath(osgDB::findDataFile(config)));


	// create a root node
	osg::ref_ptr<osg::Group> root = new osg::Group;

	osgViewer::Viewer viewer;

	// attach root node to the viewer
	viewer.setSceneData(root.get());

	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::ThreadingHandler);
	viewer.addEventHandler(new osgViewer::HelpHandler);



	config = osgDB::findDataFile(config);

	/* loading the config file */
	std::ifstream file(config.c_str());

	std::string _cameraconfiguration_file("data/camera_para.dat");

	// A video plugin.
	osg::ref_ptr<osgART::Video> video;
	//osg::ref_ptr<osgART::Tracker> tracker;
	osg::ref_ptr<osgART::VisualTracker> tracker;
//	osg::ref_ptr<osgART::GPSInertialTracker> tracker;

	typedef std::map< std::string, std::string > StringMap;
	typedef std::map< std::string, double > DoubleMap;
	typedef std::map< std::string, osg::Vec3 > Vec3Map;
	typedef std::map< std::string, osg::Vec4 > Vec4Map;

	StringMap _target_map;
	StringMap _model_map;
	Vec3Map _translate_map;
	Vec3Map _scale_map;
	Vec4Map	_rotate_map;
	Vec3Map _pivot_map;

	while (!file.eof() && file.is_open())
	{
		std::string line;
		std::getline(file,line);

		std::vector<std::string> tokens = osgART::tokenize(line, " ");

		if (!tokens.size()) continue;

		if (tokens[0] == "tracker" && tokens.size() == 2)
		{
            osgART::PluginManager::instance()->load("osgart_" + tokens[1]);
			tracker = dynamic_cast<osgART::VisualTracker*>(osgART::PluginManager::instance()->get("osgart_tracker_" + tokens[1]));
		}

		if (tokens[0] == "video" && tokens.size() == 2)
		{
            osgART::PluginManager::instance()->load("osgart_" + tokens[1]);
			video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get("osgart_video_" + tokens[1]));
		}

		if (tokens[0] == "cameraconfig" && tokens.size() == 2)
		{
			_cameraconfiguration_file = tokens[1];
		}

		if (tokens[0] == "target" && tokens.size() == 3)
		{
			_target_map[tokens[1]] = tokens[2];
		}

		if (tokens[0] == "translate" && tokens.size() == 5)
		{
			osg::Vec3 vec;
			vec[0] = ::atof(tokens[2].c_str());
			vec[1] = ::atof(tokens[3].c_str());
			vec[2] = ::atof(tokens[4].c_str());

			_translate_map[tokens[1]] = vec;
		}

		if (tokens[0] == "pivot" && tokens.size() == 5)
		{
			osg::Vec3 vec;
			vec[0] = ::atof(tokens[2].c_str());
			vec[1] = ::atof(tokens[3].c_str());
			vec[2] = ::atof(tokens[4].c_str());

			_pivot_map[tokens[1]] = vec;
		}

		if (tokens[0] == "rotate" && tokens.size() == 6)
		{
			osg::Vec4 vec;
			vec[0] = ::atof(tokens[2].c_str());
			vec[1] = ::atof(tokens[3].c_str());
			vec[2] = ::atof(tokens[4].c_str());
			vec[3] = ::atof(tokens[5].c_str());

			_rotate_map[tokens[1]] = vec;
		}

		if (tokens[0] == "scale" && tokens.size() == 5)
		{
			osg::Vec3 vec;
			vec[0] = ::atof(tokens[2].c_str());
			vec[1] = ::atof(tokens[3].c_str());
			vec[2] = ::atof(tokens[4].c_str());

			_scale_map[tokens[1]] = vec;
		}

		if (tokens[0] == "model" && tokens.size() == 3)
		{
			_model_map[tokens[1]] = tokens[2];
		}

	}

	if (file.is_open())
		file.close();
	else
		std::cerr << "osgart could not find appropriate config file '" << config << "'" << std::endl;


	// check if an instance of the video stream could be started
	if (!video.valid())
	{
		// Without video an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
		exit(-1);
	}

	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->init();


	if (!tracker.valid())
	{
		// Without tracker an AR application can not work. Quit if none found.
		osg::notify(osg::FATAL) << "Could not initialize tracker plugin!" << std::endl;
		exit(-1);
	}

	// get the tracker camera configuration object
	osg::ref_ptr<osgART::CameraConfiguration> cameraconfig = tracker->getOrCreateCameraConfiguration();

	// load a camera configuration file
	if (!cameraconfig->load(osgDB::findDataFile(_cameraconfiguration_file)))
	{

		// the camera configuration file was non-existing or couldnt be loaded
		osg::notify(osg::FATAL) << "Non existing or incompatible camera configuration file" << std::endl;
		exit(-1);
	}

	// set the image source for the tracker
	tracker->setImage(video->getStream());

	//add video update callback (update video + video stream)
	osgART::VideoUpdateCallback::addOrSet(root.get(),video.get());

	//add tracker update callback (update tracker from video stream)
	osgART::TrackerUpdateCallback::addOrSet(root.get(),tracker.get());

	osg::ref_ptr<osg::Group> videoBackground = osgART::createBasicVideoBackground(video->getStream());
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

	osg::ref_ptr<osg::Camera> cam = osgART::createBasicCamera(cameraconfig);


	cam->addChild(videoBackground.get());

	typedef std::map< std::string, osg::Group* > targetSubGraph;
	targetSubGraph _target_graph;

	StringMap::iterator iter = _target_map.begin();
	while (iter != _target_map.end())
	{
		osg::ref_ptr<osgART::Target> target = tracker->addTarget(iter->second);
		if (!target.valid())
		{
			// Without target an AR application can not work. Quit if none found.
			osg::notify(osg::FATAL) << "Could not add target!" << std::endl;
			exit(-1);

		} else {

			target->setActive(true);

			osg::ref_ptr<osg::MatrixTransform> arTransform = new osg::MatrixTransform();

			osgART::attachDefaultTargetCallbacks(arTransform.get(),target.get());

			arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");
			arTransform->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

			cam->addChild(arTransform.get());

			osg::ProxyNode* proxynode = new osg::ProxyNode();
			osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();

			pat->addChild(proxynode);
			arTransform->addChild(pat);

			proxynode->setLoadingExternalReferenceMode(osg::ProxyNode::LOAD_IMMEDIATELY);

			if (_translate_map.find(iter->first) != _translate_map.end())
			{
				pat->setPosition(_translate_map.find(iter->first)->second);
				osg::notify() << "PAT::setPosition() " << _translate_map.find(iter->first)->second << std::endl;
			}

			if (_rotate_map.find(iter->first) != _rotate_map.end())
			{
				osg::Quat quat(_rotate_map.find(iter->first)->second);
				pat->setAttitude(quat);
			}

			if (_pivot_map.find(iter->first) != _pivot_map.end())
			{
				pat->setPivotPoint(_pivot_map.find(iter->first)->second);
			}

			if (_scale_map.find(iter->first) != _scale_map.end())
			{
				osg::notify() << "PAT::setScale() " << _scale_map.find(iter->first)->second << std::endl;
				pat->setScale(_scale_map.find(iter->first)->second);
			}

			if (_model_map.find(iter->first) != _model_map.end())
			{
				if (_model_map.find(iter->first)->second == "cube") {
					proxynode->addChild(osgART::createTopCube(80));
				} else {
					proxynode->setFileName(0,_model_map.find(iter->first)->second);
					osg::notify() << "ProxyNode::getFileName() " << proxynode->getFileName(0) << std::endl;
				}
			}

		}

		iter++;

	}

	root->addChild(cam.get());

	if (!dump2file.empty()) {

		osgDB::writeNodeFile(*root,dump2file);

	}


	video->start();
	return viewer.run();

}
