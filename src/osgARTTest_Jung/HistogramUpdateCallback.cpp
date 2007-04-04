#include "HistogramUpdateCallback.h"

HistogramUpdateCallback::HistogramUpdateCallback()
{
}

HistogramUpdateCallback::~HistogramUpdateCallback()
{
	delete histogramMaker;
}
void HistogramUpdateCallback::operator()(osg::Node *nd, osg::NodeVisitor* nv)
{
	histogramMaker->update();
}

void HistogramUpdateCallback::init(osg::Image *inputImage )
{
	histogramMaker = new HistogramMaker();
	histogramMaker->init(inputImage);

}

osg::ref_ptr<osg::Texture> HistogramUpdateCallback::getHistogram()
{
	return histogramMaker->getHistogram();
}

////////////////////////////////////////////////////////////////////////////////

HistogramMaker::HistogramMaker()
{
	inverse = false;
}

HistogramMaker::~HistogramMaker()
{
	delete buf;
}

void HistogramMaker::init( osg::Image *_inputImage  )
{
	inputImage = _inputImage;

	histogram = new osg::Texture1D;
	osg::Texture1D *histogram1D = dynamic_cast<osg::Texture1D*>(histogram.get());

	histogram1D->setTextureWidth(256);
	histogram1D->setSourceFormat( GL_LUMINANCE );
	histogram1D->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	histogram1D->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR); 
	histogram1D->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);

	histogramImage = new osg::Image();
	 
	//unsigned char buf[256 * sizeof(unsigned char)];
	buf = new unsigned char[256 * sizeof(unsigned char)];

	for ( int k=0; k < 256 ; k++)
	{
		buf[k] = k;
	}

	histogramImage->setImage( 256, 0, 0, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, buf, osg::Image::NO_DELETE);
	
	
	histogramImage->dirty();

	histogram1D->setImage( histogramImage );
}

void HistogramMaker::update()
{
	int w,h, index;
	w = inputImage->s();
	h = inputImage->t();
	int total = w * h;
	float r,g,b;

	float counter[256];
	int counterPixel=0;

	unsigned char *data = inputImage->data();

	for ( int k=0; k < 256 ; k++)
	{
		counter[k] = 0;
	}

	// Make PDF (histogram)
	if ( inputImage->getPixelFormat () == GL_RGB )
	{
		int size = total * 3;
		for ( int k=0; k < size ; k = k + 3 )
		{
			r = (float)data[k] * 0.3;
			g = (float)data[k+1] * 0.59;
			b = (float)data[k+2] * 0.11;
			index = (int)(r + g + b);

			if ( index > 256 )
				index = 256;

			counter[index] = counter[index] + 1.0;
		}
		//total = total / 3;
	}
	else if ( inputImage->getPixelFormat () == GL_RGBA )
	{
		int size = total * 4;
		for ( int k=0; k < size ; k = k + 4 )
		{
			r = (float)data[k] * 0.3;
			g = (float)data[k+1] * 0.59;
			b = (float)data[k+2] * 0.11;
			index = (int)(r + g + b);

			if ( index > 256 )
				index = 256;

			counter[index] = counter[index] + 1.0;
		}
	}
	else if ( inputImage->getPixelFormat () == GL_BGR )
	{
		std::cout << "GL_BGR" << std::endl;
	}
	else if ( inputImage->getPixelFormat () == GL_BGRA )
	{
		int size = total * 4;
		for ( int k=0; k < size ; k = k+4 )
		{
			r = (float)data[k+2] * 0.3;
			g = (float)data[k+1] * 0.59;
			b = (float)data[k] * 0.11;
			index = (int)(r + g + b);

			if ( index > 255 )
				index = 255;

			counter[index] = counter[index] + 1.0;
			counterPixel++;

		}
	}
	
	//Make CDF (accumulate)
	for ( int k=1; k < 256 ; k++)
	{
		counter[k] = counter[k] + counter[k-1];

		std::cout << counter[k] << std::endl;
	}

	//normalize it
	for ( int k=0; k < 256 ; k++)
	{
		counter[k] =  ((float) counter[k] * 255.0f / (float)total) ;
		//std::cout << counter[k] << std::endl;
	}

	if ( inverse )
	{
		int id;
		for ( int k=0; k < 256 ; k++)
		{
			id =  (int)(counter[k]) ;
			buf[id] = k;
		}
	}
	else 
	{
		for ( int k=0; k < 256 ; k++)
		{
			buf[k] =  (int)(counter[k]) ;
		}		
	}
	
	histogramImage->setImage( 256, 0, 0, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, buf, osg::Image::NO_DELETE);
}	

osg::ref_ptr<osg::Texture> HistogramMaker::getHistogram()
{
	return histogram;
}