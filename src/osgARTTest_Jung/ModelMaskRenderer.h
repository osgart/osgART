#ifndef MODELMASKRENDERER_H
#define MODELMASKRENDERER_H

#include <osg/Node>
#include <osg/Group>
#include <osg/GL>

#include <osg/Geometry>
#include <osg/Texture2D>

#include "ShaderFactory.h"

class ModelMaskRenderer : public osg::Group
{
public:
	ModelMaskRenderer();
	~ModelMaskRenderer();

	void init();
	void setAlpha(float alpha);

private:

	

};


#endif
