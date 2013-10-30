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
*  \file  DummySensor
*  \brief dummy sensor
*
* 
* A demo plugin which show how to create a sensor class
*   \remark 
*
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
#include "osgART/Sensor"
#include "osgART/SensorConfiguration"


// you can create your own video configuration with
//additional parameters, overloaded function
class DummySensorConfiguration: public osgART::SensorConfiguration {
public:
    DummySensorConfiguration() : osgART::SensorConfiguration() {};

    ~DummySensorConfiguration() {};

};

/**
 * class DummySensor.
 *
 */
class DummySensor : public osgART::Sensor
{
public:        
// Standard Services
    
    /** 
    * \brief default constructor.
    * The default constructor.
    * @param config a string definition of the Video. See documentation
    * of DummyImage for further details.
    */
    DummySensor();
    
    /** 
    * \brief copy constructor.
    */
     DummySensor(const DummySensor &, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);



    META_Object(osgART,DummySensor);

    
    /** 
    * \brief affectation operator.
    *
    */
    DummySensor& operator=(const DummySensor &);
    	
    /**
	 * Get the video configuration struct for Dummy Video.
	 * in this example, we create our own VideoConfiguration class and use the config string 
	 * to setup the name of an image file.
	 * \return struct VideoConfiguration
	*/
    virtual osgART::SensorConfiguration* getOrCreateConfiguration();

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
	
protected:

	/** 
    * \brief destructor.
    *
    */
    virtual ~DummySensor();
    

private:

	osg::Timer updateTimer;

};

DummySensor::DummySensor():
    osgART::Sensor()
{
	//define specific field variables and functions

	//in this example, we create some options to change
	//the orientation of the image
    //_fields["sensor_freq"] = new osgART::TypedField<bool>(&_horizontalFlip);

	//you can also create some specific get/set function
	//such as setting up the name of the image file
	//or calling specific function such as camera exposure, ROI video mode, etc
    //_fields["image_file"]		= new osgART::CallbackField<DummySensor, std::string>(this,
    //    &DummySensor::getImageFile,
     //   &DummySensor::setImageFile);
}

DummySensor::DummySensor(const DummySensor &, const osg::CopyOp& copyop):	osgART::Sensor()
{
    
}

DummySensor::~DummySensor(void) {
    
}

DummySensor&  DummySensor::operator=(const DummySensor &) {
    return *this;
}


bool DummySensor::init() {

    //open the sensor

    //first, you can check if there is a video configuration defined
    if (_sensorConfiguration)
	{
        if (!_sensorConfiguration->config.empty())
		{
		}
	}
	else
	{

	}

	//we need to create one video stream
    _sensorDataList.push_back(new osgART::SensorData());

	//this is main function you need to call to be sure to
	//allocate your image
	//here you define the image format, image size
	//that will be streamed by your plugin
	
    //_sensorData[0]->init();

    //_sensorData[0].setDefaultValue();

	return true;

}


bool DummySensor::update(osg::NodeVisitor* nv) {

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

    osg::notify(osg::DEBUG_INFO)<<"osgART::DummySensor::update() get new image.."<<std::endl;

	//3. don't forget to call this to notify the rest of the application
	//that you have a new video image
    //_sensorDataList[0]->dirty();
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
      //  _sensorDataList[0]->dirty();
		updateTimer.setStartTick();
	}

	return true;
}

//if you define your own Video Configuration, you can overload
//this method
osgART::SensorConfiguration* DummySensor::getOrCreateConfiguration() {

    if (!_sensorConfiguration)
	{
      //create your own video configuration
        _sensorConfiguration=new DummySensorConfiguration();
	}
    return _sensorConfiguration;
}

bool DummySensor::start()
{ 

	return true;
}

bool DummySensor::stop()
{ 


	return true;
}

bool DummySensor::close(bool waitForThread)
{ 
		return true;
}

//at the end you register your video plugin, the syntax generally
//osgART::PluginProxy<PluginClassNameVideo> g_PluginClassNameVideo("osgart_video_pluginclassnamevideo");
osgART::PluginProxy<DummySensor> g_DummySensor("osgart_video_Ddmmysensor");
