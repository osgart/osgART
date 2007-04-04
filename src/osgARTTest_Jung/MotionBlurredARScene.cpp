#include "MotionBlurredARScene.h"


MotionBlurredARScene::MotionBlurredARScene()
{
}

MotionBlurredARScene::~MotionBlurredARScene()
{
}

void MotionBlurredARScene::init(osg::ref_ptr<osgART::GenericTracker> tracker, int _trakerID)
{
	this->ARScene::init(tracker, _trakerID); 
	projectionMatrixForOffset = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
	
}
	

osg::ref_ptr<ARNode> MotionBlurredARScene::addNewARNodeWith(osg::ref_ptr<osg::Node> node, int binNum)
{
	osg::ref_ptr<ARNode> arNode = new ARNode;
	osg::ref_ptr<ARNode> arNodeForOffset = new ARNode;

	int lastIndex = (int)arNodes.size();
	arNode->init(lastIndex, osgART::TrackerManager::getInstance()->getTracker(trackerID));
	arNodeForOffset->init(lastIndex,  osgART::TrackerManager::getInstance()->getTracker(trackerID) );

	osg::Node *copyedNode = dynamic_cast<osg::Node*> (node.get()->clone(osg::CopyOp::DEEP_COPY_ALL));
		
	arNode->addModel(node);
	arNodeForOffset->addModel(copyedNode);

	addARNode( arNode, binNum );

	osg::ref_ptr<MotionBlurredModel> motionBluredModel = new MotionBlurredModel;

	motionBlurredModels.push_back(motionBluredModel);
	motionBluredModel->init(arNodeForOffset.get(), attriIndex);

	projectionMatrixForOffset->addChild(motionBluredModel.get());

	return arNode;
}

osg::ref_ptr<MotionBlurredModel> MotionBlurredARScene::getMotionBlurredModelAt(int id)
{
	return motionBlurredModels.at(id);
}

osg::ref_ptr<osg::Texture> MotionBlurredARScene::initOffsetTexture(int _attriIndex)
{
	attriIndex = _attriIndex;
	
	ShaderFactory sf;
	sf.addVertexAndFragmentShaderFromFile("./data/shader/MotionBlurOffset.vert",
	                                      "./data/shader/MotionBlurOffset.frag", 
									      projectionMatrixForOffset.get());

	
	osg::StateAttribute *shaderProgramAtt 
		= projectionMatrixForOffset->getOrCreateStateSet()->getAttribute( osg::StateAttribute::PROGRAM );

	
	osg::Program *shaderProgram = dynamic_cast<osg::Program*>(shaderProgramAtt);
	shaderProgram->addBindAttribLocation("motionVec", attriIndex);	


	
	fboManager->attachTarget( projectionMatrixForOffset.get(), 1200, osg::Vec4(0.0,0.0f,0.0f,0.0f));
	offsetTexture = fboManager->getTexture(2);


	textureAccumHandler = new TextureAccumHandler;
	textureAccumHandler->init(bgWidth, bgHeight, this, 2, bgWidth/10, bgHeight/10, 1300);
	textureAccumHandler->setSeedTexture( offsetTexture );
	//textureAccumHandler->setTextureInput( offsetTexture, 0, 1);
	textureAccumHandler->setTextureInput( foregroundTexture, 1, 1);
	
	vector<osg::ref_ptr<DummyImageLayer> > layers = textureAccumHandler->getLayers();
	
	osg::ref_ptr<DummyImageLayer> layer1 = layers.at(0);			
	sf.addFragmentShaderFromFile( "./data/shader/SimpleBlur.frag",									
								  layer1.get());
	//sf.addFragmentShaderFromFile( "./data/shader/GaussianBlur.frag",									
	//							  layer1.get());


	layer1->getOrCreateStateSet()->addUniform(new osg::Uniform("tex01", 0)); 
	layer1->getOrCreateStateSet()->addUniform(
		new osg::Uniform("imageDimension", osg::Vec2f( bgWidth, bgHeight)));


	// adding original image
	
	osg::ref_ptr<DummyImageLayer> layer2 = layers.at(1);			
	sf.addVertexAndFragmentShaderFromFile( "./data/shader/Merge.vert",
										   "./data/shader/Merge.frag",									
											 layer2.get());

	layer2->getOrCreateStateSet()->addUniform(new osg::Uniform("tex01", 0)); 
	layer2->getOrCreateStateSet()->addUniform(new osg::Uniform("tex02", 1)); 


	//osg::ref_ptr<DummyImageLayer> layer3 = layers.at(2);			
	//sf.addFragmentShaderFromFile("./data/shader/GaussianBlur.frag",									
	//									  layer3.get());

	//layer3->getOrCreateStateSet()->addUniform(new osg::Uniform("tex", 0)); 
	//layer3->getOrCreateStateSet()->addUniform(
	//	new osg::Uniform("imageDimension", osg::Vec2f( bgWidth, bgHeight)));


	osg::ref_ptr<DummyImageLayer> dummy = new DummyImageLayer;
	dummy->init(bgWidth,bgHeight, 1, 1);
	dummy->setTexture( textureAccumHandler->getResultTexture() );	
	//dummy->setTexture( offsetTexture );	
	
	
	dummy->getOrCreateStateSet()->setRenderBinDetails(20 , "RenderBin");
	dummy->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	
	sceneGroup->addChild( dummy.get() );
	
	return offsetTexture;
}

