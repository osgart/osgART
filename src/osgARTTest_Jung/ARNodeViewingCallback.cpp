#include "ARNodeViewingCallback.h"

ARNodeViewingCallback::ARNodeViewingCallback()
{
	viewLine = new osg::LineSegment;
}

ARNodeViewingCallback::~ARNodeViewingCallback()
{
}

void ARNodeViewingCallback::calcLine()
{
	
}

void ARNodeViewingCallback::operator()(osg::Node *nd , osg::NodeVisitor* nv)
{
	ARNode *arNode = dynamic_cast<ARNode*>( nd ); 
	
	calcLine();

	arNode->increaseImportanceVal();
	arNode->increaseImportanceVal();
	arNode->increaseImportanceVal();
}


