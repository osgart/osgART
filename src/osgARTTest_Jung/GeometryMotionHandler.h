#ifndef GEOMETRYMOTIONHANDLER_H
#define GEOMETRYMOTIONHANDLER_H


#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osg/Matrixf>
#include <osg/Geometry>
#include <osg/Array>

#include <string>
#include <iostream>
using namespace std;
class GeometryMotionHandler
{
public:
	GeometryMotionHandler();
	virtual ~GeometryMotionHandler();

	void init(osg::Geometry*, int); // geometry and index for shader attrib location
	void update(osg::Matrix);

private:

	bool firstUpdate;
	osg::Geometry *geom;
	int attriIndex;
	const osg::Vec3Array *origialVertex;

	int currentID;
	int prevID;
	
	osg::Vec3Array *points[3];
	
	
};

#endif
