#include "ARScene.h"

ARScene::ARScene()
{
	sceneGroup = new osg::Group();       
	backgroundGroup = new osg::Group(); 
	backgroundGroup->ref();

	foregroundGroup = new osg::Group(); 
	foregroundGroup->ref();

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
	
osg::ref_ptr<ARNode> ARScene::addNewARNodeWith(osg::ref_ptr<osg::Node> node, int binNum)
{
	osg::ref_ptr<ARNode> arNode = new ARNode;
	int lastIndex = (int)arNodes.size();

	arNode->init(lastIndex, trackerID);
	arNode->addModel(node);
	
	addARNode( arNode, binNum );
	return arNode;
}

osg::ref_ptr<osgART::VideoBackground> ARScene::makeVideoBackground(int id)
{
		
	osg::ref_ptr<osgART::VideoBackground> videoBackground = new osgART::VideoBackground(id);
	videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_RECTANGLE);
	videoBackground->init();

	videoBackground->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");
	
	bgWidth   = videoBackground->getWidth();
	bgHeight  = videoBackground->getHeight();
	
	return videoBackground;
}

osg::ref_ptr<osgART::VideoBackground> ARScene::initDefaultVideoBackground(int id)
{
	osg::ref_ptr<osgART::VideoBackground> videoBackground = makeVideoBackground(id);
	sceneGroup->addChild(videoBackground.get());
	
	return videoBackground;
}

osg::ref_ptr<osg::Texture> ARScene::initTextureVideoBackground(int id, bool addDummyLayer, int colNum , int rowNum )
{
	osg::ref_ptr<osgART::VideoBackground> videoBackground = makeVideoBackground(id);
	backgroundGroup->addChild(videoBackground.get());

	fboManager->init(bgWidth, bgHeight, this);
	
	fboManager->attachTarget( backgroundGroup.get(), 1000);
	backgroundTexture = fboManager->getTexture(0);

	if ( addDummyLayer )
	{
		osg::ref_ptr<DummyImageLayer> dummy = new DummyImageLayer;
		dummy->init(bgWidth,bgHeight, colNum, rowNum);
		dummy->setTexture( backgroundTexture.get() );	
		dummy->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");

		sceneGroup->addChild( dummy.get() );
	}

	return backgroundTexture;
}

	
void ARScene::initDefaultForeground()
{
	this->addChild(projectionMatrix.get());
}

osg::ref_ptr<osg::Texture> ARScene::initTextureVideoBackground(bool addDummyLayer , int colNum, int rowNum)
{

	fboManager->attachTarget( foregroundGroup.get(), 1000);
	foregroundTexture = fboManager->getTexture(1);

	if ( addDummyLayer )
	{
		osg::ref_ptr<DummyImageLayer> dummy = new DummyImageLayer;
		dummy->init(bgWidth,bgHeight, colNum, rowNum);
		dummy->setTexture( backgroundTexture.get() );	
		dummy->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");

		sceneGroup->addChild( dummy.get() );
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