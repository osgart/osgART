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
#include "osgART/VideoConfiguration"

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


	META_Object(osgART,OpenCVVideo)

	/**
	* Affectation operator.
	*
	*/
	OpenCVVideo& operator = (const OpenCVVideo &);

	/**
		* Open the video stream. Access the video stream (hardware or file) and get an handle on it.
		*/
	bool init();

	/**
		* Close the video stream. Terminates the connection with the video stream and clean handle.
		*/
	bool close(bool = true);

	/**
		* Start the video stream grabbing. Start to get image from the video stream. In function of the
		* implementation on different platform, this function can run a thread, signal or
		* real-time function.
		*/
	bool start();

	/**
		* Stop the video stream grabbing. Stop to get image from the video stream. In function
		* of the implementation on different platform, this function can stop a thread, signal or
		* real-time function.
		*/
	bool stop();

	/**
		* Update the video stream grabbing. Try to get an image of the video instance, usable
		* by your application.
		*/
	bool update(osg::NodeVisitor* nv = 0L);

	/**
	* Deallocate image memory. Deallocates any internal memory allocated by the instance of this
	* class.
	*/
	void releaseImage();

	//we use the default video configuration class
	//virtual osgART::VideoConfiguration* getOrCreateConfiguration();


private:

	VideoCapture m_video;
	int m_camIndex;

	Mat m_OCVImage;

	GLint _internalformat_GL;
	GLenum _format_GL;
	GLenum _datatype_GL;

};


OpenCVVideo::OpenCVVideo() : osgART::Video()
{

}

OpenCVVideo::OpenCVVideo(const OpenCVVideo &,
		const osg::CopyOp& copyop/* = osg::CopyOp::SHALLOW_COPY*/) : osgART::Video()
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
OpenCVVideo::init()
{
	//check first device name, after device id
	//if (m_config.devicename!="")
	//{
	//
	//}
	
	if (_videoConfiguration->deviceid!=-1)
	{
		if (m_video.open(_videoConfiguration->deviceid))
		{
			osg::notify() << std::dec<< "OpenCVVideo::open() succesful.."<<std::endl;
		}
	}
	else
	{
		m_video.open(0);
		std::cout << "OpenCVVideo::open() use default device " <<0<<std::endl;
	}
	

	//if (m_config.deviceconfig != "") {
	//	config = (char*)&m_config.deviceconfig.c_str()[0];
	//}

//GL_BGRA, GL_UNSIGNED_BYTE
	// create an image that same size (packing set to 1)

	
#ifdef __APPLE__
	// in OpenCV version 2.4.6, The QTKit or AVFoundation interface 
	// only uses BGRA format, only allow you to specify (or query) width, height format
	// with AVFoundation, default resolution is 480x360 and min frame duration 1/30
	// by default we convert BGRA to RGB in the update call
	_format_GL=GL_BGRA;
	_format_GL=GL_RGB;
	_datatype_GL=GL_UNSIGNED_BYTE;
#else
	_format_GL=GL_BGR;
	_datatype_GL=GL_UNSIGNED_BYTE;
#endif



//m_video.set(CV_CAP_PROP_FRAME_WIDTH,800);
//	m_video.set(CV_CAP_PROP_FRAME_HEIGHT,600);
	if ((_videoConfiguration->width!=-1)&&(_videoConfiguration->height!=-1))
	{
		m_video.set(CV_CAP_PROP_FRAME_WIDTH,_videoConfiguration->width);
		m_video.set(CV_CAP_PROP_FRAME_HEIGHT,_videoConfiguration->height);
	}
	else
	{
#ifdef __APPLE__
	m_video.set(CV_CAP_PROP_FRAME_WIDTH,1280);
	m_video.set(CV_CAP_PROP_FRAME_HEIGHT,720);
	std::cout << "OpenCVVideo::open() use default resolution " <<1280<<"x"<<720<<std::endl;
#else
	m_video.set(CV_CAP_PROP_FRAME_WIDTH,640);
	m_video.set(CV_CAP_PROP_FRAME_HEIGHT,480);
	std::cout << "OpenCVVideo::open() use default resolution " <<640<<"x"<<480<<std::endl;
#endif	

	}
	
	if (_videoConfiguration->framerate!=-1)
	{
		m_video.set(CV_CAP_PROP_FPS,_videoConfiguration->framerate);	
	}
	else
	{
		m_video.set(CV_CAP_PROP_FPS,30);		
		std::cout << "OpenCVVideo::open() use default framerate " <<30<<std::endl;
	}
		
	_videoConfiguration->selectedWidth = m_video.get(CV_CAP_PROP_FRAME_WIDTH);
	_videoConfiguration->selectedHeight = m_video.get(CV_CAP_PROP_FRAME_HEIGHT);
	_videoConfiguration->selectedFrameRate =  m_video.get(CV_CAP_PROP_FPS);

	std::cout << "OpenCVVideo::open() size of video " <<
			_videoConfiguration->selectedWidth << " x " << _videoConfiguration->selectedHeight << " format="<< m_video.get(CV_CAP_PROP_FOURCC)<<std::endl;

	_videoStreamList.push_back(new osgART::VideoStream());

	_videoStreamList[0]->allocateImage(_videoConfiguration->selectedWidth, _videoConfiguration->selectedHeight, 1, _format_GL, _datatype_GL, 1);

	_videoStreamList[0]->setDataVariance(osg::Object::DYNAMIC);

	return true;
}

bool
OpenCVVideo::close(bool waitForThread)
{
	m_video.release();
	
	return true;
}

bool
OpenCVVideo::start()
{
	_videoStreamList[0]->play();
	
	return true;
}

bool
OpenCVVideo::stop()
{
	_videoStreamList[0]->pause();
	
	return true;
}

bool
OpenCVVideo::update(osg::NodeVisitor* nv)
{
	osg::Timer t;

	if (m_video.isOpened())
	{
		if (m_video.grab())
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());

			cv::Mat frame;
		
			m_video.retrieve(frame);

					// only clone if it is not continous (most of the time)
			  //      _image = (frame.isContinuous()) ? frame : frame.clone();

					// check for nv (NodeVisitor) - otherwise being called from a constructor!
				   // if (nv && _subscriber.valid()) _subscriber->updateWithImage(_image);

					// set format - OpenCV always BGR
					//setPixelFormat(GL_BGR);

			// copy data
			//memcpy(data(),frame.data,getImageSizeInBytes());

					// need to use dirty() as setModifiedCount(int) does not update
					// the backend buffer object

			Mat bgra_to_rgb;

#ifdef __APPLE__
			//on mac os, video capture (QTKit/AVFoundation backend) always return bgra
			cvtColor(frame, bgra_to_rgb, CV_BGRA2RGB);
			memcpy(_videoStreamList[0]->data(),(unsigned char*)bgra_to_rgb.data,_videoStreamList[0]->getImageSizeInBytes());
#else	
			//on windows, video capture (videoInput backend) return rgb
			memcpy(_videoStreamList[0]->data(),(unsigned char*)frame.data,_videoStreamList[0]->getImageSizeInBytes());
#endif

				
			/*
			m_video>>m_OCVImage;

			Mat bgr_to_rgb;

			cvtColor(m_OCVImage, bgr_to_rgb, CV_BGR2RGB);

			memcpy(this->data(),(unsigned char*)bgr_to_rgb.data,this->getImageSizeInBytes());
			*/
			_videoStreamList[0]->dirty();

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
	return true;
}

void OpenCVVideo::releaseImage()
{
}


// initializer for dynamic loading
osgART::PluginProxy<OpenCVVideo> g_opencvvideo("osgart_video_opencv");


