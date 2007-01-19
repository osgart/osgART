/**
 * Plugin for capturing through FFMPEG libavcodec
 * Licensed under the terms of the LGPL 
 * (c) 2001-2006 Hartmut Seichter
 * (c) 2006 HIT Lab NZ
 */

#ifndef OSGART_FFMPEG_VIDEO
#define OSGART_FFMPEG_VIDEO

#include <iostream>
#include <string>

//graphics include
#include <osgART/Export>
#include <OpenThreads/Mutex>

//personnal include
#include <osgART/GenericVideo>
#include <osgART/VideoConfig>


class FFMPEGVideo: public osgART::GenericVideo
{
public:        
// Standard Services
    
    /** 
	* \brief default constructor.
	* The default constructor.
	*/
    FFMPEGVideo();
    
    /** 
    * \brief copy constructor.
    *
    */
    FFMPEGVideo(const FFMPEGVideo &);
    

    
    /** 
    * \brief affectation operator.
    *
    */
    FFMPEGVideo& operator=(const FFMPEGVideo &);
    
   
    /**
    * \brief open the FFMPEGVideo stream.
    * Access the FFMPEGVideo stream (hardware or file) and get an handle on it.
    */
	void open();
	
	/**
    * \brief close the FFMPEGVideo stream.
    * Terminate the connection with the FFMPEGVideo stream and clean handle.
    */
	void close();
	
	/**
    * \brief start the FFMPEGVideo stream grabbing.
    * Start to get image from the FFMPEGVideo stream. In function of the implementation on different
    * platform, this function can run a thread, signal or real-time function. 
    */
	void start();
	
	/**
    * \brief stop the FFMPEGVideo stream grabbing.
    * Stop to get image from the FFMPEGVideo stream. In function of the implementation on different
    * platform, this function can stop a thread, signal or real-time function. 
    */
	void stop();
	
	/**
    * \brief update the FFMPEGVideo stream grabbing.
    * Try to get an image of the FFMPEGVideo instance, usable by your application.
    */
	void update();
	
	inline virtual void releaseImage() {};
	
	
protected:

	virtual ~FFMPEGVideo();

	struct HandleType;
	
	HandleType *mHandle;

	osgART::VideoConfiguration mVideoConfig;



};

#endif