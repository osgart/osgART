#include "DSVLVideo"

#include <iostream>


#include "osgART/PluginManager"

// initializer for dynamic loading
osgART::PluginProxy<osgART::DSVL2Video> g_artoolkitvideo("video_dsvl2");


namespace osgART 
{
	DSVL2Video::DSVL2Video() : 
		Video()
	{
		CoInitialize(NULL);
	}

	DSVL2Video::~DSVL2Video(void)
	{	
		this->close();    
	}

	DSVL2Video& 
	DSVL2Video::operator=(const DSVL2Video &)
	{
		return *this;
	}

	VideoConfiguration* DSVL2Video::getVideoConfiguration() 
	{
		return &m_videoconfig;
	}

	void
	DSVL2Video::open()
	{	
		graphManager = new DSVL_VideoSource();
		
		if(FAILED(graphManager->BuildGraphFromXMLFile((char*)m_videoconfig.deviceconfig.c_str())))
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

		allocateImage(frame_width, frame_height, 1, GL_BGRA, GL_UNSIGNED_BYTE, 1);
		
	}


	void
	DSVL2Video::close()
	{
		this->stop();
		if (graphManager) {
			
			delete graphManager;
			graphManager = 0L;
		}

	}

	void
	DSVL2Video::start()
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
		
		graphManager->CheckoutMemoryBuffer(&(m_Handle), &newImage, NULL, NULL, NULL, &(m_Handle.t));
	}

	void
	DSVL2Video::stop()
	{
		graphManager->CheckinMemoryBuffer(m_Handle, true);

		if(FAILED(graphManager->Stop()))
		{
			std::cerr<<"OSGART->ERROR:can't stop the graph manager!!"<<std::endl;
			exit(-1);
		}
	}

	void
	DSVL2Video::update()
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

		if (newImage && this->valid()) 
			setImage(this->s(), this->t(), 1, GL_RGB, GL_BGRA, 
				GL_UNSIGNED_BYTE, newImage, osg::Image::NO_DELETE, 1);	

	}
}

