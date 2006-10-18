
#include "ARNode.h"

ARNode::ARNode()
{
}

ARNode::~ARNode()
{
}

void ARNode::init(int markerID, int trackerID)
{
	markerTrans = new osgART::ARTTransform(markerID, trackerID);

	// never assume the Marker really exists
	osg::ref_ptr<osgART::Marker> marker = markerTrans->getMarker();

	// check before accessing the linked marker
	if (marker.valid()) marker->setActive(true);

	this->addChild(markerTrans.get());
}
	
void ARNode::addModel(osg::ref_ptr<osg::Node> model)
{
	markerTrans->addChild(model.get());
}

osg::ref_ptr<osgART::ARTTransform> ARNode::getMarkerTrans()
{
	return markerTrans;
}