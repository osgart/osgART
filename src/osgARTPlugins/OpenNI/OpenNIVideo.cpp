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
*  \file  OpenNIVideo
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

#include "XnOpenNI.h"
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <XnFPSCalculator.h>

using namespace xn;

#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
{																\
	printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
}

/**
 * class OpenNIVideo.
 *
 */
class OpenNIVideo : public osgART::Video
{
public:        
// Standard Services
    
    /** 
    * \brief default constructor.
    * The default constructor.
    * @param config a string definition of the Video. See documentation
    * of DummyImage for further details.
    */
    OpenNIVideo();
    
    /** 
    * \brief copy constructor.
    */
     OpenNIVideo(const OpenNIVideo &, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);



	META_Object(osgART,OpenNIVideo);

    
    /** 
    * \brief affectation operator.
    *
    */
    OpenNIVideo& operator=(const OpenNIVideo &);
    	
    /**
	 * Get the video configuration struct for Dummy Video.
	 * in this example, we will use the config string 
	 * to setup the name of an image file.
	 * \return struct VideoConfiguration
	*/
    virtual osgART::VideoConfiguration* getConfiguration();

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
    virtual ~OpenNIVideo();       
    

private:

	//video configuration
	osgART::VideoConfiguration* vconf;

	//set/get variables
	std::string videoName;

	bool m_flip_horizontal;
	bool m_flip_vertical;

	unsigned int m_max_width;

	osg::Timer updateTimer;

	xn::Context context;
	xn::DepthGenerator depth_generator;
	xn::ImageGenerator image_generator;
	XnFPSData xnFPS;
	
	xn::DepthMetaData depthMD;
	xn::ImageMetaData imageMD;

	unsigned short* _depthBufferShort;
	unsigned char* _depthBufferByte;
};

OpenNIVideo::OpenNIVideo():
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
}

OpenNIVideo::OpenNIVideo(const OpenNIVideo &, const osg::CopyOp& copyop) {
    
}

OpenNIVideo::~OpenNIVideo(void) {
    
}

OpenNIVideo&  OpenNIVideo::operator=(const OpenNIVideo &) {
    return *this;
}


bool OpenNIVideo::init() {

	//open the video 
	//if you are using a device, you can open the device
	//if you are using video streaming, you can initialize the connection
	//if you are using video files, you can read the configuration, cache the data, etc.

	xn::EnumerationErrors errors;

	int resolutionX = 640;
	int resolutionY = 480;
	unsigned int FPS = 30;

	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = context.Init();
	CHECK_RC(nRetVal, "context global init");

	//xn::NodeInfoList list;
	//nRetVal = context.EnumerateProductionTrees(XN_NODE_TYPE_DEVICE, NULL, list, &errors);
	//CHECK_RC(nRetVal, "enumerate production tree");
	
	// HandsGenerator hands;
	//UserGenerator user;
	//GestureGenerator gesture;
	//SceneAnalyzer scene;

	nRetVal = depth_generator.Create(context);
	CHECK_RC(nRetVal, "creating depth generator");

	nRetVal = image_generator.Create(context);
	CHECK_RC(nRetVal, "creating image generator");

	
	if(depth_generator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT))
	{
		nRetVal = depth_generator.GetAlternativeViewPointCap().SetViewPoint(image_generator);
		CHECK_RC(nRetVal, "creating registered image/depth generator");
	}
	else
	{
		printf("WARNING: XN_CAPABILITY_ALTERNATIVE_VIEW_POINT not supported");
	}

	if (depth_generator.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC))
	{
		if( depth_generator.GetFrameSyncCap().CanFrameSyncWith(image_generator)) {
			//nRetVal=depth.GetFrameSyncCap().FrameSyncWith(image);
			//CHECK_RC(nRetVal, "creating frame sync image/depth generator");
		}
	}
	else
	{
		printf("WARNING: XN_CAPABILITY_FRAME_SYNC not supported");
	}

	XnMapOutputMode mode = {resolutionX,resolutionY,FPS};

	nRetVal = depth_generator.SetMapOutputMode(mode);
	CHECK_RC(nRetVal, "set output mode");

	//NOT NEEDED IF SYNCHRO
	nRetVal = image_generator.SetMapOutputMode(mode);
	CHECK_RC(nRetVal, "set output mode");
	
	_depthBufferShort=new unsigned short[resolutionX*resolutionY];
	_depthBufferByte=new unsigned char[resolutionX*resolutionY];
	
	//we need to create one video stream
	_videoStreamList.push_back(new osgART::VideoStream());
	
	_videoStreamList[0]->allocateImage(resolutionX,resolutionY, 1, GL_RGB, GL_UNSIGNED_BYTE);

	//we need to create one video stream
	_videoStreamList.push_back(new osgART::VideoStream());
	
//	_videoStreamList[1]->allocateImage(resolutionX,resolutionY, 1, GL_LUMINANCE, GL_FLOAT);
	_videoStreamList[1]->allocateImage(resolutionX,resolutionY, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE);
	//_videoStreamList[1]->allocateImage(resolutionX,resolutionY, 1, GL_LUMINANCE, GL_UNSIGNED_SHORT);
	//_videoStreamList[1]->allocateImage(w, h, 1, GL_DEPTHCOMPONENT16, GL_UNSIGNED_BYTE);
	
	if (m_flip_vertical) 
	{
		_videoStreamList[0]->flipVertical();
		_videoStreamList[1]->flipVertical();
	}
	if (m_flip_horizontal) 
	{
		_videoStreamList[0]->flipHorizontal();
		_videoStreamList[1]->flipHorizontal();
	}
	return true;

}


bool OpenNIVideo::update(osg::NodeVisitor* nv) {

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

	osg::notify(osg::DEBUG_INFO)<<"osgART::OpenNIVideo::update() get new image.."<<std::endl;

	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal=context.WaitAndUpdateAll();
	CHECK_RC(nRetVal, "Update Data");

	xnFPSMarkFrame(&xnFPS);

	depth_generator.GetMetaData(depthMD);
	const XnDepthPixel* pDepthMap = depthMD.Data();
	//depth pixel floating point depth map.
	
	image_generator.GetMetaData(imageMD);
	const XnUInt8* pImageMap = imageMD.Data();

	// Hybrid mode isn't supported in this sample
	if (imageMD.FullXRes() != depthMD.FullXRes() || imageMD.FullYRes() != depthMD.FullYRes())
	{
		std::cerr<<"The device depth and image resolution must be equal!"<<std::endl;
		exit(1);
	}

	// RGB is the only image format supported.
	if (imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24)
	{
		std::cerr<<"The device image format must be RGB24"<<std::endl;
		exit(1);
	}
	
	const XnDepthPixel* pDepth=pDepthMap;
	const XnUInt8* pImage=pImageMap;
	
	XnDepthPixel zMax = depthMD.ZRes();
    //convert float buffer to unsigned short
	for ( unsigned int i=0; i<(depthMD.XRes() * depthMD.YRes()); ++i )
    {
            *(_depthBufferByte + i) = 255 * (float(*(pDepth + i)) / float(zMax));
    }

	memcpy(_videoStreamList[0]->data(),pImage, _videoStreamList[0]->getImageSizeInBytes());
	
	memcpy(_videoStreamList[1]->data(),_depthBufferByte, _videoStreamList[1]->getImageSizeInBytes());

	//3. don't forget to call this to notify the rest of the application
	//that you have a new video image
	_videoStreamList[0]->dirty();
	_videoStreamList[1]->dirty();
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
		_videoStreamList[0]->dirty();
		_videoStreamList[1]->dirty();
		updateTimer.setStartTick();
	}

	return true;
}

osgART::VideoConfiguration* OpenNIVideo::getConfiguration() {

	if (!vconf)
	{
		vconf=new osgART::VideoConfiguration();
	}
	return vconf;
}

bool OpenNIVideo::start() 
{ 
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = context.StartGeneratingAll();
	CHECK_RC(nRetVal, "start generators");

	nRetVal = xnFPSInit(&xnFPS, 180);
	CHECK_RC(nRetVal, "FPS Init");

	_videoStreamList[0]->play();	

	return true;
}

bool OpenNIVideo::stop() 
{ 

	//here you can stop any streaming, camera acquisition or video file decompression

	//if you run a threaded video plugin, you can stop the thread here

	//in this example we only pause the VideoStream 0

	_videoStreamList[0]->pause();	

	return true;
}

bool OpenNIVideo::close(bool waitForThread) 
{ 
	//here you can close any streaming open, close a camera 
	//and clean your specific data structure

	//in this example we don't do anything
	
	return true;
}

//at the end you register your video plugin, the syntax generally
//osgART::PluginProxy<PluginClassNameVideo> g_PluginClassNameVideo("osgart_video_pluginclassnamevideo");
osgART::PluginProxy<OpenNIVideo> g_OpenNIVideo("osgart_video_openni");
