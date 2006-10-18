#ifndef DUMMYIMAGELAYER_H
#define DUMMYIMAGELAYER_H

#include <osg/Node>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Projection>
#include <osg/Geode>
#include <osg/Depth>
#include <osg/Texture2D>

#include <osgDB/ReadFile>

#include <string>
#include <iostream>
using namespace std;

class DummyImageLayer : public osg::Group
{

public:
	DummyImageLayer();
	virtual ~DummyImageLayer();

	virtual void init(int, int);
	
	int getWidth()
	{
		return width;
	};

	int getHeight()
	{
		return height;
	};

	void setRenderBin(int);
	void setTexture( osg::ref_ptr<osg::Texture> _tex );

private:
	
	void buildGeometry();
	int width, height;
		
	
	osg::ref_ptr<osg::MatrixTransform>	m_layerModelViewMatrix;
	osg::ref_ptr<osg::Projection>		m_layerProjectionMatrix;
	osg::ref_ptr<osg::Geometry>			m_geometry;
	osg::ref_ptr<osg::Geode>			m_layerGeode;
	osg::ref_ptr<osg::StateSet>			m_layerStateSet;
	osg::ref_ptr<osg::Texture>        tex;

	osg::ref_ptr<osg::Group> layerGroup;
};
#endif