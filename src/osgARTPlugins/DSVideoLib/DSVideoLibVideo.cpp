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
*  \file  DSVideoLibVideo
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

#include <OpenThreads/Thread>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osg/Notify>

#include <osgART/PluginManager>
#include <osgART/Video>
#include <osgART/VideoConfiguration>

#include <iostream>
#include <string>

// graphics include
#include <osgART/Export>
#include <OpenThreads/Mutex>
#include <osg/Image>
#include <osg/Timer>

// local include

#include "osgART/PluginManager"
#include "osgART/Video"
#include "osgART/VideoConfiguration"

#include <DSVL.h>


/**
 * class DSVideoLibVideo.
 *
 */
class DSVideoLibVideo : public osgART::Video
{
public:        
// Standard Services
    
    /** 
    * \brief default constructor.
    * The default constructor.
    * @param config a string definition of the Video. See documentation
    * of DummyImage for further details.
    */
    DSVideoLibVideo();
    
    /** 
    * \brief copy constructor.
    */
     DSVideoLibVideo(const DSVideoLibVideo &, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);



	META_Object(osgART,DSVideoLibVideo);

    
    /** 
    * \brief affectation operator.
    *
    */
    DSVideoLibVideo& operator=(const DSVideoLibVideo &);
 
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
    virtual ~DSVideoLibVideo();       
    

private:

	//set/get variables
	std::string videoName;
	DSVL_VideoSource	*graphManager;
	MemoryBufferHandle  m_Handle;

};

DSVideoLibVideo::DSVideoLibVideo():
	osgART::Video()
{

}

DSVideoLibVideo::DSVideoLibVideo(const DSVideoLibVideo &, const osg::CopyOp& copyop):	osgART::Video()
{
    
}

DSVideoLibVideo::~DSVideoLibVideo(void) {
    
}

DSVideoLibVideo&  DSVideoLibVideo::operator=(const DSVideoLibVideo &) {
    return *this;
}


bool DSVideoLibVideo::init() {

	if (_videoConfiguration)
	{
		if (!_videoConfiguration->config.empty())
		{
			videoName=_videoConfiguration->config;
		}
	}
	else
	{
	//if there is no configuration we use the field variable
	//to check if there is a defined videoName

		if (videoName.empty()) {
			osg::notify(osg::FATAL) << "Error in DSVideoLibVideo::open(), File name is empty!";
			return false;
		}
	}
	
	CoInitialize(NULL);

	graphManager = new DSVL_VideoSource();

	if(FAILED(graphManager->BuildGraphFromXMLFile((char*)videoName.c_str())))
	{
		std::cerr<<"OSGART->ERROR:Failed to build graph manager!!"<<std::endl;
		exit(-1);
	}

	if(FAILED(graphManager->EnableMemoryBuffer())) 
	{
		std::cerr<<"OSGART->ERROR:Failed to get memory buffer!!"<<std::endl;
		exit(-1);
	}

	long frame_width;
	long frame_height;
	double frame_fps;
	PIXELFORMAT frame_format;

	graphManager->GetCurrentMediaFormat(&frame_width, &frame_height,&frame_fps,&frame_format);

	_videoConfiguration->selectedWidth=frame_width;
	_videoConfiguration->selectedHeight=frame_height;
	_videoConfiguration->selectedFrameRate=frame_fps;

	//we need to create one video stream
	_videoStreamList.push_back(new osgART::VideoStream());

	//this is main function you need to call to be sure to
	//allocate your image
	//here you define the image format, image size
	//that will be streamed by your plugin

	GLint _internalformat_GL;
	GLenum _format_GL;
	GLenum _datatype_GL;

	switch (frame_format)
	{
	case PIXELFORMAT::PIXELFORMAT_RGB32:
		_format_GL=GL_BGRA;
		_datatype_GL=GL_UNSIGNED_BYTE;
		break;
	case PIXELFORMAT::PIXELFORMAT_RGB24:
		_format_GL=GL_BGR;
		_datatype_GL=GL_UNSIGNED_BYTE;
		break;
	case PIXELFORMAT::PIXELFORMAT_RGB565:
		_format_GL=GL_RGB;
		_datatype_GL=GL_UNSIGNED_SHORT_5_6_5;
		break;	
	case PIXELFORMAT::PIXELFORMAT_YUY2:
	case PIXELFORMAT::PIXELFORMAT_UYVY:
	case PIXELFORMAT::PIXELFORMAT_RGB555:
	case PIXELFORMAT::PIXELFORMAT_UNKNOWN:
	case PIXELFORMAT::PIXELFORMAT_INVALID:
	default:
		std::cout<<"video format 555 not supported"<<std::endl;
		exit(-1);
		//_format_GL=GL_RGB;
		//_datatype_GL=GL_UNSIGNED_SHORT_55
		break;
	}

	_videoStreamList[0]->allocateImage(frame_width, frame_height, 1, _format_GL, _datatype_GL, 1);

	return true;

}


bool DSVideoLibVideo::update(osg::NodeVisitor* nv) {

	if (!graphManager) return false;

	osg::Timer t;

	unsigned char *newImage = 0L;
	unsigned int wait_result;

	graphManager->CheckinMemoryBuffer(m_Handle);	
	// 
	wait_result = graphManager->WaitForNextSample();

	if(wait_result == WAIT_OBJECT_0) 
	{

		//1. mutex lock access to the image video stream
		OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());

		if (FAILED(graphManager->CheckoutMemoryBuffer(&(m_Handle), &newImage, 
			NULL, NULL, NULL, &(m_Handle.t))))	
		{
			newImage = NULL;
		}
	}
	if (newImage) 
	{
		memcpy(_videoStreamList[0]->data(),newImage, _videoStreamList[0]->getImageSizeInBytes());
		// the newImage can be retrieved from another thread
		// in this example we do nothing (already make a dummy copy in init())

		osg::notify(osg::DEBUG_INFO)<<"osgART::DSVideoLibVideo::update() get new image.."<<std::endl;

		//3. don't forget to call this to notify the rest of the application
		//that you have a new video image
		_videoStreamList[0]->dirty();
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

	return true;
}

bool DSVideoLibVideo::start() 
{ 
	unsigned char *newImage = NULL;
	unsigned int wait_result;

	if (FAILED(graphManager->Run()))
	{
		std::cerr<<"osgart_dsvl ERROR: can't start the graph manager!!"<<std::endl;
		exit(-1);
	}
	// get one image to efficiencly run the video buffering
	// not using the timeout
	wait_result = graphManager->WaitForNextSample();

	// 
	graphManager->CheckoutMemoryBuffer(&(m_Handle), &newImage, NULL, NULL, NULL, &(m_Handle.t));

	_videoStreamList[0]->play();	

	return true;
}

bool DSVideoLibVideo::stop() 
{ 
	graphManager->CheckinMemoryBuffer(m_Handle, true);

	if(FAILED(graphManager->Stop()))
	{
		std::cerr<<"OSGART->ERROR:can't stop the graph manager!!"<<std::endl;
		exit(-1);
	}

	_videoStreamList[0]->pause();	

	return true;
}

bool DSVideoLibVideo::close(bool waitForThread) 
{ 
	this->stop();

	delete graphManager;

	return true;
}

//at the end you register your video plugin, the syntax generally
//osgART::PluginProxy<PluginClassNameVideo> g_PluginClassNameVideo("osgart_video_pluginclassnamevideo");
osgART::PluginProxy<DSVideoLibVideo> g_DSVideoLibVideo("osgart_video_dsvideolib");
