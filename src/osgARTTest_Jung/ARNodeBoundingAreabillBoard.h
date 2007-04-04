#ifndef ARNODEBOUNDINGAREABILLBOARD_H
#define ARNODEBOUNDINGAREABILLBOARD_H

#include <osg/Node>
#include <osg/Group>
#include <osg/GL>

#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Billboard>
#include <osg/BlendFunc>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgProducer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osg/AlphaFunc>

#include "ARNode.h"

#include <vector>
using namespace std;

#include "NodeBoundingAreaBillboard.h"

class ARNodeBoundingAreaBillboard : public NodeBoundingAreaBillboard
{
public:
	ARNodeBoundingAreaBillboard();
	~ARNodeBoundingAreaBillboard();

	void addBillboardFor( osg::ref_ptr<ARNode> arNode, float sacle = 1.0 );

private:
};

class BillboardUpdateHandlerForARNode : public BillboardUpdateHandler
{
public:
	BillboardUpdateHandlerForARNode(){};
	~BillboardUpdateHandlerForARNode(){};

	virtual void setNode(osg::ref_ptr<osg::Node> _node)
	{
		node = _node;	
	};

	virtual void update(int id)
	{

		ARNode *arNode = dynamic_cast<ARNode*>(node.get());

		bool isMarkerVisible = arNode->getMarkerTrans()->getMarker()->isValid();

		osg::Vec3 ppp = arNode->getMarkerTrans()->getMatrix().getTrans();

		osg::Drawable *drawable = nodeBoundingAreaBillboard->getDrawable(id);
		osg::StateSet *drawableStateSet = drawable->getOrCreateStateSet();

		osg::ColorMask *colorMask = new osg::ColorMask();
		colorMask->setMask( isMarkerVisible,isMarkerVisible,isMarkerVisible,isMarkerVisible);
		drawableStateSet->setAttributeAndModes(colorMask, osg::StateAttribute::ON);

		osg::Vec3 pos = this->getPosition() + ppp;
		nodeBoundingAreaBillboard->setPosition(id, pos);

		//if ( isMarkerVisible )
		//	std::cout << ppp[0] << " " << ppp[1] << " " << ppp[2] << std::endl;


		if ( isMarkerVisible )
		{			
			arNode->increaseImportanceVal();
			arNode->increaseImportanceVal();
			arNode->increaseImportanceVal();
			arNode->increaseImportanceVal();

			osg::Geometry *geo = dynamic_cast<osg::Geometry*>(drawable);

			osg::Vec4Array* colorArray = dynamic_cast<osg::Vec4Array*>(geo->getColorArray());
			static float alpha =0;
			colorArray->at(0) = osg::Vec4(1,1,1, arNode->getImportanceVal() );
			
			geo->dirtyDisplayList();
			//std::cout << "Increasing" << std::endl;
		}
		else
		{
			arNode->decreaseImportanceVal();
			arNode->decreaseImportanceVal();
			arNode->decreaseImportanceVal();
			arNode->decreaseImportanceVal();
			arNode->decreaseImportanceVal();
			//std::cout << "Decreasing" << std::endl;
		
		}

	
	};

protected:

};

#endif