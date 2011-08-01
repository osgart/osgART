#include "osgART/VideoConfig"
#include "DummyImageVideo"
#include "OpenThreads/Thread"

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osg/Notify>

DummyImageVideo::DummyImageVideo():
	osgART::Video(),
	m_flip_horizontal(false),
	m_flip_vertical(false),
	m_max_width(640)
{

	_fields["flip_horizontal"] = new osgART::TypedField<bool>(&m_flip_horizontal);
	_fields["flip_vertical"]	= new osgART::TypedField<bool>(&m_flip_vertical);

	_fields["max_width"] = new osgART::TypedField<unsigned int>(&m_max_width);

	_fields["image_file"]		= new osgART::CallbackField<DummyImageVideo, std::string>(this,
		&DummyImageVideo::getImageFile,
		&DummyImageVideo::setImageFile);
}

DummyImageVideo::DummyImageVideo(const DummyImageVideo &, const osg::CopyOp& copyop) {
    
}

DummyImageVideo::~DummyImageVideo(void) {
    
}

DummyImageVideo&  DummyImageVideo::operator=(const DummyImageVideo &) {
    return *this;
}


bool DummyImageVideo::open() {

	osg::notify() << "DummyImageVideo::open()  open image : " << videoName << std::endl;
	
	if (videoName.empty()) {
		osg::notify(osg::WARN) << "Error in DummyImageVideo::open(), File name is empty!";
		return false;
	}

	osg::Image* img = osgDB::readImageFile(videoName.c_str());

	if (!img) {
		osg::notify(osg::WARN) << "Error in DummyImageVideo::open(), Could not open File!";
		return false;
	}

	int w = img->s();
	int h = img->t();

	if (w > m_max_width) {
		osg::notify() << "DummyImageVideo: Image width exceeds maximum (" << m_max_width << "). Image will be resized";

		float aspect = (float)h / (float)w;
		w = m_max_width;
		h = w * aspect;

		img->scaleImage(w, h, 1);

		//osgDB::writeImageFile(*img, "test_resize.jpg");

	}

	

	int components = osg::Image::computeNumComponents(img->getPixelFormat());

	this->allocateImage(w, h, 1, GL_BGRA, GL_UNSIGNED_BYTE);

	for (int j = 0; j < h; j++) {

		for (int i = 0; i < w; i++) {

			unsigned char* srcPtr = img->data(i, j);
			unsigned char* dstPtr = this->data(i, j);

			switch (img->getPixelFormat()) {
				case GL_RGB:
				case GL_RGBA:
					dstPtr[0] = srcPtr[2];
					dstPtr[1] = srcPtr[1];
					dstPtr[2] = srcPtr[0];
					dstPtr[3] = 0;
					break;
			}

		}

	}


	/*
	xsize = m_image->s();
	ysize = m_image->t();

	if (osg::Image::computeNumComponents(m_image->getPixelFormat()) == 3) {
		m_image->setPixelFormat(GL_RGB);
		pixelsize=3;
		pixelformat=VIDEOFORMAT_RGB24;
	} else {
		if (osg::Image::computeNumComponents(m_image->getPixelFormat()) == 4) {
			m_image->setPixelFormat(GL_RGBA);
			pixelsize=4;
			pixelformat=VIDEOFORMAT_RGBA32;
		} else {
			std::cerr<<"ERROR:can't load the image, format not supported."<<std::endl;
			exit(-1);
		}
	}
	*/

	if (m_flip_vertical) this->flipVertical();
	if (m_flip_horizontal) this->flipHorizontal();

	return true;

}


void DummyImageVideo::update(osg::NodeVisitor* nv) {

	// Increase modified count every X ms to ensure tracker updates
	if (updateTimer.time_m() > 50) {
		this->dirty();
		updateTimer.setStartTick();
	}

}



void DummyImageVideo::setImageFile(const std::string & _NewFile) {
	videoName = _NewFile;
	open();
}
	
std::string DummyImageVideo::getImageFile() const {
	return videoName;
}

void DummyImageVideo::start() { }
void DummyImageVideo::stop() { }
//void DummyImageVideo::update() { }
void DummyImageVideo::close() { }



osgART::PluginProxy<DummyImageVideo> g_dummyimagevideo("osgart_video_dummyimage");
