///////////////////////////////////////////////////////////////////////////////
// File name : VidCaptureVideo.C
//
// Creation : YYY
//
// Version : YYY
//
// Author : Raphael Grasset
//
// email : Raphael.Grasset@imag.fr
//
// Purpose : ??
//
// Distribution :
//
// Use :
//	??
//
// Todo :
//	O add more video formats
// 
//	/
//	X
//
// History :
//	YYY : Mr Grasset : Creation of the file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include file
///////////////////////////////////////////////////////////////////////////////

#include "VidCaptureVideo"
#include <OpenThreads/Thread>

#include <process.h>

using namespace std;
using namespace osgART;

///////////////////////////////////////////////////////////////////////////////
// Macro 
///////////////////////////////////////////////////////////////////////////////

#if defined(NO_DEBUG)
#define ASSERT(x)
#else //defined(NO_DEBUG)
#define ASSERT(x) if(!(x)) \
    { cerr << "Assertion failed : (" << #x << ')' << endl \
    << "In file : " << __FILE__ << "at line #" << __LINE__ << endl \
    << "Compiled the " << __DATE__ << " at " << __TIME__ << endl; abort();}
#endif // else defined(NO_DEBUG)

const char* const Video_RCS_ID = "@(#)class Video definition.";

///////////////////////////////////////////////////////////////////////////////
// class Video
///////////////////////////////////////////////////////////////////////////////

/// capCallback is the main image capture callback function
/// for continuous capture runs.

bool capCallback (   CVRES                   status,
                     CVImage*                imagePtr,
                     void*                   userParam)
{
   static int xOffset  = 0;
   static int yOffset  = 0;

   VidCaptureVideo* video=(VidCaptureVideo*)userParam;
  
   // Only try to work with the image pointer if the
   // status is successful!
   if (CVSUCCESS(status))
   {      
	 //  std::cerr<<"get one image.."<<std::endl;
     // CVAssert(imagePtr != 0, "This shouldn't happen. Bad image pointer.");

	  CVImage* newImage;

	  //clean previously acquired image (double buffer)
	  if (video->lastCVImage2!=NULL)
		  CVImage::ReleaseImage(video->lastCVImage2);
	  imagePtr->CopyImage(imagePtr,newImage);
	  video->lastCVImage2=video->lastCVImage;
	  video->lastCVImage=newImage;

	  video->getMutex().lock();
	  *(video->getRefImage())=newImage->GetRawDataPtr();//newImage->GetRawDataPtr();
	  video->haveNewImage=true;
	  video->getMutex().unlock();
  
      //CVImage::ReleaseImage(subSubImg);
        
     //video->gFrameNum++;
   }
   else
   {
	   std::cerr<<"ERROR:Capture failure in callback! Did you disconnect camera?"<<std::endl;
   }
   return true;
}



///////////////////////////////////////////////////////////////////////////////
// Static variable
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PUBLIC: Standard services 
///////////////////////////////////////////////////////////////////////////////

VidCaptureVideo::VidCaptureVideo(int id_cam,int _xsize,int _ysize,PixelFormatType pft, FrameRateType frt):camIndex(id_cam)
{
	xsize=_xsize;
	ysize=_ysize;

	gFrameNum=0;
	gDevNum=0;

	switch (pft)
	{
	case VIDEOFORMAT_RGB24:
		pixelsize=3;
		pixelformat=VIDEOFORMAT_RGB24;
		framerate=frt;
		vidFormat=VIDCAP_FORMAT_RGB24;
		switch (frt)
		{
		case VIDEOFRAMERATE_15:vidFrameRate=15;break;
		case VIDEOFRAMERATE_30:vidFrameRate=30;break;
		default:std::cerr<<"ERROR:Frame Rate not supported.."<<std::endl; exit(-1);break;
		}
		break;
	default: std::cerr<<"ERROR:Format not supported.."<<std::endl; exit(-1);break;
	}
	isRunning=false;
	haveNewImage=false;
	lastCVImage2=NULL;
	lastCVImage=NULL;
}

/*
VidCaptureVideo::VidCaptureVideo(const VidCaptureVideo &)
{
    
}*/

VidCaptureVideo::~VidCaptureVideo(void)
{
    
}

VidCaptureVideo& 
VidCaptureVideo::operator=(const VidCaptureVideo &)
{
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void
VidCaptureVideo::open()
{
	int selectedMode=-1;

	vidCap = CVPlatform::GetPlatform()->AcquireVideoCapture();
	if (CVFAILED(vidCap->Init()))
	{
		std::cerr<<"ERROR:error initializing video capture object.\n"<<std::endl;
		CVPlatform::GetPlatform()->Release(vidCap);                  
		exit(-1);
	}
	// Enumerate devices.
	// The enumCallback is called for each available
	// video capture device.  Our current enumeration
	// callback just saves the first one. In practice,
	// you'd usually want to add these to a list and
	// let the user select one if the last selected
	// one was not available.
	//
	// Note that the video capture interface must
	// first be initialized before enumerating devices.
	memset(gDeviceName,0,256);

	int numDevices = 0;
	if (CVFAILED(vidCap->GetNumDevices(numDevices)))
	{
		std::cerr<<"ERROR:Failed enumerating devices."<<std::endl;
		vidCap->Uninit();
		CVPlatform::GetPlatform()->Release(vidCap);      
		exit(-1);
	}
	if (camIndex>numDevices)
	{
		std::cerr<<"ERROR:Can't access device"<<camIndex<<std::endl;
		exit(-1);
	}

	CVVidCapture::VIDCAP_DEVICE devInfo;
	
	int curDevIndex = 0;
	for (curDevIndex = 0; curDevIndex < numDevices; curDevIndex++)
	{
		if (CVSUCCESS(vidCap->GetDeviceInfo(curDevIndex,devInfo)))
		{
			//std::cout<<"DEBUG:Device "<<curDevIndex<<": "<<devInfo.DeviceString<<std::endl;
		}
	}
  // Now connect to the selected device.
   if (CVSUCCESS(vidCap->Connect(camIndex)))
   {
      int devNameLen = 0;
      vidCap->GetDeviceName(0,devNameLen);
      devNameLen++;
      char *devName = new char[devNameLen];
      vidCap->GetDeviceName(devName,devNameLen);
      
	  //std::cout<<"DEBUG:Connection succeeded to "<<devName<<std::endl;
	  delete [] devName;
   }
   else
   {
	  std::cerr<<"ERROR:Connection failed."<<std::endl;
      vidCap->Uninit();
      CVPlatform::GetPlatform()->Release(vidCap);      
      exit(-1);
   }

   // Get the number of supported modes.
   // Mode changes can be done any time we're not
   // grabbing.
   //
   CVVidCapture::VIDCAP_MODE modeInfo;
   int numModes = 0;
   vidCap->GetNumSupportedModes(numModes);

   // Dump each mode 
   for (int curmode = 0; curmode < numModes; curmode++)
   {
      if (CVSUCCESS(vidCap->GetModeInfo(curmode, modeInfo)))
      {
		  std::cout<<"DEBUG:Available mode:"<<curmode<<","<<modeInfo.XRes<<"x"<<modeInfo.YRes<<" @ "<<modeInfo.EstFrameRate<<" frames/sec ("<<vidCap->GetFormatModeName(modeInfo.InputFormat)<<")"<<std::endl;
		  if ((xsize==modeInfo.XRes)&&(ysize==modeInfo.YRes)&&(vidFrameRate=modeInfo.EstFrameRate)&&(vidFormat==modeInfo.InputFormat))
			selectedMode=curmode;
      } 
   }
	
   if (selectedMode==-1)
   { 
	   std::cerr<<"ERROR:Can't access the selected mode.."<<std::endl;
	   exit(-1);
   }

   if (CVFAILED(vidCap->SetMode(selectedMode)))    // Set to first available mode
   //if (CVFAILED(vidCap->SetMode(2)))      // Set to the third mode
   {
	  std::cerr<<"ERROR:Error setting video mode"<<std::endl;
   }

   // Get the mode info of our selected mode and print it.
   if (CVFAILED(vidCap->GetCurrentMode(modeInfo)))
   {
	   std::cerr<<"ERROR:Error activating mode!"<<std::endl;
   }
   else
   {
	   std::cout<<"DEBUG:Activated mode: "<<selectedMode<<" "<<modeInfo.XRes<<"x"<<modeInfo.YRes<<" @ "<<modeInfo.EstFrameRate<<" frames/sec"<<std::endl;   
   }
/*
   // Set contrast to 50% if supported. Only some video capture devices
   // will support the properties.
   long minval,maxval;
   if (CVSUCCESS(vidCap->GetPropertyInfo( CVVidCapture::CAMERAPROP_CONTRAST,
                                          0,
                                          0,
                                          &minval, 
                                          &maxval)))
   {
      printf("Contrast supported\n", minval, maxval);    
      vidCap->SetProperty( CVVidCapture::CAMERAPROP_CONTRAST,
                           (minval+maxval)/2);
   }
   else
   {
      printf("Contrast unsupported.\n");
   }

   // Now set the brightness if available.
   if (CVSUCCESS(vidCap->GetPropertyInfo( CVVidCapture::CAMERAPROP_BRIGHT,
                                          0,
                                          0,
                                          &minval, 
                                          &maxval)))
   {
      printf("Brightness supported\n", minval, maxval);     
      vidCap->SetProperty( CVVidCapture::CAMERAPROP_BRIGHT,
                           (minval+maxval)/2);
   }
   else
   {
      printf("Brightness unsupported.\n");
   }
*/
}

void
VidCaptureVideo::close()
{
	vidCap->Uninit();
   // Release video capture object.
   CVPlatform::GetPlatform()->Release(vidCap);   
}

void
VidCaptureVideo::start()
{
   bool started = CVSUCCESS(vidCap->StartImageCap(CVImage::CVIMAGE_RGB24, capCallback,(void *)this));
   if (!started)
   {
	   std::cerr<<"ERROR:Error starting capture...."<<std::endl;
	   exit(-1);
   }
   isRunning=true;
}

void
VidCaptureVideo::stop()
{
	// Always disconnect from the device when you're done
   vidCap->Disconnect();   
}

void
VidCaptureVideo::update()
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);

	if (!haveNewImage)
	{
		image=NULL;
	}
	else
	{
		image=newImage;
	}
	haveNewImage=false;
}



///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////
