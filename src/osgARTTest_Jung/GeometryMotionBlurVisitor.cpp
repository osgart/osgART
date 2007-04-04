#include "GeometryMotionBlurVisitor.h"


GeometryMotionBlurVisitor::GeometryMotionBlurVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
}

GeometryMotionBlurVisitor::~GeometryMotionBlurVisitor()
{
}

void GeometryMotionBlurVisitor::init(int _attriIndex)
{
	attriIndex = _attriIndex;
}

void GeometryMotionBlurVisitor::apply(osg::Node& node)
{

	gHandlers.clear();
	if (osg::Geode* geode = dynamic_cast<osg::Geode*>(&node)) 
	{
		for (int i = 0; i < geode->getNumDrawables(); i++) 
		{
			if (osg::Geometry* geom = geode->getDrawable(i)->asGeometry()) 
			{

				GeometryMotionHandler gMotionHandler;
				gMotionHandler.init( geom, attriIndex);

				gHandlers.push_back(gMotionHandler);
			}
		}
	}
	
    traverse(node);
}

void GeometryMotionBlurVisitor::update(osg::Matrix m)
{
	int size = gHandlers.size();

	for (int k=0; k < size ; k++)
	{
		gHandlers.at(k).update(m);
	}



}