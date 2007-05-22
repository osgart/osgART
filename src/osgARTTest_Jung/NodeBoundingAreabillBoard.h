#ifndef NODEBOUNDINGAREABILLBOARD_H
#define NODEBOUNDINGAREABILLBOARD_H

#include <osg/Node>
#include <osg/Group>
#include <osg/GL>

#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Billboard>
#include <osg/BlendFunc>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
//#include <osgProducer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osg/AlphaFunc>


#include <vector>
using namespace std;

class BillboardUpdateHandler;

enum ONOFF_TYPE
{
	STATIC_IMPORTANTVAL,
	DYNAMIC_IMPORTANTVAL
};

class NodeBoundingAreaBillboard : public osg::Billboard
{
public:
	NodeBoundingAreaBillboard();
	~NodeBoundingAreaBillboard();

	void init(osg::ref_ptr<osg::Texture2D> texture);	
	void addBillboardFor( osg::ref_ptr<osg::Node> node, float scale = 1.0, osg::Vec3f offset = osg::Vec3f(0,0,0) );
	void updateBillboard();

	void setBillboardPosition( int id, osg::Vec3f pos);

	float getScale()
	{
		return scale;
	};

	void setOnOffType(ONOFF_TYPE a );


protected:
	vector< BillboardUpdateHandler* > updateHandlers;
	osg::Drawable* createDrawable(float size);
	osg::StateSet* billBoardStateSet;

	float scale;
	ONOFF_TYPE onoffType;
};

class BillboardUpdateCallback : public osg::NodeCallback
{
public:
	BillboardUpdateCallback();
	virtual ~BillboardUpdateCallback();
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);
};


class BillboardUpdateHandler : public osg::NodeCallback
{
public:
	BillboardUpdateHandler(){};
	~BillboardUpdateHandler(){};

	void init( NodeBoundingAreaBillboard *_billboard)
	{
		nodeBoundingAreaBillboard = _billboard;
	};

	void setID(int _id)
	{
		drawableID = _id;
	};

	virtual void setNode(osg::ref_ptr<osg::Node> _node)
	{
		node = _node;	
	};

	virtual void update(int id)
	{
		nodeBoundingAreaBillboard->setPosition(id, this->getPosition() + offset);
	};

	virtual osg::Vec3f getPosition()
	{		
		return node->getBound().center() - osg::Vec3(0,node->getBound().radius()*nodeBoundingAreaBillboard->getScale(), 0 );
	};

	void setOnOffType(ONOFF_TYPE a )
	{
		onoffType = a;
	};

	void setOffset( osg::Vec3f _offset)
	{
		offset = _offset;
	};

	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv)
	{
		update(drawableID);
		traverse(nd,nv);
	};
protected:
	osg::ref_ptr<osg::Node> node;
	NodeBoundingAreaBillboard *nodeBoundingAreaBillboard;
	ONOFF_TYPE onoffType;
	int drawableID;

	osg::Vec3f offset;
};

#endif