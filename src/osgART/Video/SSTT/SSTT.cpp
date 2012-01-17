/* -*-c++-*-
 *
 * osgART - ARToolKit for OpenSceneGraph 
 * Copyright (C) 2005-2008 Human Interface Technology Laboratory New Zealand
 * Portions Copyright (C) 2005-2007 ARToolworks Inc
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

#include <osg/Object>
#include <osg/Notify>
#include <osg/Timer>

#include <osgDB/FileUtils>

#include <iostream>
#include <iomanip>
#include <cstring>


#include "osgART/PluginManager"
#include "osgART/Video"
#include "osgART/Utils"
#include "osgART/VideoConfig"

#include <sstt/sstt.h>

class SSTT_Video : public osgART::Video {

public:


	SSTT_Video();

	/**
		* Copy constructor.
		*
		*/
	SSTT_Video(const SSTT_Video &,
		const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

	/**
		* Destructor.
		*
		*/
	virtual ~SSTT_Video();


	META_Object(osgART,SSTT_Video);

	/**
	* Affectation operator.
	*
	*/
	SSTT_Video& operator = (const SSTT_Video &);

	/**
		* Open the video stream. Access the video stream (hardware or file) and get an handle on it.
		*/
	bool open();

	/**
		* Close the video stream. Terminates the connection with the video stream and clean handle.
		*/
	void close(bool = true);

	/**
		* Start the video stream grabbing. Start to get image from the video stream. In function of the
		* implementation on different platform, this function can run a thread, signal or
		* real-time function.
		*/
	void play();

	/**
		* Stop the video stream grabbing. Stop to get image from the video stream. In function
		* of the implementation on different platform, this function can stop a thread, signal or
		* real-time function.
		*/
	void pause();

	/**
		* Update the video stream grabbing. Try to get an image of the video instance, usable
		* by your application.
		*/
	void update(osg::NodeVisitor* nv);

	virtual osgART::VideoConfiguration* getVideoConfiguration();


private:

	sstt_capture* _capture;
	sstt_image _image;

	osgART::VideoConfiguration _config;

};


SSTT_Video::SSTT_Video() 
	: osgART::Video()
	, _capture(0L)
{
}

SSTT_Video::SSTT_Video(const SSTT_Video &, const osg::CopyOp& copyop/* = osg::CopyOp::SHALLOW_COPY*/) 
{
}

SSTT_Video::~SSTT_Video() 
{
	this->close(false);
}

SSTT_Video& 
SSTT_Video::operator=(const SSTT_Video &) {
	return *this;
}

bool 
SSTT_Video::open() 
{
	sstt_capture_setting capture_settings;
	
	sstt_capture_create(&_capture,SSTT_CAPTURE_DEFAULT);

	// Default values
	capture_settings.minWidth = 320;
	capture_settings.minHeight = 240;
	capture_settings.minFPS = 30;
	capture_settings.name = 0;
	capture_settings.uid = 0;
	

	// Try to get custom values from configuration string
	// Format for device string:
	// Device UID;width;height;framerate;flip
	std::vector<std::string> tokens = osgART::tokenize(_config.deviceconfig, ";");
	if (tokens.size() > 0) capture_settings.uid = const_cast<char*>(tokens[0].c_str());
	if (tokens.size() > 1) capture_settings.minWidth  = atoi(tokens[1].c_str());
	if (tokens.size() > 2) capture_settings.minHeight  = atoi(tokens[2].c_str());
	if (tokens.size() > 3) capture_settings.minFPS = atoi(tokens[3].c_str());
	if (tokens.size() > 4) capture_settings.flip_control = atoi(tokens[4].c_str());


	capture_settings.flip_control = 0;

	// Open device
	sstt_capture_open( _capture, &capture_settings);
	
	sstt_capture_start(_capture);

	sstt_capture_update(_capture);
	
	sstt_image probe;
	
	// get_image is locking the video capture
	sstt_capture_get_image( _capture, &probe, SSTT_IMAGE_BGR24 );
	// we can immediate unlock because we don't copy the image
	sstt_capture_get_image( _capture, 0, 0);

	// newer API supports flipping in place
	sstt_capture_set_param_i(_capture, SSTT_CAPTURE_FLIP, capture_settings.flip_control);

	// now create the image
	this->allocateImage(probe.width, probe.height, 1, GL_BGR, GL_UNSIGNED_BYTE, 1);
	this->setDataVariance(osg::Object::DYNAMIC);

	
	this->update(0);

	return true;

}

void 
SSTT_Video::close(bool waitForThread) 
{
	sstt_capture_stop(_capture);
}

void
SSTT_Video::play() {
	osg::ImageStream::play();
}

void
SSTT_Video::pause() {
	osg::ImageStream::pause();
}

void
SSTT_Video::update(osg::NodeVisitor* nv) 
{

	if (0 == _capture) 
	{
		return;
	}
	
	sstt_image probe;
	sstt_capture_get_image( _capture, &probe, SSTT_IMAGE_BGR24 );

	if (probe.frame != this->getModifiedCount()) 
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());
		memcpy(this->data(), probe.data, getImageSizeInBytes());
		this->dirty();
	}
	
	sstt_capture_get_image(_capture,0,0);

}

osgART::VideoConfiguration* SSTT_Video::getVideoConfiguration() 
{
	return &_config;
}

// initializer for dynamic loading
osgART::PluginProxy<SSTT_Video> g_sstt_video("osgart_video_sstt");

