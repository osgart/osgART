
#include "ARNode.h"
#include "ARScene.h"

ARNode::ARNode()
{
	importanceVal = 1.0f;
	importanceValUnit = 0.03f;
}

ARNode::~ARNode()
{
}

void ARNode::init(int markerID, osgART::GenericTracker* tracker)
{
	if (tracker) 
	{
		// never assume the Marker really exists
		osg::ref_ptr<osgART::Marker> marker = tracker->getMarker(markerID);

		// check before accessing the linked marker
		if (marker.valid()) 
		{
			markerTrans = new osgART::ARTTransform(marker.get());

			marker->setActive(true);

			this->addChild(markerTrans.get());
		}
	} else 
	{
		std::cerr << "Argrgh1" << std::endl;

	}
}
	
void ARNode::setParentScene(osg::ref_ptr<ARScene> _arScene)
{
	arScene = _arScene;
}

osg::ref_ptr<ARScene> ARNode::getParentScene()
{
	return arScene;
}

void ARNode::addModel(osg::ref_ptr<osg::Node> model)
{
	markerTrans->addChild(model.get());
}

osg::ref_ptr<osgART::ARTTransform> ARNode::getMarkerTrans()
{
	return markerTrans;
}