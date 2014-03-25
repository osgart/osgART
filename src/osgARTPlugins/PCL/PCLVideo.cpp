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
*  \file  PCLVideo
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
#include "osgART/Utils"
#include "osgART/PluginManager"
#include "osgART/Video"
#include "osgART/VideoConfiguration"

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/openni_grabber.h>
#include <pcl/common/time.h>
#include <pcl/console/parse.h>

/**
 * class PCLVideo.
 *
 */
class PCLVideo : public osgART::Video
{
public:        
// Standard Services
    
    /** 
    * \brief default constructor.
    * The default constructor.
    * @param config a string definition of the Video. See documentation
    * of DummyImage for further details.
    */
    PCLVideo();
    
    /** 
    * \brief copy constructor.
    */
     PCLVideo(const PCLVideo &, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);



	META_Object(osgART,PCLVideo);

    
    /** 
    * \brief affectation operator.
    *
    */
    PCLVideo& operator=(const PCLVideo &);
    	
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
    virtual ~PCLVideo();       
    
    void cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr &cloud);

	void imageDepthImageCallback (const boost::shared_ptr<openni_wrapper::Image>&, const boost::shared_ptr<openni_wrapper::DepthImage>& d_img, float constant);

private:

	//video configuration
	osgART::VideoConfiguration* vconf;

	//set/get variables
	std::string videoNameColor;
	std::string videoNameDepth;

	bool m_flip_horizontal;
	bool m_flip_vertical;

	unsigned int m_max_width;

	osg::Timer updateTimer;

	xn::DepthMetaData* depthMD;
	xn::ImageMetaData* imageMD;

	pcl::OpenNIGrabber interface;

	bool save;
	
	unsigned char* _depthBufferByte;

};

PCLVideo::PCLVideo():
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

PCLVideo::PCLVideo(const PCLVideo &, const osg::CopyOp& copyop) {
    
}

PCLVideo::~PCLVideo(void) {
    
}

PCLVideo&  PCLVideo::operator=(const PCLVideo &) {
    return *this;
}

void 
PCLVideo::cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr &cloud)
{
  static unsigned count = 0;
  static double last = pcl::getTime ();
  if (++count == 30)
  {
	double now = pcl::getTime ();
	std::cout << "distance of center pixel :" << cloud->points [(cloud->width >> 1) * (cloud->height + 1)].z << " mm. Average framerate: " << double(count)/double(now - last) << " Hz" <<  std::endl;
	count = 0;
	last = now;
  }

  if (save)
  {
	std::stringstream ss;
	ss << std::setprecision (12) << pcl::getTime () * 100 << ".pcd";
	pcl::PCDWriter w;
	w.writeBinaryCompressed (ss.str (), *cloud);
	std::cout << "wrote point clouds to file " << ss.str () << std::endl;
  }
}

void 
PCLVideo::imageDepthImageCallback (const boost::shared_ptr<openni_wrapper::Image>& color_img, const boost::shared_ptr<openni_wrapper::DepthImage>& depth_img, float constant)
{
  static unsigned count = 0;
  static double last = pcl::getTime ();
  if (++count == 30)
  {
	double now = pcl::getTime ();
	std::cout << "got synchronized image x depth-image with constant factor: " << constant << ". Average framerate: " << double(count)/double(now - last) << " Hz" <<  std::endl;
	std::cout << "Depth baseline: " << depth_img->getBaseline () << " and focal length: " << depth_img->getFocalLength () << std::endl;
	count = 0;
	last = now;
  }
  
  	{

	//1. mutex lock access to the image video stream
	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());

	osg::notify(osg::DEBUG_INFO)<<"osgART::OpenNIVideo::update() get new image.."<<std::endl;

//	depthMD=depth_img->getDepthMetaData();
	const XnDepthPixel* pDepthMap = depth_img->getDepthMetaData().Data();
	//depth pixel floating point depth map.
	
//	imageMD=color_img->getMetaData();
	const XnUInt8* pImageMap = color_img->getMetaData().Data();

	const XnDepthPixel* pDepth=pDepthMap;
	const XnUInt8* pImage=pImageMap;
	
	XnDepthPixel zMax = depth_img->getDepthMetaData().ZRes();
    //convert float buffer to unsigned short
	for ( unsigned int i=0; i<(depth_img->getDepthMetaData().XRes() * depth_img->getDepthMetaData().YRes()); ++i )
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
}
    
bool PCLVideo::init() {

	//open the video 
	//if you are using a device, you can open the device
	//if you are using video streaming, you can initialize the connection
	//if you are using video files, you can read the configuration, cache the data, etc.

	// Set the depth output format
    interface.getDevice()->setDepthOutputFormat(openni_wrapper::OpenNIDevice::OpenNI_12_bit_depth);

      // make callback function from member function
      boost::function<void (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr&)> f =
        boost::bind (&PCLVideo::cloud_cb_, this, _1);

      // connect callback function for desired signal. In this case its a point cloud with color values
      boost::signals2::connection c = interface.registerCallback (f);

      // make callback function from member function
      boost::function<void (const boost::shared_ptr<openni_wrapper::Image>&, const boost::shared_ptr<openni_wrapper::DepthImage>&, float constant)> f2 =
        boost::bind (&PCLVideo::imageDepthImageCallback, this, _1, _2, _3);

      // connect callback function for desired signal. In this case its a point cloud with color values
      boost::signals2::connection c2 = interface.registerCallback (f2);

	int w=640;
	int h=480;
	
	_depthBufferByte=new unsigned char[w*h];

	//we need to create one video stream
	_videoStreamList.push_back(new osgART::VideoStream());

	//this is main function you need to call to be sure to
	//allocate your image
	//here you define the image format, image size
	//that will be streamed by your plugin
	

	_videoStreamList[0]->allocateImage(w, h, 1, GL_BGRA, GL_UNSIGNED_BYTE);

	
	//we need to create a second video stream for depth
	_videoStreamList.push_back(new osgART::VideoStream());

	//this is main function you need to call to be sure to
	//allocate your image
	//here you define the image format, image size
	//that will be streamed by your plugin

	//depth image
	_videoStreamList[1]->allocateImage(w, h, 1, GL_BGRA, GL_UNSIGNED_BYTE);

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


bool PCLVideo::update(osg::NodeVisitor* nv) {

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
	//memcpy(_videoStreamList[0]->data(),newImage, _videoStreamList[0]->getImageSizeInBytes());
	// the newImage can be retrieved from another thread
	// in this example we do nothing (already make a dummy copy in init())

	//same for second stream
	//memcpy(_videoStreamList[1]->data(),newImage, _videoStreamList[]->getImageSizeInBytes());
	
	osg::notify(osg::DEBUG_INFO)<<"osgART::PCLVideo::update() get new image.."<<std::endl;

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

osgART::VideoConfiguration* PCLVideo::getConfiguration() {

	if (!vconf)
	{
		vconf=new osgART::VideoConfiguration();
	}
	return vconf;
}

bool PCLVideo::start() 
{ 
	//here you can start to stream the images, starting the camera acquisition
	//or starting to decompress video files

	//if you run a threaded video plugin, you can start the thread here

	//in this example we only start to play the VideoStream 0
	
	interface.start();

	_videoStreamList[0]->play();	
	_videoStreamList[1]->play();	

	return true;
}

bool PCLVideo::stop() 
{ 

	//here you can stop any streaming, camera acquisition or video file decompression

	//if you run a threaded video plugin, you can stop the thread here

	//in this example we only pause the VideoStream 0

    interface.stop();

	_videoStreamList[0]->pause();	
	_videoStreamList[1]->pause();	

	return true;
}

bool PCLVideo::close(bool waitForThread) 
{ 
	//here you can close any streaming open, close a camera 
	//and clean your specific data structure

	//in this example we don't do anything
	
	return true;
}

//at the end you register your video plugin, the syntax generally
//osgART::PluginProxy<PluginClassNameVideo> g_PluginClassNameVideo("osgart_video_pluginclassnamevideo");
osgART::PluginProxy<PCLVideo> g_PCLVideo("osgart_video_pcl");
