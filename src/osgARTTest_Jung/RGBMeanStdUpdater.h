#ifndef RGBMeanStdUpdater_h
#define RGBMeanStdUpdater_h

#include <osg/Node>
#include <osg/Texture>
#include <osg/Texture2D>
#include <osg/Image>

#include <string>
#include <iostream>
using namespace std;

class MeanStdCalculator
{
public:
	MeanStdCalculator();
	~MeanStdCalculator();

	
	void init( osg::Image* );
	void update();
	
	osg::Vec3f getMean()
	{
		return mean;
	};

	osg::Vec3f getStd()
	{
		return rgbStd;
	};

private:
	osg::Image *inputImage;
	osg::Image *stdImage;
	
	osg::Vec3f mean;
	osg::Vec3f rgbStd;
};

class MeanStdUpdateCallback : public osg::NodeCallback
{
public:
	MeanStdUpdateCallback();
	~MeanStdUpdateCallback();
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);

	void init(osg::Image*, osg::Uniform*, osg::Uniform*, osg::Uniform*);

	osg::Vec3f getMean()
	{
		return meanStdCalc->getMean();
	};

	osg::Vec3f getStdTexture()
	{
		return meanStdCalc->getStd();
	};

	void setMeanStd( osg::Vec3f sm, osg::Vec3f sStd)
	{
		sourceMean = sm;
		sourceStd  = sStd;
	}
private:

	MeanStdCalculator *meanStdCalc;

	osg::Vec3f sourceMean;
	osg::Vec3f sourceStd;

	osg::Uniform *sMean;
	osg::Uniform *tMean;
	osg::Uniform *stdScaler;
};

#endif
