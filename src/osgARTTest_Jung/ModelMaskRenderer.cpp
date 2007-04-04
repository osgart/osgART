#include "ModelMaskRenderer.h"
#include <osg/BlendFunc>

ModelMaskRenderer::ModelMaskRenderer()
{
}

ModelMaskRenderer::~ModelMaskRenderer()
{
}

void ModelMaskRenderer::init()
{
	ShaderFactory sf;
	sf.addFragmentShaderFromFile("./data/shader/ModelMask.frag", this);

	this->getOrCreateStateSet()->setAttributeAndModes
		(new osg::BlendFunc, osg::StateAttribute::ON );
}

void ModelMaskRenderer::setAlpha(float alpha)
{
	getOrCreateStateSet()->addUniform(new osg::Uniform("alpah", alpha)); 
}