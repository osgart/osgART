#include "ARNodeBoundingAreaBillboard.h"
#include <iostream>
#include <sstream>

ARNodeBoundingAreaBillboard::ARNodeBoundingAreaBillboard()
{
}

ARNodeBoundingAreaBillboard::~ARNodeBoundingAreaBillboard()
{
}

void ARNodeBoundingAreaBillboard::addBillboardFor( osg::ref_ptr<ARNode> arNode, float scale, osg::Vec3f offset )
{

	static int nameID=0;
	
	osg::BoundingSphere bound = arNode->getBound();
	osg::Drawable *drawable = createDrawable(bound.radius()* 2 * scale);

	std::stringstream sss;
	sss << "Name " << nameID << std::endl;
	drawable->setName( sss.str() );
	nameID++;

	this->addDrawable( drawable , bound.center() );	

	BillboardUpdateHandlerForARNode *bupdateHandler = new BillboardUpdateHandlerForARNode();
	this->scale = scale;
	bupdateHandler->init(this);
	bupdateHandler->setNode( arNode.get() );
	bupdateHandler->setOnOffType( onoffType );
	bupdateHandler->setID( updateHandlers.size() );
		
	arNode->setUpdateCallback( bupdateHandler );


    updateHandlers.push_back( bupdateHandler );
}
