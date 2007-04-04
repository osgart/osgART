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
#include <osgProducer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osg/AlphaFunc>


#include <vector>
using namespace std;

class BillboardUpdateHandler;

class NodeBoundingAreaBillboard : public osg::Billboard
{
public:
	NodeBoundingAreaBillboard();
	~NodeBoundingAreaBillboard();

	void init(osg::ref_ptr<osg::Texture2D> texture);	
	void addBillboardFor( osg::ref_ptr<osg::Node> node, float scale = 1.0 );
	void updateBillboard();

	void setBillboardPosition( int id, osg::Vec3f pos);

	float getScale()
	{
		return scale;
	};
protected:
	vector< BillboardUpdateHandler* > updateHandlers;
	osg::Drawable* createDrawable(float size);
	osg::StateSet* billBoardStateSet;

	float scale;
};

class BillboardUpdateCallback : public osg::NodeCallback
{
public:
	BillboardUpdateCallback();
	virtual ~BillboardUpdateCallback();
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);
};


class BillboardUpdateHandler
{
public:
	BillboardUpdateHandler(){};
	~BillboardUpdateHandler(){};

	void init( NodeBoundingAreaBillboard *_billboard)
	{
		nodeBoundingAreaBillboard = _billboard;
	};

	virtual void setNode(osg::ref_ptr<osg::Node> _node)
	{
		node = _node;	
	};

	virtual void update(int id)
	{
		nodeBoundingAreaBillboard->setPosition(id, this->getPosition());
	};

	virtual osg::Vec3f getPosition()
	{		
		return node->getBound().center() - osg::Vec3(0,node->getBound().radius()*nodeBoundingAreaBillboard->getScale(), 0 );
	};

protected:
	osg::ref_ptr<osg::Node> node;
	NodeBoundingAreaBillboard *nodeBoundingAreaBillboard;
};

#endif