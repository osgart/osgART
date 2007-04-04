#ifndef ARNodeViewingCallback_h
#define ARNodeViewingCallback_h

#include <osg/LineSegment>
#include "ARNode.h"

class ARNodeViewingCallback : public osg::NodeCallback
{
public:
	ARNodeViewingCallback();
	~ARNodeViewingCallback();

	//void init(osg::Vec3f camPos, osg::Vec3f viewVec);

	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);
private:
	void calcLine();

	osg::ref_ptr<osg::LineSegment> viewLine;


};


#endif