#ifndef HistogramUpdateCallback_h
#define HistogramUpdateCallback_h

#include <osg/Node>
#include <osg/Image>
#include <osg/Texture>
#include <osg/Texture1D>
#include <osg/Texture2D>

#include <string>
#include <iostream>
using namespace std;

class HistogramMaker 
{
public:
	HistogramMaker();
	~HistogramMaker();

	void init( osg::Image* );
	void update();
	
	void setInverse(bool inv)
	{
		inverse = inv;
	}

	bool isInverse()
	{
		return inverse;
	};
	osg::ref_ptr<osg::Texture> getHistogram();
private:
	osg::Image *inputImage;
	osg::Image *histogramImage;
	unsigned char *buf;

	osg::ref_ptr<osg::Texture> histogram;

	bool inverse;
};

class HistogramUpdateCallback : public osg::NodeCallback
{
public:
	HistogramUpdateCallback();
	~HistogramUpdateCallback();
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);

	void init(osg::Image*);
	osg::ref_ptr<osg::Texture> getHistogram();
private:

	HistogramMaker *histogramMaker;
};


#endif
