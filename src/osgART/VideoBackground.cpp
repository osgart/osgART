#include "osgART/VideoBackground"

namespace osgART {
#ifndef __APPLE__		
	// Implementation
	VideoBackground::VideoBackground(int videoId) 
		: VideoLayer(videoId, 1)
	{
	}

	VideoBackground::~VideoBackground() 
	{
	}

	void VideoBackground::init() {
		VideoLayer::init();
	}
#endif
};