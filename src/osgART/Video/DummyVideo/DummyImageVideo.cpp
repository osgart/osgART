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


#include "osgART/VideoConfiguration"
#include "DummyImageVideo"
#include "OpenThreads/Thread"

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osg/Notify>

DummyVideo::DummyVideo():
	osgART::Video(),
	m_flip_horizontal(false),
	m_flip_vertical(true),
	m_max_width(640),
	vconf(0L)
{

	//initialize here any specific variables

	//define specific field variables and functions

	//in this example, we create some options to change
	//the orientation of the image
	_fields["flip_horizontal"] = new osgART::TypedField<bool>(&m_flip_horizontal);
	_fields["flip_vertical"]	= new osgART::TypedField<bool>(&m_flip_vertical);

	_fields["max_width"] = new osgART::TypedField<unsigned int>(&m_max_width);

	//you can also create some specific get/set function
	//such as setting up the name of the image file
	//or calling specific function such as camera exposure, ROI video mode, etc
	_fields["image_file"]		= new osgART::CallbackField<DummyVideo, std::string>(this,
		&DummyVideo::getImageFile,
		&DummyVideo::setImageFile);
}

DummyVideo::DummyVideo(const DummyVideo &, const osg::CopyOp& copyop) {
    
}

DummyVideo::~DummyVideo(void) {
    
}

DummyVideo&  DummyVideo::operator=(const DummyVideo &) {
    return *this;
}


bool DummyVideo::open() {

	//open the video 
	//if you are using a device, you can open the device
	//if you are using video streaming, you can initialize the connection
	//if you are using video files, you can read the configuration, cache the data, etc.

	//first, you can check if there is a video configuration defined
	if (vconf)
	{
		if (!vconf->config.empty())
		{
			videoName=vconf->config;
		}
	}
	else
	{
	//if there is no configuration we use the field variable
	//to check if there is a defined videoName

		if (videoName.empty()) {
			osg::notify(osg::FATAL) << "Error in DummyVideo::open(), File name is empty!";
			return false;
		}
	}
	
	osg::notify(osg::INFO) << "DummyVideo::open()  open image : " << videoName << std::endl;

	//for this example, we load a picture
	osg::Image* img = osgDB::readImageFile(videoName.c_str());

	if (!img) {
		osg::notify(osg::FATAL) << "Error in DummyVideo::open(), Could not open File!";
		return false;
	}

	unsigned int w = img->s();
	unsigned int h = img->t();

	if (w > m_max_width) {
		osg::notify() << "DummyVideo: Image width exceeds maximum (" << m_max_width << "). Image will be resized";

		float aspect = (float)h / (float)w;
		w = m_max_width;
		h = w * aspect;

		img->scaleImage(w, h, 1);

		//osgDB::writeImageFile(*img, "test_resize.jpg");

	}


	int components = osg::Image::computeNumComponents(img->getPixelFormat());

	//this is main function you need to call to be sure to
	//allocate your image

	this->allocateImage(w, h, 1, GL_BGRA, GL_UNSIGNED_BYTE);

	for (int j = 0; j < h; j++) {

		for (int i = 0; i < w; i++) {

			unsigned char* srcPtr = img->data(i, j);
			unsigned char* dstPtr = this->data(i, j);

			switch (img->getPixelFormat()) {
				case GL_RGB:
				case GL_RGBA:
					dstPtr[0] = srcPtr[2];
					dstPtr[1] = srcPtr[1];
					dstPtr[2] = srcPtr[0];
					dstPtr[3] = 0;
					break;
			}

		}

	}


	/*
	xsize = m_image->s();
	ysize = m_image->t();

	if (osg::Image::computeNumComponents(m_image->getPixelFormat()) == 3) {
		m_image->setPixelFormat(GL_RGB);
		pixelsize=3;
		pixelformat=VIDEOFORMAT_RGB24;
	} else {
		if (osg::Image::computeNumComponents(m_image->getPixelFormat()) == 4) {
			m_image->setPixelFormat(GL_RGBA);
			pixelsize=4;
			pixelformat=VIDEOFORMAT_RGBA32;
		} else {
			std::cerr<<"ERROR:can't load the image, format not supported."<<std::endl;
			exit(-1);
		}
	}
	*/

	if (m_flip_vertical) this->flipVertical();
	if (m_flip_horizontal) this->flipHorizontal();

	return true;

}


void DummyVideo::update(osg::NodeVisitor* nv) {

	//this is the main function of your video plugin
	//you can either retrieve images from your video stream/camera/file
	//or communicate with a thread to synchronize and get the data out
	
	//the most important is to synchronize your data
	//and copy the result to the VideoImageSteam used in this plugin
	//

	//0. you can collect some stats, for that you can use a timer
	osg::Timer t;

	{

	//1. mutex lock access to the image video stream
	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());

	//2. you can copy here the video buffer to the main image video stream
	//memcpy(this->data(),newImage, this->getImageSizeInBytes());
	// it can be retrieved from another thread

	osg::notify(osg::DEBUG_INFO)<<"osgART::DummyVideo::update() get new image.."<<std::endl;

	//3. don't forget to call this to notify the rest of the application
	//that you have a new video image
	this->dirty();
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
	if (updateTimer.time_m() > 50) {
		this->dirty();
		updateTimer.setStartTick();
	}

}

osgART::VideoConfiguration* DummyVideo::getConfiguration() {

	if (!vconf)
	{
		vconf=new osgART::VideoConfiguration();
	}
	return vconf;
}


//field function get/set
void DummyVideo::setImageFile(const std::string & _NewFile) {
	videoName = _NewFile;
	open();
}
	
std::string DummyVideo::getImageFile() const {
	return videoName;
}

void DummyVideo::start() 
{ 
	//here you can start to stream the images, starting the camera acquisition
	//or starting to decompress video files

	//if you run a threaded video plugin, you can start the thread here


	//in this example we don't do anything
}

void DummyVideo::stop() 
{ 

	//here you can stop any streaming, camera acquisition or video file decompression

	//if you run a threaded video plugin, you can stop the thread here

	//in this example we don't do anything

}

void DummyVideo::close() 
{ 
	//here you can close any streaming open, close a camera 
	//and clean your specific data structure

	//in this example we don't do anything
}


//at the end you register your video plugin, the syntax generally
//osgART::PluginProxy<PluginClassNameVideo> g_PluginClassNameVideo("osgart_video_pluginclassnamevideo");
osgART::PluginProxy<DummyVideo> g_DummyVideo("osgart_video_dummyvideo");
