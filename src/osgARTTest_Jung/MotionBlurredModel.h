#ifndef MOTIONBLURREDMODEL_H
#define MOTIONBLURREDMODEL_H


#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/Matrix>
#include <osg/Matrixf>

#include "ARNode.h"
#include "GeometryMotionBlurVisitor.h"
#include "DummyImageLayer.h"

class MotionBlurredModel : public osg::Group
{

public:
	MotionBlurredModel();
	virtual ~MotionBlurredModel();

	void init(osg::ref_ptr<osg::Node> _targetModel, int attriIndex);
	void update();
	
private:
	
	osg::ref_ptr<osg::Node> targetModel;
	
    GeometryMotionBlurVisitor geoVisitor;

	


};

class MotionBlurredModelCallback : public osg::NodeCallback
{
public:
	MotionBlurredModelCallback();
	virtual ~MotionBlurredModelCallback();
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);
};

#endif
