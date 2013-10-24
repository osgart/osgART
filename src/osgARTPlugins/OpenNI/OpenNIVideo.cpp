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


	/**
    * \brief Select a new image and open the Video stream.
    * Access the Video stream (hardware or file) and get an handle on it.
    */
	void setImageFile(const std::string &_NewFile);
	
	/**
    * \brief Get the image filename.
    */	
	std::string getImageFile()const;
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

	//you can also create some specific get/set function
	//such as setting up the name of the image file
	//or calling specific function such as camera exposure, ROI video mode, etc
	_fields["image_file"]		= new osgART::CallbackField<OpenNIVideo, std::string>(this,
		&OpenNIVideo::getImageFile,
		&OpenNIVideo::setImageFile);
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
			osg::notify(osg::FATAL) << "Error in OpenNIVideo::open(), File name is empty!";
			return false;
		}
	}
	
	osg::notify(osg::INFO) << "OpenNIVideo::open()  open image : " << videoName << std::endl;

	//for this example, we load a picture
	osg::Image* img = osgDB::readImageFile(videoName.c_str());

	if (!img) {
		osg::notify(osg::FATAL) << "Error in OpenNIVideo::open(), Could not open File!";
		return false;
	}

	unsigned int w = img->s();
	unsigned int h = img->t();
	
	//std::cerr <<"image size="<<w<<"x"<<h<<std::endl;	

	if (w > m_max_width) {
		osg::notify() << "OpenNIVideo: Image width exceeds maximum (" << m_max_width << "). Image will be resized";

		float aspect = (float)h / (float)w;
		w = m_max_width;
		h = w * aspect;

		img->scaleImage(w, h, 1);

		//osgDB::writeImageFile(*img, "test_resize.jpg");

	}

	xn::EnumerationErrors errors;

	int resolutionX = 640;
	int resolutionY = 480;
	unsigned int FPS = 30;

	XnStatus nRetVal = XN_STATUS_OK;

	std::cout<<"Kinect init.."<<std::endl;

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
	
	//we need to create one video stream
	_videoStreamList.push_back(new osgART::VideoStream());

	//this is main function you need to call to be sure to
	//allocate your image
	//here you define the image format, image size
	//that will be streamed by your plugin
	
	_videoStreamList[0]->allocateImage(w, h, 1, GL_BGRA, GL_UNSIGNED_BYTE);

	//if you need any format conversion you need to handle it internally
	//in the update before updating the image
	
	//here we do a simple conversion as our dummy plugin only read the content
	//once
	//int components = osg::Image::computeNumComponents(img->getPixelFormat());
	
	//std::cout<<"image format="<<img->getPixelFormat()<<"(GL_RGB="<<GL_RGB<<" GL_RGBA="<<GL_RGBA<<" GL_BGR="<<GL_BGR<<" GL_BGRA="<<GL_BGRA<<std::endl;

	//if we have BGRA we just do a copy
	if (img->getPixelFormat()==GL_BGRA)
		memcpy(_videoStreamList[0]->data(),img->data(), _videoStreamList[0]->getImageSizeInBytes());
	//otherwise
	//if we have RGB, or RGBA we convert
	if ((img->getPixelFormat()==GL_RGB)||(img->getPixelFormat()==GL_RGBA))
	{
		for (unsigned int j = 0; j < h; j++) {
			for (unsigned int i = 0; i < w; i++) {

				// image->data() return a pixel in the supported channel format (3 or 4 bytes or less)
				unsigned char* srcPtr = img->data(i, j);
				unsigned char* dstPtr = _videoStreamList[0]->data(i, j);

				dstPtr[0] = srcPtr[2];
				dstPtr[1] = srcPtr[1];
				dstPtr[2] = srcPtr[0];
				dstPtr[3] = 0;
			}
		}
	}
	//if we have BGR, we just return an error here
	if (img->getPixelFormat()==GL_BGR)
	{
		osg::notify(osg::FATAL) << "OpenNIVideo::open() doesn't support BGR image";
		return false;
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
	
	if (m_flip_vertical) _videoStreamList[0]->flipVertical();
	if (m_flip_horizontal) _videoStreamList[0]->flipHorizontal();

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

	//2. you can copy here the video buffer to the main image video stream
	//with a call like
	//memcpy(this->data(),newImage, this->getImageSizeInBytes());
	// the newImage can be retrieved from another thread
	// in this example we do nothing (already make a dummy copy in init())

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


	// Increase modified count every X ms to ensure tracker updates
	if (updateTimer.time_m() > 50) {
		_videoStreamList[0]->dirty();
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


//field function get/set
void OpenNIVideo::setImageFile(const std::string & _NewFile) {
	videoName = _NewFile;
	init();
}
	
std::string OpenNIVideo::getImageFile() const {
	return videoName;
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
osgART::PluginProxy<OpenNIVideo> g_OpenNIVideo("osgart_video_OpenNIVideo");
