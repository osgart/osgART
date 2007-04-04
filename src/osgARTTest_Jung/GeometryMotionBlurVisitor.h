#ifndef GeometryMotionBlurVisitor_H
#define GeometryMotionBlurVisitor_H


#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osg/Matrixf>

#include <vector>
using namespace std;

#include "GeometryMotionHandler.h"

class GeometryMotionBlurVisitor : public osg::NodeVisitor 
{
public:
	GeometryMotionBlurVisitor();
	virtual ~GeometryMotionBlurVisitor();

	void init(int);
	virtual void apply(osg::Node& node);

	void update(osg::Matrix);
private:
	int attriIndex;
	vector<GeometryMotionHandler> gHandlers;	
};

#endif

