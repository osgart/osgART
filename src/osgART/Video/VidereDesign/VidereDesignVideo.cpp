///////////////////////////////////////////////////////////////////////////////
// File name : VidereDesignVideo.C
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
//	O add support for bayer and mono format
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

#include <osgART/VidereDesignVideo>
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


///////////////////////////////////////////////////////////////////////////////
// Static variable
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PUBLIC: Standard services 
///////////////////////////////////////////////////////////////////////////////

VidereDesignVideo::VidereDesignVideo(int _id,PixelFormatType pf,int _xsize,int _ysize,FrameRateType fr):g_Id(_id)
{
	g_Camera=NULL;
	g_System=NULL;
	timeOut=50;
	xsize=_xsize;
	ysize=_ysize;
	pixelformat=pf;
	framerate=fr;

	switch (pixelformat)
	{
	case VIDEOFORMAT_RGB24:
		g_Type=S_RGB24;
		break;
	case VIDEOFORMAT_YUV422:
		g_Type=S_RGB24;
		break;
	case VIDEOFORMAT_YUV411:
		g_Type=S_RGB24;
		break;
	default:
		std::cerr<<"ERROR:Video format not supported.."<<std::endl;
		exit(-1);
	}
	switch (framerate)
	{
	case VIDEOFRAMERATE_3_75:
		g_Speed=S_3;
		break;
	case VIDEOFRAMERATE_5:
		g_Speed=S_5;
		break;
	case VIDEOFRAMERATE_7_5:
		g_Speed=S_7;
		break;
	case VIDEOFRAMERATE_10:
		g_Speed=S_10;
		break;
	case VIDEOFRAMERATE_15:
		g_Speed=S_15;
		break;
	case VIDEOFRAMERATE_30:
		g_Speed=S_30;
		break;
	case VIDEOFRAMERATE_60:
		g_Speed=S_60;
		break;
	default:
		std::cerr<<"ERROR:Frame rate format not supported.."<<std::endl;
		exit(-1);
	}
	if ((xsize==160)&&(ysize==120))
	{
		g_Size=S_160x120;
	}
	else 	
		if ((xsize==320)&&(ysize==240))
		{
			g_Size=S_320x240;
		}	
		else 
			if ((xsize==640)&&(ysize==480))
			{
				g_Size=S_640x480;
			}
			else
				if ((xsize==512)&&(ysize==384))
				{
					g_Size=S_512x384;
				}
				else if ((xsize==1024)&&(ysize==768))
				{
					g_Size=S_1024x768;
				}	else 
					if ((xsize==1280)&&(ysize==960))
					{
						g_Size=S_1280x960;
					}
					else 
					{
						std::cerr<<"ERROR: size not supported !!"<<std::endl;
					}


	
}

/*
VidereDesignVideo::VidereDesignVideo(const VidereDesignVideo &)
{
    
}*/

VidereDesignVideo::~VidereDesignVideo(void)
{
    
}

VidereDesignVideo& 
VidereDesignVideo::operator=(const VidereDesignVideo &)
{
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void 
VidereDesignVideo::SelfDisplay(ostream& stream) const
{
   stream << "< class Video >" << endl;
}

bool 
VidereDesignVideo::OK(void) const
{
    return true;
}

void
VidereDesignVideo::open()
{
	if ((g_System=dSysInit())==NULL)
	{
		std::cerr<<"ERROR:no camera found on 1394 Bus.."<<std::endl;
		exit(-1);
	}
	if (g_System->NumCameras()<g_Id)
	{
		std::cerr<<"ERROR:There is no"<<g_Id<<"camera on the bus"<<std::endl;
		exit(-1);
	}
	if ((g_Camera=g_System->InitCamera(g_Id))==NULL)
	{
		std::cerr<<"ERROR:can't initialize"<<g_Id<<"camera on the bus"<<std::endl;
		exit(-1);
	}

	if (!g_Camera->SetFormat(g_Size,g_Type,g_Speed))
	{
		std::cerr<<"ERROR:can't set combination of format (size, speed, image format) to the camera"<<g_Id<<std::endl;
		exit(-1);
	}
}

void
VidereDesignVideo::close()
{

}

void
VidereDesignVideo::start()
{
	if (!g_Camera->Start())
	{
		std::cerr<<"ERROR:Can't start the camera"<<g_Id<<":"<<std::endl;
		exit(-1);
	}
}

void
VidereDesignVideo::stop()
{
	// Always disconnect from the device when you're done
	if (!g_Camera->Stop())
	{
		std::cerr<<"ERROR:Can't stop the camera"<<g_Id<<std::endl;
		exit(-1);
	}
}

void
VidereDesignVideo::update()
{
	unsigned char** newImage = NULL;

	if (g_Camera) {
			bool have_new=g_Camera->GetImage(newImage);
		mutex.lock();
		if (!have_new) {
			image = NULL;
		} else {
			image = (unsigned char*)newImage;
		}
		mutex.unlock();
	}
}



///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////
