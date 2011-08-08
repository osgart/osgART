/* -*-c++-*-
 *
 * osgART - OpenCV for OpenSceneGraph
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


//OpenCV header include
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include <osg/Object>
#include <osg/Notify>
#include <osg/Timer>

#include <osgDB/FileUtils>

#include <iostream>
#include <iomanip>
#include <cstring>


#include "osgART/PluginManager"
#include "osgART/Video"
#include "osgART/VideoConfig"

using namespace cv;

class OpenCVVideo : public osgART::Video
{
public:




// Standard Services

	/**
		* Default constructor. It creates a video source from a configuration string
		* as it is been used in the original AR Toolkit 2.71
		* \param videoName a string definition of the video background. See documentation
		* of OpenCV for further details.
		*/
	OpenCVVideo();

	/**
		* Copy constructor.
		*
		*/
	OpenCVVideo(const OpenCVVideo &,
		const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

	/**
		* Destructor.
		*
		*/
	virtual ~OpenCVVideo();


	META_Object(osgART,OpenCVVideo);

	/**
	* Affectation operator.
	*
	*/
	OpenCVVideo& operator = (const OpenCVVideo &);

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

	VideoCapture m_video;
	int m_camIndex;

	Mat m_OCVImage;


	osgART::VideoConfiguration m_config;

	GLint _internalformat_GL;
	GLenum _format_GL;
	GLenum _datatype_GL;

};


OpenCVVideo::OpenCVVideo() : osgART::Video()
{

}

OpenCVVideo::OpenCVVideo(const OpenCVVideo &,
		const osg::CopyOp& copyop/* = osg::CopyOp::SHALLOW_COPY*/)
{
}

OpenCVVideo::~OpenCVVideo()
{
	//this->close(false);
}

OpenCVVideo&
OpenCVVideo::operator=(const OpenCVVideo &)
{
	return *this;
}

bool
OpenCVVideo::open()
{
	int xsize=0, ysize=0;
	m_camIndex=0;

	if (m_video.open(m_camIndex))
	{
		osg::notify() << std::dec<< "OpenCVVideo::open() succesful.."<<std::endl;
	}

/*
	if (m_config.deviceconfig != "") {
		config = (char*)&m_config.deviceconfig.c_str()[0];
	}

		// report the actual
		osg::notify() << std::dec << "OpenCVVideo::open() size of video " <<
			xsize << " x " << ysize << ", fps: " << fps << std::endl;

	//	m_config.selectedWidth = xsize;
	//	m_config.selectedHeight = ysize;
	//	m_config.selectedFrameRate = fps;

	}

//GL_BGRA, GL_UNSIGNED_BYTE
	// create an image that same size (packing set to 1)

	*/
	xsize=m_video.get(CV_CAP_PROP_FRAME_WIDTH);
	ysize=m_video.get(CV_CAP_PROP_FRAME_HEIGHT);
	m_video.set(CV_CAP_PROP_FPS,30);
//	m_video.set(CV_CAP_PROP_CONVERT_RGB,1.0);
//	m_video.set(CV_CAP_PROP_FOURCC,0x32424752);//RGB
	_format_GL=GL_RGB;
	_datatype_GL=GL_UNSIGNED_BYTE;

	m_config.selectedWidth = xsize;
	m_config.selectedHeight = ysize;
	m_config.selectedFrameRate = 30;

	this->allocateImage(xsize, ysize, 1, _format_GL, _datatype_GL, 1);

	this->setDataVariance(osg::Object::DYNAMIC);

	return true;

}

void
OpenCVVideo::close(bool waitForThread)
{
	m_video.release();
}

void
OpenCVVideo::play()
{
	osg::ImageStream::play();
}

void
OpenCVVideo::pause()
{
	osg::ImageStream::pause();
}

void
OpenCVVideo::update(osg::NodeVisitor* nv)
{
	osg::Timer t;
	
	if (m_video.grab())
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());
		
		m_video>>m_OCVImage;

		Mat bgr_to_rgb;

		cvtColor(m_OCVImage, bgr_to_rgb, CV_BGR2RGB);
		
		memcpy(this->data(),(unsigned char*)bgr_to_rgb.data,this->getImageSizeInBytes());
		
		this->dirty();
		
	}
	
	if (nv) 
	{
		const osg::FrameStamp *framestamp = nv->getFrameStamp();

		if (framestamp && _stats.valid())
		{
			_stats->setAttribute(framestamp->getFrameNumber(), "Capture time taken", t.time_m());
		}
	}
}

osgART::VideoConfiguration*
OpenCVVideo::getVideoConfiguration()
{
	return &m_config;
}


void OpenCVVideo::releaseImage()
{
}


// initializer for dynamic loading
osgART::PluginProxy<OpenCVVideo> g_opencvvideo("osgart_video_opencv");


