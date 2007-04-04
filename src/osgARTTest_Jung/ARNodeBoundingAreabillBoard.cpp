#include "ARNodeBoundingAreaBillboard.h"

ARNodeBoundingAreaBillboard::ARNodeBoundingAreaBillboard()
{
}

ARNodeBoundingAreaBillboard::~ARNodeBoundingAreaBillboard()
{
}

void ARNodeBoundingAreaBillboard::addBillboardFor( osg::ref_ptr<ARNode> arNode, float scale )
{
	osg::BoundingSphere bound = arNode->getBound();
	osg::Drawable *drawable = createDrawable(bound.radius()* 2 * scale);

	this->addDrawable( drawable , bound.center() );	

	BillboardUpdateHandlerForARNode *bupdateHandler = new BillboardUpdateHandlerForARNode();
	this->scale = scale;
	bupdateHandler->init(this);
	bupdateHandler->setNode( arNode.get() );

    updateHandlers.push_back( bupdateHandler );
}
