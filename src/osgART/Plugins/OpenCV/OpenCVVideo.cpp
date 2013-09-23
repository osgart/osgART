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
	//void update();

	/**
		* Update the video stream grabbing. Try to get an image of the video instance, usable
		* by your application.
		*/
	void update(osg::NodeVisitor* nv);
//	void updateCB(osg::NodeVisitor* nv);

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
#ifdef __APPLE__
	// in OpenCV version 2.4.6, The QTKit or AVFoundation interface 
	// only uses BGRA format, only allow you to specify (or query) width, height format
	// with AVFoundation, default resolution is 480x360 and min frame duration 1/30
	//_format_GL=GL_BGRA;
	//TODO add a warning here about format
#else
	_format_GL=GL_BGR;
#endif

	_format_GL=GL_RGB;

//m_video.set(CV_CAP_PROP_FRAME_WIDTH,800);
//	m_video.set(CV_CAP_PROP_FRAME_HEIGHT,600);
	m_video.set(CV_CAP_PROP_FRAME_WIDTH,1280);
	m_video.set(CV_CAP_PROP_FRAME_HEIGHT,720);

	xsize=m_video.get(CV_CAP_PROP_FRAME_WIDTH);
	ysize=m_video.get(CV_CAP_PROP_FRAME_HEIGHT);
	m_video.set(CV_CAP_PROP_FPS,30);
	//m_video.get(CV_CAP_PROP_FPS,30);
	
//	m_video.set(CV_CAP_PROP_CONVERT_RGB,1.0);
//	m_video.set(CV_CAP_PROP_FOURCC,0x32424752);//RGB
	//_format_GL=GL_RGB;
//	_format_GL=GL_RGB;
	
	std::cout << "OpenCVVideo::open() size of video " <<
			xsize << " x " << ysize << "format="<< m_video.get(CV_CAP_PROP_FOURCC)<<std::endl;
	
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
/*
void
OpenCVVideo::update()
{


}*/

void
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
			
			cvtColor(frame, bgra_to_rgb, CV_BGRA2RGB);

			memcpy(data(),(unsigned char*)bgra_to_rgb.data,getImageSizeInBytes());
				
					/*
			m_video>>m_OCVImage;

			Mat bgr_to_rgb;

			cvtColor(m_OCVImage, bgr_to_rgb, CV_BGR2RGB);

			memcpy(this->data(),(unsigned char*)bgr_to_rgb.data,this->getImageSizeInBytes());
			*/
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


