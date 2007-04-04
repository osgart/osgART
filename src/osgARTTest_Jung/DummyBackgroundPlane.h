#ifndef DUMMYBACKGROUNDPLANE_H
#define DUMMYBACKGROUNDPLANE_H

#include <osg/Node>
#include <osg/Group>

#include <osg/GL>
#include <osg/Viewport>
#include <osg/Texture>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/MatrixTransform>
#include <osg/FrameBufferObject>
#include <osg/Node>
#include <osg/CameraNode>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/Depth>
#include <osg/Vec3f>
#include <math.h>

#include <string>
#include <iostream>
using namespace std;

class DummyBackgroundPlane : public osg::Group
{
public:
	DummyBackgroundPlane();
	virtual ~DummyBackgroundPlane();

	virtual void init(int _width, int _height, int _row, int _col, float _angle);
	void setTexture( osg::ref_ptr<osg::Texture> _tex, int id = 0 );					  

private:


	osg::ref_ptr<osg::Geode> buildGeometry();

	
	osg::ref_ptr<osg::MatrixTransform>	m_layerModelViewMatrix;
	osg::ref_ptr<osg::Projection>		m_layerProjectionMatrix;

	//osg::ref_ptr<osg::CameraNode> camera;
	osg::ref_ptr<osg::Texture> tex;
	osg::ref_ptr<osg::Group> group;


	int width, height, row, col;
	float dist;
	float angle;

	float minVal;
	float minValHalf;

};




#endif