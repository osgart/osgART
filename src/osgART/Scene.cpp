/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
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

#include "osgART/Scene"


#include "osgART/VideoLayer"
#include "osgART/PluginManager"
#include "osgART/VideoGeode"
#include "osgART/Utils"
#include "osgART/GeometryUtils"
#include "osgART/MarkerCallback"
#include "osgART/TransformFilterCallback"
#include "osgART/ImageStreamCallback"


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
		_videoBackground->addChild(
			new osgART::VideoGeode(
				_video.get(), 
				_calibration.get(), 
				1.0, 1.0, 20, 20, 
				(_settings._use_texturerect) ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D
			)
		);

		_videoBackground->getOrCreateStateSet()->setRenderBinDetails(_settings._background_renderbin, "RenderBin");

	}



	//"osgart_video_artoolkit2"
	osgART::Video* Scene::addVideoBackground(  const std::string& v )
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

		if (!_video->open()) {
			osg::notify(osg::FATAL) << "Could not open video!" << std::endl;
			return NULL;
		}

		osgART::addEventCallback(this, new VideoStartCallback(_video.get()));

		configureVideoBackground();		

		if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(_video.get())) 
		{
			osgART::addEventCallback(this, new osgART::ImageStreamCallback(imagestream));
		}

		// If tracker was added already, associate video with tracker
		if (_tracker.valid()) {
			_tracker->setImage(_video.get());
		}

		return _video.get();

	}

	//"osgart_tracker_artoolkit2"

	osgART::Tracker* Scene::addTracker( const std::string& t )
	{

		osgART::PluginManager::instance()->load(t);
		_tracker = dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get(t));

		if (!_tracker.valid()) 
		{
			// Without tracker an AR application can not work. Quit if none found.
			osg::notify(osg::FATAL) << "Could not initialize tracker plugin!" << std::endl;
			return NULL;
		}

		// get the tracker calibration object
		_calibration = _tracker->getOrCreateCalibration();

		// load a calibration file
		if (!_calibration->load(std::string("data/camera_para.dat"))) 
		{

			// the calibration file was non-existing or couldnt be loaded
			osg::notify(osg::FATAL) << "Non existing or incompatible calibration file" << std::endl;
			return NULL;
		}

		// If video was added already, associate video with tracker
		if (_video.valid()) {
			// set the image source for the tracker
			_tracker->setImage(_video.get());

			// Update the video background with new tracker calibration etc...
			configureVideoBackground();	

		}

		osgART::addEventCallback(this, new osgART::TrackerCallback(_tracker.get()));

		_camera = _calibration->createCamera();
		this->addChild(_camera.get());

		return _tracker.get();

	}


	//"single;data/patt.hiro;80;0;0"
	osgART::Marker* Scene::addMarker( const std::string& cfg )
	{
		return _tracker.valid() ? _tracker->addMarker(cfg) : 0L;
	}

	osg::MatrixTransform* Scene::addTrackedTransform(osgART::Marker* marker) {

		osg::MatrixTransform* arTransform = new osg::MatrixTransform();
		_camera->addChild(arTransform);

		if (!marker) 
		{

			osg::notify(osg::FATAL) << "No marker specified for tracking!" << std::endl;

		} else {
 
			marker->setActive(true);
			osgART::attachDefaultEventCallbacks(arTransform, marker);

		}

		return arTransform;

	}

	osg::MatrixTransform* Scene::addTrackedTransform( const std::string& cfg )
	{
		return addTrackedTransform(addMarker(cfg));
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


	SceneSetting& SceneSetting::setMarkerActive( bool active )
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

