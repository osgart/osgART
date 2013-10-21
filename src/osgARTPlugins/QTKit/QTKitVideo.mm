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

#import <QTKit/QTKit.h>

#include <iostream>

@interface VideoQTKit : NSObject 
{

//capture controller: supporting input source / output destination
QTCaptureSession            *mCaptureSession;

//capture source: device (isght, dv camera)
QTCaptureDevice				*mCaptureVideoDevice;
//connections
//inputDevices, open:, isOpen, close, localizedDisplayName
//deviceAttributes and attributeForKey: methods
//key-value coding to get and set attributes. If you wish to observe changes for a given attribute, you can add a key-value observer where the key path is the attribute key

//file /streaming input

//capture input: device -> session
QTCaptureDeviceInput        *mCaptureVideoDeviceInput;
//initWithDevice:; returns an instance of QTCaptureDeviceInput associated with the given device.


//capture output: session -> video buffer
QTCaptureDecompressedVideoOutput  *mCaptureDecompressedVideoOutput;
//setDelegate:, captureOutput:didOutputVideoFrame:withSampleBuffer:fromConnection:

//buffer
CVImageBufferRef	mCurrentImageBuffer;
//QTSampleBuffer *mSampleBuffer;

//capture output: session -> debug view
QTCaptureVideoPreviewOutput* mCaptureVideoPreviewOutput;
//visualContextForConnection:, setDelegate:, captureOutput:didOutputVideoFrame:withSampleBuffer:fromConnection:

//capture display: view
QTCaptureView* mCaptureVideoPreview;
//setCaptureSession: 

//capture output: session -> movie file
QTCaptureMovieFileOutput    *mCaptureMovieFileOutput;
//recordToOutputFileURL:, setDelegate:, captureOutput:didFinishRecordingToOutputFileAtURL:forConnections:dueToError:

QTCompressionOptions* *mCaptureCompressionOptions;
//compressionOptionsIdentifiersForMediaType:, mediaType

QTFormatDescription *mFormatDescription;
//localizedFormatSummary. The constant, QTFormatDescriptionVideoCleanApertureDisplaySizeAttribute

//formatDescription

//QTCaptureLayer 

QTSampleBuffer *mSampleBuffer;

int _textureWidth;
int _textureHeight;
}

- (int)initVideo;
- (unsigned char*)getVideoImage;
- (void)captureOutput:(QTCaptureOutput *)captureOutput didOutputVideoFrame:
	(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:
	(QTCaptureConnection *)connection;
- (void)captureOutput:(QTCaptureOutput *)captureOutput didDropVideoFrameWithSampleBuffer:
	(QTSampleBuffer *)sampleBuffer fromConnection:
	(QTCaptureConnection *)connection;
-(void)setSize: (int)height width:(int)width;
@end

@implementation VideoQTKit
- (int)initVideo {

	BOOL success = NO;

    NSError *error;
      
	//create a session
	mCaptureSession = [[QTCaptureSession alloc] init];
	
	
	NSArray* devices = [[[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo]
            arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]] retain];

    if ([devices count] == 0) {
        std::cout << "QTKit didn't find any attached Video Input Devices!" << std::endl;
      //  [localpool drain];
        return 0;
    }
    else
    {
    	std::cout<<"device num="<<[devices count]<<std::endl;
    }

	//use a capture device
	
	//default
 	mCaptureVideoDevice = [QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeVideo];
	
	//uuid device
 	//mCaptureVideoDevice = [QTCaptureDevice deviceWithUniqueID:uuid];
	
	// 	mCaptureVideoDevice = [QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeVideo];

	//get list of devices
	//NSArray devicesList;
 	//mCaptureVideoDevices = [QTCaptureDevice inputDevices];

// 	mCaptureVideoDevices = [QTCaptureDevice inputDevices];

// 	mCaptureVideoDevices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];


	//mCaptureVideoDevice = [QTCaptureDevice inputDeviceWithMediaType:QTMediaTypeVideo];

	if (!mCaptureVideoDevice)
	{
		std::cout<<"no devices.."<<std::endl;
		[[NSAlert alertWithError:error] runModal];

		return 0;	
	}
	
	
	success = [mCaptureVideoDevice open:&error];

	if (!success) {

		[[NSAlert alertWithError:error] runModal];

		return 0;
	}
    
    std::cout<<"device name="<<[[mCaptureVideoDevice localizedDisplayName] UTF8String]<<std::endl;
    
    //1280x720
    mCaptureVideoDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:mCaptureVideoDevice];

	NSDictionary* attributes=[mCaptureVideoDevice deviceAttributes];
	
	//NSLog(@"The code runs through here!");
	
	//for (NSDictionary *attrib in attributes ){
    // NSString* description = [alert  objectForKey:@"description"];
    //etc...
    NSString *key;
    for (key in attributes) {

  //   NSLog(@"Key: %@, Value %@", key, [attributes objectForKey: key]);
	std::string* keyString=new std::string([key UTF8String]);

	//std::cout<<"key="<<keyString<<" "<<[[attributes objectForKey: key] UTF8String]<<std::endl;
     //NSLog(@"Key: %@, Value %@", key, [attributes objectForKey: key]);

	}
	
	/*
		for(QTCaptureDevice* captureDevice in sortedCaptureArray) { if([captureDevice hasMediaType:QTMediaTypeVideo] || [captureDevice
hasMediaType:QTMediaTypeMuxed]) { NSLog(@"Model Unique ID --- %@", [captureDevice modelUniqueID]); NSLog(@"Unique ID:--------- %@", [captureDevice uniqueID]); NSLog(@"Device Attributes - %@", [captureDevice deviceAttributes]); for(QTFormatDescription* fmtDesc in [captureDevice
formatDescriptions]) NSLog(@"Format Description - %@", [fmtDesc
formatDescriptionAttributes]);
*/

/*
for(QTFormatDescription* format in formats )
{
    // print format description from:  [format localizedFormatSummary]

    // now take a look at the available attributes
    NSDictionary* formatAttribs = [format formatDescriptionAttributes];
    for(NSString* key in formatAttribs)
    {
        // print key
    }*/
    
    //UVC compliant camera, 
    
	//outputImageProviderFromTextureWithPixelFormat:pixelsWide:pixelsHigh:name:flipped:releaseCallback:releaseContext:colorSpace:shouldColorMatch
	NSArray* formats=[mCaptureVideoDevice formatDescriptions];

 	NSEnumerator *enumerator = [formats objectEnumerator];
 
 	QTFormatDescription *desc; 
 
 	while ((desc = [enumerator nextObject])) 
 	{ 
 		if ([desc mediaType] == QTMediaTypeVideo)
 		{ 
 			UInt32 formatType = [desc formatType]; // Report video size and compression type. 
 			printf("video format='"); 
 			if (formatType > 0x28) 
 					printf("%c%c%c%c", (char)((formatType >> 24)& 0xFF), (char)((formatType >> 16) & 0xFF), (char)((formatType >> 8) & 0xFF), (char)((formatType >> 0) & 0xFF)); 
 				else 
 					printf("%d", formatType); 
 			NSSize size; 
 			
 			[[desc attributeForKey:QTFormatDescriptionVideoEncodedPixelsSizeAttribute] getValue:&size]; 
 			
			printf("', size=%fx%f\n", size.width, size.height); printf(".\n"); 
// 			[[desc attributeForKey:QTFormatDescriptionVideoEncodedPixelsSizeAttribute] getValue:&size]; 
 			
//			printf("', size is %fx%f.\n", size.width, size.height); printf(".\n"); 
// 			[[desc attributeForKey:QTFormatDescriptionVideoEncodedPixelsSizeAttribute] getValue:&size]; 
 			
//			printf("', size is %fx%f.\n", size.width, size.height); printf(".\n"); 
		} 
		else
		{
//			std::cout<<"whateve man.."<<std::endl;
		}
	}

	for (QTFormatDescription *element in formats) {

    	// NSLog(@"element: %@", element);
	//std::string* keyString=new std::string([element UTF8String]);

//	std::cout<<"key="<<keyString<<std::endl;

	}

    success = [mCaptureSession addInput:mCaptureVideoDeviceInput error:&error];
    
    if (!success) {

		[[NSAlert alertWithError:error] runModal];

		return 0;
	}    

    mCaptureDecompressedVideoOutput = [[QTCaptureDecompressedVideoOutput alloc] init];

//the QTCaptureDecompressedVideoOutput object will discard extra frames that are queued up while you are blocking the delegate callback thread.
	[mCaptureDecompressedVideoOutput setAutomaticallyDropsLateVideoFrames:YES];
	
	[mCaptureDecompressedVideoOutput setDelegate:self];

	//0 unlimited frame rate (0 default value)
	//[mCaptureDecompressedVideoOutput setMinimumVideoFrameInterval:0];
	
	//in seconds
	[mCaptureDecompressedVideoOutput setMinimumVideoFrameInterval:0.01];

	//std::cout<<_textureWidth<<_textureHeight<<std::endl;
    [mCaptureDecompressedVideoOutput setPixelBufferAttributes:[NSDictionary dictionaryWithObjectsAndKeys:
                                                       [NSNumber numberWithDouble:_textureWidth], (id)kCVPixelBufferWidthKey,
                                                       [NSNumber numberWithDouble:_textureHeight], (id)kCVPixelBufferHeightKey,
                                                       [NSNumber numberWithUnsignedInt:kCVPixelFormatType_24RGB], (id)kCVPixelBufferPixelFormatTypeKey,
														nil]];
				
	//outputVideoFrame:withSampleBuffer:fromConnection:
	//or the captureOutput:didOutputVideoFrame:withSampleBuffer:fromConnection:
												
	success = [mCaptureSession addOutput:mCaptureDecompressedVideoOutput error:&error];

	if (!success) {

	   [[NSAlert alertWithError:error] runModal];

	   return 0;

	}
	
//	 [mCaptureView setCaptureSession:mCaptureSession];

	[mCaptureSession startRunning];        
	
	return 1;
}

- (void)captureOutput:(QTCaptureOutput *)captureOutput didOutputVideoFrame:
	(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:
	(QTCaptureConnection *)connection
{
    CVImageBufferRef imageBufferToRelease;
    
    CVBufferRetain(videoFrame);

	//std::cout<<"get new image.."<<std::endl;
    @synchronized (self) {

        imageBufferToRelease = mCurrentImageBuffer;
        mCurrentImageBuffer = videoFrame;

    }

    CVBufferRelease(imageBufferToRelease);
}	

 -(unsigned char*)getVideoImage
 {
 	//CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer); 
	
	CVPixelBufferRef pixels;
	 
	@synchronized (self){
        pixels = CVBufferRetain(mCurrentImageBuffer);
      //  newFrame = 0;
      
      if (pixels)
      {
      
      
      }
      
    }
    
	/*Lock the image buffer*/
	//   @synchronized (self) {
	CVPixelBufferLockBaseAddress(pixels,0); 
	
	/*Get information about the image*/
	uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(pixels); 
	size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixels); 
	size_t width = CVPixelBufferGetWidth(pixels); 
	size_t height = CVPixelBufferGetHeight(pixels); 


	if (baseAddress) 
	{
//		#ifdef 	
	}
	//std::cout<<"w="<<width<<"h="<<height<<"b="<<bytesPerRow<<std::endl;//<<"a="<<baseAddress<<std::endl;
	/*We unlock the  image buffer*/
	CVPixelBufferUnlockBaseAddress(pixels,0);

	CVBufferRelease(pixels);

 	return (unsigned char*)baseAddress;
	
	//}
	
//return 0;
 }
 
 -(void)captureOutput:(QTCaptureOutput *)captureOutput didDropVideoFrameWithSampleBuffer:
	(QTSampleBuffer *)sampleBuffer fromConnection:
	(QTCaptureConnection *)connection
	{
		std::cout<<"dropped frame.."<<std::endl;
	
	}
 -(void)setSize: (int)height width:(int)width;
 {
 	_textureHeight=height;
 	_textureWidth=width;
 
 }
@end

class QTKitVideo : public osgART::Video
{
public:


// Standard Services

	/**
		* Default constructor. It creates a video source from a configuration string
		* as it is been used in the original AR Toolkit 2.71
		* \param videoName a string definition of the video background. See documentation
		* of OpenCV for further details.
		*/
	QTKitVideo();

	/**
		* Copy constructor.
		*
		*/
	QTKitVideo(const QTKitVideo &,
		const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

	/**
		* Destructor.
		*
		*/
	virtual ~QTKitVideo();


	META_Object(osgART,QTKitVideo)

	/**
	* Affectation operator.
	*
	*/
	QTKitVideo& operator = (const QTKitVideo &);

	/**
		* Open the video stream. Access the video stream (hardware or file) and get an handle on it.
		*/
	bool init();

	/**
		* Close the video stream. Terminates the connection with the video stream and clean handle.
		*/
	bool close(bool waitForThread = true);

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

	virtual osgART::VideoConfiguration* getVideoConfiguration();


private:

	int m_camIndex;

	VideoQTKit* capture;

	osgART::VideoConfiguration m_config;

	GLint _internalformat_GL;
	GLenum _format_GL;
	GLenum _datatype_GL;

};


QTKitVideo::QTKitVideo() : osgART::Video()
{

}

QTKitVideo::QTKitVideo(const QTKitVideo &,
		const osg::CopyOp& copyop/* = osg::CopyOp::SHALLOW_COPY*/)
{
}

QTKitVideo::~QTKitVideo()
{
	//this->close(false);
}

QTKitVideo&
QTKitVideo::operator=(const QTKitVideo &)
{
	return *this;
}

bool
QTKitVideo::init()
{
	int xsize=0, ysize=0;
	m_camIndex=0;

	//if (m_video.open(m_camIndex))
	{
		osg::notify() << std::dec<< "QTKitVideo::open() succesful.."<<std::endl;
	}

/*
	if (m_config.deviceconfig != "") {
		config = (char*)&m_config.deviceconfig.c_str()[0];
	}

		// report the actual
		osg::notify() << std::dec << "QTKitVideo::open() size of video " <<
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
	//m_video.set(CV_CAP_PROP_FRAME_WIDTH,1280);
	//m_video.set(CV_CAP_PROP_FRAME_HEIGHT,720);

	//xsize=m_video.get(CV_CAP_PROP_FRAME_WIDTH);
	//ysize=m_video.get(CV_CAP_PROP_FRAME_HEIGHT);
	//m_video.set(CV_CAP_PROP_FPS,30);
	//m_video.get(CV_CAP_PROP_FPS,30);
	
//	m_video.set(CV_CAP_PROP_CONVERT_RGB,1.0);
//	m_video.set(CV_CAP_PROP_FOURCC,0x32424752);//RGB
	//_format_GL=GL_RGB;
//	_format_GL=GL_RGB;
	
	//std::cout << "QTKitVideo::open() size of video " <<
	//		xsize << " x " << ysize << "format="<< m_video.get(CV_CAP_PROP_FOURCC)<<std::endl;
	
	//_datatype_GL=GL_UNSIGNED_BYTE;

	//m_config.selectedWidth = xsize;
	//m_config.selectedHeight = ysize;
	//m_config.selectedFrameRate = 30;

	if (m_config.width==-1)
	{
		m_config.width=1280;
		m_config.height=720;
	}
	
	_videoStreamList[0]->allocateImage(m_config.width, m_config.height, 1, GL_RGB, GL_UNSIGNED_BYTE, 1);

	_videoStreamList[0]->setDataVariance(osg::Object::DYNAMIC);

	return true;

}

bool
QTKitVideo::close(bool waitForThread)
{
	//m_video.release();
	
	return true;
}

bool
QTKitVideo::start()
{
	//	VideoQTKit* video new VideoQTKit;
	capture = [[VideoQTKit alloc] init];
	[capture setSize:m_config.height width:m_config.width];

	[capture initVideo];

	_videoStreamList[0]->play();	
	
	return true;
}

bool
QTKitVideo::stop()
{
	_videoStreamList[0]->pause();
	
	return true;
}

/*
void
QTKitVideo::update()
{


}*/

bool
QTKitVideo::update(osg::NodeVisitor* nv)
{
	osg::Timer t;


	//if (m_video.isOpened())
	{
		//if (m_video.grab())
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());

			// cv::Mat frame;
		
			unsigned char* buffer=[capture getVideoImage];
			//m_video.retrieve(frame);

					// only clone if it is not continous (most of the time)
			  //      _image = (frame.isContinuous()) ? frame : frame.clone();

					// check for nv (NodeVisitor) - otherwise being called from a constructor!
				   // if (nv && _subscriber.valid()) _subscriber->updateWithImage(_image);

					// set format - OpenCV always BGR
					//setPixelFormat(GL_BGR);

			// copy data
			if (buffer)
				memcpy(_videoStreamList[0]->data(),buffer,_videoStreamList[0]->getImageSizeInBytes());

					// need to use dirty() as setModifiedCount(int) does not update
					// the backend buffer object

			//Mat bgra_to_rgb;
			
			//cvtColor(frame, bgra_to_rgb, CV_BGRA2RGB);

			//memcpy(data(),(unsigned char*)bgra_to_rgb.data,getImageSizeInBytes());
				
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

osgART::VideoConfiguration*
QTKitVideo::getVideoConfiguration()
{
	return &m_config;
}


void QTKitVideo::releaseImage()
{
}


// initializer for dynamic loading
osgART::PluginProxy<QTKitVideo> g_QTKitVideo("osgart_video_qtkit");


