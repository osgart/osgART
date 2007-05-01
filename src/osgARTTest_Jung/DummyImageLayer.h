#ifndef DUMMYIMAGELAYER_H
#define DUMMYIMAGELAYER_H

#include <osg/Node>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Projection>
#include <osg/Geode>
#include <osg/Drawable>
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

	virtual void init(int w, int h, int colNum = 1, int rowNum = 1);
	
	int getWidth()
	{
		return width;
	};

	int getHeight()
	{
		return height;
	};

	void setRenderBin(int);
	void setTexture( osg::ref_ptr<osg::Texture> _tex, int id = 0, osg::StateAttribute::Values v = osg::StateAttribute::ON );

private:
	
	void buildGeometry();
	osg::Drawable* createDrawable();
	int width, height;
	int row, col;	
	
	osg::ref_ptr<osg::MatrixTransform>	m_layerModelViewMatrix;
	osg::ref_ptr<osg::Projection>		m_layerProjectionMatrix;
	osg::ref_ptr<osg::Geometry>			m_geometry;
	osg::ref_ptr<osg::Geode>			m_layerGeode;
	osg::ref_ptr<osg::StateSet>			m_layerStateSet;
	
	osg::ref_ptr<osg::Group> layerGroup;
};
#endif