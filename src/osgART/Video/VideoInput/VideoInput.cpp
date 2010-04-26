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

#include "VideoInput.h"

class VideoInputVideo : public osgART::Video {

public:


	VideoInputVideo();

	/**
		* Copy constructor.
		*
		*/
	VideoInputVideo(const VideoInputVideo &,
		const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

	/**
		* Destructor.
		*
		*/
	virtual ~VideoInputVideo();


	META_Object(osgART,VideoInputVideo);

	/**
	* Affectation operator.
	*
	*/
	VideoInputVideo& operator = (const VideoInputVideo &);

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


	/**
	* Deallocate image memory. Deallocates any internal memory allocated by the instance of this
	* class.
	*/
	void releaseImage();

	virtual osgART::VideoConfiguration* getVideoConfiguration();


private:

	videoInput mVideoInput;

	int mFrameWidth;
	int mFrameHeight;
	int mFrameSize;

	unsigned char* mFrameData;

	osgART::VideoConfiguration m_config;

};







VideoInputVideo::VideoInputVideo() : osgART::Video(),
	mFrameData(NULL),
	mFrameWidth(-1),
	mFrameHeight(-1),
	mFrameSize(-1) {
}

VideoInputVideo::VideoInputVideo(const VideoInputVideo &, const osg::CopyOp& copyop/* = osg::CopyOp::SHALLOW_COPY*/) {
}

VideoInputVideo::~VideoInputVideo() {
	this->close(false);
}

VideoInputVideo& VideoInputVideo::operator=(const VideoInputVideo &) {
	return *this;
}

bool VideoInputVideo::open() {

	// Default values
	int deviceID = 0;
	int width = 320;
	int height = 240;

	// Try to get custom values from configuration string
	std::vector<std::string> tokens = osgART::tokenize(m_config.deviceconfig, ";");
	if (tokens.size() >= 3) {
		deviceID = atoi(tokens[0].c_str());
		width = atoi(tokens[1].c_str());
		height = atoi(tokens[2].c_str());
	}

	// Open device
	if (!mVideoInput.setupDevice(deviceID, width, height)) {
		osg::notify(osg::WARN) << "VideoInput: Error setting up device " << deviceID << " at " << width << "x" << height << std::endl;
		return false;
	}

	mFrameWidth = mVideoInput.getWidth(0);
	mFrameHeight = mVideoInput.getHeight(0);
	mFrameSize = mVideoInput.getSize(0);

	mFrameData = new unsigned char[mFrameWidth * mFrameHeight * 4];

	this->allocateImage(mFrameWidth, mFrameHeight, 1, GL_BGRA, GL_UNSIGNED_BYTE, 1);
	this->setDataVariance(osg::Object::DYNAMIC);

	return true;

}

void VideoInputVideo::close(bool waitForThread) {
	mVideoInput.stopDevice(0);
	delete[] mFrameData;
}

void VideoInputVideo::play() {
	osg::ImageStream::play();
}

void VideoInputVideo::pause() {
	osg::ImageStream::pause();
}

void VideoInputVideo::update(osg::NodeVisitor* nv) {

	if (mVideoInput.isFrameNew(0)) {

		if (unsigned char* frame = mVideoInput.getPixels(0, false, true)) {

			for (int y = 0; y < mFrameHeight; y++) {
				for (int x = 0; x < mFrameWidth; x++) {
					int i = y * mFrameWidth + x;
					mFrameData[i * 4 + 0] = frame[i * 3 + 0];
					mFrameData[i * 4 + 1] = frame[i * 3 + 1];
					mFrameData[i * 4 + 2] = frame[i * 3 + 2];		
					mFrameData[i * 4 + 3] = 0;
				}
			}


			{
				OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());
				memcpy(this->data(), mFrameData, mFrameWidth * mFrameHeight * 4);
				this->dirty();
			}

		}

	}

}

osgART::VideoConfiguration* VideoInputVideo::getVideoConfiguration() {
	return &m_config;
}

void VideoInputVideo::releaseImage() {
}


// initializer for dynamic loading
osgART::PluginProxy<VideoInputVideo> g_videoinputvideo("video_videoinput");


