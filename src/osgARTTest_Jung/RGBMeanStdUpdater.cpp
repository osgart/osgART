#include "RGBMeanStdUpdater.h"

MeanStdCalculator::MeanStdCalculator()
{
}

MeanStdCalculator::~MeanStdCalculator()
{
	//delete buf;
}

void MeanStdCalculator::init( osg::Image* _inputImage)
{
	inputImage = _inputImage;
}

void MeanStdCalculator::update()
{
	int w,h;
	w = inputImage->s();
	h = inputImage->t();
	int total = w * h;

	mean[0]=0;
	mean[1]=0;
	mean[2]=0;

	rgbStd[0] = 0;
	rgbStd[1] = 0;
	rgbStd[2] = 0;

	unsigned char *data = inputImage->data();

	// Make PDF (histogram)
	if ( inputImage->getPixelFormat () == GL_RGB )
	{
		int size = total * 3;
		
		// calc mean
		for ( int k=0; k < size ; k=k+3 )
		{
			//data[k] 
			mean[0] = mean[0] + data[k  ];
			mean[1] = mean[1] + data[k+1];
			mean[2] = mean[2] + data[k+2];
		}
		mean = mean / (float)(total);

		// calc std		
		for ( int k=0; k < size ; k=k+4)
		{
			rgbStd[0] = rgbStd[0] + pow( data[k  ] - mean[0], 2);
			rgbStd[1] = rgbStd[1] + pow( data[k+1] - mean[1], 2);
			rgbStd[2] = rgbStd[2] + pow( data[k+2] - mean[2], 2);
		}
		
		// div by (n-1)
		rgbStd = rgbStd / (float)(total - 1);
			
		rgbStd[0] = sqrt( rgbStd[0] );
		rgbStd[1] = sqrt( rgbStd[1] );
		rgbStd[2] = sqrt( rgbStd[2] );

	}
	else if ( inputImage->getPixelFormat () == GL_RGBA )
	{
		int size = total * 4;
		
		// calc mean
		for ( int k=0; k < size ; k=k+4 )
		{
			//data[k] 
			mean[0] = mean[0] + data[k  ];
			mean[1] = mean[1] + data[k+1];
			mean[2] = mean[2] + data[k+2];
		}
		mean = mean / (float)(total);

		// calc std		
		for ( int k=0; k < size ; k=k+4)
		{
			rgbStd[0] = rgbStd[0] + pow( data[k  ] - mean[0], 2);
			rgbStd[1] = rgbStd[1] + pow( data[k+1] - mean[1], 2);
			rgbStd[2] = rgbStd[2] + pow( data[k+2] - mean[2], 2);
		}
		
		// div by (n-1)
		rgbStd = rgbStd / (float)(total - 1);
			
		rgbStd[0] = sqrt( rgbStd[0] );
		rgbStd[1] = sqrt( rgbStd[1] );
		rgbStd[2] = sqrt( rgbStd[2] );
	}
	else if ( inputImage->getPixelFormat () == GL_BGR )
	{
		std::cout << "GL_BGR" << std::endl;
	}
	else if ( inputImage->getPixelFormat () == GL_BGRA )
	{
		int size = total * 4;
		
		// calc mean
		for ( int k=0; k < size ; k=k+4 )
		{
			//data[k] 
			mean[0] = mean[0] + data[k+2];
			mean[1] = mean[1] + data[k+1];
			mean[2] = mean[2] + data[k  ];
		}
		mean = mean / (float)(total);

		// calc std		
		for ( int k=0; k < size ; k=k+4)
		{
			rgbStd[0] = rgbStd[0] + pow( data[k+2] - mean[0], 2);
			rgbStd[1] = rgbStd[1] + pow( data[k+1] - mean[1], 2);
			rgbStd[2] = rgbStd[2] + pow( data[k  ] - mean[2], 2);
		}
		
		// div by (n-1)
		rgbStd = rgbStd / (float)(total - 1);
			
		rgbStd[0] = sqrt( rgbStd[0] );
		rgbStd[1] = sqrt( rgbStd[1] );
		rgbStd[2] = sqrt( rgbStd[2] );
	}

	mean   = mean   / 255.0;
	rgbStd = rgbStd / 255.0;
	


}
	
///////////////////////////////////////////////////////////

MeanStdUpdateCallback::MeanStdUpdateCallback()
{
}

MeanStdUpdateCallback::~MeanStdUpdateCallback()
{
}
	
void MeanStdUpdateCallback::operator()(osg::Node *nd, osg::NodeVisitor* nv)
{
	meanStdCalc->update();
	osg::StateSet *state = nd->getOrCreateStateSet();

	//sMean->set( sourceMean );
	//std::cout << "scaler  " << sourceMean[0] << " " << sourceMean[1] << " " << sourceMean[2]  <<  std::endl;

	osg::Vec3f scaler;
	osg::Vec3f currentStd = meanStdCalc->getStd();
	scaler[0] = currentStd[0] / sourceStd[0];
	scaler[1] = currentStd[1] / sourceStd[1];
	scaler[2] = currentStd[2] / sourceStd[2];

	stdScaler->set( scaler );
	tMean->set( meanStdCalc->getMean() );
	

	traverse(nd,nv);
	//std::cout << "scaler  " << scaler[0] << " " << scaler[1] << " " << scaler[2]  <<  std::endl;
}

void MeanStdUpdateCallback::init(osg::Image *_inputImage, osg::Uniform *_sMean, osg::Uniform *_tMean, osg::Uniform *_stdScaler)
{
	meanStdCalc = new MeanStdCalculator();
	meanStdCalc->init(_inputImage);

	sMean = _sMean;
	tMean = _tMean;
	stdScaler = _stdScaler;
}


