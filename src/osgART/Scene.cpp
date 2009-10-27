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




	Scene::Scene() : osg::Group() {

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

	//"osgart_video_artoolkit2"
	void Scene::addVideoBackground(  const std::string& v )
	{

		int _video_id = osgART::PluginManager::instance()->load(v);

		_video = 
			dynamic_cast<osgART::Video*>(osgART::PluginManager::instance()->get(_video_id));

		// check if an instance of the video stream could be started
		if (!_video.valid()) 
		{   
			// Without video an AR application can not work. Quit if none found.
			osg::notify(osg::FATAL) << "Could not initialize video plugin!" << std::endl;
			//exit(-1);
		}

		_video->open();

		osgART::addEventCallback(this,new VideoStartCallback(_video.get()));

		osg::ref_ptr<osgART::VideoLayer> videoBackground = new osgART::VideoLayer();
		videoBackground->addChild(new osgART::VideoGeode(_video.get(), _calibration.get(), 
			1.0, 1.0, 20, 20, (_settings._use_texturerect) ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D));
		
		this->addChild(videoBackground.get());

		videoBackground->getOrCreateStateSet()->setRenderBinDetails(_settings._background_renderbin, "RenderBin");

		if (osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(_video.get())) 
		{
			osgART::addEventCallback(this, new osgART::ImageStreamCallback(imagestream));
		}

	}

	//"osgart_tracker_artoolkit2"

	void Scene::addTracker( const std::string& t )
	{

		int _tracker_id = osgART::PluginManager::instance()->load(t);



		_tracker = 
			dynamic_cast<osgART::Tracker*>(osgART::PluginManager::instance()->get(_tracker_id));

		if (!_tracker.valid()) 
		{
			// Without tracker an AR application can not work. Quit if none found.
			osg::notify(osg::FATAL) << "Could not initialize tracker plugin!" << std::endl;
			//exit(-1);
		}

		// get the tracker calibration object
		_calibration = _tracker->getOrCreateCalibration();

		// load a calibration file
		if (!_calibration->load(std::string("data/camera_para.dat"))) 
		{

			// the calibration file was non-existing or couldnt be loaded
			osg::notify(osg::FATAL) << "Non existing or incompatible calibration file" << std::endl;
			//exit(-1);
		}

		// set the image source for the tracker
		_tracker->setImage(_video.get());

		osgART::addEventCallback(this, new osgART::TrackerCallback(_tracker.get()));

		_camera = _calibration->createCamera();
		this->addChild(_camera.get());

	}


	//"single;data/patt.hiro;80;0;0"
	osgART::Marker* Scene::addMarker( const std::string& cfg )
	{

		return _tracker.valid() ? _tracker->addMarker(cfg) : 0L;

	}

	osg::MatrixTransform* Scene::addTrackedTransform(osgART::Marker* marker) {

		if (!marker) 
		{
			// Without marker an AR application can not work. Quit if none found.
			osg::notify(osg::FATAL) << "Could not add marker!" << std::endl;
			// exit(-1);
		}
 
		marker->setActive(true);


		osg::MatrixTransform* arTransform = new osg::MatrixTransform();

		osgART::attachDefaultEventCallbacks(arTransform, marker);
		//osgART::addEventCallback(arTransform, new osgART::MarkerDebugCallback(marker.get()));


		//arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

		_camera->addChild(arTransform);

		return arTransform;

	}

	osg::MatrixTransform* Scene::addTrackedTransform( const std::string& cfg )
	{
		osg::ref_ptr<osgART::Marker> marker = addMarker(cfg);
		return addTrackedTransform(marker.get());
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

