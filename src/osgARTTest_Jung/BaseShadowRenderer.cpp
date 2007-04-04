#include <osg/BlendFunc>
#include "BaseShadowRenderer.h"
#include "DummyImageLayer.h"
#include "ShaderFactory.h"

void osgART::BaseShadowRenderer::init()
{   

	shadowedSceneWithPhantom = new osg::Group;

	// setup for shadowTextureRef
    shadowTextureRef = new osg::Texture2D;
	shadowTextureRef->setTextureSize(2048, 2048);

	shadowTextureRef->setInternalFormat(GL_DEPTH_COMPONENT);
	shadowTextureRef->setShadowComparison(true);
	shadowTextureRef->setShadowTextureMode(osg::Texture2D::LUMINANCE);
	shadowTextureRef->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	shadowTextureRef->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	shadowTextureRef->setBorderColor(osg::Vec4(1, 1, 1, 1));  
	shadowTextureRef->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);
	shadowTextureRef->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER);
	
	osg::CameraNode* camera = new osg::CameraNode;
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	//camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->setClearColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	
	camera->setComputeNearFarMode(osg::CameraNode::DO_NOT_COMPUTE_NEAR_FAR);
	camera->setViewport(0, 0, 2048, 2048);
	camera->setRenderOrder(osg::CameraNode::PRE_RENDER);
	camera->setRenderTargetImplementation(osg::CameraNode::FRAME_BUFFER_OBJECT);
	camera->attach(osg::CameraNode::DEPTH_BUFFER, shadowTextureRef);
	//camera->attach(osg::CameraNode::COLOR_BUFFER, shadowTexture);

	osg::StateSet* camStateSet = camera->getOrCreateStateSet();
	camStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	camStateSet->setAttribute(new osg::PolygonOffset(4, 8), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	camStateSet->setMode(GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	
	//osg::Group* shadowedGroup = new osg::Group;
	shadowedSceneWithPhantom->addChild(m_shadowedScene);

	camera->addChild(shadowedSceneWithPhantom.get());
	this->addChild(camera);


	// setup for shadowTexture

	shadowTexture = new osg::Texture2D;
	shadowTexture->setTextureSize(m_shadowWidth, m_shadowHeight);

	shadowTexture->setSourceFormat(GL_RGBA);
	shadowTexture->setInternalFormat(GL_RGBA);

	shadowTextureRef->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	shadowTextureRef->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

	shadowTextureRef->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);
	shadowTextureRef->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER);
	
	osg::CameraNode* camera2 = new osg::CameraNode;
	camera2->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera2->setClearColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	
	camera2->setReferenceFrame(osg::CameraNode::ABSOLUTE_RF);
	camera2->setComputeNearFarMode(osg::CameraNode::DO_NOT_COMPUTE_NEAR_FAR);
	camera2->setViewport(0, 0, m_shadowWidth, m_shadowHeight);
	camera2->setRenderOrder(osg::CameraNode::PRE_RENDER);
	camera2->setRenderTargetImplementation(osg::CameraNode::FRAME_BUFFER_OBJECT);
	camera2->attach(osg::CameraNode::COLOR_BUFFER, shadowTexture);
	
	osg::ref_ptr<osg::Projection> projectionMatrix = new osg::Projection(osg::Matrix(m_tracker->getProjectionMatrix()));	
	osg::ref_ptr<osgART::ARTTransform> markerTrans = new osgART::ARTTransform(m_markerId);


	this->addChild(camera2);
	camera2->addChild( projectionMatrix.get() );
	projectionMatrix->addChild(markerTrans.get());
	markerTrans->addChild(shadowedSceneWithPhantom.get());

	osg::StateSet* stateset = shadowedSceneWithPhantom->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, shadowTextureRef, osg::StateAttribute::ON);

	osg::Program* program = new osg::Program;   

	osg::Shader* vertShader = new osg::Shader(osg::Shader::VERTEX);
	vertShader->loadShaderSourceFromFile("./data/shader/shadow.vert");
	program->addShader(vertShader);

	osg::Shader* fragShader = new osg::Shader(osg::Shader::FRAGMENT);
	fragShader->loadShaderSourceFromFile("./data/shader/shadow.frag");
	program->addShader(fragShader);
	
	stateset->setAttribute(program);
	stateset->addUniform(new osg::Uniform("shadowTexture", 0));
	stateset->addUniform(new osg::Uniform("mvpt", osg::Matrix::identity()));

	this->setUpdateCallback(new UpdateCameraAndTexGenCallback(m_lightTransform, camera, stateset,m_markerId));

	this->addChild(m_lightTransform);
	


	//
	// render the real model to a texture
	//

	modelTexture = new osg::Texture2D;
	modelTexture->setTextureSize(m_shadowWidth, m_shadowHeight);

	modelTexture->setSourceFormat(GL_RGBA);
	modelTexture->setInternalFormat(GL_RGBA);

	modelTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	modelTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

	modelTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);
	modelTexture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER);
	
	osg::CameraNode* camera3 = new osg::CameraNode;
	camera3->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera3->setClearColor(osg::Vec4(0.0f,0.0f,0.0f,0.0f));
	
	camera3->setReferenceFrame(osg::CameraNode::ABSOLUTE_RF);
	camera3->setComputeNearFarMode(osg::CameraNode::DO_NOT_COMPUTE_NEAR_FAR);
	camera3->setViewport(0, 0, m_shadowWidth, m_shadowHeight);
	camera3->setRenderOrder(osg::CameraNode::PRE_RENDER);
	camera3->setRenderTargetImplementation(osg::CameraNode::FRAME_BUFFER_OBJECT);
	camera3->attach(osg::CameraNode::COLOR_BUFFER, modelTexture);
	
	osg::ref_ptr<osg::Projection> projectionMatrix2 = new osg::Projection(osg::Matrix(m_tracker->getProjectionMatrix()));	
	osg::ref_ptr<osgART::ARTTransform> markerTrans2 = new osgART::ARTTransform(m_markerId);

	this->addChild(camera3);
	camera3->addChild( projectionMatrix2.get() );
	projectionMatrix2->addChild(markerTrans2.get());
	markerTrans2->addChild(m_shadowedScene);

    

	///////////////////////////Merge the model and the shadow////////////////////////////////////////
	osg::ref_ptr<DummyImageLayer> aDummyImageLayerForShadow = new DummyImageLayer;
	aDummyImageLayerForShadow->init(m_shadowWidth,m_shadowHeight);
	
	aDummyImageLayerForShadow->setTexture( modelTexture );	
	aDummyImageLayerForShadow->setTexture( shadowTexture, 1 );	


	aDummyImageLayerForShadow->getOrCreateStateSet()->setRenderBinDetails(1000 , "RenderBin");
	aDummyImageLayerForShadow->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

	ShaderFactory sf;
	sf.addVertexAndFragmentShaderFromFile( "./data/shader/shadowMerge.vert",
										   "./data/shader/shadowMerge.frag",
								           aDummyImageLayerForShadow.get());

	aDummyImageLayerForShadow->getOrCreateStateSet()->addUniform(new osg::Uniform("modelTexture" , 0)); 
	aDummyImageLayerForShadow->getOrCreateStateSet()->addUniform(new osg::Uniform("shadowTexture", 1)); 

	this->addChild(aDummyImageLayerForShadow.get());


	m_shadowedScene->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
}

void osgART::BaseShadowRenderer::addPhantomNode(osg::ref_ptr<osg::Node> phantomNode)
{		   
	shadowedSceneWithPhantom->addChild(phantomNode.get());
}