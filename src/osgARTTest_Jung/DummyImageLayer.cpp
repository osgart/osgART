#include "DummyImageLayer.h"

DummyImageLayer::DummyImageLayer()
{
}
DummyImageLayer::~DummyImageLayer()
{
}

void DummyImageLayer::init(int w, int h)
{
	width = w;
	height = h;

	buildGeometry();
}

void DummyImageLayer::setRenderBin(int binNum)
{
	m_layerProjectionMatrix->getOrCreateStateSet()->setRenderBinDetails(binNum, "RenderBin");
}

void DummyImageLayer::buildGeometry()
{
	m_layerProjectionMatrix = new osg::Projection(osg::Matrix::ortho2D(0, width, 0, height));
	this->addChild(	m_layerProjectionMatrix.get());

	m_layerModelViewMatrix = new osg::MatrixTransform();
	m_layerModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_layerProjectionMatrix->addChild(m_layerModelViewMatrix.get());

	layerGroup = new osg::Group();
	m_layerModelViewMatrix->addChild(layerGroup.get());

	m_layerStateSet = layerGroup->getOrCreateStateSet();
	
	layerGroup->getOrCreateStateSet()->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 1.0f, 1.0f));

	m_layerGeode = new osg::Geode();
	layerGroup->addChild(m_layerGeode.get());
	
	m_layerGeode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0)));
	m_layerGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex.get(), osg::StateAttribute::ON);

		
	getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}

void DummyImageLayer::setTexture( osg::ref_ptr<osg::Texture> _tex )
{
	tex = _tex;
	//m_layerStateSet->setTextureAttributeAndModes(0, tex.get() ,osg::StateAttribute::ON);
	getOrCreateStateSet()->setTextureAttributeAndModes(0, tex.get() ,osg::StateAttribute::ON);
	//layerGroup->getOrCreateStateSet()->setTextureAttribute(1, tex.get(),osg::StateAttribute::ON);
}