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
 
/**
*  \file  videoInput
*  \brief A Video class for image input
*
* 
* A video class that just displaying always the same image, ideal for
* debugging or taking snapshot. The image format is mainly the one
* implicitly supported in OpenSceneGraph.
*	
*   \remark 
*
*   History :
*
*  \author Raphael Grasset Raphael.Grasset@hitlabnz.org
*  \version 3.1
*  \date 07/05/31
**/

#include <iostream>
#include <string>

#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Notify>
#include <osg/Image>
#include <osg/Timer>

// graphics include
#include <osgART/Export>

// local include
#include "osgART/PluginManager"
#include "osgART/Video"
#include "osgART/VideoConfiguration"
#include "osgART/Utils"

#include "videoInput.h"

/**
 * class videoInput.
 *
 */
class VideoInputVideo : public osgART::Video
{
public:        
// Standard Services
    
    /** 
    * \brief default constructor.
    * The default constructor.
    * @param config a string definition of the Video. See documentation
    * of DummyImage for further details.
    */
    VideoInputVideo();
    
    /** 
    * \brief copy constructor.
    */
     VideoInputVideo(const VideoInputVideo &, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);



	META_Object(osgART,VideoInputVideo);

    
    /** 
    * \brief affectation operator.
    *
    */
    VideoInputVideo& operator=(const VideoInputVideo &);
    	
 //==================
	    /**
    * \brief init the Video stream.
    * Access the Video stream (hardware or file) and get an handle on it.
    */
	bool init();
	
	/**
    * \brief close the Video stream.
    * Terminate the connection with the Video stream and clean handle.
    */
	bool close(bool waitForThread=true);
	
	/**
    * \brief start the Video stream grabbing.
    * Start to get image from the Video stream. In function of the implementation on different
    * platform, this function can run a thread, signal or real-time function. 
    */
	bool start();
	
	/**
    * \brief stop the Video stream grabbing.
    * Stop to get image from the Video stream. In function of the implementation on different
    * platform, this function can stop a thread, signal or real-time function. 
    */
	bool stop();
	
	/**
    * \brief update the Video stream grabbing.
    * Try to get an image of the Video instance, usable by your application.
    */
	bool update(osg::NodeVisitor* nv = 0L);
	
    
    inline virtual void releaseImage() {};
protected:

	/** 
    * \brief destructor.
    *
    */
    virtual ~VideoInputVideo();       
    

private:

	//set/get variables

	videoInput _videoInput;

	float _frameFps;
	int _frameWidth;
	int _frameHeight;
	unsigned char* _frameData;
	int _deviceId;
	std::string _deviceName;

};

VideoInputVideo::VideoInputVideo():
	osgART::Video()
{	
	//initialize here any specific variables

}

VideoInputVideo::VideoInputVideo(const VideoInputVideo &, const osg::CopyOp& copyop):	osgART::Video()
{
    
}

VideoInputVideo::~VideoInputVideo(void) {
    
}

VideoInputVideo&  VideoInputVideo::operator=(const VideoInputVideo &) {
    return *this;
}


bool VideoInputVideo::init() {

	// Get list of devices (for internal use, don't print out)
	videoInput::listDevices(true);


	//open the video 
	//if you are using a device, you can open the device
	//if you are using video streaming, you can initialize the connection
	//if you are using video files, you can read the configuration, cache the data, etc.

	//first, you can check if there is a video configuration defined
	if (_videoConfiguration)
	{
		if (!_videoConfiguration->config.empty())
		{
			std::vector<std::string> tokens = osgART::tokenize(_videoConfiguration->config, ";");
			if (tokens.size() > 0) _deviceId = atoi(tokens[0].c_str());
			if (tokens.size() > 1) _frameWidth = atoi(tokens[1].c_str());
			if (tokens.size() > 2) _frameHeight = atoi(tokens[2].c_str());
			if (tokens.size() > 3) _frameFps = atoi(tokens[3].c_str());		
		}
		else
		{
			if (_videoConfiguration->deviceid!=-1)
				_deviceId=_videoConfiguration->deviceid;
			else
				_deviceId=0;
			if (_videoConfiguration->width=-1)
			{
				_frameWidth=_videoConfiguration->width;
				_frameHeight=_videoConfiguration->height;
			}
			else
			{
				_frameWidth=640;
				_frameHeight=480;
			}
			//fix equal for non integer
			if (_videoConfiguration->framerate=-1.0)
			{
				_frameFps=_videoConfiguration->framerate;
			}
			else
				_frameFps=30;
		}
	}
	else
	{
		_deviceId=0;
		_frameWidth=640;
		_frameHeight=480;
		_frameFps=30;
	}
	
	_videoInput.setIdealFramerate(_deviceId, _frameFps);

	//_VideoInput.setUseCallback(true);

	// Open device
	if (!_videoInput.setupDevice(_deviceId, _frameWidth, _frameHeight)) {
		osg::notify(osg::WARN) << "VideoInput: Error setting up device " << _deviceId << " at " << _frameWidth << "x" << _frameHeight << std::endl;
		return false;
	}

	_deviceName = _videoInput.getDeviceName(_deviceId);
	_videoConfiguration->selectedWidth = _videoInput.getWidth(_deviceId);
	_videoConfiguration->selectedHeight = _videoInput.getHeight(_deviceId);

	//frameSize = _videoInput.getSize(_deviceId);

	osg::notify(osg::WARN) << "VideoInput: Opened device " << _deviceId << " (" << _deviceName << ") at " <<
		_videoConfiguration->selectedWidth << "x" << _videoConfiguration->selectedHeight << std::endl;

	_frameData = new unsigned char[_videoConfiguration->selectedWidth * _videoConfiguration->selectedHeight * 4];

	//we need to create one video stream
	_videoStreamList.push_back(new osgART::VideoStream());

	_videoStreamList[0]->allocateImage(_videoConfiguration->selectedWidth, _videoConfiguration->selectedHeight, 1, GL_BGRA, GL_UNSIGNED_BYTE);

	return true;

}


bool VideoInputVideo::update(osg::NodeVisitor* nv) {

	if (!_videoInput.isDeviceSetup(_deviceId)) return false;

	osg::Timer t;

	if (_videoInput.isFrameNew(_deviceId)) 
	{
		//rg: todo use flip option for second and third parameter
		if (unsigned char* frame = _videoInput.getPixels(_deviceId, false, true)) 
		{

			for (int y = 0; y < _videoConfiguration->selectedHeight; y++) 
			{
				for (int x = 0; x < _videoConfiguration->selectedWidth; x++) 
				{
					int i = y * _frameWidth + x;
					_frameData[i * 4 + 0] = frame[i * 3 + 0];
					_frameData[i * 4 + 1] = frame[i * 3 + 1];
					_frameData[i * 4 + 2] = frame[i * 3 + 2];		
					_frameData[i * 4 + 3] = 0;
				}
			}

			{
				//1. mutex lock access to the image video stream
				OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());

				memcpy(_videoStreamList[0]->data(), _frameData, _videoStreamList[0]->getImageSizeInBytes());
				_videoStreamList[0]->dirty();
			}

		}

	}

	//4. hopefully report some interesting data
	if (nv) {

		const osg::FrameStamp *framestamp = nv->getFrameStamp();

		if (framestamp && _stats.valid())
		{
			_stats->setAttribute(framestamp->getFrameNumber(),
				"Capture time taken", t.time_m());
		}
	}


	// Increase modified count every X ms to ensure tracker updates
	//if (updateTimer.time_m() > 50) {
	//	_videoStreamList[0]->dirty();
	//	updateTimer.setStartTick();
	//}

	return true;
}

bool VideoInputVideo::start() 
{ 
	//here you can start to stream the images, starting the camera acquisition
	//or starting to decompress video files

	//if you run a threaded video plugin, you can start the thread here

	//in this example we only start to play the VideoStream 0
	
	_videoStreamList[0]->play();	

	return true;
}

bool VideoInputVideo::stop() 
{ 

	//here you can stop any streaming, camera acquisition or video file decompression

	//if you run a threaded video plugin, you can stop the thread here

	//in this example we only pause the VideoStream 0

	_videoStreamList[0]->pause();	

	return true;
}

bool VideoInputVideo::close(bool waitForThread) 
{ 
	//here you can close any streaming open, close a camera 
	//and clean your specific data structure

	//in this example we don't do anything
	
	this->stop();

	_videoInput.stopDevice(_deviceId);

	delete[] _frameData;

	return true;
}

//at the end you register your video plugin, the syntax generally
//osgART::PluginProxy<PluginClassNameVideo> g_PluginClassNameVideo("osgart_video_pluginclassnamevideo");
osgART::PluginProxy<VideoInputVideo> g_videoInput("osgart_video_videoinput");
