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

#include "osgART/Scene"


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


namespace osgART {

	Scene::Scene() : osg::Group(),
		_videoBackground(new osgART::VideoLayer()) {

		this->addChild(_videoBackground.get());

	}

	Scene::Scene( const Scene&,const osg::CopyOp& copyop/*=osg::CopyOp::SHALLOW_COPY*/ )
	{

	}

	class VideoStartCallback : public osg::NodeCallback {

		osg::observer_ptr<osgART::Video> _video;
		bool _oneshot;
	public:
		VideoStartCallback(osgART::Video* Video) : _video(Video), _oneshot(false) {}

		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
		{

			if (!_oneshot) _video->start();

			traverse(node,nv);
		}


	};

	void Scene::configureVideoBackground() {

		if (!_videoBackground.valid() || !_video.valid()) return;

		// Clear out all existing children of the video background
		_videoBackground->removeChildren(0, _videoBackground->getNumChildren());

		// Add a new background video geode using the current set of parameters
		_videoBackground->addChild(osgART::createBasicVideoBackground(_video->getStream()));

		_videoBackground->getOrCreateStateSet()->setRenderBinDetails(_settings._background_renderbin, "RenderBin");

	}



	//"osgart_artoolkit"
	osgART::Video* Scene::addVideoBackground(  const std::string& v, std::string videoconf)
	{

		osgART::PluginManager::instance()->load(v);

		_video = dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get(v));

		// check if an instance of the video stream could be started
		if (!_video.valid())
		{
			// Without video an AR application can not work. Quit if none found.
			osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
			return NULL;
		}

		// found video - configure now
		osgART::VideoConfiguration* _configvideo = _video->getConfiguration();

		// if the configuration is existing
		if (_configvideo)
		{
			if (videoconf!="")
				_configvideo->config=videoconf.c_str();
		}


		if (!_video->init()) {
			osg::notify(osg::FATAL) << "Could not open video!" << std::endl;
			return NULL;
		}

		osgART::addEventCallback(this, new VideoStartCallback(_video.get()));

		configureVideoBackground();


		//add video update callback (update video + video stream)
		osgART::VideoUpdateCallback::addOrSet(this,_video.get());


		// If tracker was added already, associate video with tracker
		if (_tracker.valid()) {
			_tracker->setImage(_video->getStream());
		}

		return _video.get();

	}

	//"osgart_tracker_artoolkit"

	osgART::Tracker* Scene::addTracker( const std::string& t, std::string cameraconfigurationconfigfile,  std::string trackerconfigfile)
	{

		osgART::PluginManager::instance()->load(t);
		_tracker = dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get(t));

		if (!_tracker.valid())
		{
			// Without tracker an AR application can not work. Quit if none found.
			osg::notify(osg::FATAL) << "Could not initialize tracker plugin!" << std::endl;
			return NULL;
		}

		// found tracker - configure now
		osgART::TrackerConfiguration* _configtracker = _tracker->getConfiguration();

		// if the configuration is existing
		if (_configtracker)
		{
			// it is possible to configure the plugin before opening it
			if (trackerconfigfile!="")
			_configtracker->config=trackerconfigfile.c_str();
			

		}

		// get the tracker Camera Configuration object
		_cameraconfiguration = _tracker->getOrCreateCameraConfiguration();

		// load a Camera Configuration file
		//if (cameraconfigurationconfigfile!="")
		//{
			if (!_cameraconfiguration->load(cameraconfigurationconfigfile))
			{
				// the Camera Configuration file was non-existing or couldn't be loaded
				osg::notify(osg::FATAL) << "Non existing or incompatible camera configuration file" << std::endl;
				return NULL;
			}
		//}

		// If video was added already, associate video with tracker
		if (_video.valid()) {
			// set the image source for the tracker
			_tracker->setImage(_video->getStream());

			// Update the video background with new tracker camera configuration etc...
			//configureVideoBackground();

		}

		osgART::addEventCallback(this, new osgART::TrackerUpdateCallback(_tracker.get()));

		_camera = osgART::createBasicCamera(_cameraconfiguration);
		this->addChild(_camera.get());

		return _tracker.get();

	}


	//"single;data/patt.hiro;80;0;0"
	osgART::Target* Scene::addTarget( const std::string& cfg )
	{
		return _tracker.valid() ? _tracker->addTarget(cfg) : 0L;
	}

	osg::MatrixTransform* Scene::addTrackedTransform(Target* target) {

		osg::MatrixTransform* arTransform = new osg::MatrixTransform();
		arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

		_camera->addChild(arTransform);

		if (!target)
		{
			osg::notify(osg::FATAL) << "No target specified for tracking!" << std::endl;

		} else {

			target->setActive(true);
			osgART::attachDefaultEventCallbacks(arTransform, target
				);

		}

		return arTransform;

	}

	osg::MatrixTransform* Scene::addTrackedTransform( const std::string& cfg )
	{
		return addTrackedTransform(addTarget(cfg));
	}

	Scene::~Scene()
	{

		if (_video.valid()) {
			_video->stop();
			_video->close();
		}
		_video = 0L;
		_tracker = 0L;

	}

	/************************************************************************/
	/* SceneSetting                                                         */
	/************************************************************************/


	SceneSetting& SceneSetting::setTargetActive( bool active )
	{
		_default_active = active; return *this;
	}

	SceneSetting::SceneSetting() : _background_renderbin(-1), _default_active(false), _use_texturerect(false)
	{

	}

	SceneSetting& SceneSetting::useTextureRectangle( bool use /*= true*/ )
	{
		_use_texturerect = use; return *this;
	}
}

