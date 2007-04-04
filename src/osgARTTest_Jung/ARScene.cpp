#include "ARScene.h"

ARScene::ARScene()
{
	sceneGroup = new osg::Group();       
	backgroundGroup = new osg::Group(); 
	

	foregroundGroup = new osg::Group(); 
	

	sceneGroup->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");
	backgroundGroup->getOrCreateStateSet()->setRenderBinDetails(10 , "RenderBin");
	foregroundGroup->getOrCreateStateSet()->setRenderBinDetails(20 , "RenderBin");
	
	fboManager = new FBOManager;
}

ARScene::~ARScene()
{
}

void ARScene::init(osg::ref_ptr<osgART::GenericTracker> tracker, int _trakerID)
{
	projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
	projectionMatrixForFBO = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
	trackerID = _trakerID;
	
	projectionMatrix->addChild( foregroundGroup.get() );
	backgroundGroup->addChild(projectionMatrixForFBO.get());

	this->addChild( sceneGroup.get() );
}
	
void ARScene::addARNode(osg::ref_ptr<ARNode> arNode, int binNum,  bool addToSceneGraph )
{	
	arNodes.push_back(arNode);
	arNode->getOrCreateStateSet()->setRenderBinDetails(binNum, "RenderBin");

	if ( addToSceneGraph )
		foregroundGroup->addChild( arNode.get() );	
}
	
osg::ref_ptr<ARNode> ARScene::addNewARNodeWith(osg::ref_ptr<osg::Node> node, int binNum, bool addToSceneGraph)
{
	osg::ref_ptr<ARNode> arNode = new ARNode;
	int lastIndex = (int)arNodes.size();

	arNode->init(lastIndex,  osgART::TrackerManager::getInstance()->getTracker(trackerID));
	arNode->addModel(node);
	
	addARNode( arNode, binNum, addToSceneGraph  );
	return arNode;
}

osg::ref_ptr<osgART::VideoLayer> ARScene::makeVideoBackground(osgART::GenericVideo* video)
{
		
	osg::ref_ptr<osgART::VideoLayer> videoBackground = new osgART::VideoLayer(video);
	videoBackground->init();
	videoBackground->getOrCreateStateSet()->setRenderBinDetails(1 , "RenderBin");
	
	bgWidth   = videoBackground->getWidth();
	bgHeight  = videoBackground->getHeight();
	
	return videoBackground;
}

osg::ref_ptr<osgART::VideoLayer> ARScene::initDefaultVideoBackground(osgART::GenericVideo* video, bool addToSceneGraph )
{
	osg::ref_ptr<osgART::VideoLayer> videoBackground = makeVideoBackground(video);

	if ( addToSceneGraph )
		sceneGroup->addChild(videoBackground.get());
	
	return videoBackground;
}

osg::ref_ptr<osg::Texture> ARScene::initTextureVideoBackground(osgART::GenericVideo* video, int colNum , int rowNum, bool addDummyLayer, bool _useFloatTexture, GLuint _texInternalFormat )
{
	osg::ref_ptr<osgART::VideoLayer> videoBackground = makeVideoBackground(video);
	backgroundGroup->addChild(videoBackground.get());

	fboManager->init(bgWidth, bgHeight, this, _useFloatTexture, _texInternalFormat);
	
	fboManager->attachTarget( backgroundGroup.get(), 1000);
	backgroundTexture = fboManager->getTexture(0);

	if ( addDummyLayer )
	{
		osg::ref_ptr<DummyImageLayer> dummy = new DummyImageLayer;
		dummy->init(bgWidth,bgHeight, colNum, rowNum);
		dummy->setTexture( backgroundTexture.get() );	
		dummy->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");

		sceneGroup->addChild( dummy.get() );

		backgroundDummyLayer = dummy;
	}

	return backgroundTexture;
}

	
void ARScene::initDefaultForeground()
{
	this->addChild(projectionMatrix.get());
}

osg::ref_ptr<osg::Texture> ARScene::initTextureForeground(int colNum, int rowNum, bool addDummyLayer)
{

	fboManager->attachTarget( projectionMatrix.get(), 1100, osg::Vec4(0.0,0.0f,0.0f,0.0f));
	foregroundTexture = fboManager->getTexture(1);
	
	if ( addDummyLayer )
	{
		osg::ref_ptr<DummyImageLayer> dummy = new DummyImageLayer;
		dummy->init(bgWidth,bgHeight, colNum, rowNum);
		dummy->setTexture( foregroundTexture.get() );	
		dummy->getOrCreateStateSet()->setRenderBinDetails(6 , "RenderBin");
		dummy->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
		
		sceneGroup->addChild( dummy.get() );

		foregroundDummyLayer = dummy;
	}

	return foregroundTexture;
}


void ARScene::addToBackgroundTextureGroup(osg::Node *aNode, bool isARNode)
{
	
	if ( isARNode )
		projectionMatrixForFBO->addChild(aNode);
	else
		backgroundGroup->addChild(aNode);
}

void ARScene::addToBackgroundGroup(osg::Node *aNode)
{
	sceneGroup->addChild(aNode);
}


osg::ref_ptr<ARNode> ARScene::at(int id)
{
	return arNodes.at(id);
}
int ARScene::size()
{
	return arNodes.size();
}