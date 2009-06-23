///////////////////////////////////////////////////////////////////////////////
// File name : DSVLVideo.C
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
//	O ??
//	/
//	X
//
// History :
//	YYY : Mr Grasset : Creation of the file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include file
///////////////////////////////////////////////////////////////////////////////

#include "DSVLVideo"

#include <iostream>


#include <osgART/PluginManager>

// initializer for dynamic loading
osgART::PluginProxy<DSVLVideo> g_dsvlvideo("video_dsvl");




using namespace osgART;


DSVLVideo::DSVLVideo() : Video()
{
}

DSVLVideo::~DSVLVideo(void)
{
    
}

DSVLVideo& 
DSVLVideo::operator=(const DSVLVideo &)
{
    return *this;
}


void
DSVLVideo::open()
{
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

	graphManager->GetCurrentMediaFormat(&frame_width, &frame_height,NULL,NULL);


	// create an image that same size (packing set to 1)
	this->allocateImage(frame_width, frame_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, 1);


}


void
DSVLVideo::close()
{
	this->stop();

	delete graphManager;
}
void
DSVLVideo::start()
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
}

void
DSVLVideo::stop()
{
	graphManager->CheckinMemoryBuffer(m_Handle, true);

	if(FAILED(graphManager->Stop()))
	{
		std::cerr<<"OSGART->ERROR:can't stop the graph manager!!"<<std::endl;
		exit(-1);
	}
}

void
DSVLVideo::update()
{
	if (NULL == this->graphManager) return;

	unsigned char *newImage = 0L;
	unsigned int wait_result;

	graphManager->CheckinMemoryBuffer(m_Handle);	
	// 
	wait_result = graphManager->WaitForNextSample();
	
	if(wait_result == WAIT_OBJECT_0) 
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);

		if (FAILED(graphManager->CheckoutMemoryBuffer(&(m_Handle), &newImage, 
				NULL, NULL, NULL, &(m_Handle.t))))	
		{
			newImage = NULL;
		}
	}
	
	if (newImage) 
		this->setImage(this->s(), this->t(), 1, GL_BGRA, GL_BGRA, 
			GL_UNSIGNED_BYTE, newImage, osg::Image::NO_DELETE, 1);	
}



///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////
