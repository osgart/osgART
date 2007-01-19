#include "Video.h"

extern "C"
{
#include <avcodec.h>
#include <avformat.h>
};


using namespace osgART;

#include <string>


struct FFMPEGVideo::HandleType {
	// AVCodec			*codec;
    // AVCodecContext	*context;

	AVInputFormat *iformat;
	AVFormatContext *ic;
	AVFormatParameters params;

};

struct CodecMapping
{
	char* name;
	int   id;
};

#define CodecEntry(name) {#name,name}

static CodecMapping sCodecMap[] = 
{
    CodecEntry(CODEC_ID_NONE), 
    CodecEntry(CODEC_ID_MPEG1VIDEO),
    CodecEntry(CODEC_ID_MPEG2VIDEO),
    CodecEntry(CODEC_ID_MPEG2VIDEO_XVMC),
    CodecEntry(CODEC_ID_H263),
    CodecEntry(CODEC_ID_RV10),
    CodecEntry(CODEC_ID_MP2),
    CodecEntry(CODEC_ID_MP3),
    CodecEntry(CODEC_ID_VORBIS),
    CodecEntry(CODEC_ID_AC3),
    CodecEntry(CODEC_ID_MJPEG),
    CodecEntry(CODEC_ID_MJPEGB),
    CodecEntry(CODEC_ID_LJPEG),
    CodecEntry(CODEC_ID_MPEG4),
    CodecEntry(CODEC_ID_RAWVIDEO),
    CodecEntry(CODEC_ID_MSMPEG4V1),
    CodecEntry(CODEC_ID_MSMPEG4V2),
    CodecEntry(CODEC_ID_MSMPEG4V3),
    CodecEntry(CODEC_ID_WMV1),
    CodecEntry(CODEC_ID_WMV2),
    CodecEntry(CODEC_ID_H263P),
    CodecEntry(CODEC_ID_H263I),
    CodecEntry(CODEC_ID_FLV1),
    CodecEntry(CODEC_ID_SVQ1),
    CodecEntry(CODEC_ID_SVQ3),
    CodecEntry(CODEC_ID_DVVIDEO),
    CodecEntry(CODEC_ID_DVAUDIO),
    CodecEntry(CODEC_ID_WMAV1),
    CodecEntry(CODEC_ID_WMAV2),
    CodecEntry(CODEC_ID_MACE3),
    CodecEntry(CODEC_ID_MACE6),
    CodecEntry(CODEC_ID_HUFFYUV),
    CodecEntry(CODEC_ID_CYUV),
    CodecEntry(CODEC_ID_H264),
    CodecEntry(CODEC_ID_INDEO3),
    CodecEntry(CODEC_ID_VP3),
    CodecEntry(CODEC_ID_AAC),
    CodecEntry(CODEC_ID_MPEG4AAC),
    CodecEntry(CODEC_ID_ASV1),
    CodecEntry(CODEC_ID_ASV2),
    CodecEntry(CODEC_ID_FFV1),
    CodecEntry(CODEC_ID_4XM),
    CodecEntry(CODEC_ID_VCR1),
    CodecEntry(CODEC_ID_CLJR),
    CodecEntry(CODEC_ID_MDEC),
    CodecEntry(CODEC_ID_ROQ),
    CodecEntry(CODEC_ID_INTERPLAY_VIDEO),
    CodecEntry(CODEC_ID_XAN_WC3),
    CodecEntry(CODEC_ID_XAN_WC4),
	{0,0}
};
#undef CodecEntry

FFMPEGVideo::FFMPEGVideo() : GenericVideo(), mHandle(new HandleType)
{
	// register all video plugins
	av_register_all();
}

FFMPEGVideo::FFMPEGVideo(const FFMPEGVideo &)
{
   
}

FFMPEGVideo::~FFMPEGVideo(void)
{
	this->stop();
	this->close();
}

FFMPEGVideo& 
FFMPEGVideo::operator=(const FFMPEGVideo &)
{
    return *this;
}

void
FFMPEGVideo::open()
{
	
	std::string _url = 
		// "rtsp://hitlvideo1";
		// "http://localhost";
		"file:///hitlab.avi";

	// initialize the video handle
	memset(&mHandle->params, 0, sizeof(*&mHandle->params));

	// try to open the video
    if (int err = av_open_input_file(&mHandle->ic, 
		_url.c_str(), 
		mHandle->iformat, 0, 
		&mHandle->params) < 0) 
	{
		std::cerr << "FFMPEGVideo::open() Can not open '" 
			<< _url << "' Error Code:" << err << std::endl;

		return;
    }

	// Retrieve stream information
	if (av_find_stream_info(mHandle->ic) < 0) 
	{
		std::cerr << "FFMPEGVideo::open() Can not open '" 
			<< mVideoConfig.deviceconfig << "', Format unknown!" << std::endl;
		return;
	}
}

void
FFMPEGVideo::close()
{

}

void
FFMPEGVideo::start()
{
}

void
FFMPEGVideo::stop()
{
}


void
FFMPEGVideo::update()
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);
/*
	if (m_frame->buffer && m_image.valid()) {
		m_image->setImage(this->xsize, this->ysize, 1, GL_BGRA, GL_BGRA, 
			GL_UNSIGNED_BYTE, m_frame->buffer, osg::Image::NO_DELETE, 1);
        
	}
*/
}
