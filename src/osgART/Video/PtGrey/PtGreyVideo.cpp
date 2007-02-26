/*
 * osgART / AR Toolkit for OpenSceneGraph
 * (C) 2004-2006 HIT Lab NZ, University of Canterbury
 *
 * Licensing is governed by the LICENSE.txt which is 
 * part of this library distribution.
 *
 */
 
#include "PtGreyVideo"
#include <OpenThreads/Thread>

#include <process.h>

using namespace std;
using namespace osgART;

#include <osg/Notify>

class PtGreyVideoThread: public OpenThreads::Thread
{
public:
	PtGreyVideoThread(FlyCaptureContext,PtGreyVideo*);
	~PtGreyVideoThread();

protected:
    virtual void  run();

	PtGreyVideo* ptgrey;
	FlyCaptureContext context;
};

void PtGreyVideoThread::run()
{
   FlyCaptureImage flyimage;
   FlyCaptureImage convertedimage;
   FlyCaptureImage convertedimage2;
   FlyCaptureImage* imagePtr;
   FlyCaptureError   error;

   convertedimage.pixelFormat = FLYCAPTURE_BGRU;
   convertedimage.pData = new unsigned char[ ptgrey->getWidth()*ptgrey->getHeight()*ptgrey->pixelSize() ];
   memset(convertedimage.pData, 0x0,  ptgrey->getWidth()*ptgrey->getHeight()*ptgrey->pixelSize() );

   convertedimage2.pixelFormat = FLYCAPTURE_BGRU;
   convertedimage2.pData = new unsigned char[ ptgrey->getWidth()*ptgrey->getHeight()*ptgrey->pixelSize() ];
   memset(convertedimage2.pData, 0x0,  ptgrey->getWidth()*ptgrey->getHeight()*ptgrey->pixelSize() );

   imagePtr=&convertedimage;

	while (ptgrey->IsRunning())
	{
		if ((error =  flycaptureGrabImage2(context, &flyimage ))!= FLYCAPTURE_OK)
		{
			std::cerr<<"ERROR:"<<flycaptureErrorToString( error );
		}

        if (( error = flycaptureConvertImage(context,&flyimage,imagePtr))!= FLYCAPTURE_OK)
		{
			std::cerr<<"ERROR:"<<flycaptureErrorToString( error );
		}

		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(ptgrey->getMutex());
			*(ptgrey->getRefImage()) = imagePtr->pData;
			ptgrey->haveNewImage=true;

		}
		imagePtr = (imagePtr == &convertedimage) ? (&convertedimage2):(&convertedimage);
		
		/*

		
		ptgrey->getMutex().unlock();

		imagePtr=(imagePtr==&convertedimage)?(&convertedimage2):(&convertedimage);

		*/
	}
}

PtGreyVideoThread::PtGreyVideoThread(FlyCaptureContext _context,PtGreyVideo* _ptgrey)
{
	context=_context;
	ptgrey=_ptgrey;
}

PtGreyVideoThread::~PtGreyVideoThread() 
{
    cancel();

}


PtGreyVideo::PtGreyVideo() : 
	GenericVideo(),
	xsize(0),
	ysize(0),
	camIndex(0),
	newImage(0L),
	isRoi(false),
	isRunning(false)
{

	m_fields["ROI"] = new TypedField<bool>(&isRoi);
	m_fields["setROI"] = new TypedField<osg::Vec4s>(&m_roi);
}

PtGreyVideo::~PtGreyVideo()
{
}


VideoConfiguration* 
PtGreyVideo::getVideoConfiguration() 
{
	return &m_config;
}

void 
PtGreyVideo::configure()
{

	camIndex = m_config.id;

	framerate = this->getVideoConfiguration()->framerate;
	pixelsize = 4;

	xsize = m_config.width;
	ysize = m_config.height;

	if ((xsize==160)&&(ysize==120)||
		(xsize==320)&&(ysize==240)||
		(xsize==640)&&(ysize==480)||
		(xsize==800)&&(ysize==600)||
		(xsize==1024)&&(ysize==768)||
		(xsize==1280)&&(ysize==960)||
		(xsize==1600)&&(ysize==1200))
	{

		///Pixel Format type: use internally but also for the type of image returned
		switch (this->getVideoConfiguration()->framerate)
		{
			case VIDEOFRAMERATE_1_875:videoSpeed=FLYCAPTURE_FRAMERATE_1_875;break;
			case VIDEOFRAMERATE_3_75:videoSpeed=FLYCAPTURE_FRAMERATE_3_75;break;
			case VIDEOFRAMERATE_7_5:videoSpeed=FLYCAPTURE_FRAMERATE_7_5;break;
			case VIDEOFRAMERATE_15:videoSpeed=FLYCAPTURE_FRAMERATE_15;break;
			case VIDEOFRAMERATE_30:videoSpeed=FLYCAPTURE_FRAMERATE_30;break;
			case VIDEOFRAMERATE_50:videoSpeed=FLYCAPTURE_FRAMERATE_50;break;	
			case VIDEOFRAMERATE_60:videoSpeed=FLYCAPTURE_FRAMERATE_60;break;
			case VIDEOFRAMERATE_120:videoSpeed=FLYCAPTURE_FRAMERATE_120;break;
			case VIDEOFRAMERATE_240:videoSpeed=FLYCAPTURE_FRAMERATE_240;break;
			case VIDEOFRAMERATE_ANY:videoSpeed=FLYCAPTURE_FRAMERATE_ANY;break;
			default:
				std::cerr<<"OSGART->ERROR:frame speed not supported !!!"<<std::endl;
				exit(-1);
				break;
		}
		switch (this->getVideoConfiguration()->type)
		{
		case VIDEOFORMAT_RGB24:
			switch (xsize)
			{
			case 640:videoMode=FLYCAPTURE_VIDEOMODE_640x480RGB;break;
			case 800:videoMode=FLYCAPTURE_VIDEOMODE_800x600RGB;break;
			case 1024:videoMode=FLYCAPTURE_VIDEOMODE_1024x768RGB;break;
			case 1280:videoMode=FLYCAPTURE_VIDEOMODE_1280x960RGB;break;
			case 1600:videoMode=FLYCAPTURE_VIDEOMODE_1600x1200RGB;break;
			default:
				std::cerr<<"OSGART->ERROR:video mode not supported !!!"<<std::endl;
				exit(-1);
				break;
			}
			break;
		case VIDEOFORMAT_Y8:
			switch (xsize)
			{
			case 640:videoMode=FLYCAPTURE_VIDEOMODE_640x480Y8;break;
			case 800:videoMode=FLYCAPTURE_VIDEOMODE_800x600Y8;break;
			case 1024:videoMode=FLYCAPTURE_VIDEOMODE_1024x768Y8;break;
			case 1280:videoMode=FLYCAPTURE_VIDEOMODE_1280x960Y8;break;
			case 1600:videoMode=FLYCAPTURE_VIDEOMODE_1600x1200Y8;break;
			default:
				std::cerr<<"OSGART->ERROR:video mode not supported !!!"<<std::endl;
				exit(-1);
				break;
			}
			break;
		case VIDEOFORMAT_Y16:
			switch (xsize)
			{
			case 640:videoMode=FLYCAPTURE_VIDEOMODE_640x480Y16;break;
			case 800:videoMode=FLYCAPTURE_VIDEOMODE_800x600Y16;break;
			case 1024:videoMode=FLYCAPTURE_VIDEOMODE_1024x768Y16;break;
			case 1280:videoMode=FLYCAPTURE_VIDEOMODE_1280x960Y16;break;
			case 1600:videoMode=FLYCAPTURE_VIDEOMODE_1600x1200Y16;break;
			default:
				std::cerr<<"OSGART->ERROR:video mode not supported !!!"<<std::endl;
				exit(-1);
				break;
			}
			break;
		case VIDEOFORMAT_YUV444:
			if (xsize==160)
			videoMode=FLYCAPTURE_VIDEOMODE_160x120YUV444;
			else
			{
				std::cerr<<"OSGART->ERROR:video mode not supported !!!"<<std::endl;
				exit(-1);
			}
			break;
		case VIDEOFORMAT_YUV422:
			switch (xsize)
			{
				case 320:videoMode=FLYCAPTURE_VIDEOMODE_320x240YUV422;break;
				case 640:videoMode=FLYCAPTURE_VIDEOMODE_640x480YUV422;break;
				case 800:videoMode=FLYCAPTURE_VIDEOMODE_800x600YUV422;break;
				case 1024:videoMode=FLYCAPTURE_VIDEOMODE_1024x768YUV422;break;
				case 1280:videoMode=FLYCAPTURE_VIDEOMODE_1280x960YUV422;break;
				case 1600:videoMode=FLYCAPTURE_VIDEOMODE_1600x1200YUV422;break;
				default:
					std::cerr<<"OSGART->ERROR:video mode not supported !!!"<<std::endl;
					exit(-1);
					break;
			}
			break;
		case VIDEOFORMAT_YUV411:
			if (xsize==640)
				videoMode=FLYCAPTURE_VIDEOMODE_640x480YUV411;
			else
			{
				std::cerr << 
					"OSGART->ERROR:video mode not supported !!!" 
					<< std::endl;

				exit(-1);
			}
			break;
		default:
			std::cerr<<"OSGART->ERROR:video mode not supported !!!"<<std::endl;
			exit(-1);
			break;
		}
	}
	else
	{
		osg::notify(osg::FATAL) << "osgART::PtGreyVideo::configure() image dimensions: " <<
			this->xsize << "x" << this->ysize << " incorrect!" << std::endl;
		exit(-1);
	}
	isRunning=false;


	//we are using ART so we are converting the video to the readable ART format
	pixelformat=VIDEOFORMAT_BGRA32;

}

void
PtGreyVideo::open()
{

	this->configure();

	if (isRoi) 
	{
		// capture with Region of Interest
		xsize = m_roi[2] - m_roi[0];
		ysize = m_roi[3] - m_roi[1];
	}

    FlyCaptureError   error;
	if ((error = flycaptureCreateContext( &context ))!= FLYCAPTURE_OK )
	{
		std::cerr<<"ERROR:1"<<flycaptureErrorToString( error );
	}
	if ((error = flycaptureInitialize( context, camIndex ))!= FLYCAPTURE_OK )
	{
		std::cerr<<"ERROR:"<<flycaptureErrorToString( error );
	}

	// 
	osg::notify() << "Create Image " << xsize << "x" << ysize << std::endl;

	// create an image that same size (packing set to 1)
	this->allocateImage(xsize,ysize, 1,
			GL_BGRA,GL_UNSIGNED_BYTE, 1);
}

void
PtGreyVideo::close()
{
	FlyCaptureError   error;

	if ((error = flycaptureDestroyContext( context ))!= FLYCAPTURE_OK)
	{
		std::cerr<<"ERROR:"<<flycaptureErrorToString( error );
	}
}

void
PtGreyVideo::start()
{
	FlyCaptureError   error;

	if (isRoi) {

		if ((error = flycaptureStartCustomImage(context,0,
				m_roi[0],m_roi[1],m_roi[2],m_roi[3],
				100.0,FLYCAPTURE_RAW8 )) != FLYCAPTURE_OK)
		{
			std::cerr<< "PtGreyVideo::start() " << flycaptureErrorToString( error ) << std::endl;

			exit(-1);
		}
	}
	else
	{
		if ((error = flycaptureStart(context,videoMode,videoSpeed))!= FLYCAPTURE_OK)
		{
			std::cerr<< "PtGreyVideo::start() " << flycaptureErrorToString( error ) << std::endl;
			
			exit(-1);
		}
	}

	if (isRoi) {

	}

	isRunning=true;
	PtGreyVideoThread* thread=new PtGreyVideoThread(context,this);
	thread->start();
}

void
PtGreyVideo::stop()
{
	FlyCaptureError   error;
	
	//thread->stop();
	isRunning=false;
	//thread need be finished here..
	if ((error = flycaptureStop( context ))!= FLYCAPTURE_OK)
	{
		std::cerr<<"ERROR:"<<flycaptureErrorToString( error );
	}
}

void
PtGreyVideo::update()
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);
	
	if (haveNewImage) 
	{ 
		this->setImage(this->xsize, this->ysize, 1, GL_BGRA, GL_BGRA, 
			GL_UNSIGNED_BYTE, newImage, osg::Image::NO_DELETE, 1);

		this->m_isupdated = true;

		if (m_horizontal_flip || m_vertical_flip) 
		{
			if (m_horizontal_flip) this->flipHorizontal();
			if (m_vertical_flip) this->flipVertical();
		}


		haveNewImage = false;

	} else {

		m_isupdated = false;

	}
}

void
PtGreyVideo::getInfo()
{
	FlyCaptureInfoEx info;
	FlyCaptureError   error;

	if ((error = flycaptureGetCameraInfo( context, &info ))!= FLYCAPTURE_OK)
	{
		std::cerr<<"ERROR:cant't get info"<<flycaptureErrorToString( error );
		exit(-1);
	}
	std::cout<<"Serial number: "<<info.SerialNumber<<endl
			<<"Camera model: "<<info.pszModelName<<endl
			<<"Camera vendor: "<<info.pszVendorName<<endl
			<<"Sensor: "<<info.pszSensorInfo<<endl
			<<"DCAM compliance:  "<<(float)(info.iDCAMVer / 100.0)<<endl
			<<"Bus position: ("<<info.iBusNum<<","<<info.iNodeNum<<")"<<endl;
}
	
void 
PtGreyVideo::setROIParameters(int _xstart,int _ystart,int _xend,int _yend)
{
	xstart=_xstart;
	ystart=_ystart;
	xend=_xend;
	yend=_yend;
	xsize=xend-xstart;
	ysize=yend-ystart;
}



///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////
